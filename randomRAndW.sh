#!/bin/bash

recordSize=$1
totalFileSize=$2
num_threads=(1 2 4 8 12 24 48)

# generate small size file

echo "sequential write small file"
for num_thread in ${num_threads[@]}
do 
    filename="config${num_thread}_write.txt"
    echo $filename
    ./diskbenchmark -w -S -b $recordSize -n $num_thread -f $filename -s $totalFileSize
done

# sequetial write test
echo "random read test"
for num_thread in ${num_threads[@]}
do 
    filename="config${num_thread}_write.txt"
    echo $filename
    ./diskbenchmark -r -R -b $recordSize -n $num_thread -f $filename -s $totalFileSize
done

# random write test
echo "random write test"
for num_thread in ${num_threads[@]}
do 
    filename="config${num_thread}_write.txt"
    echo $filename
    ./diskbenchmark -w -R -b $recordSize -n $num_thread -f $filename -s $totalFileSize
done
