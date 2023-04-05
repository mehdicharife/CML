#ifndef GENERAL_H
#define GENERAL_H

enum dtype {
    dstr,
    dint,
    ddouble,
    dvoid
} typedef dtype;

struct datum {
    dtype type;
    union {
        int i;
        double x;
        char* str;
    };
} typedef datum;


void print_datum(datum* pdatum);
char* get_pdatum_string(datum* pdatum);
void set_datum_from_str(datum* pdatum, char* str);

#endif