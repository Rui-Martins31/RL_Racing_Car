## Outputs

---

### output_01.csv

Generation:
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 5% occuring a mutation
    - 0.1 change in weights in case of mutation

Neural Network:
    - Topology: 3 (input), 3 (output)
    - All weights have values between -1 and 1

### output_02.csv

Generation:
    - 25 agents
    - 10 survivors
    - 25% probability of a new agent being born
    - 5% occuring a mutation
    - 0.1 change in weights in case of mutation   

Neural Networks:
    - Topology: 3 (input), 3 (output)
    - All weights have values between -10 and 10

Control:
    - Despite accel and brake being defined by a value in [0, 1] we are not remaping the output of tanh, still remains at [-1, 1].

### output_03.csv

Generation:
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 5% occuring a mutation
    - 1.0 change in weights in case of mutation

Neural Networks:
    - Topology: 3 (input), 3 (output)
    - All weights have values between -1 and 1 except for one of them that is scaled by 20.

Control:
    - We now remap both the accel and the brake to [0, 1].
    - Brake is limited to 25% of its intensity.

### output_04.csv

Generation:
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 5% occuring a mutation
    - 1.0 change in weights in case of mutation

Neural Networks:
    - Topology: 3 (input), 3 (output)
    - All weights have values between -1 and 1 except for one of them that is scaled by 20.

Control:
    - Before remaping we ignore all the accel and brake (NN outputs) values below 0.0 (we consider all of them as 0.0) and only remap values in [0.0, 1.0] to [0.0, 0.1].
    - Brake is limited to 10% of its intensity.

### output_05.csv

Generation:
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 5% occuring a mutation
    - 1.0 change in weights in case of mutation

Neural Networks:
    - Topology: 3 (input), 3 (output)
    - All weights have values between -1 and 1 except for one of them that is scaled by 20.

Control:
    - Same as output_04.csv ...
    - Speed input that commands from the server is normalized (divided by 80.0, the max speed we can get to in first gear). Prevents output exploding from inputing really large values of speed.

### output_06.csv

Generation:
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 5% occuring a mutation
    - 1.0 change in weights in case of mutation

Neural Networks:
    - Topology: 3 (input), 3 (output)
    - All weights have values between -1 and 1 except for one of them that is scaled by 20.

Control:
    - Same as output_05.csv ...
    - angle and trackPos are remaped to [0.0, 1.0].

### output_07.csv

Generation: (Completely new species)
    - 50 agents
    - 25 survivors
    - 25% probability of a new agent being born
    - 5% occuring a mutation
    - 0.1 change in weights in case of mutation

Neural Networks:
    - Topology: 3 (input), 3 (output)
    - All weights have values between [-1.0, 1.0].
    - Inputs:
        - angle: [-1.0, 1.0]
        - trackPos: [-1.0, 1.0]
        - speed: [0.0, 1.0]
    - Outputs:
        - accel: [0.0, 1.0]
        - brake: [0.0, 1.0]
        - steer: [-1.0, 1.0]

Control:
    - angle and trackPos are back to [-1.0, 1.0].
    - the activation function is now a modified ReLU. It basically clamps the output values to [-1.0, 1.0].
    - Since all output values are now between [-1.0, 1.0], accel and brake are remaped to [0.0, 1.0].

### output_08.csv

Generation: (From the last generation of output_07)
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
        - accel: [0.0, 1.0]
        - brake: [0.0, 1.0]
        - steer: [-1.0, 1.0]

Control:
    - angle and trackPos are back to [-1.0, 1.0].
    - the activation function is now a modified ReLU. It basically clamps the output values to [-1.0, 1.0].
    - Since all output values are now between [-1.0, 1.0], accel and brake are remaped to [0.0, 1.0].