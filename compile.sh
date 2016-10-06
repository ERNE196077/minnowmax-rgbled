#!/bin/bash

echo '#############################'
echo '#     gcc routine           #'
echo '#############################'
echo '#     main.c           #'
gcc -Wall -g  -c /home/minnow/workspace/minnow-ws281x/main.c -o obj/Debug/main.o
echo '#     rgbled.c           #'
gcc -Wall -g  -c /home/minnow/workspace/minnow-ws281x/rgbled.c -o obj/Debug/rgbled.o


echo '#############################'
echo '#     g++ routine           #'
echo '#############################'

g++  -o bin/Debug/rgbled obj/Debug/main.o obj/Debug/rgbled.o
