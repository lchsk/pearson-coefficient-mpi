#include "files.h"
#include "serial.h"
#include "parallel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
read_file(char* p_filename, double* p_dest)
{
    FILE* f;

    f = fopen(p_filename, "r");
    rewind(f);

    if (f == NULL)
    {
        printf("Cannot open file '%s'. Exiting...\n\n", p_filename);
        exit(0);
    }

    int i = 0;
    for (i = 0; i < values_n; i++)
    {
        if (fscanf(f, "%lf,", &p_dest[i]) <= 0)
            printf("Cannot read value.\n");
    }

    fclose(f);
}

void 
init_arrays_from_files()
{
    read_file(filename_x, input_data.a);
    read_file(filename_y, input_data.b);
}

void
read_arguments(int argc, char* argv[])
{
    int i;
    
    // flags
    int file_x = 0;
    int file_y = 0;
    int read_n = 0;

    for (i = 1; i < argc; ++i)
    {
        /* Check if arguments are presents */

        if (strcmp(argv[i], "-x") == 0)
        {
            file_x = 1;
            continue;
        }

        if (strcmp(argv[i], "-y") == 0)
        {
            file_y = 1;
            continue;
        }

        if (strcmp(argv[i], "-n") == 0)
        {
            read_n = 1;
            continue;
        }

        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            printf("Usage:\n");
            printf("\t./pearson\n\tto run algorithm with input data from source code\n\n");
            printf("\t./pearson -x input_file_X -y input_file_Y -n N\n\tto run algorithm with input data from files; set [N] argument to the number of values in input files\n\n");
            exit(0);
        }

        /* Interpret flags */

        if (file_x == 1)
        {
            strcpy(filename_x, argv[i]);
            file_x = 0;
        }

        if (file_y == 1)
        {
            strcpy(filename_y, argv[i]);
            file_y = 0;
        }

        if (read_n == 1)
        {
            values_n = atoi(argv[i]);
            read_n = 0;
        }
    }

    if (values_n == 0)
    {
        values_n = INPUT_LENGTH;
    }
}

void
load_data(int argc, char* argv[])
{
    read_arguments(argc, argv);

    if (strlen(filename_x) > 0 && strlen(filename_y) > 0)
    {
        //printf("\tInput data will be read from the files: %s and %s\n", filename_x, filename_y);
        init_arrays_from_files();
    }
    else
    {
        //printf("\tInput data sets will be initialized using function-generated numbers\n");
        init_arrays();
    }

    parallel.input_a = input_data.a;
    parallel.input_b = input_data.b;
}