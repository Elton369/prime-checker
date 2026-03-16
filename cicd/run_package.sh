#!/bin/bash
set -e

echo "===== PACKAGE STAGE ====="

cd src
make clean
make
cd ..

rm -rf build
mkdir -p build/prime-checker_1.0.0_amd64/usr/bin
mkdir -p build/prime-checker_1.0.0_amd64/DEBIAN

cp usr/bin/prime-checker build/prime-checker_1.0.0_amd64/usr/bin/
cp DEBIAN/control build/prime-checker_1.0.0_amd64/DEBIAN/control

dpkg-deb --build build/prime-checker_1.0.0_amd64

echo "Package created:"
ls -l build/*.deb