#!/bin/bash

recordSize=$1
# sequetial read test
num_threads=(1 2 4 8 12 24 48)
for num_thread in ${num_threads[@]}
do
    filename="config${num_thread}.txt"
    ./diskbenchmark -r -S -b $recordSize -n $num_thread -f $filename
done
