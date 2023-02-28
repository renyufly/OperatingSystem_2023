#!/bin/bash
filename=$1
newfile=$2

touch $newfile

sed -n '8p; 32p; 128p; 512p; 1024p' $filename > $newfile

