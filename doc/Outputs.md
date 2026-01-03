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
    - Brake is limited to 25% of its intensity.