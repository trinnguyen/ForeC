/* Generated by EEC */
#ifndef __MatrixMultiply_config_H
#define __MatrixMultiply_config_H

#include "MatrixMultiply.h"

/* Macros for the reactive machine and its reset function */
#define INIT_MODULE()
#define RUN_MODULE() ( MatrixMultiply(0) )
#define RESET_MODULE() ( MatrixMultiply_reset() )

union SetFuncs {
    void (*set)(integer);
};

struct signal_binding {
    char name[64];
    unsigned char status;
    union SetFuncs u;
};

struct signal_binding input_signal[] = {
    {"size", 0, {MatrixMultiply_I_size}}
};
#define NUM_OF_INPUT_SIGNALS (sizeof(input_signal)/sizeof(struct signal_binding))

#ifdef TESTBENCH

#endif

#endif