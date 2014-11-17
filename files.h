#define FIILENAME_LEN 25

char filename_x[FIILENAME_LEN];
char filename_y[FIILENAME_LEN];
int values_n;

void read_file(char* p_filename, double* p_dest);
void init_arrays_from_files();
void read_arguments(int argc, char* argv[]);
void load_data(int argc, char* argv[]);