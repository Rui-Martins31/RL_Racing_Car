# Neural Network and Evolutionary System

This document describes the neural network architecture, agent behavior, and evolutionary algorithm used in this project.

---

## Agent

The `Agent` class represents a single driving agent in the simulation.

### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `nn` | `NeuralNetwork` | The neural network that controls the agent's driving |
| `id` | `int` | Unique identifier for the agent |
| `reward` | `float` | Accumulated fitness score during an episode |
| `previous_message` | `MessageClient` | Last control output (used for consistency checks) |

### Lifecycle

1. Agent is created with either random or inherited weights
2. Agent runs through an episode in the simulator
3. Neural network processes sensor inputs and outputs driving controls
4. Rewards/penalties are accumulated based on performance
5. At episode end, the final reward determines survival probability

---

## Generation

The `Generation` class manages the population of agents and implements the evolutionary algorithm.

### Evolutionary Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| `AGENTS_NUM_TOTAL` | 50 | Total population size per generation |
| `AGENTS_NUM_SURVIVE` | 25 | Number of top agents that survive to reproduce |
| `AGENT_PROB_NEW` | 0.25 | Probability of creating a completely new random agent |
| `MUTATION_PROB` | 0.30 | Probability of a weight being mutated |
| `MUTATION_CHANGE` | 0.20 | Amount added/subtracted during mutation |

### Evolution Process

1. **Evaluation**: Each agent runs an episode and accumulates rewards
2. **Selection**: After all 50 agents run, they are sorted by reward
3. **Survival**: Top 25 agents are kept
4. **Reproduction**: 25 new agents are created:
   - 25% chance: Completely new random agent
   - 75% chance: Child created via crossover and mutation
5. **Crossover**: Two random parents are selected; child inherits alternating weights from each
6. **Mutation**: Each weight has a 30% chance of being modified by +/- 0.2
7. **Repeat**: New generation of 50 agents begins

### Persistence

Training progress is saved to `output.csv` in the format:
```
generation, reward, weight1, weight2, ..., weightN
```

When the agent restarts, it loads the last complete generation and continues training.

---

## Neural Network Architecture

The neural network is a simple feed-forward network implemented using Eigen3.

### Topology

Current configuration (Species 4):
```
Input Layer: 8 neurons
Output Layer: 4 neurons
```

### Activation Function

Modified ReLU that clamps output values to the range `[-1.0, 1.0]`:

```cpp
if (x < -1.0) return -1.0;
if (x > 1.0) return 1.0;
return x;
```

### Inputs

| Index | Input | Range | Description |
|-------|-------|-------|-------------|
| 0 | Speed | [0.0, 1.0] | Velocity normalized by MAX_SPEED (83 km/h) |
| 1 | Angle | [-1.0, 1.0] | Angle between car direction and track axis |
| 2 | Track Position | [-1.0, 1.0] | Position relative to track center (-1 = left edge, 1 = right edge) |
| 3 | RPM | [0.0, 1.0] | Engine RPM normalized by RPM_MAX (10000) |
| 4 | Gear | [-1.0, 1.0] | Current gear (-1 to 6) remapped to [-1, 1] |
| 5 | Sensor Left | [0.0, 1.0] | Distance sensor at -40 degrees |
| 6 | Sensor Middle | [0.0, 1.0] | Distance sensor at 0 degrees (straight ahead) |
| 7 | Sensor Right | [0.0, 1.0] | Distance sensor at +40 degrees |

### Outputs

| Index | Output | NN Range | Final Range | Description |
|-------|--------|----------|-------------|-------------|
| 0 | Acceleration | [-1.0, 1.0] | [0.0, 1.0] | Throttle intensity |
| 1 | Brake | [-1.0, 1.0] | [0.0, 1.0] | Brake intensity |
| 2 | Steering | [-1.0, 1.0] | [-1.0, 1.0] | Steering angle (-1 = full left, 1 = full right) |
| 3 | Gear Change | [-1.0, 1.0] | {-1, 0, 1} | Downshift, maintain, or upshift |

---

## Weight Storage

Weights are stored as a flat vector and include bias weights. The serialization format is CSV with:
- Column 1: Generation number
- Column 2: Final reward
- Columns 3+: Network weights

This allows easy analysis of training progress and resumption of interrupted training sessions.
