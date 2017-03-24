#!/bin/bash 

make -C /Users/ilyazuev/prog/Compiler/stuff/

for file in /Users/ilyazuev/prog/Compiler/tests/parser_tests/*.in
do
	file=${file##*/}
	file=${file%.*}
	echo -n "$file "

	test=$(/Users/ilyazuev/prog/Compiler/stuff/Compiler -p /Users/ilyazuev/prog/Compiler/tests/parser_tests/$file.in | diff - /Users/ilyazuev/prog/Compiler/tests/parser_tests/$file.out)
	if [ "$test" != "" ] 
	then	
		echo "FAIL"
	else
		echo "OK"
	fi
done

exit 0