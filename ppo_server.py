import socket
import struct
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from torch.distributions import Normal
import time

# ======================================================
# CONFIG
# ======================================================
UDP_PORT = 5555
OBS_DIM = 25  # CORRIGIDO: 3 base + 19 track + 2 velocities + 1 damage
ACT_DIM = 2   # steer, throttle

RESET_MAGIC = 0xDEADBEEF

GAMMA = 0.99
LAMBDA = 0.95
CLIP = 0.2
LR = 3e-4

BATCH = 2048
EPOCHS = 10

DEVICE = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# ======================================================
# NETWORK
# ======================================================
class ActorCritic(nn.Module):
    def __init__(self):
        super().__init__()

        self.actor = nn.Sequential(
            nn.Linear(OBS_DIM, 256),
            nn.Tanh(),
            nn.Linear(256, 128),
            nn.Tanh(),
            nn.Linear(128, ACT_DIM),
        )

        self.critic = nn.Sequential(
            nn.Linear(OBS_DIM, 256),
            nn.Tanh(),
            nn.Linear(256, 128),
            nn.Tanh(),
            nn.Linear(128, 1),
        )

        self.log_std = nn.Parameter(torch.zeros(ACT_DIM))

    def act(self, obs):
        obs = torch.tensor(obs, dtype=torch.float32, device=DEVICE)

        mean = self.actor(obs)
        std = torch.exp(self.log_std)

        dist = Normal(mean, std)
        action = dist.sample()

        # Clamping para garantir valores válidos
        action[0] = torch.clamp(action[0], -1.0, 1.0)  # steer
        action[1] = torch.clamp(action[1], 0.0, 1.0)   # throttle

        logp = dist.log_prob(action).sum()
        value = self.critic(obs).squeeze()

        return (
            action.cpu().detach().numpy(),
            logp.detach(),
            value.detach(),
        )

    def evaluate(self, obs, act):
        mean = self.actor(obs)
        std = torch.exp(self.log_std)
        dist = Normal(mean, std)

        logp = dist.log_prob(act).sum(-1)
        entropy = dist.entropy().sum(-1)
        value = self.critic(obs).squeeze()

        return logp, entropy, value


# ======================================================
# ROLLOUT STORAGE
# ======================================================
obs_buf = []
act_buf = []
logp_buf = []
rew_buf = []
done_buf = []
val_buf = []

# ======================================================
# PPO UPDATE
# ======================================================
def ppo_update(model, optimizer):
    if len(obs_buf) == 0:
        print("[PPO] Warning: Empty buffer, skipping update")
        return

    obs = torch.from_numpy(np.array(obs_buf)).float().to(DEVICE)
    act = torch.from_numpy(np.array(act_buf)).float().to(DEVICE)
    old_logp = torch.stack(logp_buf).detach().to(DEVICE)
    val = torch.stack(val_buf).detach().to(DEVICE)

    rewards = np.array(rew_buf)
    dones = np.array(done_buf)

    # Compute advantages using GAE
    advantages = []
    gae = 0.0
    next_value = 0.0

    for i in reversed(range(len(rewards))):
        delta = rewards[i] + GAMMA * next_value * (1 - dones[i]) - val[i].cpu().item()
        gae = delta + GAMMA * LAMBDA * (1 - dones[i]) * gae
        advantages.insert(0, gae)
        next_value = val[i].cpu().item()

    advantages = torch.from_numpy(np.array(advantages)).float().to(DEVICE)
    returns = advantages + val

    # Normalize advantages
    advantages = (advantages - advantages.mean()) / (advantages.std() + 1e-8)

    # PPO update
    total_actor_loss = 0.0
    total_critic_loss = 0.0
    
    for epoch in range(EPOCHS):
        logp, entropy, values = model.evaluate(obs, act)

        ratio = torch.exp(logp - old_logp)
        surr1 = ratio * advantages
        surr2 = torch.clamp(ratio, 1 - CLIP, 1 + CLIP) * advantages

        actor_loss = -torch.min(surr1, surr2).mean()
        critic_loss = (returns - values).pow(2).mean()
        entropy_loss = -entropy.mean()

        loss = actor_loss + 0.5 * critic_loss + 0.01 * entropy_loss

        optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(model.parameters(), 0.5)
        optimizer.step()

        total_actor_loss += actor_loss.item()
        total_critic_loss += critic_loss.item()

    print(f"[PPO] Update complete - Actor Loss: {total_actor_loss/EPOCHS:.4f}, "
          f"Critic Loss: {total_critic_loss/EPOCHS:.4f}, "
          f"Avg Reward: {np.mean(rewards):.2f}, "
          f"Buffer size: {len(rewards)}")

    obs_buf.clear()
    act_buf.clear()
    logp_buf.clear()
    rew_buf.clear()
    done_buf.clear()
    val_buf.clear()


