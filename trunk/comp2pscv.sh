#!/bin/bash
##
g++ $1.cpp -o $1 -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc -lm `pkg-config opencv --cflags opencv --libs opencv`
##
## gcc %1.c -o %1 -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc
##
