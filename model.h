#ifndef MODEL_H
#define MODEL_H

#include <stdlib.h>
#include <math.h>


#include "dtframe.h"
#include "helpers.h"
#include "glob.h"

struct model;
typedef struct model model;

struct model {
    void* params;
    int params_count;
    double* (*get_nth_param) (void* params, int n);
    datum* (*relation) (model* pmodel, datum** X);
    double (*pd_relation) (model* pmodel, datum** B, int par_index);
    double* (*d_loss_func) (model* pmodel, datum*** data, datum** labels, int* data_size);
    void (*train) (model* pmodel, datum*** data, datum** labels, int* data_size);
};

model* clone_model(model* pmodel) ;
double* clone_model_params(model* pmodel);
double error(datum** X, datum* y, model* pmodel);
double MSE(model* pmodel, datum*** data, datum** labels, int* data_size);
double* dMSE(model* pmodel, datum*** data, datum** labels, int* data_size);
void walk(model* from, datum*** data, datum** labels, int* data_size, double step_size);
void train(model* pmodel, datum*** data, datum** labels, int* data_size);
datum* predict(model* pmodel, datum** X);
 

#endif