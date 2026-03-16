#!/bin/bash
set -e

echo "===== TEST STAGE ====="

echo "Running test 1: prime number"
output1=$(printf "7\n" | ./usr/bin/prime-checker)
echo "$output1"
echo "$output1" | grep -qi "prime"

echo "Running test 2: non-prime number"
output2=$(printf "8\n" | ./usr/bin/prime-checker)
echo "$output2"
echo "$output2" | grep -qi "not prime"

echo "All tests passed"