#ifndef __LSYSTEM__
#define __LSYSTEM__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define BUFFER_SIZE 1024 * 1024
#define FIELD_SIZE 0xFF

typedef struct rule {
    char in;
    char out[FIELD_SIZE];
} Rule;


typedef struct lsystem {
    char constants[FIELD_SIZE];
    char axiom[FIELD_SIZE];
    Rule rules[FIELD_SIZE];
    double angle;
} Lsystem;

double *create_lsystem(Lsystem *lsystem, size_t num_iter, int *size_ptr);

#endif
