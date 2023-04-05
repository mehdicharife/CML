#include <stdlib.h>
#include <stdarg.h>

#define PERTURBATION  0.0001


struct model {
    float* parameters;
    int parameters_count;
    int (*relation) (int* params, int* X, int params_count);
} typedef model;



float error(float* X, float y, model* pmodel) {
    return y - (*pmodel->relation)(pmodel->parameters, X, pmodel->parameters_count);
}




float linear_function(float* parameters, float* X, int params_count) {
    int y = 0;
    for(int k = 0; k < param_count - 1; k++) {
        y = y + parameters[k]*X[k];
    }

    y = y + parameters[param_count - 1];

    return y;
}


model* new_lm_model() {
    model* pmodel = malloc(sizeof(model));
    pmodel->relation = &linear_function;
}


model* clone_model(model* pmodel) {
    model* pclone = malloc(sizeof(model));
    pclone->relation = pmodel->relation;
    pclone->parameters_count = pmodel->parameters_count;
    pclone->parameters = malloc(pclone->parameters_count*sizeof(float));
    for(int k = 0; k < pclone->parameters_count; k++) {
        pclone->parameters[k] = pmodel->parameters[k];
    }

    return pclone;
}

float* clone_params(model* pmodel) {
    float* clones = malloc(pmodel->parameters_count*sizeof(float));
    for(int k = 0; k < pmodel->parameters_count; k++) {
        clones[k] = pmodel->parameters[k];
    }

    return clones;
}




float MSE(float** data, float* labels, int* data_size, model* pmodel) {
    mse = 0;
    for(int i = 0; i < data_size[0]; i++) {
        mse = mse + error(data[i], labels[i], pmodel)**2;
    }    
    
    return mse/data_size[0];
}


float* gradient(float (*function) (float**, float*, int*, model* point), float** data, float* labels, float* data_size, model* point) {
    float* gradient = malloc(pmodel->parameters_count*sizeof(float));

    for(int k = 0; k < pmodel->parameters_count; k++) {
        model* ptemp_model = clone_model(point);
        ptemp_model->parameters[k] += PERTURBATION;
        gradient[k] = (function(data, labels, data_size, ptemp_model) - function(data, labels, data_size, point))/PERTURBATION;
        free(ptemp_model);
    }

    return gradient;
}

void walk(model* from, float** data, float* labels, float* data_size , float (*function) (float**, float*, int*, model* pmodel), float step_size) {
    float* gradient = gradient(function, data, labels, data_size, from);

    for(int k = 0; k <= from->parameters_count; k++) {
        from->parameters[k] = from->parameters[k] - step_size*gradient;
    }

    free(gradient);
}


void train(model* pmodel, float** data, float labels, int* data_size, int (*loss_function (float** data, float* labels, int* data_size, model* pmodel))) {
    set_randomly(pmodel->parameters, pmodel->parameters_count);
    while(1) {
        old_params = clone_params(pmodel);
        walk(pmodel, data, labels, data_size, &MSE);
        if(difference(old_params, pmodel->parameters, pmodel->parameters_count) < 0.0001) {
            break;
        }
    }
    
}

void predict(model* pmodel, int* X) {
    return (*pmodel->relation) (pmodel->parameters, X, pmodel->parameters_count);
} 



int main(int argc, char* argv[]) {

}