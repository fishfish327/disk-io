#!/bin/bash

recordSize=$1
totalFileSize=$2
num_threads=(1 2 4 8 12 24 48)
# sequetial read test
echo "sequential read test"
for num_thread in ${num_threads[@]}
do
    filename="config${num_thread}.txt"
    ./diskbenchmark -r -S -b $recordSize -n $num_thread -f $filename -s $totalFileSize
done


# random read test
echo "random read test"
for num_thread in ${num_threads[@]}
do 
    filename="config${num_thread}.txt"
    #echo $filename
    ./diskbenchmark -r -R -b $recordSize -n $num_thread -f $filename -s $totalFileSize
done
