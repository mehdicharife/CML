#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>

#include "datum.h"
#include "dtframe.h"
double PERTURBATION = 1.0e-7;


void set_randomly(double* arr, int arr_size) {
    for(int k = 0; k < arr_size; k++) {
        arr[k] = 0.01;
    }
}

double difference(double* arr1, double* arr2, int arr_size) {
    double difference = 0;

    for(int k = 0; k < arr_size; k++) {
        double term = arr2[k] - arr1[k];
        term = pow(term, 2);

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


double part_der(datum* (function) (datum**, double*, int), datum** row, int row_size, double* vars, int var_index) {
    double term1, term2;
    datum *dterm1, *dterm2;

    vars[var_index] += PERTURBATION;
    dterm1 = function(row, vars, row_size);
    term1 = get_datum_as_double(dterm1);
    
    vars[var_index] -= 2*PERTURBATION;
    dterm2 = function(row, vars, row_size);
    term2 = get_datum_as_double(dterm2);

    vars[var_index] += PERTURBATION;
    free(dterm1);
    free(dterm2);

    return (term1-term2)/(2*PERTURBATION);
}



struct model;
typedef struct model model;

struct model {
    double* params;
    int params_count;
    dtype target_type;
    datum* (*relation) (datum** X, double* params, int params_count);
    double* (*d_loss_func) (model* pmodel, datum*** data, datum** labels, int* data_size);
};

datum* predict(model* pmodel, datum** X);

model* clone_model(model* pmodel) {
    model* pclone = malloc(sizeof(model));
    pclone->relation = pmodel->relation;
    pclone->target_type = pmodel->target_type;
    pclone->d_loss_func = pmodel->d_loss_func;
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
    datum* dpred = (pmodel->relation)(X, pmodel->params, pmodel->params_count);
    double pred = get_datum_as_double(dpred);
    free(dpred);
    return get_datum_as_double(y) - pred;
}


double* dMSE(model* pmodel, datum*** data, datum** labels, int* data_size) {
    double* grad = malloc(pmodel->params_count*sizeof(double));

    double errors[data_size[0]]; 
    for(int k = 0; k < data_size[0]; k++) {
        datum* prediction = (pmodel->relation)(data[k], pmodel->params, pmodel->params_count);
        errors[k] = get_datum_as_double(labels[k]) - get_datum_as_double(prediction);
        free(prediction);
    }

    for(int j = 0; j < data_size[1] + 1; j++) {
        grad[j] = 0;
        for(int i = 0; i < data_size[0]; i++) {            
            grad[j] -= 2*errors[i]*part_der(pmodel->relation, data[i], pmodel->params_count, pmodel->params, j); 
        }
        grad[j] = grad[j]/data_size[0];
    }

    
    return grad;
}

// calculates the mean squared error made by a model over a given data set
double MSE(model* pmodel, datum*** data, datum** labels, int* data_size) {
    double mse = 0;

    for(int i = 0; i < data_size[0]; i++) {
        mse = mse + pow(error(data[i], labels[i], pmodel), 2);
    }    
    
    return mse/data_size[0];
}

/*double MSE_tr(model* pmodel, int k, int sign, datum*** data, datum** labels, int* data_size) {
    pmodel->params[k] += sign*PERTURBATION;

    double mse = MSE(pmodel, data, labels, data_size);

    pmodel->params[k] -= sign*PERTURBATION;

    return mse;
}*/



// computes the gradient of (loss) function at a certain point (model/array of parameters)
// The first argument of the loss function (the model) is the one in respect to which the gradient 
// is calculated. The other arguments are treated as constants as far as the `grad` is concerned.
/*double* grad(double (*function) (model*,int, int, datum***, datum**, int*), datum*** data, datum** labels, int* data_size, model* pmodel) {
    double* gradient = malloc(pmodel->params_count*sizeof(double));        

    for(int k = 0; k < pmodel->params_count; k++) {
        gradient[k] = (function(pmodel, k, 1, data, labels, data_size) - function(pmodel, k, -1, data, labels, data_size))/(2*PERTURBATION);
    }

    return gradient;
}*/



// starting from a given point (model) moves in a step that minimizes the loss function associated with the model
void walk(model* from, datum*** data, datum** labels, int* data_size, double step_size) {
    double* gradient = (from->d_loss_func)(from, data, labels, data_size);

    for(int k = 0; k < from->params_count; k++) {
        from->params[k] = from->params[k] - step_size*gradient[k];
    }

    free(gradient);
}


// tries to fit the model to the (data, labels) by changing its parameters
void train(model* pmodel, datum*** data, datum** labels, int* data_size) {
    pmodel->params_count = data_size[1] + 1;
    pmodel->params = malloc((data_size[1] + 1)*sizeof(double));
    
    set_randomly(pmodel->params, pmodel->params_count);
    while(1) {
        double* old_params = clone_model_params(pmodel);
        walk(pmodel, data, labels, data_size, 3.3e-6);
        printf("MSE: %f\n", MSE(pmodel, data, labels, data_size));
        if(difference(old_params, pmodel->params, pmodel->params_count) < 0.00001) {
            free(old_params);
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
    pmodel->d_loss_func = dMSE;
    pmodel->target_type = ddouble;
    return pmodel;
}


int main(int argc, char* argv[]) {
    dtframe* pdtframe = malloc(sizeof(dtframe));
    init_dtframe_from_csv(pdtframe, 10, "BostonHousing.csv");
    set_dtframe_from_csv(pdtframe, "BostonHousing.csv");

    int* data_size = malloc(2*sizeof(int));
    datum*** data = get_dtframe_cols(pdtframe, data_size, 13, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
    datum** labels  = get_dtframe_col(pdtframe, 14);


    datum ***train_data, ***test_data;
    datum **train_labels, **test_labels;
    double train_per = 0.5;
    train_test_split(data, labels, &train_data, &train_labels, &test_data, &test_labels, data_size, train_per);
    int train_data_size[2] = {(int) data_size[0]*train_per, data_size[1]};
    printf("training data size: %d x %d \n", train_data_size[0], train_data_size[1]);

    printf("dtframe size: %d x %d\n", pdtframe->vert_capacity, pdtframe->hor_capacity);

    model* pmodel = new_lm_model();
    train(pmodel, train_data, train_labels, train_data_size);

    /*for(int k = 0; k < train_data_size[0]; k++) {
        print_datum(predict(pmodel, train_data[k]));
        printf(" ");
        print_datum(train_labels[k]);
        printf("\n");
    }*/

    printf("MSE on test data set: %f\n", MSE(pmodel, train_data, train_labels, train_data_size));
    free(pmodel);
    free(data_size);

    destroy_dtframe(pdtframe);
    return 0;
}