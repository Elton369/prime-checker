#!/bin/bash

set -e

echo "===== BUILD STAGE ====="

cd src

make clean
make

if [ ! -f ../usr/bin/prime-checker ]; then
    echo "Build failed"
    exit 1
fi

echo "Build successful"