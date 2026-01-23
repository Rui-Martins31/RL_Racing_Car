#include "control.hpp"
#include <cstring>
#include <iostream>
#include <sys/select.h>

/* ================= PPO SOCKET ================= */

static int ppo_sock = -1;
static sockaddr_in ppo_addr{};
static bool ppo_initialized = false;

/* ================= EPISODE STATE ================= */

static float last_dist = 0.0f;
static float last_damage = 0.0f;
static int stagnation_steps = 0;
static int low_speed_steps = 0;
static int offtrack_steps = 0;
static int global_episode_count = 0;  // NOVO: Contador global

/* ================= CONSTANTS ================= */

#define PPO_IP "127.0.0.1"
#define PPO_PORT 5555
#define PPO_ACTION_TIMEOUT_MS 200

#define MAX_SPEED 83.0f
#define MAX_EPISODE_STEPS 2500
#define STAGNATION_LIMIT 300
#define LOW_SPEED_LIMIT 200
#define WARMUP_STEPS 150
#define MIN_SPEED_THRESHOLD 0.3f
#define STAGNATION_THRESHOLD 0.01f

// THRESHOLDS DE TERMINAÇÃO - Sistema de 3 níveis (INICIAL - PERMISSIVO)
#define SOFT_OFFTRACK_THRESHOLD 1.0f   // Início da grama (warning)
#define HARD_OFFTRACK_THRESHOLD 2.0f   // Muito fora (era 1.5, aumentado para treino inicial)
#define OFFTRACK_TOLERANCE 100         // Steps permitidos fora (era 50, 2 segundos agora)
#define DAMAGE_THRESHOLD 1000.0f       // Dano crítico (era 500, mais permissivo)
#define DAMAGE_RATE_THRESHOLD 200.0f   // Dano por step (era 100, dobrado)

/* ================= UTILS ================= */

float velocity(float vx, float vy, float vz)
{
    return std::sqrt(vx * vx + vy * vy + vz * vz);
}

float remap(float v, float omin, float omax, float nmin, float nmax)
{
    return nmin + (v - omin) * (nmax - nmin) / (omax - omin);
}

/* ================= PPO ================= */

static void init_ppo()
{
    if (ppo_initialized) return;

    ppo_sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&ppo_addr, 0, sizeof(ppo_addr));

    ppo_addr.sin_family = AF_INET;
    ppo_addr.sin_port = htons(PPO_PORT);
    inet_pton(AF_INET, PPO_IP, &ppo_addr.sin_addr);

    ppo_initialized = true;
    std::cout << "[PPO] Initialized socket to " << PPO_IP << ":" << PPO_PORT << std::endl;
}

void ppo_send_observation(const std::vector<float>& obs)
{
    init_ppo();
    
    int flags = MSG_DONTWAIT;
    ssize_t sent = sendto(ppo_sock, obs.data(), obs.size() * sizeof(float), flags,
                          (sockaddr*)&ppo_addr, sizeof(ppo_addr));
    
    if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        static int error_count = 0;
        if (++error_count % 100 == 0) {
            std::cerr << "[PPO] Error sending observation (" << error_count << " times)" << std::endl;
        }
    }
}

std::vector<float> ppo_receive_action()
{
    static float last_valid_action[2] = {0.0f, 0.95f};
    float buf[2] = {0.0f, 0.95f};

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(ppo_sock, &fds);

    timeval tv{0, PPO_ACTION_TIMEOUT_MS * 1000};
    int r = select(ppo_sock + 1, &fds, nullptr, nullptr, &tv);

    if (r > 0) {
        ssize_t received = recvfrom(ppo_sock, buf, sizeof(buf), MSG_DONTWAIT, nullptr, nullptr);
        if (received == sizeof(buf)) {
            last_valid_action[0] = buf[0];
            last_valid_action[1] = buf[1];
            return {buf[0], buf[1]};
        }
    }

    static int timeout_count = 0;
    if (++timeout_count % 100 == 1) {
        std::cout << "[PPO] Warning: Timeout receiving action (" << timeout_count << " times)" << std::endl;
    }
    
    return {last_valid_action[0], last_valid_action[1]};
}

