#!/bin/bash
make clean FILE=calculate_pi.incomplete.c
make FILE=calculate_pi.incomplete.c
mpirun -np 3 ./calculate_pi.incomplete
