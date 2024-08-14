#!/bin/bash

# Loop from 1 to 10
for i in {1..100}
do
    filename="test${i}.xml"

    if [ -f "$filename" ]; then
        echo "Running client with $filename"
        ./client "$filename"
        echo "--------------------------------"
    #else
        #echo "$filename not found."
    fi
done
