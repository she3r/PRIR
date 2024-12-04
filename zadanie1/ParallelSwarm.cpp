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

ParallelSwarm::ParallelSwarm(int robots, Antenna *antenna, Function *function) : Swarm(robots, antenna, function) {
    MPI_Comm_size(MPI_COMM_WORLD, &_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
    //cout<<"Constructor. procs="<<_procs<<" rank="<<_rank<<endl;
    _size = robots / _procs;
    _remainder = robots % _procs;
    _is_extended = _remainder && _remainder > _rank;
    //cout<< "Constructor. size=" << _size <<" . Remainder=" << _remainder << ". IsExtended=" << _is_extended << endl;
    allocate_memory();
    init_sizes();
    init_displacements();
    set_start_and_end();
    initialize_antennas();
    this->step=0;
}

ParallelSwarm::~ParallelSwarm() {
    //cout << "destructor start" <<endl;
    //delete[] neighbour_id;
    free(neighbour_id);
    neighbour_id= nullptr;
    //cout << "destructor neighbour_id" <<endl;
    //delete[] nearest_neighbours;
    free(nearest_neighbours);
    nearest_neighbours= nullptr;
    //cout << "destructor nearest_neighbours" <<endl;
    //delete[] function_value;
    free(function_value);
    function_value= nullptr;
    //cout << "destructor function_value" <<endl;
    //delete[] antenna_range_sq;
    free(antenna_range_sq);
    //antenna_range_sq= nullptr;
    //cout << "destructor antenna_range_sq" <<endl;
    //Allocator<double>::dealloc(position);
    free(_position);
    //cout << "destructor position" <<endl;
    //Allocator<double>::dealloc(new_position);
    free(_new_position);
    //cout << "destructor new_position" <<endl;

    // displacements
    //cout << "descructor displacements" << endl;
    free(_displacements);
    free(_displacements_positions);
    // sizes
    free(_sizes);
    free(_sizes_positions);
}

void ParallelSwarm::find_neighbours_and_remember_best() {
    for (int robot = _start; robot < _end; robot++) {
        best_id = robot;
        nearest_neighbours[robot] = 0;

        best_function_value = function_value[robot];
        my_antenna_range_sq = antenna_range_sq[robot];
        my_position = &(_position[robot * dimensions]);

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
    antenna_range_sq = (double*) malloc(size_of_double_arrays);

    ulong size_of_displacements_and_sizes = _procs * sizeof(int);
    _displacements = (int*) malloc(size_of_displacements_and_sizes);
    _displacements_positions = (int*) malloc(size_of_displacements_and_sizes);
    _sizes=(int*) malloc(size_of_displacements_and_sizes);
    _sizes_positions = (int*) malloc(size_of_displacements_and_sizes);
}
void ParallelSwarm::initialize_antennas() {
    double vSQ = antenna->initial_range();
    vSQ *= vSQ;
    for (int r = _start; r < _end; r++)
        antenna_range_sq[r] = vSQ;
}

void ParallelSwarm::init_displacements() {
    _displacements[0] = 0;
    _displacements_positions[0] = 0;
    for (int i=1; i<_procs; i++) {
        _displacements[i] = _displacements[i-1] + _sizes[i-1];
        _displacements_positions[i] = _displacements_positions[i-1] + _sizes[i-1] * dimensions;
    }
    if(!_rank) {
        cout << "displacements: " << endl;
        for (int i = 0; i < _procs; ++i) {
            cout << _displacements[i] << " ";
        }
        cout << endl;
        cout << "_displacements_positions: " << endl;
        for (int i = 0; i < _procs; ++i) {
            cout << _displacements_positions[i] << " ";
        }
        cout << endl;
    }
}


void ParallelSwarm::move() {
    for (int robot = _start; robot < _end; robot++)
        MathHelper::move(&(_position[robot * dimensions]),&(_position[neighbour_id[robot] * dimensions]),
                         &(_new_position[ robot * dimensions]), dimensions, STEP_SIZE / sqrt(step));

    for (int robot = _start; robot < _end; robot++)
        for (int d = 0; d < dimensions; d++)
            _position[robot * dimensions + d] = _new_position[robot * dimensions + d];
//    int search = 31;
//    if(search >= _start && search < _end){
//        cout << "rank: " << _rank << " neighbour id: " << neighbour_id[search] << " ";
//        cout<< "step: " << step << ". newposition " << search << ": " << _new_position[search * dimensions + 0]
//        << ","<<_new_position[search * dimensions + 1]
//        << ","<< _new_position[search * dimensions + 2] << endl;
//        cout<< "step: " << step << ". _position " << search << ": " << _position[search * dimensions + 0]
//            << ","<<_position[search * dimensions + 1]
//            << ","<< _position[search * dimensions + 2] << endl;
//    }
}

void ParallelSwarm::set_start_and_end() {
    _start = _displacements[_rank];
    _end = _displacements[_rank] + _sizes[_rank];
    cout << "rank: " << _rank << " start: " << _start << " end: " << _end << endl;
}

void ParallelSwarm::init_sizes() {
    for(int i=0;i<_procs;++i) {
        _sizes[i] = _size;
        if(_remainder && i < _remainder)
            _sizes[i] += 1;
        _sizes_positions[i] = _sizes[i]*dimensions;
    }
    if(!_rank) {
        cout << "sizes: " << endl;
        for (int i = 0; i < _procs; ++i)
            cout << _sizes[i] << " ";
        cout << endl << "sizes_positions: " << endl;
        for (int i = 0; i < _procs; ++i)
            cout << _sizes_positions[i] << " ";
        cout << endl;
    }
}

void ParallelSwarm::compare_with_other_robot(int robot, int other_robot) {
    if (MathHelper::distanceSQ(my_position, &(_position[other_robot * dimensions]), dimensions) < my_antenna_range_sq) {
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
    for (int robot = _start; robot < _end; robot++) {
        function_value[robot] = function->value(&(_position[robot * dimensions]));
    }
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
    for (int robot = _start; robot < _end; robot++) {
        double range = antenna->range(sqrt(antenna_range_sq[robot]),
                                      nearest_neighbours[robot]);
        antenna_range_sq[robot] = range * range;
    }
}
void ParallelSwarm::before_first_run() {
    if(!_rank)
        MPI_Bcast(_position, robots*dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    else{
        double *position_buffer = (double*) malloc(sizeof(double) * robots * dimensions);
        MPI_Bcast(position_buffer, robots*dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        // todo experimental
        free(_position);
        _position=position_buffer;
    }
}
void ParallelSwarm::before_get_position() {
    gather_positions();
}
void ParallelSwarm::set_position( int dimension, int robot, double position ) {
    //this->position[robot][dimension] = position;
    this->_position[robot * dimensions + dimension] = position;
}
double ParallelSwarm::get_position( int robot, int dimension )  {
    //return position[robot][dimension];
    return this->_position[robot * dimensions + dimension];
}



