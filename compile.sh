#!/bin/bash

echo '#############################'
echo '#     gcc routine           #'
echo '#############################'
echo '#     main.c           #'
gcc -Wall -g  -c main.c -o obj/Debug/main.o
echo '#     rgbled.c           #'
gcc -Wall -g  -c rgbled.c -o obj/Debug/rgbled.o

echo '#     rgbled_x11.c           #'
gcc -Wall -g  -c rgbled_x11.c -o obj/Debug/rgbled_x11.o
echo '#############################'
echo '#     g++ routine           #'
echo '#############################'

g++  -o bin/Debug/rgbled obj/Debug/main.o obj/Debug/rgbled.o obj/Debug/rgbled_x11.o -lX11 -lXext