void ppo_send_reward(float reward, bool done)
{
    float msg[2] = {reward, done ? 1.0f : 0.0f};
    sendto(ppo_sock, msg, sizeof(msg), 0,
           (sockaddr*)&ppo_addr, sizeof(ppo_addr));
}

void ppo_send_reset()
{
    uint32_t magic = 0xDEADBEEF;
    sendto(ppo_sock, &magic, sizeof(magic), 0,
           (sockaddr*)&ppo_addr, sizeof(ppo_addr));
    
    std::cout << "[PPO] Reset signal sent" << std::endl;
}

/* ================= GEAR LOGIC ================= */

int calculate_gear(float speed)
{
    if (speed < 3.0f) return 1;
    if (speed < 20.0f) return 2;
    if (speed < 35.0f) return 3;
    if (speed < 50.0f) return 4;
    if (speed < 65.0f) return 5;
    return 6;
}

/* ================= MAIN STEP ================= */

MessageClient step(int episode_cycles, const MessageServer& m)
{
    MessageClient c{};

    float speed = velocity(m.speedX, m.speedY, m.speedZ);

    // ============================================
    // OBSERVATION VECTOR (25 dims agora!)
    // ============================================
    std::vector<float> obs;
    
    obs.push_back(std::clamp(m.trackPos, -1.0f, 1.0f));
    obs.push_back(std::clamp(m.angle / 3.1416f, -1.0f, 1.0f));
    obs.push_back(std::clamp(speed / MAX_SPEED, 0.0f, 1.0f));
    
    for (int i = 0; i < 19; i++) {
        obs.push_back(std::clamp(m.track[i] / 200.0f, 0.0f, 1.0f));
    }
    
    obs.push_back(std::clamp(m.speedX / MAX_SPEED, -1.0f, 1.0f));
    obs.push_back(std::clamp(m.speedY / MAX_SPEED, -1.0f, 1.0f));
    
    // NOVO: Adicionar damage normalizado
    obs.push_back(std::clamp(m.damage / 10000.0f, 0.0f, 1.0f));
    
    ppo_send_observation(obs);

    auto act = ppo_receive_action();
    float steer = std::clamp(act[0], -1.0f, 1.0f);
    float throttle = std::clamp(act[1], 0.0f, 1.0f);
    
    // CURRICULUM LEARNING: Limitar velocidade nos primeiros episódios
    float max_speed_limit = 83.0f;  // Velocidade máxima padrão
    
    if (global_episode_count < 50) {
        max_speed_limit = 40.0f;  // Primeiros 50 episódios: aprende devagar
    } else if (global_episode_count < 150) {
        max_speed_limit = 60.0f;  // Episódios 50-150: velocidade média
    }
    // Depois de 150 episódios: sem limites!
    
    // Aplicar limitador
    if (speed > max_speed_limit) {
        throttle *= 0.5f;  // Reduzir throttle se muito rápido
    }
    
    if (episode_cycles % 50 == 0) {
        std::cout << "[OBS] trackPos=" << m.trackPos 
                  << ", angle=" << m.angle 
                  << ", speed=" << speed 
                  << ", damage=" << m.damage
                  << ", frontSensor=" << m.track[9] << std::endl;
        std::cout << "[ACT] steer=" << steer 
                  << ", throttle=" << throttle << std::endl;
    }

    c.steer = steer;
    c.accel = throttle;
    
    // NOVO: Brake automático em situações perigosas
    c.brake = 0.0f;
    
    // Se está muito rápido E com ângulo MAU E próximo da borda
    bool dangerous_situation = (speed > 25.0f) && 
                               (std::abs(m.angle) > 0.5f) && 
                               (std::abs(m.trackPos) > 0.7f);
    
    // Ou se sensor frontal detecta parede muito próxima
    bool wall_ahead = m.track[9] < 10.0f && speed > 15.0f;
    
    if (dangerous_situation || wall_ahead) {
        c.brake = 0.5f;  // Travagem moderada
        c.accel = 0.0f;  // Cortar aceleração
        
        if (episode_cycles % 50 == 0) {
            std::cout << "[SAFETY] Auto-brake! speed=" << speed 
                      << ", angle=" << m.angle 
                      << ", trackPos=" << m.trackPos 
                      << ", frontSensor=" << m.track[9] << std::endl;
        }
    }
    
    c.gear = calculate_gear(speed);
    
    if (episode_cycles < 10 && speed < 2.0f) {
        c.clutch = 0.5f;
    } else {
        c.clutch = 0.0f;
    }
    
    c.focus = 0.0f;
    c.meta = false;
    
    /* ============================================
       TERMINATION LOGIC - SISTEMA HÍBRIDO
       ============================================ */

    float dist_delta = m.distRaced - last_dist;
    float damage_delta = m.damage - last_damage;
    
    // NÍVEL 1: Soft off-track (warning via reward)
    bool soft_offtrack = std::abs(m.trackPos) > SOFT_OFFTRACK_THRESHOLD;
    
    // NÍVEL 2: Hard off-track (tolera temporariamente)
    bool hard_offtrack = std::abs(m.trackPos) > HARD_OFFTRACK_THRESHOLD;
    
    if (hard_offtrack)
        offtrack_steps++;
    else
        offtrack_steps = 0;
    
    // NÍVEL 3: Dano crítico
    bool critical_damage = m.damage > DAMAGE_THRESHOLD;
    bool continuous_damage = damage_delta > DAMAGE_RATE_THRESHOLD;
    
    // Terminar se:
    // - Muito fora da pista POR MUITO TEMPO (não um toque rápido)
    // - Dano crítico acumulado (bateu forte)
    // - Dano contínuo (a bater na parede)
    bool off_track = (offtrack_steps > OFFTRACK_TOLERANCE) || 
                     critical_damage || 
                     continuous_damage;
    
    bool timeout = episode_cycles >= MAX_EPISODE_STEPS;

    // Stagnation check
    if (dist_delta < STAGNATION_THRESHOLD)
        stagnation_steps++;
    else
        stagnation_steps = 0;

    last_dist = m.distRaced;
    last_damage = m.damage;
    
    if (episode_cycles % 100 == 0) {
        std::cout << "[Step " << episode_cycles << "] "
                  << "Speed=" << speed << " | "
                  << "Gear=" << c.gear << " | "
                  << "Throttle=" << throttle << " | "
                  << "Dist=" << m.distRaced << "m (Δ=" << dist_delta << ") | "
                  << "TrackPos=" << m.trackPos << " | "
                  << "Damage=" << m.damage << " (Δ=" << damage_delta << ") | "
                  << "OfftrackSteps=" << offtrack_steps << "/" << OFFTRACK_TOLERANCE << std::endl;
    }

    // Speed checks
    if (speed < MIN_SPEED_THRESHOLD)
        low_speed_steps++;
    else
        low_speed_steps = 0;

    bool stuck = (stagnation_steps > STAGNATION_LIMIT) || 
                 (low_speed_steps > LOW_SPEED_LIMIT);

    bool done = off_track || timeout ||
                (episode_cycles > WARMUP_STEPS && stuck);
    
    if (done) {
        std::cout << "\n[TERMINATION] Episode " << episode_cycles << ":" << std::endl;
        std::cout << "  - off_track: " << off_track << std::endl;
        std::cout << "    · trackPos: " << m.trackPos << " (hard=" << hard_offtrack 
                  << ", steps=" << offtrack_steps << "/" << OFFTRACK_TOLERANCE << ")" << std::endl;
        std::cout << "    · damage: " << m.damage << " (critical=" << critical_damage 
                  << ", delta=" << damage_delta << ")" << std::endl;
        std::cout << "  - timeout: " << timeout << std::endl;
        std::cout << "  - stuck: " << stuck << " (stag=" << stagnation_steps 
                  << "/" << STAGNATION_LIMIT << ", low_spd=" << low_speed_steps 
                  << "/" << LOW_SPEED_LIMIT << ")" << std::endl;
        std::cout << "  - speed: " << speed << " m/s" << std::endl;
        std::cout << std::endl;
    }

    /* ============================================
       REWARD SHAPING - OTIMIZADO PARA APRENDIZADO
       ============================================ */

    float reward = 0.0f;
    
    // Rewards positivos (AUMENTADOS para encorajar progresso)
    reward += dist_delta * 5.0f;           // Era 2.0 - PRIORIZAR progredir
    reward += speed * 0.1f;                // Era 0.05 - Velocidade importa
    
    // Penalties graduais (REDUZIDAS no início)
    reward -= std::abs(m.trackPos) * 0.1f; // Era 0.2 - Mais tolerante
    reward -= std::abs(m.angle) * 0.1f;    // Era 0.05 - AUMENTADO - ângulo é crítico!
    
    // Bonus por estar centralizado E com ângulo correto
    if (std::abs(m.trackPos) < 0.5f && std::abs(m.angle) < 0.3f) {
        reward += 1.0f;  // NOVO: Bonus por controle
    }
    
    // NOVO: Bonus por antecipar curvas (reduzir velocidade quando sensores detectam)
    float front_sensor = m.track[9];  // Sensor central frontal
    if (front_sensor < 30.0f && speed < 20.0f) {
        reward += 0.5f;  // Boa decisão: desacelerar em curva
    } else if (front_sensor < 20.0f && speed > 25.0f) {
        reward -= 2.0f;  // Má decisão: muito rápido para a curva!
    }
    
    // Penalty suave por estar na grama (soft off-track)
    if (soft_offtrack && !hard_offtrack) {
        reward -= 1.0f;  // Era 0.5 - Aumentado: EVITA a relva!
    }
    
    // Penalty moderada por estar muito fora
    if (hard_offtrack) {
        reward -= 5.0f;  // Era 2.0 - Aumentado: PERIGO REAL!
    }
    
    // Penalty por dano
    reward -= damage_delta * 0.1f;  // Cada ponto de dano = -0.1 reward

    // Penalties de terminação
    if (off_track) {
        if (critical_damage || continuous_damage) {
            reward -= 100.0f;  // Era 50 - Crash é MUITO MAU
            std::cout << "[TERM] CRASH! Damage=" << m.damage << std::endl;
        } else {
            reward -= 50.0f;  // Era 20 - Sair da pista é MAU
            std::cout << "[TERM] Off track for too long!" << std::endl;
        }
    }
    
    if (stuck) {
        reward -= 30.0f;  // Era 10 - Ficar parado é MAU
        std::cout << "[TERM] Stuck! (Speed=" << speed << " m/s)" << std::endl;
    }

    ppo_send_reward(reward, done);

    if (done)
    {
        float avg_speed = (episode_cycles > 0) ? m.distRaced / (episode_cycles * 0.02f) : 0.0f;
        std::cout << "[EPISODE END] Steps: " << episode_cycles 
                  << " | Distance: " << m.distRaced << "m"
                  << " | Avg Speed: " << avg_speed << " m/s"
                  << " | Total Damage: " << m.damage 
                  << " | Global Episode: " << global_episode_count << std::endl;
        
        stagnation_steps = 0;
        low_speed_steps = 0;
        offtrack_steps = 0;
        last_dist = 0.0f;
        last_damage = 0.0f;
        global_episode_count++;  // NOVO: Incrementar contador

        ppo_send_reset();
        c.meta = true;
    }

    return c;
}

void cleanup_ppo()
{
    if (ppo_sock >= 0) {
        close(ppo_sock);
        ppo_sock = -1;
        ppo_initialized = false;
        std::cout << "[PPO] Socket closed" << std::endl;
    }
}