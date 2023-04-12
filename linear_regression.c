#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

#include "model.h"
#include "lmodel.h"



int main(int argc, char* argv[]) {
    dtframe* pdtframe = malloc(sizeof(dtframe));
    init_dtframe_from_csv(pdtframe, 10, "BostonHousing.csv");
    set_dtframe_from_csv(pdtframe, "BostonHousing.csv");

    int* data_size = malloc(2*sizeof(int));
    datum*** data = get_dtframe_cols(pdtframe, data_size, 4, 1, 6, 11, 13);
    datum** labels  = get_dtframe_col(pdtframe, 14);


    datum ***train_data, ***test_data;
    datum **train_labels, **test_labels;
    double train_per = 0.8;
    train_test_split(data, labels, &train_data, &train_labels, &test_data, &test_labels, data_size, train_per);
    int train_data_size[2] = {(int) data_size[0]*train_per, data_size[1]};
    int test_data_size[2] = {(int) data_size[0] - data_size[0]*train_per, data_size[1]};
    printf("training data size: %d x %d \n", train_data_size[0], train_data_size[1]);

    printf("dtframe size: %d x %d\n", pdtframe->vert_capacity, pdtframe->hor_capacity);

    model* pmodel = new_lmodel();
    train(pmodel, train_data, train_labels, train_data_size);


    // print pairs of predicted and expected values in the data set
    for(int k = 0; k < data_size[0] - train_data_size[0]; k++) {
        print_datum(predict(pmodel, test_data[k]));
        printf(" ");
        print_datum(test_labels[k]);
        printf("\n");
    }


    printf("MSE on test data set: %f\n", MSE(pmodel, test_data, test_labels, test_data_size));

    free(pmodel);
    free(data_size);

    destroy_dtframe(pdtframe);


    return 0;
}