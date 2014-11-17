#include "definitions.h"

extern int values_n;

typedef struct 
input_data_t{
    double a[INPUT_LENGTH];
    double b[INPUT_LENGTH];
} input_data_t;

input_data_t input_data;

double compute_mean(double* p_array, int p_len);
double compute_std_dev(double* p_array, double p_mean, int p_len);
double compute_pearson(
    double* p_array_a, 
    double* p_array_b, 
    double p_mean_a, 
    double p_mean_b, 
    double p_std_dev_a, 
    double p_std_dev_b, 
    int p_len
);
double run_serial_pearson();
void start_serial_pearson();