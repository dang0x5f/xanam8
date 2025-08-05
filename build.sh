#!/bin/sh
cc -o prog --std=c2x -I/usr/local/include -D_THREAD_SAFE -L/usr/local/lib -lX11 main.c
