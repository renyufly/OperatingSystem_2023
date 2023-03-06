#!/bin/bash
path=$1          #object path

if [ $# -eq 1 ];
then
    # Your code here. (1/4)
    grep -E 'WARN | ERROR' $path > bug.txt

else
    case $2 in
    "--latest")
        # Your code here. (2/4)
	tail -5 $path
    ;;
    "--find")
        # Your code here. (3/4)
	 grep -E $3 $path > $3.txt

    ;;
    "--diff")
        # Your code here. (4/4)
	diff $path $3 > /dev/null
	if [ $? -eq 0 ]
	then
		echo "same"
	else
		echo "different"
	fi	
    ;;
    esac
fi
