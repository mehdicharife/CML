#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>

#include "datum.h"
#include "dtframe.h"
double PERTURBATION = 0.00001;


void set_randomly(double* arr, int arr_size) {
    for(int k = 0; k < arr_size; k++) {
        arr[k] = 0.5;
    }
}

double difference(double* arr1, double* arr2, int arr_size) {
    double difference = 0;

    for(int k = 0; k < arr_size; k++) {
        double term = arr2[k] - arr1[k];
        term = term*term;

        difference += term;
    }

    difference = powl(difference, 0.5);

    return difference;
}

datum* linear_function(datum** X, double* params, int params_count) {
    datum* pdatum = malloc(sizeof(datum));
    pdatum->type = ddouble;

    pdatum->x = weighted_sum(X, params, params_count - 1);
    pdatum->x += params[params_count - 1];

    return pdatum;
}   




struct model;
typedef struct model model;

struct model {
    double* params;
    int params_count;
    dtype target_type;
    datum* (*relation) (datum** X, double* params, int params_count);
    double (*loss_function) (model* pmodel, datum*** data, datum** labels, int* data_size);
};


model* clone_model(model* pmodel) {
    model* pclone = malloc(sizeof(model));
    pclone->relation = pmodel->relation;
    pclone->target_type = pmodel->target_type;
    pclone->loss_function = pmodel->loss_function;
    pclone->params_count = pmodel->params_count;
    pclone->params = malloc(pclone->params_count*sizeof(double));
    for(int k = 0; k < pclone->params_count; k++) {
        pclone->params[k] = pmodel->params[k];
    }

    return pclone;
}

double* clone_model_params(model* pmodel) {
    double* clones = malloc(pmodel->params_count*sizeof(double));
    for(int k = 0; k < pmodel->params_count; k++) {
        clones[k] = pmodel->params[k];
    }

    return clones;
}

// measures how bad the model did in predicting an expected value
double error(datum** X, datum* y, model* pmodel) {  
    return get_datum_as_double(y) - get_datum_as_double((pmodel->relation)(X, pmodel->params, pmodel->params_count));
}

// calculates the mean squared error made by a model over a given data set
double MSE(model* pmodel, datum*** data, datum** labels, int* data_size) {
    double mse = 0;

    for(int i = 0; i < data_size[0]; i++) {
        mse = mse + pow(error(data[i], labels[i], pmodel), 2);
    }    
    
    //printf("MSE: %lf\n", mse/data_size[0]);

    return mse/data_size[0];
}


// computes the gradient of (loss) function at a certain point (model/array of parameters)
// The first argument of the loss function (the model) is the one in respect to which the gradient 
// is calculated. The other arguments are treated as constants as far as the `grad` is concerned.
double* grad(double (*function) (model*, datum***, datum**, int*), datum*** data, datum** labels, int* data_size, model* pmodel) {
    double* gradient = malloc(pmodel->params_count*sizeof(double));

    for(int k = 0; k < pmodel->params_count; k++) {
        model* ptemp_model1 = clone_model(pmodel);
        model* ptemp_model2 = clone_model(pmodel);

        ptemp_model1->params[k] += PERTURBATION;
        ptemp_model2->params[k] -= PERTURBATION;
        gradient[k] = (function(ptemp_model1, data, labels, data_size) - function(ptemp_model2, data, labels, data_size))/(2*PERTURBATION);
        //printf("gradient: %lf\n", gradient[k]);
        printf("gradient: %lf\n", gradient[k]);
        free(ptemp_model1);
        free(ptemp_model2);
    }

    return gradient;
}


// starting from a given point (model) moves in a step that minimizes func
void walk(model* from, datum*** data, datum** labels, int* data_size, double (*func) (model*, datum***, datum**, int*), double step_size) {
    double* gradient = grad(func, data, labels, data_size, from);

    for(int k = 0; k < from->params_count; k++) {
        from->params[k] = from->params[k] - step_size*gradient[k];
        printf("step size times gradient comp: %lf\n", step_size*gradient[k]);
    }


    free(gradient);
}


// tries to fit the model to the (data, labels) by changing its parameters
void train(model* pmodel, datum*** data, datum** labels, int* data_size) {
    pmodel->params_count = data_size[1] + 1;
    pmodel->params = malloc((data_size[1] + 1)*sizeof(double));
    
    set_randomly(pmodel->params, pmodel->params_count);
    while(100) {
        double* old_params = clone_model_params(pmodel);
        walk(pmodel, data, labels, data_size, &MSE, 0.00001);
        printf("DIFF: %lf\n", difference(old_params, pmodel->params, pmodel->params_count));
        if(difference(old_params, pmodel->params, pmodel->params_count) < 0.00001) {
            break;
        }
        
        free(old_params);
        printf("\n\n");
        sleep(4);
    }
}

// computes the model's prediction corresponding to a certain input
datum* predict(model* pmodel, datum** X) {
    return (*pmodel->relation) (X, pmodel->params, pmodel->params_count);
} 


// create a linear regression model
model* new_lm_model() {
    model* pmodel = malloc(sizeof(model));
    pmodel->relation = linear_function;
    pmodel->loss_function = MSE;
    pmodel->target_type = ddouble;
    return pmodel;
}


int main(int argc, char* argv[]) {
    dtframe dtf;
    init_dtframe_from_csv(&dtf, 10, "BostonHousing.csv");
    set_dtframe_from_csv(&dtf, "BostonHousing.csv");

    int* data_size = malloc(2*sizeof(int));
    datum*** data = get_dtframe_cols(&dtf, data_size, 13, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
    datum** labels  = get_dtframe_col(&dtf, 14);


    datum ***train_data, ***test_data;
    datum **train_labels, **test_labels;
    float train_per = 0.9;
    train_test_split(data, labels, &train_data, &train_labels, &test_data, &test_labels, data_size, train_per);
    int train_data_size[2] = {(int) data_size[0]*train_per, data_size[1]};
    printf("training data size: %d x %d \n", train_data_size[0], train_data_size[1]);



    model* pmodel = new_lm_model();
    pmodel->params_count = data_size[1] + 1;
    pmodel->params = malloc(pmodel->params_count*sizeof(double));
    set_randomly(pmodel->params, pmodel->params_count);

    model* ptemp_model = clone_model(pmodel);
    ptemp_model->params[9] += PERTURBATION;

    //printf("%lf: %lf\n", pmodel->params[0], ptemp_model->params[0]);


    printf("%lf\n", 0.001*(MSE(ptemp_model, train_data, train_labels, train_data_size) - MSE(pmodel, train_data, train_labels, train_data_size))/PERTURBATION);

    //train(pmodel, train_data, train_labels, train_data_size);


    //print_datum(predict(pmodel, test_data[2]));
    //printf("\n");
    //print_datum(test_labels[2]);

    return 0;
}