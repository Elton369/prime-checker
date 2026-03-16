#!/bin/bash

set -e

echo "===== PACKAGE STAGE ====="

mkdir -p build/prime-checker/usr/bin
mkdir -p build/prime-checker/DEBIAN

cp usr/bin/prime-checker build/prime-checker/usr/bin/
cp DEBIAN/control build/prime-checker/DEBIAN/

dpkg-deb --build build/prime-checker

echo "Package created"