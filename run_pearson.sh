#!/bin/bash

if [ "$1" == "" ]
then
	mpirun -n 4 ./pearson
else
	mpirun -n $1 ./pearson
fi
