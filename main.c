#include "c_pandas.h"
#include <stdlib.h>

int main() {
    // Open file and store in pointer
    struct DataFrame *df = (struct DataFrame *)malloc(sizeof(struct DataFrame));

    read_csv("output.csv", df);

    to_csv(df, "test_output.csv");
    print_column_data(df, (char*[]){"Website"}, 1); 


}
