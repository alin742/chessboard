#!/bin/sh

clear
set -xe

BUILD_DIR="./build"
FILE_NAME="chessboard"

[[ -e $BUILD_DIR ]] || mkdir $BUILD_DIR;
gcc -o $BUILD_DIR/$FILE_NAME main.c
$BUILD_DIR/$FILE_NAME
