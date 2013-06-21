#!/bin/bash
##
gcc $1.c -o $1 -I/usr/local/include/player-3.0 -L/usr/local/lib -lplayerc
##
## gcc %1.c -o %1 -I/usr/local/include/player-3.0 -L/usr/local/lib64 -lplayerc
##
