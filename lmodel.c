#include <stdio.h>

#include "helpers.h"
#include "datum.h"
#include "lmodel.h"
#include <unistd.h>

lmodel* clone_lmodel(lmodel* pmodel) {
    lmodel* pclone = malloc(sizeof(lmodel));
    pclone->parent.relation = pmodel->parent.relation;
    pclone->d_loss_func = pmodel->d_loss_func;
    pclone->parent.params_count = pmodel->parent.params_count;
    pclone->parent.params = malloc(pclone->parent.params_count*sizeof(double));
    for(int k = 0; k < pclone->parent.params_count; k++) {
       ((double*) pclone->parent.params)[k] = ((double*)pmodel->parent.params)[k];
    }

    return pclone;
}


double* clone_model_params(model* pmodel) {
    double* clones = malloc(pmodel->params_count*sizeof(double));
    for(int k = 0; k < pmodel->params_count; k++) {
        clones[k] = ((double*) pmodel->params)[k];
    }
    return clones;
}



datum* lrelation(model* pmodel, datum** X) {
    datum* pdatum = malloc(sizeof(datum));
    pdatum->type = ddouble;

    pdatum->x = weighted_sum(X, (double*) pmodel->params, pmodel->params_count - 1);
    pdatum->x += ((double*) pmodel->params)[pmodel->params_count - 1];

    return pdatum;
}

double lpd_relation(model* pmodel, datum** X, int par_index) {
    if(par_index < pmodel->params_count - 1) {
        return get_datum_as_double(X[par_index]);
    }
    
    return 1;
}


double* lget_nth_param(void* params, int n) {
    double* parameters = params;

    return &(parameters[n]);
}


void ltrain(model* pmodel, datum*** data, datum** labels, int* data_size) {

    FILE* fp = fopen("mse.csv", "w");

    pmodel->params_count = data_size[1] + 1;
    pmodel->params = malloc((data_size[1] + 1)*sizeof(double));
    
    set_randomly(pmodel->params, pmodel->params_count);

    
    int i = 0;
    while(1) {
        double* old_params = clone_model_params(pmodel);
        
        walk(pmodel, data, labels, data_size, 1.0e-3);
        i++;
        fprintf(fp, "%d,%lf\n", i, MSE(pmodel, data, labels, data_size));

        if(difference(old_params, pmodel->params, pmodel->params_count) < 0.00001) {
            free(old_params);
            break;
        }
        
    
        free(old_params);
    }
}




// create a linear regression model
model* new_lmodel() {
    model* plmodel = malloc(sizeof(lmodel));
    plmodel->relation = lrelation;
    plmodel->pd_relation = lpd_relation;
    plmodel->d_loss_func = dMSE;
    plmodel->get_nth_param = lget_nth_param;
    plmodel->train = ltrain;

    return plmodel;
}