/*
PARALLEL_SWARM.h
*/

#ifndef PARALLEL_SWARM_H_
#define PARALLEL_SWARM_H_

#include "Antenna.h"
#include "Function.h"
#include "Swarm.h"

class ParallelSwarm: public Swarm {
    private: 
      int step;
      int *neighbour_id; // identyfikator najbliższego sąsiada
      int *nearest_neighbours; // liczba sąsiadów w zasięgu
      double *antenna_range_sq; // kwadrat zasięgu anteny
      double *function_value;  // wartość funkcji
      int *histogramD0;
      int *histogramD1;

      void single_step();
      void evaluate_function();
      void find_neighbours_and_remember_best();
    void compare_with_other_robot(int robot, int other_robot, double *my_position,
                                  const double *my_antenna_range_sq, int *best_id, double *best_function_value) const;
      void move() const;
      void fit_antenna_range();
      void allocate_memory();
      void initialize_antennas();
      void distributionForRobot(int robotA, double d2idx, int histogram_size);
    public:
      ParallelSwarm( int robots, Antenna *antenna, Function *function  );
      void run( int steps );
      void set_position( int dimension, int robot, double position );
      double get_position( int robot, int dimension );
      void distributionOfRobots( int *histogramP0, int *histogramP1, int size, double d2idx );
};

#endif
