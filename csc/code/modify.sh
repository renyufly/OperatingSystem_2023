#!/bin/bash

file=$1
oldstr=$2
newstr=$3

sed -i "s/$oldstr/$newstr/g" $file
