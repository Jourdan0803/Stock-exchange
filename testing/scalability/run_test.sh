#!/bin/bash

fileNum=30
maxThread=50
# Loop over different CPU core configurations
for cores in 1 2 4 8
do
    start_time=$(date +%s.%N)
    echo "Running clients with $cores cores"
    total_requests=0

    for ((j=1; j<=maxThread; j++))
    do
        # Loop from 1 to 100 for testing
        for ((i=1; i<=fileNum; i++))
        do
            filename="test${i}.xml"

            if [ -f "$filename" ]; then
                # Run the client with CPU affinity
                ./client "$filename" &
                client_pid=$!
                taskset -cpa 0-$(($cores-1)) $client_pid > /dev/null
                wait $client_pid
                ((total_requests++))
            fi
        done
    done

    # Record the end time after receiving the response
    end_time=$(date +%s.%N)
    execution_time=$(echo "$end_time - $start_time" | bc)

    # Calculate throughput (requests per second)
    throughput=$(echo "scale=2; $total_requests / $execution_time" | bc)

    echo "Execution time: $execution_time seconds"
    echo "Total Requests: $total_requests"
    echo "Throughput: $throughput requests/second"
    echo "--------------------------------"
done
