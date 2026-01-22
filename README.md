# Table of Contents

1. [Dependencies](#dependencies)
2. [How to Run](#how-to-run)
3. [Documentation](#documentation)

---

# Dependencies

- **C++11 compiler** (g++ recommended)
- **Eigen3** - Header-only linear algebra library for neural network computations
- **POSIX sockets** - For UDP communication (Linux/macOS)
- **TORCS Simulator** - [TORCS Repository](https://github.com/fmirus/torcs-1.3.7)

### Installing Eigen3

On Ubuntu/Debian:
```bash
sudo apt-get install libeigen3-dev
```

On macOS (with Homebrew):
```bash
brew install eigen
```

---
# How to Run

Both the agent and the simulator (TORCS) should be run separately in two different terminals.

To run the simulator you should check the README in [TORCS Repository](https://github.com/fmirus/torcs-1.3.7).

A window will pop up after initializing the simulator. To test the agent we should start a race by clicking on *Race* -> *Quick Race* -> *New Race*.
The simulator will wait until the agent is started.

To run the agent, execute the following command in the terminal:
```
bash start_client.bash
```

The race should now start!

---

# Documentation

For more detailed information about the project, see the following documents:

- [Message Protocol](doc/Message_Protocol.md) - TORCS communication protocol specification
- [Neural Network](doc/Neural_Network.md) - Agent, generation, and neural network architecture
- [Reward System](doc/Reward_System.md) - Reward function and training incentives
- [Architecture](doc/Architecture.md) - System architecture and component overview
- [Species](doc/Species.md) - Evolution experiment history
- [Outputs](doc/Outputs.md) - Historical training runs