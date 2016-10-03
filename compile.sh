#!/bin/bash

echo '#############################'
echo '#     gcc routine           #'
echo '#############################'
echo '#     main.c           #'
gcc -Wall -g  -c /home/minnow/workspace/minnow-ws281x/main.c -o obj/Debug/main.o
echo '#     ws281x.c           #'
gcc -Wall -g  -c /home/minnow/workspace/minnow-ws281x/ws281x.c -o obj/Debug/ws281x.o


echo '#############################'
echo '#     g++ routine           #'
echo '#############################'

g++  -o bin/Debug/minnow-ws281x obj/Debug/main.o obj/Debug/pwm.o obj/Debug/spi.o obj/Debug/ws281x.o
