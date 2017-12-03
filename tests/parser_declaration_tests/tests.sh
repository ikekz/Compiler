#!/bin/bash 

stuff="$PWD/../../stuff"
make -C $stuff

echo "Parser declaration tests:"
for file in $PWD/*.in
do
	file=${file##*/}
	file=${file%.*}
	echo -n "$file "

	test=$($stuff/Compiler -pd $PWD/$file.in | diff - $PWD/$file.out)
	if [ "$test" != "" ] 
	then	
		echo "FAIL"
	else
		echo "OK"
	fi
done

exit 0