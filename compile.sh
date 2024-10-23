#!/bin/bash

# compile
cd custom-scripts/webserver
../../output/host/bin/i686-buildroot-linux-gnu-cc server.c -O2 -o server
../../output/host/bin/i686-buildroot-linux-gnu-cc app.c -O2 -o app

# move compiled code to buildroot bin
cp server app ../../output/target/usr/bin/
