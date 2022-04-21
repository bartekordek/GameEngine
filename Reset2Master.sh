#!/bin/bash 
time git checkout -f master
time git reset --hard origin/master
time git submodule update --recursive --init
cd deps/helpers && ./Reset2Master.sh
cd ../SDL2Wrapper && ./Reset2Master.sh
