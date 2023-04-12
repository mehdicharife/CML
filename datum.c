#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "datum.h"
#include "glob.h"

int is_int(char* str) {
    for(int k = 0; k < strlen(str); k++) {
        if(!isdigit(str[k])) {
            return 0;
        }

        if(k == 0 && str[k] == '0') {
            return !(strlen(str) - 1);
        }
    }

    return 1;
}


int is_double(char* str) {
    char* p;
    double d = strtod(str, &p);

    return !strlen(p);
}


void set_datum_from_str(datum* pdatum, char* str) {
    if(is_int(str)) {
        pdatum->type = dint;
        pdatum->i = atoi(str);
    } else if(is_double(str)) {
        char* p;
        pdatum->type = ddouble;
        pdatum->x =  strtod(str,&p);                
    } else if(strlen(str)){
        pdatum->type = dstr;
        pdatum->str = malloc(sizeof(char*)*strlen(str));
        strcpy(pdatum->str, str);
    } else {
        pdatum->type = dvoid;
    }
}


void print_datum(datum* pdatum) {
    switch(pdatum->type) {
        case dint:
            printf("%d", pdatum->i);
            break;
        case ddouble:
            printf("%lf", pdatum->x);
            break;
        case dstr:
            printf("%s", pdatum->str);
            break;
        case dvoid:
            printf(" ");
            break;
    }

    fflush(stdout);
}


char* get_pdatum_string(datum* pdatum) {
    char* str_datum;
    switch(pdatum->type) {
        case dint:
            snprintf(str_datum, snprintf(NULL, 0, "%d", pdatum->i) + 1, "%d", pdatum->i);
            break;
        case ddouble:
            snprintf(str_datum, snprintf(NULL, 0, "%lf", pdatum->x) + 1, "%lf", pdatum->x);
            break;
        case dstr:
            return pdatum->str;
            break;
    }

    return str_datum;
}


void* get_datum_pcontent(datum* pdatum) {
    switch(pdatum->type) {
        case dstr:
            return &(pdatum->str);
            break;
        case dint:
            return &(pdatum->i);
            break;
        case ddouble:
            return &(pdatum->x);
            break;
    }
}

double get_datum_as_double(datum* pdatum) {
    switch(pdatum->type) {
        case dint:
            return (double) pdatum->i;
            break;

        case ddouble:
            return  (double) pdatum->x;
            break;

        case dstr:
            return atof(pdatum->str);
            break;
    }
}


double weighted_sum(datum** pX, double* params, int datum_count) {
    double sum = 0;

    for(int k = 0; k < datum_count; k++) {
        double raw;

        switch(pX[k]->type) {
            case dint:
                raw = pX[k]->i;
                break;
            case ddouble:
                raw = pX[k]->x;
                break;

            case dstr:
                raw = atof(pX[k]->str);
                break;
        }

        sum = sum + raw*params[k];

    }

    return sum;
}



void train_test_split(datum*** data, datum** labels, datum**** ptrain_data, datum*** ptrain_labels, datum**** ptest_data, datum*** ptest_labels, int* data_size, double train_percentage) {
    int train_rows_count = (int) (data_size[0]*train_percentage);
    int test_rows_count = data_size[0] - train_rows_count;

    *ptrain_data = malloc(train_rows_count*sizeof(datum**));
    *ptrain_labels = malloc(train_rows_count*sizeof(datum*));
    *ptest_data = malloc(test_rows_count*sizeof(datum**));
    *ptest_labels = malloc(test_rows_count*sizeof(datum**));

    for(int i = 0; i < data_size[0]; i++) {
        if(i < train_rows_count) {
            (*ptrain_data)[i] = data[i];
            (*ptrain_labels)[i] = labels[i];
        } else {
            (*ptest_data)[i - train_rows_count] = data[i];
            (*ptest_labels)[i - train_rows_count] = labels[i];
        }
    }
}


int datumcmp(datum* pdatum1, datum* pdatum2) {
    if(pdatum1->type != pdatum2->type) {
        return 0;
    }

    switch(pdatum1->type) {
        case dint:
            return (pdatum1->i - pdatum2->i);
            break;

        case ddouble:
            return (pdatum1->x - pdatum2->x);
            break;

        case dstr:
            return strcmp(pdatum1->str, pdatum2->str);
            break;

        case dvoid:
            return 1;
            break;
    }
}