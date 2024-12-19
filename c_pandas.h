#pragma once
#include <stdio.h>

struct DataFrame {
    int num_rows;
    int num_cols;
    char *filepath;
    FILE *file;
    char **headers;
    char ***rows;
};


void get_headers(struct DataFrame *df); 
void get_rows(struct DataFrame *df); 
void print_column_data(struct DataFrame *df, char* columns[], int size); 
void read_csv(char *filepath, struct DataFrame *df); 
void print_columns(struct DataFrame *df); 
void to_csv(struct DataFrame *df, char* output_file); 
struct DataFrame* sub_df(struct DataFrame *df, char* columns[], int size);
void print_df(struct DataFrame *df); 
