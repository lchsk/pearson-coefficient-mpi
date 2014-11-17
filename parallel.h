

typedef struct 
parallel_t{
    int nodes;
    int rank;
    int err;
    int per_node;

    double mean_x, mean_y;
    double stddev_x, stddev_y;

    double* input_a;
    double* input_b;

    // time of parallel part start
    double start_time;

    // time of parallel part end
    double end_time;

    // serial part overall time
    double serial_time;

    // parallel part overall time
    double parallel_time;
} parallel_t;

parallel_t parallel;

double parallel_mean(double* p_array, double* return_value);
double parallel_stddev(double* p_array, double* return_value);
double parallel_pearson(double* return_value);
void init_parallel(int argc, char* argv[]);

void broadcast_parallel_mean(double sum_a, double sum_b);
void broadcast_parallel_stddev(double a, double b);
void finish_parallel_pearson(double pearson_nominator);