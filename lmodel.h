#ifndef LM_H
#define LM_H

#include "model.h"

typedef struct lmodel lmodel;

struct lmodel {
    model parent;
    double* (*d_loss_func) (model* pmodel, datum*** data, datum** labels, int* data_size);
};


model* new_lmodel();

#endif