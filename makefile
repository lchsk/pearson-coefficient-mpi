all: pearson

pearson: main.o serial.o files.o parallel.o
	mpicc main.o serial.o files.o parallel.o -o pearson -lm

main.o: main.c
	mpicc main.c -lm -c

serial.o: serial.c
	mpicc serial.c -lm -c

parallel.o: parallel.c
	mpicc parallel.c -lm -c

files.o: files.c
	mpicc files.c -c

clean:
	rm -rf *o pearson