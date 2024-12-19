#include "c_pandas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_df(struct DataFrame *df) {
    for (int i = 0; i < df->num_cols - 1; i++) {
        printf("%s  ", df->headers[i]);
    }
    printf("%s\n", df->headers[df->num_cols - 1]);

    for (int i = 0; i < df->num_rows; i++) {
        for (int j = 0; j < df->num_cols; j++) {
            printf("%s  ", df->rows[i][j]);
        }
        printf("\n");
    }
}

struct DataFrame *sub_df(struct DataFrame *df, char *columns[], int size) {
    struct DataFrame *new_df =
        (struct DataFrame *)malloc(sizeof(struct DataFrame));

    new_df->num_cols = size;
    new_df->num_rows = df->num_rows;
    new_df->headers = columns;
    int column_indexes[size];

    new_df->rows = (char ***)malloc(new_df->num_rows * sizeof(char **));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < df->num_cols; j++) {
            if (strcmp(columns[i], df->headers[j]) == 0) {
                column_indexes[i] = j;
            }
        }
    }

    for (int i = 0; i < new_df->num_rows; i++) {
        new_df->rows[i] = (char **)malloc(size * sizeof(char *));
        for (int j = 0; j < size; j++) {
            new_df->rows[i][j] = df->rows[i][column_indexes[j]];
        }
    }

    return new_df;
}
void to_csv(struct DataFrame *df, char *output_file) {
    FILE *file = fopen(output_file, "w");

    for (int i = 0; i < df->num_cols; i++) {
        char *row_data = df->headers[i];
        while (*row_data) {
            if (*row_data == '\r' || *row_data == '!' || *row_data == '\n') {
                *row_data = '\0'; // Remove carriage return
            }
            row_data++;
        }
        if (i != df->num_cols - 1) {
            fprintf(file, "%s,", df->headers[i]);
        } else {
            fprintf(file, "%s\n", df->headers[i]);
        }
    }

    for (int i = 0; i < df->num_rows; i++) {
        for (int j = 0; j < df->num_cols; j++) {
            char *row_data = df->rows[i][j];
            while (*row_data) {
                if (*row_data == '\r' || *row_data == '!' ||
                    *row_data == '\n') {
                    *row_data = '\0';
                }
                row_data++;
            }
            if (j != df->num_cols - 1) {
                fprintf(file, "%s,", df->rows[i][j]);
            } else {
                fprintf(file, "%s\n", df->rows[i][j]);
            }
        }
    }

    fclose(file);
}

void get_headers(struct DataFrame *df) {
    fopen(df->filepath, "r");
    char buffer[256];
    int word_length;
    int start_idx = 0;
    int current_column = 0;

    df->headers = (char **)malloc(df->num_cols * sizeof(char *));

    while (fgets(buffer, sizeof(buffer), df->file) != NULL) {
        for (int i = 0; i < strlen(buffer); i++) {
            if (buffer[i] == ',' || buffer[i] == '\n') {
                word_length = i - start_idx;

                df->headers[current_column] =
                    (char *)malloc(word_length * sizeof(char));
                strncpy(df->headers[current_column], buffer + start_idx,
                        word_length);
                start_idx = i + 1;
                current_column++;
            }
        }
        break;
    }
    fclose(df->file);
}

void get_rows(struct DataFrame *df) {
    df->file = fopen(df->filepath, "r");
    if (df->file == NULL) {
        perror("File open failed");
        return;
    }

    char buffer[256];
    int current_row = 0;

    int header_row = 1; // Skip header row
    df->rows = (char ***)malloc(df->num_rows * sizeof(char **));

    while (fgets(buffer, sizeof(buffer), df->file) != NULL) {
        if (!header_row) {
            int start_idx = 0;
            int current_column = 0;
            int in_string = 0;

            df->rows[current_row] =
                (char **)malloc(df->num_cols * sizeof(char *));
            for (int i = 0; i <= strlen(buffer); i++) {
                // Toggle in_string when encountering quotes
                if (buffer[i] == '"') {
                    in_string = !in_string;
                    continue; // Skip the quote itself
                }

                // End of field (if not inside quotes)
                if ((buffer[i] == ',' || buffer[i] == '\n' ||
                     buffer[i] == '\0') &&
                    !in_string) {
                    int word_length = i - start_idx;
                    df->rows[current_row][current_column] =
                        (char *)malloc((word_length + 1) * sizeof(char));
                    strncpy(df->rows[current_row][current_column],
                            buffer + start_idx, word_length);
                    df->rows[current_row][current_column][word_length] =
                        '\0'; // Null-terminate the string

                    current_column++;
                    start_idx = i + 1; // Move start index past the delimiter
                }
            }
            current_row++;
        } else {
            header_row--; // Skip the first row (header)
        }
    }
    fclose(df->file);
}

void print_column_data(struct DataFrame *df, char *columns[], int size) {
    int column_idxs[size];

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < df->num_cols; j++) {
            printf("Column: %s\n Check: %s\n", columns[i], df->headers[j]);
            if (strcmp(columns[i], df->headers[j]) == 0) {
                column_idxs[i] = j;
                break;
            }
        }
    }

    for (int i = 0; i < size - 1; i++) {
        printf("%s  ", df->headers[column_idxs[i]]);
    }
    printf("%s\n", df->headers[column_idxs[size - 1]]);

    for (int i = 0; i < df->num_rows; i++) {
        for (int j = 0; j < size; j++) {
            printf("%s  ", df->rows[i][column_idxs[j]]);
        }
        printf("\n");
    }
}

void print_columns(struct DataFrame *df) {
    printf("Columns:\n");
    for (int i = 0; i < df->num_cols; i++) {
        printf("%s", df->headers[i]);
        if (i != df->num_cols - 1) {
            printf(",");
        }
    }
    printf("\n");
}

void read_csv(char *filepath, struct DataFrame *df) {
    // Defining a buffer to store lines of the file
    df->filepath = filepath;
    df->file = fopen(df->filepath, "r");

    char buffer[256];
    int current_row = 0;

    df->num_cols = 0;
    df->num_rows = 0;

    while (fgets(buffer, sizeof(buffer), df->file) != NULL) {
        if (current_row != 0) {
            df->num_rows++;
        } else {
            for (int i = 0; i < strlen(buffer); i++) {
                if (buffer[i] == ',' || buffer[i] == '\n') {
                    df->num_cols++;
                }
            }
        }
        current_row++;
    }
    fclose(df->file);
    get_headers(df);
    get_rows(df);
}
