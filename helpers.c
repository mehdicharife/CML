#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <unistd.h>

#include "dtframe.h"
#include "glob.h"

void set_randomly(double* arr, int arr_size) {
    for(int k = 0; k < arr_size; k++) {
        arr[k] = 50000;
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

// computes the partial derivative of function with respects to its var_index-th argument
/*double part_der(datum* (function) (datum**, double*, int), datum** row, int row_size, double* vars, int var_index) {
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
}*/

