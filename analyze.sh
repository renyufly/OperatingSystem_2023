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

    ;;
    "--find")
        # Your code here. (3/4)

    ;;
    "--diff")
        # Your code here. (4/4)

    ;;
    esac
fi
