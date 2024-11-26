//
// Created by User on 19.11.2024.
//
#include "ParallelSwarm.h"
#include "MathHelper.h"
#include "Swarm.h"
#include "consts.h"
#include <iostream>

// todo usunac naglowek
#include <mpi.h>

using namespace std;


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
        free(array);
    }
};


ParallelSwarm::ParallelSwarm(int robots, Antenna *antenna, Function *function) : Swarm(robots, antenna, function) {
    MPI_Comm_size(MPI_COMM_WORLD, &_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
    _size = robots / _procs;
    _remainder = robots % _procs;
    _is_extended = _remainder && _remainder > _rank;
    set_start();
    set_end();
    allocate_memory();
    init_sizes();
    init_displacements();

}

ParallelSwarm::~ParallelSwarm() {
    delete[] neighbour_id;
    delete[] nearest_neighbours;
    delete[] function_value;
    delete[] antenna_range_sq;
    Allocator<double>::dealloc(position);
    Allocator<double>::dealloc(new_position);
}

void ParallelSwarm::find_neighbours_and_remember_best() {
    for (int robot = _start; robot < _end; robot++) {
        best_id = robot;
        nearest_neighbours[robot] = 0;

        best_function_value = function_value[robot];
        my_antenna_range_sq = antenna_range_sq[robot];
        my_position = position[robot];

        for (int other_robot = 0; other_robot < robot; other_robot++)
            compare_with_other_robot(robot, other_robot);

        for (int other_robot = robot + 1; other_robot < robots; other_robot++)
            compare_with_other_robot(robot, other_robot);

        neighbour_id[robot] = best_id;
    }
}

void ParallelSwarm::allocate_memory() {
    position = Allocator<double>::alloc_2d_oneblock(robots,dimensions);
    new_position = Allocator<double>::alloc_2d_oneblock(robots,dimensions);
    neighbour_id = new int[robots];
    nearest_neighbours = new int[robots];
    function_value = new double[robots];
    antenna_range_sq = new double[robots];
    _buffer = new double[robots];
    _buffer_positions = Allocator<double>::alloc_2d_oneblock(robots,dimensions);
}

void ParallelSwarm::init_displacements() {
    _displacements = new int[_procs];
    _displacements_positions = new int[_procs];
    _displacements[0] = 0;
    for (int i=1; i<robots; i++) {
        _displacements[i] = _displacements[i-1] + _sizes[i-1];
        _displacements_positions[i] = _displacements[i] * dimensions;
    }
}

void ParallelSwarm::move() {
    for (int robot = _start; robot < _end; robot++)
        MathHelper::move(position[robot], position[neighbour_id[robot]],
                         new_position[robot], dimensions, STEP_SIZE / sqrt(step));

    for (int robot = _start; robot < _end; robot++)
        for (int d = 0; d < dimensions; d++)
            position[robot][d] = new_position[robot][d];
}


void ParallelSwarm::set_start() {
    _start=0;
    if(!_rank) return;
    _start += _size * _rank;
    if(_is_extended)
        _start++;
}
void ParallelSwarm::set_end() {
    _end = _start + _size;
    if(_is_extended)
        _end += 1;
}

void ParallelSwarm::init_sizes() {
    _sizes=new int[_procs];
    _sizes_positions = new int[_procs];
    for(int i=0;i<_procs;++i) {
        _sizes[i] = _size;
        if(_remainder && i < _remainder)
            _sizes[i] += 1;
        _sizes_positions[i] = _sizes[i]*dimensions;
    }
}

void ParallelSwarm::compare_with_other_robot(int robot, int other_robot) {
    if (MathHelper::distanceSQ(my_position, position[other_robot], dimensions) <
    my_antenna_range_sq) {
        // inny robot jest w zasięgu anteny
        nearest_neighbours[robot]++;

        if (best_function_value < function_value[other_robot]) {
            // w dodatku ma lepszą wartość funkcji
            best_function_value = function_value[other_robot];
            best_id = other_robot;
        }
    }
}

void ParallelSwarm::evaluate_function() {
    for (int robot = _start; robot < _end; robot++)
        function_value[robot] = function->value(position[robot]);
}

void ParallelSwarm::gather_function_values() {
    // _end-_start - czy ja moge brac tak po kawalku z tabeli
    MPI_Allgatherv(function_value+_start,_end-_start,MPI_DOUBLE, _buffer,_sizes, _displacements,
        MPI_DOUBLE, MPI_COMM_WORLD);
    // czy to jest ok?
    delete[] function_value;
    function_value = _buffer;
}

void ParallelSwarm::gather_positions() {
    // &position[_start][0] - czy ja moge brac tak po kawalku z tabeli
    MPI_Allgatherv(&position[_start][0], (_end-_start)*dimensions, MPI_DOUBLE, _buffer_positions,
        _sizes_positions,_displacements_positions, MPI_DOUBLE, MPI_COMM_WORLD);
    // czy to jest ok?
    Allocator<double>::dealloc(position);
    position = _buffer_positions;
}


void ParallelSwarm::run(int steps) {
    for (int step = 0; step < steps; step++) {
        this->step++;
        evaluate_function();
        gather_function_values();
        if(step > 0)
            before_get_position();
        find_neighbours_and_remember_best();
        move();
        fit_antenna_range();
        //before_get_position();
    }
}

void ParallelSwarm::fit_antenna_range() {
    for (int robot = _start; robot < _end; robot++) {
        double range = antenna->range(sqrt(antenna_range_sq[robot]),
                                      nearest_neighbours[robot]);
        antenna_range_sq[robot] = range * range;
    }
}

void ParallelSwarm::before_first_run() {
    MPI_Bcast(&position[0][0], robots*dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}
void ParallelSwarm::before_get_position() {
    gather_positions();
}
void ParallelSwarm::set_position( int dimension, int robot, double position ) {
    this->position[robot][dimension] = position;
}
double ParallelSwarm::get_position( int robot, int dimension )  {
    return position[robot][dimension];
}



