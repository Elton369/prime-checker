#!/bin/bash
set -e

echo "===== DOCKER STAGE ====="

docker build -t prime-checker:latest .

docker run -d --name prime-checker-test -p 8080:8080 prime-checker:latest

sleep 3

curl -fsS http://127.0.0.1:8080/health | grep "OK"
curl -fsS "http://127.0.0.1:8080/prime?n=11" | grep '"is_prime": true'
curl -fsS http://127.0.0.1:8080/metrics | grep "prime_checker_requests_total"

docker stop prime-checker-test
docker rm prime-checker-test

echo "Docker stage passed"