#!/bin/bash

if [ "$1" == "" ]
then
	mpirun -n 2 ./pearson
else
	mpirun -n $1 ./pearson
fi
