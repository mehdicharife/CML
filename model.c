#include "model.h"
#include <stdio.h>






datum* relation(model* pmodel, datum** X) {
    return (pmodel->relation)(pmodel, X);
}

double pd_relation(model* pmodel, datum**X, int par_index) {
    return pmodel->pd_relation(pmodel, X, par_index);
}


// measures how bad the model did in predicting an expected value
double error(datum** X, datum* y, model* pmodel) {  
    datum* dpred = relation(pmodel, X);
    double pred = get_datum_as_double(dpred);
    free(dpred);
    return get_datum_as_double(y) - pred;
}


// calculates the mean squared error made by a model over a given data set
double MSE(model* pmodel, datum*** data, datum** labels, int* data_size) {
    double mse = 0;

    for(int i = 0; i < data_size[0]; i++) {
        mse = mse + pow(error(data[i], labels[i], pmodel), 2);
    }    
    
    return mse/data_size[0];
}


// computes the gradient of the mean squared error made by the model on a given data set
double* dMSE(model* pmodel, datum*** data, datum** labels, int* data_size) {
    double* grad = malloc(pmodel->params_count*sizeof(double));

    double errors[data_size[0]]; 
    for(int k = 0; k < data_size[0]; k++) {
        datum* prediction = relation(pmodel, data[k]);
        errors[k] = get_datum_as_double(labels[k]) - get_datum_as_double(prediction);
        free(prediction);
    }

    for(int j = 0; j < data_size[1] + 1; j++) {
        grad[j] = 0;
        for(int i = 0; i < data_size[0]; i++) {            
            grad[j] -= 2*errors[i]*pd_relation(pmodel, data[i], j); 
        }


        grad[j] = grad[j]/data_size[0];
    }
 
    
    return grad;
}


// starting from a given point (model) moves in a step that minimizes the loss function associated with the model
void walk(model* from, datum*** data, datum** labels, int* data_size, double step_size) {
    double* gradient = (from->d_loss_func)(from, data, labels, data_size);


    for(int k = 0; k < from->params_count; k++) {
        *(from->get_nth_param(from->params, k)) -= step_size*gradient[k];
    }

    free(gradient);
}


// tries to fit the model to the (data, labels) by changing its parameters
void train(model* pmodel, datum*** data, datum** labels, int* data_size) {
    pmodel->train(pmodel, data, labels, data_size);
}


// computes the model's prediction corresponding to a certain input
datum* predict(model* pmodel, datum** X) {
    return relation(pmodel, X);
} 


