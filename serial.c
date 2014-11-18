#include "serial.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "definitions.h"
#include "parallel.h"

void 
init_arrays()
{
    int i;
    for (i = 0; i < values_n; ++i)
    {
        input_data.a[i] = sin(i);
        input_data.b[i] = sin(i + 1);
    }
}

double 
compute_mean(double* p_array, int p_len)
{
    clock_t start, end;

    start = clock();
    double sum = 0;

    int i = 0;
    for (i; i < p_len; ++i)
    {
        sum = sum + p_array[i];
    }

    end = clock();

    double diff = (((double)end - (double)start) / CLOCKS_PER_SEC) * 1000;

    printf("\tSerial mean computed in %*.*f ms\n\n", RESULT_LENGTH, FLOAT_PRECISION, diff);


    return sum / p_len;
}

double 
compute_std_dev(double* p_array, double p_mean, int p_len)
{
    clock_t start, end;

    start = clock();
    double nominator = 0;

    int i = 0;
    for (i; i < p_len; ++i)
    {
        nominator += pow((p_array[i] - p_mean), 2.0);

    }

    end = clock();

    double diff = (((double)end - (double)start) / CLOCKS_PER_SEC) * 1000;

    printf("\tSerial stddev computed in %*.*f ms\n\n", RESULT_LENGTH, FLOAT_PRECISION, diff);


    return sqrt(nominator / p_len);
}

double 
compute_pearson(
    double* p_array_a, 
    double* p_array_b, 
    double p_mean_a, 
    double p_mean_b, 
    double p_std_dev_a, 
    double p_std_dev_b, 
    int p_len
    )
{
    clock_t start, end;

    start = clock();

    int i = 0;
    double sum = 0;
    for (i; i < p_len; ++i)
    {
        sum += ((p_array_a[i] - p_mean_a) * (p_array_b[i] - p_mean_b));
    }

    end = clock();

    double diff = (((double)end - (double)start) / CLOCKS_PER_SEC) * 1000;

    printf("\tSerial pearson coefficient computed in %*.*f ms\n\n", RESULT_LENGTH, FLOAT_PRECISION, diff);


    return (sum / p_len) / (p_std_dev_a * p_std_dev_b);
}

double 
run_serial_pearson()
{
    printf("\tSerial algorithm now running...\n");
    printf("\tNumber of values: %d\n", values_n);

    clock_t start, end;

    start = clock();

    double mean_a = compute_mean(input_data.a, values_n);
    double mean_b = compute_mean(input_data.b, values_n);

    double stddev_a = compute_std_dev(input_data.a, mean_a, values_n);
    double stddev_b = compute_std_dev(input_data.b, mean_b, values_n);

    double pearson = compute_pearson(input_data.a, input_data.b, mean_a, mean_b, stddev_a, stddev_b, values_n);

    printf("\tMean X: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, mean_a);
    printf("\tMean Y: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, mean_b);

    printf("\tStd dev X: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, stddev_a);
    printf("\tStd dev Y: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, stddev_b);

    printf("\tPearson coefficient: %*.*f\n", RESULT_LENGTH, FLOAT_PRECISION, pearson);

    end = clock();

    double diff = (((double)end - (double)start) / CLOCKS_PER_SEC) * 1000;

    printf("\tSerial algorithm finished in %*.*f ms\n\n", RESULT_LENGTH, FLOAT_PRECISION, diff);

    return diff;
}

void 
start_serial_pearson()
{
    if (parallel.rank == ROOT_PROC)
    {   
        parallel.serial_time = run_serial_pearson();

        printf("\tParallel algorithm now running...\n");
    }
}