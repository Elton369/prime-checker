#!/bin/bash
set -e

echo "===== DOCKER STAGE ====="

docker build -t prime-checker:latest .

echo "Running container test..."

output=$(printf "7\n" | docker run -i --rm prime-checker:latest)
echo "$output"

echo "$output" | grep -qi "prime"

echo "Docker stage passed"