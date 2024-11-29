#!/bin/bash

# compile
cd custom-scripts/T3
../../output/host/bin/i686-buildroot-linux-gnu-cc sched_profiler.c -O2 -o sched

# move compiled code to buildroot bin
cp sched ../../output/target/usr/bin/
