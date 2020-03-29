#!/bin/bash

recordSize=$1
totalFileSize=
num_threads=(1 2 4 8 12 24 48)
# sequetial read test
echo "sequential read test"
for num_thread in ${num_threads[@]}
do
    filename="config${num_thread}.txt"
    ./diskbenchmark -r -S -b $recordSize -n $num_thread -f $filename
done


# random read test
echo "random read test"
for num_thread in ${num_threads[@]}
do 
    filename="config${numthread}.txt"
    #echo $filename
    ./diskbenchmark -r -R -b $recordSize -n num_thread -f $filename
done

# sequetial write test
echo "sequential write test"
for num_thread in ${num_threads[@]}
do 
    filename="config${numthread}_write.txt"
    echo $filename
    ./diskbenchmark -w -S -b $recordSize -n num_thread -f $filename -s 10240
done

# random write test
echo "random write test"
for num_thread in ${num_threads[@]}
do 
    filename="config${numthread}_write.txt"
    echo $filename
    ./diskbenchmark -w -R -b $recordSize -n num_thread -f $filename -s 10240
done