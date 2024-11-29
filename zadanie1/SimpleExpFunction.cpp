#include "SimpleExpFunction.h"
#include "MathHelper.h"
#include <iostream>
#include <math.h>

using namespace std;

double SimpleExpFunction::singleExp(double *position, int idx) {
  double dSQ = MathHelper::distanceSQ(position, exp_location[idx], dims);

  return ampl[idx] * exp(-dSQ / decay[idx]);
}

double SimpleExpFunction::value(double *position) {
  double result = 0.0;

  for (int idx = 0; idx < exponents; idx++) {
    result += singleExp(position, idx);
  }

  return result;
}

template<typename T>
class Allocator {
public:
    static T **alloc_2d_oneblock(int rows, int cols) {
        T *data = static_cast<T *>(malloc(rows * cols * sizeof(T)));
        T **array= static_cast<T **>(malloc(rows * sizeof(T *)));
        for (int i=0; i<rows; i++)
            array[i] = &(data[cols*i]);

        return array;
    }
    // example send MPI_Send(&(A[0][0]), N*M, MPI_INT, destination, tag, MPI_COMM_WORLD);
    static void dealloc(T **array) {
        free(array[0]);
        array[0]= nullptr;
        free(array);
        array= nullptr;
    }
};



SimpleExpFunction::SimpleExpFunction(int dims, int exponents) : Function(dims) {
  this->exponents = exponents;

//  exp_location = new double *[exponents];
//  decay = new double[exponents];
//  ampl = new double[exponents];
//  for (int e = 0; e < exponents; e++)
//    exp_location[e] = new double[dims];
    exp_location = Allocator<double>::alloc_2d_oneblock(exponents,dims);
    decay = (double*) malloc(sizeof(double)*exponents);
    ampl = (double*) malloc(sizeof(double)*exponents);

  for (int d = 0; d < dims; d++) {
    min_position[d] = 0.0;
    max_position[d] = exponents + 1.0;
  }
}

void SimpleExpFunction::set_exp_params(int exp, double ampl, double decay) {
  this->ampl[exp] = ampl;
  this->decay[exp] = decay;
}

void SimpleExpFunction::set_exp_location(int exp, int dim, double value) {
  exp_location[exp][dim] = value;
}