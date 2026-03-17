#!/bin/bash
set -e

echo "===== TEST STAGE ====="

chmod +x usr/bin/prime-checker || true

./usr/bin/prime-checker &
APP_PID=$!

sleep 2

echo "Checking /health"
curl -fsS http://127.0.0.1:8080/health | grep "OK"

echo "Checking /prime?n=7"
curl -fsS "http://127.0.0.1:8080/prime?n=7" | grep '"is_prime": true'

echo "Checking /prime?n=8"
curl -fsS "http://127.0.0.1:8080/prime?n=8" | grep '"is_prime": false'

echo "Checking /metrics"
curl -fsS http://127.0.0.1:8080/metrics | grep "prime_checker_requests_total"

kill $APP_PID
wait $APP_PID 2>/dev/null || true

echo "All tests passed"