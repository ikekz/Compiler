#!/bin/bash 

file="$1"

stuff="$PWD/../../stuff"
make -C $stuff
echo -n "file name - "
echo $file
$stuff/Compiler -ps $PWD/$file.in
echo ""
echo "-----------------------------"
echo "pascal:"
fpc $PWD/$file.in

rm $file
rm $file.o