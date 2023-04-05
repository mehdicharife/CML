#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "dtframe.h"
#define PHI 1.61803398875
#define MAX_TOKEN_SIZE 600

void init_dtframe(dtframe* pdtframe, int hor_capacity, int initial_vert_capacity) {
    pdtframe->frame = malloc(initial_vert_capacity*sizeof(datum**));
    pdtframe->vert_capacity = initial_vert_capacity;
    pdtframe->vert_holding = 0;

    pdtframe->hor_capacity = hor_capacity;
    pdtframe->hor_holdings = malloc(hor_capacity*sizeof(int));
    for(int k = 0; k < initial_vert_capacity; k++) {
        pdtframe->frame[k] = malloc(hor_capacity*sizeof(datum*));
        pdtframe->hor_holdings[k] = 0; 
    }
}


void init_dtframe_from_csv(dtframe* pdtframe, int initital_row_count, char* const path) {
    FILE* fp = fopen(path, "r");
    
    pdtframe->frame = malloc(initital_row_count*sizeof(datum**));
    pdtframe->vert_capacity = initital_row_count;

    char *line, *tok;
    line = malloc(2048*sizeof(char));
    int col_count = 1;
    fgets(line, 1024, fp);
    
    for(int k = 0; k < strlen(line); k++) {
        if(line[k] ==  ',') {
            col_count++;
        }
    }
    
    free(line);
    fclose(fp);
    pdtframe->hor_capacity = col_count;
    pdtframe->hor_holdings = malloc(initital_row_count*sizeof(int));
    for(int k = 0; k < pdtframe->vert_capacity; k++) {
        pdtframe->frame[k] = malloc(col_count*sizeof(datum*));
        pdtframe->hor_holdings[k] = 0;
    }

}

void append_dtframe(dtframe* pdtframe, datum* pdatum) {

    if(pdtframe->vert_holding == 0) {
        pdtframe->frame[0][0] = pdatum;
        pdtframe->vert_holding = 1;
        pdtframe->hor_holdings[0] = 1;
        return;
    }
    
    if(pdtframe->hor_holdings[pdtframe->vert_holding - 1] == pdtframe->hor_capacity) {
        if(pdtframe->vert_holding == pdtframe->vert_capacity) {
            int new_vert_capacity = pdtframe->vert_capacity*2;
            pdtframe->frame = realloc(pdtframe->frame, new_vert_capacity*sizeof(datum**));
            pdtframe->hor_holdings = realloc(pdtframe->hor_holdings, new_vert_capacity*sizeof(int));
            for(int k = pdtframe->vert_capacity; k < new_vert_capacity; k++) {
                pdtframe->frame[k] = malloc(pdtframe->hor_capacity*sizeof(datum*));
                pdtframe->hor_holdings[k] = 0;
            }
            pdtframe->vert_capacity = floor(new_vert_capacity);

        }

        pdtframe->frame[pdtframe->vert_holding][0] = pdatum;
        pdtframe->hor_holdings[pdtframe->vert_holding] = 1;
        pdtframe->vert_holding += 1;

        return;
        
    }

    pdtframe->frame[pdtframe->vert_holding - 1][pdtframe->hor_holdings [pdtframe->vert_holding - 1]] = pdatum;
    pdtframe->hor_holdings[pdtframe->vert_holding - 1] += 1;
    
    return;
}



void set_dtframe_from_csv(dtframe* pdtframe, char* const path) {

    if(pdtframe->hor_capacity == 0) {
        fprintf(stderr, "You must initialize the data frame before setting it from a csv file");
        exit(1);
    }

    FILE* fp = fopen(path, "r");
    char line[1024];
    char* tok;
 
    while(fgets(line, 1024, fp)) {
        int offset = 0;
        char token[MAX_TOKEN_SIZE];
        int inside_quotes = 0;

        for(int k = 0; k < strlen(line); k++) {
            if(line[k] == '\"') {
                inside_quotes = !inside_quotes;
            }
            if((line[k] != ',' || inside_quotes) && k < strlen(line) - 1) {
                continue;
            }

            if (k < strlen(line) - 1) {
                if(offset == k) {
                    strcpy(token, "");
                } else {
                    strncpy(token, line + offset, k - offset);
                    token[k - offset] = '\0';
                }
            } else {
                strncpy(token, line + offset, k - offset + 1);
                token[k - offset + 1] = '\0';
            }


            datum* pdatum = malloc(sizeof(datum));
            set_datum_from_str(pdatum, token);
            append_dtframe(pdtframe, pdatum);
            memset(token, '\0', MAX_TOKEN_SIZE);
            offset = k + 1;
        }

    }
}


void print_dtframe(dtframe* pdtframe) {
    for(int i = 0; i < pdtframe->vert_holding/10 -1; i++) {
        for(int j = 0; j < pdtframe->hor_holdings[i]; j++) {
            print_datum((pdtframe->frame[i][j]));
            printf(" ");

        }

        printf("\n");
    }
}


int main(int argc, char* argv[]) {
    dtframe dtf;
    init_dtframe_from_csv(&dtf, 10, "titanic.csv");
    set_dtframe_from_csv(&dtf, "titanic.csv");

    print_dtframe(&dtf);
}