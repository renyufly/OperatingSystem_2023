#!/bin/bash
n=$1

mkdir mydir
chmod a+rwx mydir
touch myfile
echo "2023" > myfile
mv moveme mydir
cp copyme mydir
mv ~/21371283/mydir/copyme ~/21371283/mydir/copied
cat readme
gcc bad.c 2>> err.txt

mkdir gen
i=1
if [ $# -eq 0 ]
then
	while [ $i -le 10 ] 
	do
		touch ~/21371283/gen/$i.txt
		let i=i+1
	done
else
	while [ $i -le $n ]
	do
		touch ~/21371283/gen/$i.txt
		let i=i+1
	done
fi
	
