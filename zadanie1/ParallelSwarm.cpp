//
// Created by User on 19.11.2024.
//
#include "ParallelSwarm.h"
#include "MathHelper.h"
#include "Swarm.h"
#include "consts.h"
#include <mpi.h>
using namespace std;

ParallelSwarm::ParallelSwarm(int robots, Antenna *antenna, Function *function) : Swarm(robots, antenna, function) {
    MPI_Comm_size(MPI_COMM_WORLD, &_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
    _size = robots / _procs;
    _remainder = robots % _procs;
    _is_extended = _remainder && _remainder > _rank;
    // nie zmieniac ukladu
    ulong size_of_displacements_and_sizes = _procs * sizeof(int);
    _displacements = (int*) malloc(size_of_displacements_and_sizes);
    _displacements_positions = (int*) malloc(size_of_displacements_and_sizes);
    _sizes=(int*) malloc(size_of_displacements_and_sizes);
    _sizes_positions = (int*) malloc(size_of_displacements_and_sizes);
    init_sizes();
    init_displacements();
    set_start_and_end();
    //
    init_helpers();
    allocate_memory();
    initialize_antennas();
    this->step=0;
}

void ParallelSwarm::init_helpers() {
    _i_times_dimensions = (int*) malloc(sizeof(int) * (_end - _start));
    _i_times_dimensions[0] = _start * dimensions;
    for(int i=1;i<_end - _start;++i)
        _i_times_dimensions[i] = _i_times_dimensions[i-1] + dimensions;
}

ParallelSwarm::~ParallelSwarm() {
    free(neighbour_id);
    free(nearest_neighbours);
    free(function_value);
    free(antenna_range_sq);
    free(_position);
    free(_new_position);
    // displacements
    free(_displacements);
    free(_displacements_positions);
    // sizes
    free(_sizes);
    free(_sizes_positions);

    //helpers
    free(_i_times_dimensions);
}

void ParallelSwarm::find_neighbours_and_remember_best() {
    for (int robot = _start; robot < _end; robot++) {
        best_id = robot;
        nearest_neighbours[robot] = 0;

        best_function_value = function_value[robot];
        my_antenna_range_sq = antenna_range_sq[robot-_start];
        my_position = &(_position[_i_times_dimensions[robot-_start]]);

        for (int other_robot = 0; other_robot < robot; other_robot++)
            compare_with_other_robot(robot, other_robot);

        for (int other_robot = robot + 1; other_robot < robots; other_robot++)
            compare_with_other_robot(robot, other_robot);
        neighbour_id[robot] = best_id;
    }
}

void ParallelSwarm::allocate_memory() {
    unsigned long size_of_position_arrays = robots * dimensions * sizeof(double);
    _position = (double*) malloc(size_of_position_arrays);
    _new_position= (double*) malloc(size_of_position_arrays);

    ulong size_of_int_arrays = robots * sizeof(int);
    neighbour_id = (int*)malloc(size_of_int_arrays);
    nearest_neighbours = (int*) malloc(size_of_int_arrays);

    ulong size_of_double_arrays = robots * sizeof(double);
    function_value = (double*) malloc(size_of_double_arrays);
    antenna_range_sq = (double*) malloc((_end-_start)*sizeof(double));

}
void ParallelSwarm::initialize_antennas() {
    double vSQ = antenna->initial_range();
    vSQ *= vSQ;
    for (int r = 0; r < _end-_start; r++)
        antenna_range_sq[r] = vSQ;
}

void ParallelSwarm::init_displacements() {
    _displacements[0] = 0;
    _displacements_positions[0] = 0;
    for (int i=1; i<_procs; i++) {
        _displacements[i] = _displacements[i-1] + _sizes[i-1];
        _displacements_positions[i] = _displacements_positions[i-1] + _sizes[i-1] * dimensions;
    }
}


void ParallelSwarm::move() {
    for (int robot = 0; robot < _end - _start; robot++)
        MathHelper::move(&(_position[_i_times_dimensions[robot]]),&(_position[neighbour_id[robot + _start] * dimensions]),
                         &(_new_position[_i_times_dimensions[robot]]), dimensions, STEP_SIZE / sqrt(step));

    for (int robot = 0; robot < _end-_start; robot++)
        for (int d = 0; d < dimensions; d++)
            _position[_i_times_dimensions[robot] + d] = _new_position[_i_times_dimensions[robot] + d];
}

void ParallelSwarm::set_start_and_end() {
    _start = _displacements[_rank];
    _end = _displacements[_rank] + _sizes[_rank];
}

void ParallelSwarm::init_sizes() {
    for(int i=0;i<_procs;++i) {
        _sizes[i] = _size;
        if(_remainder && i < _remainder)
            _sizes[i] += 1;

        _sizes_positions[i] = _sizes[i] * dimensions;
    }
}

void ParallelSwarm::compare_with_other_robot(int robot, int other_robot) {
    if (MathHelper::distanceSQ(my_position, &(_position[other_robot * dimensions]), dimensions) < my_antenna_range_sq) {
        nearest_neighbours[robot]++;

        if (best_function_value < function_value[other_robot]) {
            best_function_value = function_value[other_robot];
            best_id = other_robot;
        }
    }
}

void ParallelSwarm::evaluate_function() {
    for (int robot = 0; robot < (_end-_start); robot++)
        function_value[robot + _start] = function->value(&(_position[_i_times_dimensions[robot]]));
}

void ParallelSwarm::gather_function_values() {
    MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, &(function_value[0]), _sizes, _displacements, MPI_DOUBLE, MPI_COMM_WORLD);
}

void ParallelSwarm::gather_positions() {
    MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, &(_position[0]), _sizes_positions, _displacements_positions, MPI_DOUBLE, MPI_COMM_WORLD);
}

void ParallelSwarm::run(int steps) {
    for (int s = 0; s < steps; s++) {
        this->step++;
        evaluate_function();
        gather_function_values();
        if(s > 0)
            gather_positions();
        find_neighbours_and_remember_best();
        move();
        fit_antenna_range();
        //before_get_position();
    }
}

void ParallelSwarm::fit_antenna_range() {
    for (int robot = 0; robot < _end-_start; robot++) {
        double range = antenna->range(sqrt(antenna_range_sq[robot]),
                                      nearest_neighbours[robot + _start]);
        antenna_range_sq[robot] = range * range;
    }
}

void ParallelSwarm::before_first_run() {
    uint robots_dimensions = robots * dimensions;
    if(!_rank)
        MPI_Bcast(_position,  robots_dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    else{
        double *position_buffer = (double*) malloc(sizeof(double) * robots_dimensions);
        MPI_Bcast(position_buffer, robots_dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(_position);
        _position=position_buffer;
    }
}

void ParallelSwarm::before_get_position() {
    gather_positions();
}

void ParallelSwarm::set_position( int dimension, int robot, double position ) {
    this->_position[robot * dimensions + dimension] = position;
}

double ParallelSwarm::get_position( int robot, int dimension )  {
    return this->_position[robot * dimensions + dimension];
}



