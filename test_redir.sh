#!/bin/bash
rm -f out err both

echo "=== Testing stdout > and >> ==="
echo hi > out
echo again >> out
cat out

echo "=== Testing numeric stdout 1> and 1>> ==="
echo hi 1> out
echo again 1>> out
cat out

echo "=== Testing numeric stderr 2> and 2>> ==="
ls missing_file 2> err
ls another_missing 2>> err
cat err

echo "=== Testing combined stdout+stderr &> and &>> ==="
echo hello &> both
ls missing_file &>> both
echo again &>> both
cat both

echo "=== Cleanup ==="
rm -f out err both

