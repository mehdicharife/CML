#ifndef DARRAY_H
#define DARRAY_H

#include "datum.h"

struct dtframe {
    datum*** frame;
    int vert_capacity;
    int vert_holding;
    int hor_capacity;
    int* hor_holdings;
} typedef dtframe;


void init_dtframe_from_csv(dtframe* pdtframe, int initital_row_count, char* const path);
void set_dtframe_from_csv(dtframe* pdtframe, char* const path);
datum*** get_dtframe_cols(dtframe* pdtframe, int* data_size, int col_count, ...);
datum** get_dtframe_col(dtframe* pdtframe, int col_index);
void print_dtframe(dtframe* pdtframe);
void destroy_dtframe(dtframe* pdtframe);

#endif