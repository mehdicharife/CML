#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "datum.h"


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

