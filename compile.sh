#!/bin/bash

echo '#############################'
echo '#     gcc routine           #'
echo '#############################'
gcc -Wall -g  -c /home/minnow/workspace/minnow-ws281x/main.c -o obj/Debug/main.o


echo '#############################'
echo '#     g++ routine           #'
echo '#############################'

g++  -o bin/Debug/minnow-ws281x obj/Debug/main.o obj/Debug/ws281x.o
