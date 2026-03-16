#!/bin/bash
set -e

echo "===== RELEASE STAGE ====="

if ls build/*.deb >/dev/null 2>&1; then
    echo "Release artifact is ready"
    ls -l build/*.deb
else
    echo "Error: no .deb package found"
    exit 1
fi