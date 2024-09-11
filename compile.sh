#!/bin/bash

# compiling files
#i686-linux-gcc ./custom-scripts/trab1/app.c -o ./output/target/usr/bin/app &&
#i686-linux-gcc ./custom-scripts/trab1/server.c -o ./output/target/usr/bin/server &&
i686-linux-gcc ./custom-scripts/parser.c -o ./output/target/usr/bin/parser &&

# creating rootfs
make
