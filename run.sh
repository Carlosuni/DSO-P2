#!/bin/bash
make clean
rm disk.dat
make
./create_disk 200
./test