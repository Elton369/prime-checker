#!/bin/bash
set -e

echo "===== DEPLOY STAGE ====="

mkdir -p release

cp build/*.deb release/

echo "Release prepared"