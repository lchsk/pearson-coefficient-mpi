#!/bin/sh

if [$1 -e ""]; then
	mpirun -n 4 ./pearson
else
	mpirun -n $1 ./pearson
fi
