#!/bin/bash
time git fetch
time git checkout -f master
time git reset --hard origin/master
time git submodule update --recursive --init

echo "Update helpers..."
cd deps/helpers && ./Reset2Master.sh
echo "Update helpers... done."

echo "Update SDL2Wrapper..."
cd ../SDL2Wrapper && ./Reset2Master.sh
echo "Update SDL2Wrapper... done."
