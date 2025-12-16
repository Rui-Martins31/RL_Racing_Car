#!/bin/bash

output_file_name="agent"

# Clean
rm -rf output_file_name

# Compile and run
g++ client.cpp control.cpp utils/parser.cpp -o "$output_file_name"
./"$output_file_name"