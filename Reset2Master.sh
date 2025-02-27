#!/bin/bash
time git fetch
time git checkout -f master
time git reset --hard origin/master
time git submodule update --recursive --init --force

echo "Update helpers..."
cd deps/helpers && ./Reset2Master.sh
echo "Update helpers... done."


cd ../..

echo "Update CUL..."
cd deps/CUL && ./Reset2Master.sh
echo "Update CUL... done."

