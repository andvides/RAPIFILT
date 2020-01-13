#!/bin/bash
echo "rapifilt compilation"
#export C_INCLUDE_PATH=${PREFIX}/include
#export LIBRARY_PATH=${PREFIX}/lib

make
mkdir -p $PREFIX/bin
cp rapifilt $PREFIX/bin
echo "Installation successful"
