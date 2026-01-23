# Reward System

This document describes the reward function used to train the evolutionary driving agent. The reward system shapes agent behavior by providing positive reinforcement for good driving and penalties for mistakes.

---

## Episode End Rewards

These rewards are calculated once at the end of each episode:

| Condition | Reward | Description |
|-----------|--------|-------------|
| Out of bounds | -10 | Car leaves the track boundaries |
| Distance raced | +10 * distance | Proportional to distance traveled |
| Speed bonus | +10 * (distRaced / expected) | Bonus for racing faster than minimum expected pace |
| Lap completion | +10,000 | Large bonus for completing a full lap |

### Speed Bonus Calculation

The expected distance is calculated based on a minimum pace:
```
expected_distance = DISTANCE_MIN * episode_cycles / EPISODE_MAX
```

Where:
- `DISTANCE_MIN` = 300 meters
- `EPISODE_MAX` = 1000 cycles

If the agent travels faster than this minimum pace, it receives a proportional bonus.

---

## Per-Frame Rewards

These rewards are applied during each simulation step:

### Steering Stability

| Condition | Reward | Description |
|-----------|--------|-------------|
| Turbulent steering | -10 | Sudden steering direction changes (diff >= 0.1) |

This penalty discourages erratic steering behavior and encourages smooth driving.

### Gear Shifting

The gear change output can be -1 (downshift), 0 (maintain), or 1 (upshift).

#### Downshift (-1)

| Condition | Reward | Description |
|-----------|--------|-------------|
| RPM >= 80% max | -10 | Downshifting at high RPM is wrong |
| Already in reverse | -10 | Cannot downshift below reverse |
| Early in episode (< 500 cycles) | -10 | Penalize early shifting |
| Otherwise | +10 | Proper downshift |

#### Maintain (0)

| Condition | Reward | Description |
|-----------|--------|-------------|
| RPM >= 80% max | -10 | Should have upshifted |
| RPM <= 20% max (after 500 cycles) | -10 | Should have downshifted |

#### Upshift (+1)

| Condition | Reward | Description |
|-----------|--------|-------------|
| RPM >= 80% max AND not in 6th gear | +10 | Proper upshift timing |
| Otherwise | -10 | Premature or unnecessary upshift |
| Early in episode (< 500 cycles) | -10 | Penalize early shifting |
| Already in 6th gear | -1 | Cannot upshift further |

---

## Reward Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `REWARD_RPM_MAX` | 8000 (80% of 10000) | High RPM threshold for shifting |
| `REWARD_RPM_MIN` | 2000 (20% of 10000) | Low RPM threshold for shifting |
| `REWARD_MIN_STEER_DIFF` | 0.1 | Minimum steering change to trigger penalty |
| `REWARD_MIN_EPISODE_CYCLES_TO_SHIFT` | 500 | Minimum cycles before gear shifting is allowed |

---

## Reward Shaping Philosophy

The reward system is designed to teach the agent:

1. **Drive fast**: Distance and speed bonuses encourage forward progress
2. **Stay on track**: Out-of-bounds penalty keeps the car on the road
3. **Smooth steering**: Turbulent steering penalty encourages smooth control
4. **Proper gear management**: RPM-based rewards teach optimal shift timing
5. **Patience at start**: Early shifting penalties prevent race-start mistakes
6. **Complete laps**: Large lap completion bonus prioritizes finishing

The combination of end-of-episode and per-frame rewards creates a balanced training signal that guides the evolutionary process toward capable driving agents.
