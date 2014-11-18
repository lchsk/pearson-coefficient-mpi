#include "definitions.h"
#include "parallel.h"
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void
init_parallel(int argc, char* argv[])
{
    parallel.err = MPI_Init(&argc, &argv);
    parallel.err = MPI_Comm_size(MPI_COMM_WORLD, &parallel.nodes);
    parallel.err = MPI_Comm_rank(MPI_COMM_WORLD, &parallel.rank);

    if (parallel.rank == ROOT_PROC)
        printf("\tWorld size: %d\n", parallel.nodes);

    parallel.per_node = INPUT_LENGTH / parallel.nodes;
}

double 
parallel_mean(double* p_array, double* return_value)
{
    double* myray = (double*) malloc(parallel.per_node * sizeof(double));
    parallel.err = MPI_Scatter(p_array, parallel.per_node, MPI_DOUBLE, /* send buffer */
                            myray, parallel.per_node, MPI_DOUBLE, /* receive buffer */
                            ROOT_PROC, MPI_COMM_WORLD);

    double total = 0;
    int i;
    for (i = 0; i < parallel.per_node; i++)
    {
        total = total + myray[i];
    }

    parallel.err = MPI_Reduce(&total, return_value, 1, MPI_DOUBLE, MPI_SUM, ROOT_PROC, MPI_COMM_WORLD);
}

double 
parallel_mean2(double* p_array, double* return_value)
{
    double* buf = (double*) malloc(parallel.nodes * sizeof(double));
    double tmp[parallel.nodes];
    parallel.err = MPI_Scatter(tmp, 1, MPI_DOUBLE, /* send buffer */
                            buf, 1, MPI_DOUBLE, /* receive buffer */
                            ROOT_PROC, MPI_COMM_WORLD);

    double total = 0;
    int i;
    int idx = parallel.per_node * parallel.rank;
    for (i = 0; i < parallel.per_node; ++i, ++idx)
    {
        total = total + p_array[idx];
    }

    parallel.err = MPI_Reduce(&total, return_value, 1, MPI_DOUBLE, MPI_SUM, ROOT_PROC, MPI_COMM_WORLD);
}

double 
parallel_stddev(double* p_array, double* return_value)
{
    double* buf = (double*) malloc(parallel.per_node * sizeof(double));
    parallel.err = MPI_Scatter(p_array, parallel.per_node, MPI_DOUBLE, /* send buffer */
                            buf, parallel.per_node, MPI_DOUBLE, /* receive buffer */
                            ROOT_PROC, MPI_COMM_WORLD);

    double nominator = 0;

    int i = 0;
    for (i; i < parallel.per_node; ++i)
    {
        nominator += pow((buf[i] - parallel.mean_x), 2.0);
    }

    parallel.err = MPI_Reduce(&nominator, return_value, 1, MPI_DOUBLE, MPI_SUM, ROOT_PROC, MPI_COMM_WORLD);
}

double 
parallel_stddev2(double* p_array, double* return_value)
{
    double* buf = (double*) malloc(parallel.nodes * sizeof(double));
    double tmp[parallel.nodes];
    parallel.err = MPI_Scatter(tmp, 1, MPI_DOUBLE, /* send buffer */
                            buf, 1, MPI_DOUBLE, /* receive buffer */
                            ROOT_PROC, MPI_COMM_WORLD);

    double nominator = 0;
    int idx = parallel.per_node * parallel.rank;
    int i = 0;
    for (i; i < parallel.per_node; ++i, ++idx)
    {
        nominator += pow((p_array[idx] - parallel.mean_x), 2.0);
    }

    parallel.err = MPI_Reduce(&nominator, return_value, 1, MPI_DOUBLE, MPI_SUM, ROOT_PROC, MPI_COMM_WORLD);
}

