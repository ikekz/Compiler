#!/bin/bash 

newfile="$1"
path=/Users/ilyazuev/prog/Compiler/tests/scanner_tests/

function sht {	
	local file="$1"

	# echo "$file"

	if [[ -e $path$file.in ]]
	then
		local name=${file//[[:digit:]]/}
		local num=${file//[^0-9]/}
		num=10#$num
		let num++
		num=$(printf "%0*d\n" 3 $num)

		sht $name$num
		
		for i in $path$file*
		do
			ext=${i##*.}
			mv $i $path$name$num.$ext
		done

	fi	
}

sht $newfile

touch $path$newfile.in
touch $path$newfile.out

# for file in $path*.in
# do
# 	base=${file##*/}
# 	base=${base%.*}
# 	if [ "$newfile" == "$base" ]
# 	then
# 		a=${newfile//[[:digit:]]/}
# 		b=${newfile//[^0-9]/}
# 		let b++
# 		b=$(printf "%0*d\n" 3 $b)
# 		for i in $path$newfile*
# 		do
# 			c=${i##*.}
# 			mv $i $path$a$b.$c
# 		done	
# 	fi


# 	# name=${file(%).*}
# 	# echo "$file"

# done

exit 0