#ifndef GENERAL_H
#define GENERAL_H

enum dtype {
    dstr,
    dint,
    dfloat,
    dvoid
} typedef dtype;

struct datum {
    dtype type;
    union {
        int i;
        float x;
        char* str;
    };
} typedef datum;


void print_datum(datum* pdatum);
char* get_pdatum_string(datum* pdatum);
void set_datum_from_str(datum* pdatum, char* str);
void* get_datum_pcontent(datum* pdatum); // get a pointer to the content of the datum
float get_datum_as_float(datum* pdatum);
int datumcmp(datum* pdatum1, datum* pdatum2);
float weighted_sum(datum** pX, float* params, int datum_count);
void train_test_split(datum*** data, datum** labels, datum**** train_data, datum*** train_labels, datum**** test_data, datum*** test_labels, int* data_size, float train_percentage);

#endif