#!/bin/bash

# Clean up old logs
rm -f server.log

# Compile
make

echo "=== Starting server ==="
./server.exe > server.log 2>&1 &
SERVER_PID=$!

sleep 1

echo "=== Launching first 6 clients ==="
for i in {1..6}
do
    ./client.exe server.c &
done

echo "=== Try requesting non-existing file ==="
./client.exe notfound.c &

sleep 1

echo "=== Send SIGINT to server ==="
kill -INT "$SERVER_PID"

sleep 1

echo "=== Attempt launching remaining 3 clients ==="
for i in {8..10}
do
    ./client.exe client.c &
done

wait