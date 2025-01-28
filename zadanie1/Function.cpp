#include <cstdlib>
#include "Function.h"

int Function::dimensions() { return dims; }
double *Function::get_min_position() { return min_position; }
double *Function::get_max_position() { return max_position; }

Function::Function( int dimensions ) {
    dims = dimensions;
    min_position = (double*) malloc(dims * sizeof(double));
    max_position = (double*) malloc(dims * sizeof(double));
//    min_position = new double[ dims ];
//    max_position = new double[ dims ];
}
