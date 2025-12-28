#!/bin/bash

output_file_name="tests_"

# Clean
rm -rf output_file_name

# Compile and run
g++ tests/test_neural_network.cpp utils/neural_network.cpp control.cpp -o "$output_file_name"
./"$output_file_name"