double 
parallel_pearson(double* return_value)
{
    double* buf = (double*) malloc(parallel.nodes * sizeof(double));
    double tmp[parallel.nodes];
    parallel.err = MPI_Scatter(tmp, 1, MPI_DOUBLE, /* send buffer */
                            buf, 1, MPI_DOUBLE, /* receive buffer */
                            ROOT_PROC, MPI_COMM_WORLD);

    double nominator = 0;

    int i = 0;
    int idx = parallel.per_node * parallel.rank;
    for (i; i < parallel.per_node; ++i, ++idx)
    {
        nominator += ((parallel.input_a[idx] - parallel.mean_x) * (parallel.input_b[idx] - parallel.mean_y));
    }

    parallel.err = MPI_Reduce(&nominator, return_value, 1, MPI_DOUBLE, MPI_SUM, ROOT_PROC, MPI_COMM_WORLD);
}

void 
broadcast_parallel_mean(double sum_a, double sum_b)
{
    // Sending both means to other processes
    if (parallel.rank == ROOT_PROC)
    {
        parallel.mean_x = sum_a / INPUT_LENGTH;
        parallel.mean_y = sum_b / INPUT_LENGTH;

        printf("\tMean X: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, parallel.mean_x);
        printf("\tMean Y: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, parallel.mean_y);

        int p = 1;
        for (p; p < parallel.nodes; p++)
        {
            parallel.err = MPI_Send(&parallel.mean_x, 1, MPI_DOUBLE, p, TAG_SET_A, MPI_COMM_WORLD); 
            parallel.err = MPI_Send(&parallel.mean_y, 1, MPI_DOUBLE, p, TAG_SET_B, MPI_COMM_WORLD);
        }
    }
    else
    {
        double r;
        MPI_Status s;
        parallel.err = MPI_Recv(&r, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_SET_A, MPI_COMM_WORLD, &s);
        parallel.mean_x = r;

        parallel.err = MPI_Recv(&r, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_SET_B, MPI_COMM_WORLD, &s);
        parallel.mean_y = r; 
    }


}

void 
broadcast_parallel_stddev(double nom_a, double nom_b)
{
    if (parallel.rank == ROOT_PROC)
    {
        parallel.stddev_x = sqrt(nom_a / INPUT_LENGTH);
        parallel.stddev_y = sqrt(nom_b / INPUT_LENGTH);

        printf("\tStd dev X: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, parallel.stddev_x);
        printf("\tStd dev Y: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, parallel.stddev_y);

        int p = 1;
        for (p; p < parallel.nodes; p++)
        {
            parallel.err = MPI_Send(&parallel.stddev_x, 1, MPI_DOUBLE, p, TAG_SET_A, MPI_COMM_WORLD); 
            parallel.err = MPI_Send(&parallel.stddev_y, 1, MPI_DOUBLE, p, TAG_SET_B, MPI_COMM_WORLD);
        }
    }
    else
    {
        double r;
        MPI_Status s;
        parallel.err = MPI_Recv(&r, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_SET_A, MPI_COMM_WORLD, &s);
        parallel.stddev_x = r;

        parallel.err = MPI_Recv(&r, 1, MPI_DOUBLE, MPI_ANY_SOURCE, TAG_SET_B, MPI_COMM_WORLD, &s);
        parallel.stddev_y = r;
    }
}

void 
finish_parallel_pearson(double pearson_nominator)
{
    if (parallel.rank == ROOT_PROC)
    {
        double pearson = (pearson_nominator / INPUT_LENGTH) / (parallel.stddev_x * parallel.stddev_y);
        printf("\tPearson coefficient %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, pearson);
    
        parallel.end_time = MPI_Wtime();
        parallel.parallel_time = (parallel.end_time * 1000) - (parallel.start_time * 1000);
        printf("\tParallel algorithm finished in %.*f ms\n", FLOAT_PRECISION, 
            parallel.parallel_time);

        if (parallel.parallel_time > parallel.serial_time)
            printf("\n\tParallel version took %.*f times longer to finish\n\n", FLOAT_PRECISION, parallel.parallel_time / parallel.serial_time);
        else
            printf("\n\tSerial version took %.*f times longer to finish\n\n", FLOAT_PRECISION, parallel.serial_time / parallel.parallel_time);
    }
}