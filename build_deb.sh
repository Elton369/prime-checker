#!/bin/bash

set -e

PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
PACKAGE_NAME="prime-checker"
PACKAGE_VERSION="1.0.0"
PACKAGE_ARCH="amd64"
BUILD_ROOT="$PROJECT_ROOT/build"
PACKAGE_DIR="$BUILD_ROOT/${PACKAGE_NAME}_${PACKAGE_VERSION}_${PACKAGE_ARCH}"

echo "[1/5] Cleaning previous build..."
rm -rf "$BUILD_ROOT"
mkdir -p "$PACKAGE_DIR/DEBIAN"
mkdir -p "$PACKAGE_DIR/usr/bin"

echo "[2/5] Building program with make..."
make -C "$PROJECT_ROOT/src" clean
make -C "$PROJECT_ROOT/src"

echo "[3/5] Copying package files..."
cp "$PROJECT_ROOT/DEBIAN/control" "$PACKAGE_DIR/DEBIAN/control"
cp "$PROJECT_ROOT/usr/bin/prime-checker" "$PACKAGE_DIR/usr/bin/prime-checker"

chmod 755 "$PACKAGE_DIR/usr/bin/prime-checker"
chmod 755 "$PACKAGE_DIR/DEBIAN"

echo "[4/5] Building deb package..."
dpkg-deb --build "$PACKAGE_DIR"

echo "[5/5] Done."
echo "Package created:"
echo "$PACKAGE_DIR.deb"