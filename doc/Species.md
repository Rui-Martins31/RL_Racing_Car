# Species

---

## Baby Steps

### Species 1

Generation:
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 5% occuring a mutation
    - 0.1 change in weights in case of mutation

Neural Networks:
    - Topology: 3 (input), 3 (output)
    - All weights have values between [-1.0, 1.0].
    - Activation Function: Modified ReLU - [-1.0, 1.0]
    - Inputs:
        - angle: [-1.0, 1.0]
        - trackPos: [-1.0, 1.0]
        - speed: [0.0, 1.0]
    - Outputs:
        - accel: [-1.0, 1.0] -> remap [0.0, 1.0]
        - brake: [-1.0, 1.0] -> remap [0.0, 1.0]
        - steer: [-1.0, 1.0]

Notes:
    - First generation is completely random.
    - Log: output/output_07.csv

### Species 2

Generation:
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 30% occuring a mutation
    - 0.2 change in weights in case of mutation

Neural Networks:
    - Topology: 3 (input), 3 (output)
    - All weights have values between [-1.0, 1.0].
    - Inputs:
        - angle: [-1.0, 1.0]
        - trackPos: [-1.0, 1.0]
        - speed: [0.0, 1.0]
    - Outputs:
        - accel: [-1.0, 1.0] -> remap [0.0, 1.0]
        - brake: [-1.0, 1.0] -> remap [0.0, 1.0]
        - steer: [-1.0, 1.0]

Notes: 
    - The first generation of this Species is the same as the last generation of [Species 1](#species-1)
    - Log: output/output_08.csv

---

## Drive Baby Drive

### Species 3

Generation:
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 5% occuring a mutation
    - 0.1 change in weights in case of mutation

Neural Networks:
    - Topology: 3 (input), 3 (output)
    - All weights have values between [-1.0, 1.0].
    - Activation Function: Modified ReLU - [-1.0, 1.0]
    - Inputs:
        - angle: [-1.0, 1.0]
        - trackPos: [-1.0, 1.0]
        - speed: [0.0, 1.0]
    - Outputs:
        - accel: [-1.0, 1.0] -> remap [0.0, 1.0]
        - brake: [-1.0, 1.0] -> remap [0.0, 1.0]
        - steer: [-1.0, 1.0]

Notes:
    - First generation based on the [Species 2](#species-2)
    - Agents are no longer limited to 1000 cycles (20 seconds) per episode.
    - Log: output/output_09.csv
