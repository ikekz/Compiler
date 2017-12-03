#!/bin/bash 

newfile="$1"
path=$PWD/

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

exit 0