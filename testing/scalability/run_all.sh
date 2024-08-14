#!/bin/bash

instances=2

for ((n=1; n<=instances; n++))
do
    ./run_test.sh > "output_$n.txt" 2>&1 &
done

wait
echo "Finished all"
