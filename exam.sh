#!/bin/bash
n=$1

mkdir mydir
chmod a+rwx mydir
touch myfile
echo "2023" > myfile
mv moveme mydir
cp copyme mydir
mv ./mydir/copyme ./mydir/copied
cat readme
gcc bad.c 2>> err.txt

mkdir gen
i=1
if [ $# -eq 0 ]
then
	while [ $i -le 10 ] 
	do
		touch ./gen/$i.txt
		let i=i+1
	done
else
	while [ $i -le $n ]
	do
		touch ./gen/$i.txt
		let i=i+1
	done
fi
	
