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

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);
    parallel.start_time = MPI_Wtime();

    clock_t pstart, pend;

    pstart = clock();

    double sum_a, sum_b;

    double start = MPI_Wtime();

    if (USE_PARALLEL_INDICES)
    {
        parallel_mean2(input_data.a, &sum_a);
        parallel_mean2(input_data.b, &sum_b);
    }
    else
    {
        parallel_mean(input_data.a, &sum_a);
        parallel_mean(input_data.b, &sum_b);
    }
    


    parallel.err = MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();
    double d = (end * 1000) - (start * 1000);
    if (parallel.rank == ROOT_PROC)
        printf("\tParallel mean computed in %.*f ms\n", FLOAT_PRECISION, d);

    
    start = MPI_Wtime();

    if (USE_BROADCAST)
    {
        parallel.mean_x = sum_a / INPUT_LENGTH;
        parallel.mean_y = sum_b / INPUT_LENGTH;
        MPI_Bcast(&parallel.mean_x, 1, MPI_DOUBLE, ROOT_PROC, MPI_COMM_WORLD);
        MPI_Bcast(&parallel.mean_y, 1, MPI_DOUBLE, ROOT_PROC, MPI_COMM_WORLD);

        if (parallel.rank == ROOT_PROC)
        {
            printf("\tMean X: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, parallel.mean_x);
            printf("\tMean Y: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, parallel.mean_y);
        }
    }
    else
    {
        broadcast_parallel_mean(sum_a, sum_b);
    }

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    d = (end * 1000) - (start * 1000);
    if (parallel.rank == ROOT_PROC)
        printf("\tBroadcasting mean took %.*f ms\n", FLOAT_PRECISION, d);

    start = MPI_Wtime();
    double nom_a, nom_b;

    if (USE_PARALLEL_INDICES)
    {
        parallel_stddev2(input_data.a, &nom_a);
        parallel_stddev2(input_data.b, &nom_b);
    }
    else
    {
        parallel_stddev(input_data.a, &nom_a);
        parallel_stddev(input_data.b, &nom_b);
    }
    

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    d = (end * 1000) - (start * 1000);
    if (parallel.rank == ROOT_PROC)
        printf("\tParallel stddev computed in %.*f ms\n", FLOAT_PRECISION, d);


    start = MPI_Wtime();
    if (USE_BROADCAST)
    {
        parallel.stddev_x = sqrt(nom_a / INPUT_LENGTH);
        parallel.stddev_y = sqrt(nom_b / INPUT_LENGTH);

        MPI_Bcast(&parallel.stddev_x, 1, MPI_DOUBLE, ROOT_PROC, MPI_COMM_WORLD);
        MPI_Bcast(&parallel.stddev_y, 1, MPI_DOUBLE, ROOT_PROC, MPI_COMM_WORLD);

        if (parallel.rank == ROOT_PROC)
        {
            printf("\tStd dev X: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, parallel.stddev_x);
            printf("\tStd dev Y: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, parallel.stddev_y);
        }
    }
    else
    {
        broadcast_parallel_stddev(nom_a, nom_b);
    }

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    d = (end * 1000) - (start * 1000);
    if (parallel.rank == ROOT_PROC)
        printf("\tBroadcasting stddev took %.*f ms\n", FLOAT_PRECISION, d);


    start = MPI_Wtime();
    double pearson_nominator;
    parallel_pearson(&pearson_nominator);

    parallel.err = MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    d = (end * 1000) - (start * 1000);
    if (parallel.rank == ROOT_PROC)
        printf("\tParallel Pearson coefficient computed in %.*f ms\n", FLOAT_PRECISION, d);
    
    finish_parallel_pearson(pearson_nominator);

    pend = clock();

    double diff = (((double)pend * 1000 - (double)pstart * 1000) / CLOCKS_PER_SEC);
    
    parallel.err = MPI_Finalize();

    return 0;
}