# ======================================================
# MAIN LOOP
# ======================================================
def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", UDP_PORT))
    sock.setblocking(False)

    model = ActorCritic().to(DEVICE)
    optimizer = optim.Adam(model.parameters(), lr=LR)

    print(f"[PPO] Server running on port {UDP_PORT}")
    print(f"[PPO] Device: {DEVICE}")
    print(f"[PPO] Observation dim: {OBS_DIM}, Action dim: {ACT_DIM}")
    print(f"[PPO] Batch size: {BATCH}, Update epochs: {EPOCHS}")

    last_obs = None
    last_act = None
    last_logp = None
    last_val = None

    ep_len = 0
    ep_reward = 0.0
    ep_idx = 0
    
    total_steps = 0
    total_updates = 0
    start_time = time.time()

    while True:
        try:
            data, addr = sock.recvfrom(4096)
        except BlockingIOError:
            continue
        except Exception as e:
            print(f"[PPO] Socket error: {e}")
            continue

        # ---------------- RESET SIGNAL ----------------
        if len(data) == 4:
            try:
                code = struct.unpack("I", data)[0]
                if code == RESET_MAGIC:
                    elapsed = time.time() - start_time
                    fps = ep_len / elapsed if elapsed > 0 else 0
                    
                    print(f"[EP {ep_idx:04d}] Steps={ep_len:4d} | "
                          f"Reward={ep_reward:7.1f} | "
                          f"Avg Rew={ep_reward/max(ep_len,1):6.2f} | "
                          f"FPS={fps:.1f} | "
                          f"Total Updates={total_updates}")
                    
                    ep_idx += 1
                    ep_len = 0
                    ep_reward = 0.0
                    last_obs = None
                    start_time = time.time()
            except struct.error:
                pass
            continue

        # ---------------- OBSERVATION ----------------
        expected_bytes = OBS_DIM * 4
        if len(data) >= expected_bytes:
            try:
                obs = np.frombuffer(data[:expected_bytes], dtype=np.float32)
                
                if len(obs) != OBS_DIM:
                    print(f"[PPO] ERROR: Expected {OBS_DIM} observations, got {len(obs)}")
                    continue

                # Validação básica
                if np.any(np.isnan(obs)) or np.any(np.isinf(obs)):
                    print(f"[PPO] Warning: Invalid observation values detected")
                    continue

                action, logp, val = model.act(obs)

                # Enviar ação de volta
                response = struct.pack("2f", float(action[0]), float(action[1]))
                sock.sendto(response, addr)

                last_obs = obs
                last_act = action
                last_logp = logp
                last_val = val
                
            except Exception as e:
                print(f"[PPO] Error processing observation: {e}")
            continue

        # ---------------- REWARD + DONE ----------------
        if len(data) >= 8:
            try:
                reward, done_flag = struct.unpack("ff", data[:8])
                done = bool(done_flag > 0.5)

                if last_obs is not None:
                    obs_buf.append(last_obs)
                    act_buf.append(last_act)
                    logp_buf.append(last_logp)
                    rew_buf.append(reward)
                    done_buf.append(done)
                    val_buf.append(last_val)

                ep_len += 1
                ep_reward += reward
                total_steps += 1

                # Trigger update when buffer is full
                if len(rew_buf) >= BATCH:
                    print(f"\n[PPO] === TRIGGERING UPDATE === (buffer: {len(rew_buf)}/{BATCH})")
                    ppo_update(model, optimizer)
                    total_updates += 1
                    print(f"[PPO] === UPDATE COMPLETE === (total updates: {total_updates})\n")
                    
            except struct.error as e:
                print(f"[PPO] Error unpacking reward: {e}")
            continue


if __name__ == "__main__":
    main()