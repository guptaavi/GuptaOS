#!/bin/bash

# This script builds a cross-compiler for x86_64-elf.
# It will be installed in $HOME/opt/cross.

set -e

export PREFIX="$HOME/opt/cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"

# Install dependencies for Debian-based systems
echo "Installing dependencies..."
sudo apt-get update
sudo apt-get install -y build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo wget

# Create the build directory
mkdir -p $HOME/src

# Build Binutils
echo "Building Binutils..."
cd $HOME/src
wget -c https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.xz
tar -xf binutils-2.42.tar.xz
cd binutils-2.42
mkdir -p build
cd build
../configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install

# Build GCC
echo "Building GCC..."
cd $HOME/src
wget -c https://ftp.gnu.org/gnu/gcc/gcc-14.1.0/gcc-14.1.0.tar.xz
tar -xf gcc-14.1.0.tar.xz
cd gcc-14.1.0
mkdir -p build
cd build
../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

echo "Cross-compiler build finished successfully."
echo "Add the following to your .bashrc or .zshrc:"
echo 'export PATH="$HOME/opt/cross/bin:$PATH"'
