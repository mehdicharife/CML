#ifndef MLMATH_H
#define MLMATH_H

#include "datum.h"

void set_randomly(double* arr, int arr_size);
double difference(double* arr1, double* arr2, int arr_size);
double part_der(datum* (function) (datum**, double*, int), datum** row, int row_size, double* vars, int var_index) ;

#endif