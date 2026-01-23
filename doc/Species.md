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

Reward:
    - Distance Raced: distRaced * 10
    - Out of bounds: -10

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

Reward:
    - Distance Raced: distRaced * 10
    - Out of bounds: -10

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
    - 30% occuring a mutation
    - 0.2 change in weights in case of mutation

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

Reward:
    - Distance Raced: distRaced * 10
    - Out of bounds: -10
    - Complete lap: 10000

Notes:
    - First generation based on the [Species 2](#species-2)
    - Agents are no longer limited to 1000 cycles (20 seconds) per episode.
    - Reward based on lap completion.
    - Log: output/output_09.csv

---

## Shift'em Up

### Species 4

Generation: (Complete new species)
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 30% occuring a mutation
    - 0.2 change in weights in case of mutation

Neural Networks:
    - Topology: 8 (input), 4 (output)
    - All weights have values between [-1.0, 1.0].
    - Activation Function: Modified ReLU - [-1.0, 1.0]
    - Inputs:
        - speed: [0.0, 1.0] (up to 84 km/h)
        - angle: [-1.0, 1.0]
        - trackPos: [-1.0, 1.0]
        - rpm: [0.0, 1.0]
        - gear: [-1.0, 1.0]
        - sensor_left: [0.0, 1.0]
        - sensor_middle: [0.0, 1.0]
        - sensor_right: [0.0, 1.0]
    - Outputs:
        - accel: [-1.0, 1.0] -> remap [0.0, 1.0]
        - brake: [-1.0, 1.0] -> remap [0.0, 1.0]
        - steer: [-1.0, 1.0]
        - gear_change: [-1.0, 1.0]

Reward:
    - Distance Raced: distRaced * 10
    - Out of bounds: -10
    - Complete lap: 10000

Notes:
    - First generation is completely random
    - This new species has the freedom to shift gears and to use the distance sensor measurements to sense objects in front of it
    - Log: output/output_10.csv

### Species 4

Generation:
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 30% occuring a mutation
    - 0.2 change in weights in case of mutation

Neural Networks:
    - Topology: 8 (input), 4 (output)
    - All weights have values between [-1.0, 1.0].
    - Activation Function: Modified ReLU - [-1.0, 1.0]
    - Inputs:
        - speed: [0.0, 1.0] (up to 84 km/h)
        - angle: [-1.0, 1.0]
        - trackPos: [-1.0, 1.0]
        - rpm: [0.0, 1.0]
        - gear: [-1.0, 1.0]
        - sensor_left: [0.0, 1.0]
        - sensor_middle: [0.0, 1.0]
        - sensor_right: [0.0, 1.0]
    - Outputs:
        - accel: [-1.0, 1.0] -> remap [0.0, 1.0]
        - brake: [-1.0, 1.0] -> remap [0.0, 1.0]
        - steer: [-1.0, 1.0]
        - gear_change: [-1.0, 1.0]

Reward:
    - Distance Raced: distRaced * 10
    - Out of bounds: -10
    - Fastest: distRaced/(predicted distRaced in that time) * 10
    - Complete lap: 10000
    - Improperly shifted gears: -10
    - Properly shifted gears: 10
    - Shifted gears at the start of race: -10
    - Turbulent steering: -10

Notes:
    - First generation is based on the last generation of [Species 3](#species-3)
    - Log: output/output_11.csv

### Species 5

Generation:
    - 50 agents
    - 25 survivors
    - 10% probability of a new agent being born
    - 75% occuring a mutation
    - 0.05 change in weights in case of mutation

Neural Networks:
    - Topology: 8 (input), 4 (output)
    - All weights have values between [-1.0, 1.0].
    - Activation Function: Modified ReLU - [-1.0, 1.0]
    - Inputs:
        - speed: [0.0, 1.0] (up to 84 km/h)
        - angle: [-1.0, 1.0]
        - trackPos: [-1.0, 1.0]
        - rpm: [0.0, 1.0]
        - gear: [-1.0, 1.0]
        - sensor_left: [0.0, 1.0]
        - sensor_middle: [0.0, 1.0]
        - sensor_right: [0.0, 1.0]
    - Outputs:
        - accel: [-1.0, 1.0] -> remap [0.0, 1.0]
        - brake: [-1.0, 1.0] -> remap [0.0, 1.0]
        - steer: [-1.0, 1.0]
        - gear_change: [-1.0, 1.0]

Reward:
    - Distance Raced: distRaced * 10
    - Out of bounds: -10
    - Fastest: distRaced/(predicted distRaced in that time) * 10
    - Complete lap: 10000
    - Improperly shifted gears: -10
    - Properly shifted gears: 10
    - Shifted gears at the start of race: -10
    - Turbulent steering: -10

Notes:
    - First generation is based on the last generation of [Species 4](#species-4)
    - This species was created to fine tune the previous one
    - Log: output/output_12.csv