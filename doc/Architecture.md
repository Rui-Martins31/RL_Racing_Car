# System Architecture

This document describes the overall system architecture and component interactions.

---

## System Overview

The system consists of two separate processes communicating via UDP:

```
+-------------------+         UDP          +-------------------+
|                   |    localhost:3001    |                   |
|   TORCS Simulator |<-------------------->|   Racing Agent    |
|                   |                      |                   |
+-------------------+                      +-------------------+
```

- **TORCS Simulator**: Runs the racing simulation and provides sensor data
- **Racing Agent**: Processes sensor data and sends control commands

---

## Directory Structure

```
pdadd_agent/
|
+-- client.cpp              # Main entry point - UDP communication
+-- control.hpp             # Agent and Generation class definitions
+-- control.cpp             # Evolutionary algorithm and control logic
|
+-- utils/
|   +-- parser.hpp          # Message struct definitions
|   +-- parser.cpp          # TORCS protocol parsing
|   +-- neural_network.hpp  # Neural network class definition
|   +-- neural_network.cpp  # Neural network implementation (Eigen3)
|
+-- doc/                    # Documentation files
|
+-- output/                 # Training output CSV files
|
+-- start_client.bash       # Build and run script
```

---

## Component Responsibilities

### client.cpp

The main entry point that handles:
- UDP socket creation and configuration
- Initial handshake with TORCS (`SCR init` -> `***identified***`)
- Main control loop: receive -> process -> send
- Episode restart handling (`***restart***` messages)

### control.cpp / control.hpp

Contains the evolutionary algorithm logic:
- `Agent` class: Wraps a neural network with its ID and reward
- `Generation` class: Manages population, selection, crossover, and mutation
- Reward calculation functions
- Training progress persistence

### utils/parser.cpp

Handles the TORCS message protocol:
- `MessageServer`: Parsed sensor data from TORCS
- `MessageClient`: Control commands to send to TORCS
- String parsing functions for the key-value format

### utils/neural_network.cpp

Neural network implementation:
- Forward propagation with configurable topology
- Weight serialization (get/set as flat vector)
- CSV save/load for persistence

---

## Data Flow

```
                    TORCS Simulator
                          |
                          v
                   [UDP Packet: Sensor Data]
                          |
                          v
     +---------------------------------------------+
     |              client.cpp                     |
     |   recv() -> parse_message_from_server()    |
     +---------------------------------------------+
                          |
                          v
     +---------------------------------------------+
     |              control.cpp                    |
     |   Generation::step()                        |
     |     -> Normalize inputs                     |
     |     -> Neural network forward pass          |
     |     -> Map outputs to controls              |
     |     -> Calculate per-frame rewards          |
     +---------------------------------------------+
                          |
                          v
     +---------------------------------------------+
     |              client.cpp                     |
     |   parse_message_from_client() -> sendto()  |
     +---------------------------------------------+
                          |
                          v
                   [UDP Packet: Control Commands]
                          |
                          v
                    TORCS Simulator
```

---

## Main Loop Sequence

1. **Initialization**
   - Create UDP socket bound to localhost:3001
   - Perform handshake with TORCS
   - Create `Generation` object (loads previous training if available)

2. **Control Loop** (repeats every simulation tick, ~50 Hz)
   - Receive sensor data from TORCS
   - Check for restart message (episode boundary)
   - Parse sensor message into `MessageServer` struct
   - Call `Generation::step()` to get control decision
   - Convert `MessageClient` to string response
   - Send control message to TORCS
   - Increment episode cycle counter

3. **Episode End** (handled within `Generation::step()`)
   - Detect end condition (out of bounds, timeout, or lap complete)
   - Calculate final reward
   - Save agent weights to CSV
   - Call `Generation::update()` to advance to next agent
   - If all agents evaluated, call `populate()` for next generation

---

## Connection Parameters

| Parameter | Value | Description |
|-----------|-------|-------------|
| `SERVER_IP` | 127.0.0.1 | TORCS server address |
| `SERVER_PORT` | 3001 | TORCS SCR port |
| `SOCKET_TIMEOUT` | 3 seconds | Receive timeout |
| `BUFFER_SIZE` | 4096 bytes | Message buffer size |

---

## Build Process

The `start_client.bash` script compiles and runs the agent:

```bash
g++ client.cpp control.cpp utils/parser.cpp utils/neural_network.cpp -o "agent"
./agent
```

Dependencies are linked via Eigen3 header-only library.
