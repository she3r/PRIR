//
// Created by User on 19.11.2024.
//


#ifndef PARALLELSWARM_H
#define PARALLELSWARM_H

#include "Antenna.h"
#include "Function.h"
#include "Swarm.h"
class ParallelSwarm: public Swarm {
private:
    int *neighbour_id; // identyfikator najbliższego sąsiada
    int *nearest_neighbours; // liczba sąsiadów w zasięgu
    double *antenna_range_sq; // kwadrat zasięgu anteny
    double *function_value;  // wartość funkcji
    int step;

    //double *_buffer;
    //double ** _buffer_positions;

    // From sequential
    int best_id;
    double best_function_value;
    double my_antenna_range_sq;
    double * my_position;
    //

    void evaluate_function();
    void gather_function_values();

    void gather_positions();

    void set_start_and_end();
    void init_sizes();
    void compare_with_other_robot(int robot, int other_robot);
    void init_displacements();
    void move();
    void fit_antenna_range();
    // podzial pracy
    int _size,_procs,_remainder,_rank,_start,_end;
    bool _is_extended;
    int* _sizes;
    int* _sizes_positions;
    int *_displacements;
    int* _displacements_positions;

    // alternatywa do 2d
    double *_position;
    double *_new_position;
    ~ParallelSwarm();

    void find_neighbours_and_remember_best();

public:
    ParallelSwarm( int robots, Antenna *antenna, Function *function  );

    void run( int steps ) override;

    void before_first_run() override;
    void before_get_position() override;
    void set_position( int dimension, int robot, double position ) override;
    double get_position( int robot, int dimension ) override;
    void allocate_memory() override;

    bool IsLogStep() const;

    void initialize_antennas();
};
#endif //PARALLELSWARM_H
