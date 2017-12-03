#!/bin/bash 

stuff="$PWD/../../stuff"
make -C $stuff

echo "Parser expression tests:"
for file in $PWD/*.in
do
	file=${file##*/}
	file=${file%.*}
	echo -n "$file "

	test=$($stuff/Compiler -pe $PWD/$file.in | diff - $PWD/$file.out)
	if [ "$test" != "" ] 
	then	
		echo "FAIL"
	else
		echo "OK"
	fi
done

exit 0