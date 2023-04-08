#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>

#include "datum.h"
#include "dtframe.h"
float PERTURBATION = 1.0e-7;


void set_randomly(float* arr, int arr_size) {
    for(int k = 0; k < arr_size; k++) {
        arr[k] = 0.01;
    }
}

float difference(float* arr1, float* arr2, int arr_size) {
    float difference = 0;

    for(int k = 0; k < arr_size; k++) {
        float term = arr2[k] - arr1[k];
        term = term*term;

        difference += term;
    }

    difference = powl(difference, 0.5);

    return difference;
}

datum* linear_function(datum** X, float* params, int params_count) {
    datum* pdatum = malloc(sizeof(datum));
    pdatum->type = dfloat;

    pdatum->x = weighted_sum(X, params, params_count - 1);
    pdatum->x += params[params_count - 1];

    return pdatum;
}   




struct model;
typedef struct model model;

struct model {
    float* params;
    int params_count;
    dtype target_type;
    datum* (*relation) (datum** X, float* params, int params_count);
    float (*loss_function) (model* pmodel, datum*** data, datum** labels, int* data_size);
};

datum* predict(model* pmodel, datum** X);

model* clone_model(model* pmodel) {
    model* pclone = malloc(sizeof(model));
    pclone->relation = pmodel->relation;
    pclone->target_type = pmodel->target_type;
    pclone->loss_function = pmodel->loss_function;
    pclone->params_count = pmodel->params_count;
    pclone->params = malloc(pclone->params_count*sizeof(float));
    for(int k = 0; k < pclone->params_count; k++) {
        pclone->params[k] = pmodel->params[k];
    }

    return pclone;
}

float* clone_model_params(model* pmodel) {
    float* clones = malloc(pmodel->params_count*sizeof(float));
    for(int k = 0; k < pmodel->params_count; k++) {
        clones[k] = pmodel->params[k];
    }

    return clones;
}

// measures how bad the model did in predicting an expected value
float error(datum** X, datum* y, model* pmodel) {  
    return get_datum_as_float(y) - get_datum_as_float((pmodel->relation)(X, pmodel->params, pmodel->params_count));
}

// calculates the mean squared error made by a model over a given data set
float MSE(model* pmodel, datum*** data, datum** labels, int* data_size) {
    float mse = 0;

    for(int i = 0; i < data_size[0]; i++) {
        mse = mse + pow(error(data[i], labels[i], pmodel), 2);
    }    
    
    //printf("MSE: %lf\n", mse/data_size[0]);

    return mse/data_size[0];
}

float MSE_tr(model* pmodel, int k, int sign, datum*** data, datum** labels, int* data_size) {
    pmodel->params[k] += sign*PERTURBATION;

    float mse = MSE(pmodel, data, labels, data_size);

    pmodel->params[k] -= sign*PERTURBATION;

    return mse;
}



// computes the gradient of (loss) function at a certain point (model/array of parameters)
// The first argument of the loss function (the model) is the one in respect to which the gradient 
// is calculated. The other arguments are treated as constants as far as the `grad` is concerned.
float* grad(float (*function) (model*,int, int, datum***, datum**, int*), datum*** data, datum** labels, int* data_size, model* pmodel) {
    float* gradient = malloc(pmodel->params_count*sizeof(float));        

    for(int k = 0; k < pmodel->params_count; k++) {
        gradient[k] = (function(pmodel, k, 1, data, labels, data_size) - function(pmodel, k, -1, data, labels, data_size))/(2*PERTURBATION);
        //printf("gradient: %lf\n", gradient[k]);
    }

    return gradient;
}



// starting from a given point (model) moves in a step that minimizes func
void walk(model* from, datum*** data, datum** labels, int* data_size, float (*func) (model*, int, int, datum***, datum**, int*), float step_size) {
    float* gradient = grad(func, data, labels, data_size, from);

    for(int k = 0; k < from->params_count; k++) {
        from->params[k] = from->params[k] - step_size*gradient[k];
        //printf("step size times gradient comp: %lf\n", step_size*gradient[k]);
    }


    free(gradient);
}


// tries to fit the model to the (data, labels) by changing its parameters
void train(model* pmodel, datum*** data, datum** labels, int* data_size, datum*** t_data) {
    pmodel->params_count = data_size[1] + 1;
    pmodel->params = malloc((data_size[1] + 1)*sizeof(float));
    
    set_randomly(pmodel->params, pmodel->params_count);
    while(1) {
        float* old_params = clone_model_params(pmodel);
        walk(pmodel, data, labels, data_size, &MSE_tr, 3.2e-6);
        if(difference(old_params, pmodel->params, pmodel->params_count) < 0.000001) {
            break;
        }
        
        free(old_params);
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
    pmodel->target_type = dfloat;
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
    float train_per = 0.35;
    train_test_split(data, labels, &train_data, &train_labels, &test_data, &test_labels, data_size, train_per);
    int train_data_size[2] = {(int) data_size[0]*train_per, data_size[1]};
    printf("training data size: %d x %d \n", train_data_size[0], train_data_size[1]);

    printf("dtframe size: %d x %d\n", dtf.vert_capacity, dtf.hor_capacity);

    model* pmodel = new_lm_model();
    pmodel->params_count = data_size[1] + 1;
    pmodel->params = malloc(pmodel->params_count*sizeof(float));
    set_randomly(pmodel->params, pmodel->params_count);

    model* ptemp_model = clone_model(pmodel);
    ptemp_model->params[9] += PERTURBATION;

    //printf("%lf: %lf\n", pmodel->params[0], ptemp_model->params[0]);


    //printf("%lf\n", 0.001*(MSE(ptemp_model, train_data, train_labels, train_data_size) - MSE(pmodel, train_data, train_labels, train_data_size))/PERTURBATION);

    train(pmodel, train_data, train_labels, train_data_size, test_data);

    //printf("MSE: %lf\n", MSE(pmodel, train_data, train_labels, train_data_size));

    for(int k = 0; k < train_data_size[0]; k++) {
        print_datum(predict(pmodel, train_data[k]));
        printf(" ");
        print_datum(train_labels[k]);
        printf("\n");
    }


    return 0;
}