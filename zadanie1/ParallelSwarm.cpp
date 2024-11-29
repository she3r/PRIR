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
        array[0]= nullptr;
        free(array);
        array= nullptr;
    }
};
//todo delete
bool ParallelSwarm::IsLogStep() const{
    return step%100==0;
}

ParallelSwarm::ParallelSwarm(int robots, Antenna *antenna, Function *function) : Swarm(robots, antenna, function) {
    MPI_Comm_size(MPI_COMM_WORLD, &_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
    //cout<<"Constructor. procs="<<_procs<<" rank="<<_rank<<endl;
    _size = robots / _procs;
    _remainder = robots % _procs;
    _is_extended = _remainder && _remainder > _rank;
    //cout<< "Constructor. size=" << _size <<" . Remainder=" << _remainder << ". IsExtended=" << _is_extended << endl;
    set_start();
    set_end();
    allocate_memory();
    init_sizes();
    init_displacements();

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
    antenna_range_sq= nullptr;
    //cout << "destructor antenna_range_sq" <<endl;
    Allocator<double>::dealloc(position);
    position= nullptr;
    //cout << "destructor position" <<endl;
    Allocator<double>::dealloc(new_position);
    new_position= nullptr;
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
//    if(IsLogStep())
//        cout << "step: " << step << " rank=" <<_rank<< " find_neighbours_and_remember_best" << endl;
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
//        if(IsLogStep())
//            cout << "step: " << step << " rank=" <<_rank<< " ended find_neighbours_and_remember_best" << endl;
    }
}

void ParallelSwarm::allocate_memory() {
    position = Allocator<double>::alloc_2d_oneblock(robots,dimensions);
    new_position = Allocator<double>::alloc_2d_oneblock(robots,dimensions);
    ulong size_of_int_arrays = robots * sizeof(int);
    //neighbour_id = new int[robots];
    neighbour_id = (int*)malloc(size_of_int_arrays);
    //nearest_neighbours = new int[robots];
    nearest_neighbours = (int*) malloc(size_of_int_arrays);
    //function_value = new double[robots];
    ulong size_of_double_arrays = robots * sizeof(double);
    function_value = (double*) malloc(size_of_double_arrays);
    //antenna_range_sq = new double[robots];
    antenna_range_sq = (double*) malloc(size_of_double_arrays);

    ulong size_of_displacements_and_sizes = _procs * sizeof(int);
    _displacements = (int*) malloc(size_of_displacements_and_sizes);
    _displacements_positions = (int*) malloc(size_of_displacements_and_sizes);

    _sizes=(int*) malloc(size_of_displacements_and_sizes);
    _sizes_positions = (int*) malloc(size_of_displacements_and_sizes);
}

void ParallelSwarm::init_displacements() {
    _displacements[0] = 0;
    _displacements_positions[0] = 0;
    for (int i=1; i<_procs; i++) {
        _displacements[i] = _displacements[i-1] + _sizes[i-1];
        _displacements_positions[i] = _displacements_positions[i-1] + _sizes[i-1] * dimensions;
    }
//    cout << "displacements:" << endl;
//    for(int i=0;i<_procs;++i){
//        cout << _displacements[i] << " ";
//    }
//    cout<<endl<<"displacements_positions:";
//    for(int i=0;i<_procs;++i){
//        cout << _displacements_positions[i] << " ";
//    }
//    cout<<endl;

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
    for(int i=0;i<_procs;++i) {
        _sizes[i] = _size;
        if(_remainder && i < _remainder)
            _sizes[i] += 1;
        _sizes_positions[i] = _sizes[i]*dimensions;
    }
//    cout << "sizes: " << endl;
//    for(int i=0;i<_procs;++i)
//        cout << _sizes[i] << " ";
//    cout << endl << "sizes_positions: " << endl;
//    for(int i=0;i<_procs;++i)
//        cout << _sizes_positions[i] << " ";
//    cout << endl;
}

void ParallelSwarm::compare_with_other_robot(int robot, int other_robot) {
    //cout << "compare_with_other_robot. " << "step="<<step<<" rank= "<<_rank<<" robot1="<<robot<<" robot2="<<other_robot<<endl;
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
//    if(IsLogStep())
//        cout << "step: " << step << ". Gathering function values. Rank: " << _rank <<endl;
    double* buffer = (double*)malloc(robots * sizeof(double));
    // _end-_start - czy ja moge brac tak po kawalku z tabeli
    //cout << "gather_function_values_MPI_Allgatherv. Start: " << _start << " sendcount: " << _end-_start << endl;
    MPI_Allgatherv(function_value+_start,_end-_start,MPI_DOUBLE, buffer,_sizes, _displacements,
        MPI_DOUBLE, MPI_COMM_WORLD);
    //cout << "done gathering function values. Rank: " << _rank <<endl;
    // czy to jest ok?
    // delete[] function_value;
    //cout << "done gathering function values. Deleted function_value. Rank: " << _rank <<endl;
    function_value = buffer;
    //cout << "done gathering function values. Assigned new function_value. Rank: " << _rank <<endl;
    //cout << "new function_value:"<<endl;
//    for(int i=0;i<8;++i)
//        cout<<"["<<i<<"]"<<function_value[i]<<" ";
//    cout<<endl;
}

void ParallelSwarm::gather_positions() {
    //cout<< "step="<<step<<". Rank="<<_rank<< ". gather_positions";
    double** buffer_positions = Allocator<double>::alloc_2d_oneblock(robots, dimensions);
    // &position[_start][0] - czy ja moge brac tak po kawalku z tabeli
    MPI_Allgatherv(&position[_start][0], (_end-_start)*dimensions, MPI_DOUBLE, &buffer_positions[0][0],
        _sizes_positions,_displacements_positions, MPI_DOUBLE, MPI_COMM_WORLD);
    // czy to jest ok?
//    Allocator<double>::dealloc(position);
//    position= nullptr;
    position = buffer_positions;
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
    //cout << "before MPI_Bcast. " << "rank="<<_rank<<" step="<<step<<endl;
    if(!_rank)
        MPI_Bcast(&position[0][0], robots*dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    else{
        // todo czy trzeba inicjalizowac position w rank!=0 ?
        double **position_buffer = Allocator<double>::alloc_2d_oneblock(robots,dimensions);
        MPI_Bcast(&position_buffer[0][0], robots*dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        Allocator<double>::dealloc(position);
        // todo to jest bez sensu, ale jestem sfrustrowany, potem to powywalac te nullptr
        position= nullptr;
        position=position_buffer;
    }

    //cout << "after MPI_Bcast. " << "rank="<<_rank<<" step="<<step<<endl;
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



