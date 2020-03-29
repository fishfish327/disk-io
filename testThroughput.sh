#!/bin/sh

recordSize=$1
# sequetial read test
num_threads=(1 2 4 8 12 24 48)
for num_thread in {arrayIndex[@]}
do 
    filename="config${numthread}.txt"
    echo $filename
    ./diskbenchmark -r -S -b $recordSize -n num_thread -f $filename
done
