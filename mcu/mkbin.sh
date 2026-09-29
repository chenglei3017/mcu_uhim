#!/bin/bash

# This script is for ST MCU make image

if [ $# != 3 ]
then
	echo "./mkbin.sh [path]/boot.bin [path]/app.bin [path]/factory.bin"
	exit 0
fi

echo "mkbin.sh $1 $2 $3"
boot=$1
app=$2
all=$3

tr '\000' '\377' < /dev/zero | dd of=$all bs=16k count=1
cat $boot > ff.bin
cat $all >> ff.bin

dd if=ff.bin of=$all bs=16k count=1
rm -rf ff.bin

cat $app >> $all
