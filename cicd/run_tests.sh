#!/bin/bash

set -e

echo "===== TEST STAGE ====="

echo "Test 1: prime number"

echo 7 | ./usr/bin/prime-checker | grep "prime"

echo "Test 2: non-prime number"

echo 8 | ./usr/bin/prime-checker | grep "not prime"

echo "All tests passed"