#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
#include "definitions.h"
#include "serial.h"
#include "parallel.h"
#include "files.h"

int 
main (int argc, char* argv[])
{   
    init_parallel(argc, argv);
    load_data(argc, argv);
    start_serial_pearson();

    double sum_a, sum_b;

    parallel_mean(input_data.a, &sum_a);
    parallel_mean(input_data.b, &sum_b);

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);

    broadcast_parallel_mean(sum_a, sum_b);

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);

    double nom_a, nom_b;
    parallel_stddev(input_data.a, &nom_a);
    parallel_stddev(input_data.b, &nom_b);

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);

    broadcast_parallel_stddev(nom_a, nom_b);

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);

    double pearson_nominator;
    parallel_pearson(&pearson_nominator);

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);
    
    finish_parallel_pearson(pearson_nominator);

    parallel.err = MPI_Finalize();

    return 0;
}