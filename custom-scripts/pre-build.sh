#!/bin/sh

cp $BASE_DIR/../custom-scripts/S41network-config $BASE_DIR/target/etc/init.d
chmod +x $BASE_DIR/target/etc/init.d/S41network-config

cp $BASE_DIR/../custom_scripts/trab1/S51trab-config $BASE_DIR/target/etc/init.d
chmod +x $BASE_DIR/target/etc/init.d/S51trab-config
