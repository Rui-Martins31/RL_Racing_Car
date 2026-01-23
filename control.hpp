#ifndef CONTROL_HPP
#define CONTROL_HPP

#include <vector>
#include <cmath>
#include <algorithm>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "utils/parser.hpp"

/* ===== PPO API ===== */

void ppo_send_observation(const std::vector<float>& obs);
std::vector<float> ppo_receive_action();
void ppo_send_reward(float reward, bool done);
void ppo_send_reset();
void cleanup_ppo(); // ADICIONADO: função de limpeza

/* ===== CONTROL ===== */

MessageClient step(int episode_cycles, const MessageServer& m);

/* ===== UTILS ===== */

float velocity(float vx, float vy, float vz);
float remap(float v, float omin, float omax, float nmin, float nmax);
int calculate_gear(float speed); // ADICIONADO: lógica de gear

#endif