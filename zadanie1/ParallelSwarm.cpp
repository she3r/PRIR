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
//    if(IsLogStep())
//        cout << "step: " << step << " rank=" <<_rank<< " find_neighbours_and_remember_best" << endl;
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
//        if(IsLogStep())
//            cout << "step: " << step << " rank=" <<_rank<< " ended find_neighbours_and_remember_best" << endl;
    }
}

void ParallelSwarm::allocate_memory() {
//    position = Allocator<double>::alloc_2d_oneblock(robots,dimensions);
//    new_position = Allocator<double>::alloc_2d_oneblock(robots,dimensions);
    unsigned long size_of_position_arrays = robots * dimensions * sizeof(double);
    _position = (double*) malloc(size_of_position_arrays);
    _new_position= (double*) malloc(size_of_position_arrays);
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
//    if(!_rank) {
//        cout << "displacements:" << endl;
//        for (int i = 0; i < _procs; ++i) {
//            cout << _displacements[i] << " ";
//        }
//        cout << endl << "displacements_positions:";
//        for (int i = 0; i < _procs; ++i) {
//            cout << _displacements_positions[i] << " ";
//        }
//        cout << endl;
//    }

}
//double distanceSQ(const double *p1, const double *p2, int dimensions) {
//    double d = 0;
//    double delta;
//
//    for (int dim = 0; dim < dimensions; dim++) {
//        delta = p1[dim] - p2[dim];
//        d += delta * delta;
//    }
//    return d;
//}
//double distance(double *p1, double *p2, int dimensions) {
//    return sqrt(distanceSQ(p1, p2, dimensions));
//}
//void movep(double *p1, double *p2, double *p3, int dimensions,
//           double ratio) {
//    double d = distance(p1, p2, dimensions);
//    double r;
//    if (d) r = ratio / d;
//    else r = 0;
//
//    for (int dim = 0; dim < dimensions; dim++)
//        p3[dim] = p1[dim] + (p2[dim] - p1[dim]) * r;
//}

void ParallelSwarm::move() {
    for (int robot = _start; robot < _end; robot++)
        MathHelper::move(&(_position[robot * dimensions]),&(_position[neighbour_id[robot] * dimensions]),
                         &(_new_position[ robot * dimensions]), dimensions, STEP_SIZE / sqrt(step));

    for (int robot = _start; robot < _end; robot++)
        for (int d = 0; d < dimensions; d++)
            //set_position(d, robot, _new_position[robot * dimensions + d]);
            _position[robot * dimensions + d] = _new_position[robot * dimensions + d];
    //position[robot][d] = new_position[robot][d];
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
    //cout << "rank: " << _rank << " start: " << _start << " end: " << _end << endl;
}

void ParallelSwarm::init_sizes() {
    for(int i=0;i<_procs;++i) {
        _sizes[i] = _size;
        if(_remainder && i < _remainder)
            _sizes[i] += 1;
        _sizes_positions[i] = _sizes[i]*dimensions;
    }
//    if(!_rank) {
//        cout << "sizes: " << endl;
//        for (int i = 0; i < _procs; ++i)
//            cout << _sizes[i] << " ";
//        cout << endl << "sizes_positions: " << endl;
//        for (int i = 0; i < _procs; ++i)
//            cout << _sizes_positions[i] << " ";
//        cout << endl;
//    }
}

void ParallelSwarm::compare_with_other_robot(int robot, int other_robot) {
    //cout << "compare_with_other_robot. " << "step="<<step<<" rank= "<<_rank<<" robot1="<<robot<<" robot2="<<other_robot<<endl;
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
    for (int robot = _start; robot < _end; robot++)
        function_value[robot] = function->value(&(_position[robot * dimensions]));
}

void ParallelSwarm::gather_function_values() {
//    if(IsLogStep())
//        cout << "step: " << step << ". Gathering function values. Rank: " << _rank <<endl;
    double* function_buffer = (double*)malloc(robots * sizeof(double));
    // _end-_start - czy ja moge brac tak po kawalku z tabeli
    //cout << "gather_function_values_MPI_Allgatherv. Start: " << _start << " sendcount: " << _end-_start << endl;
    MPI_Allgatherv(function_value+_start,_end-_start, MPI_DOUBLE, function_buffer, _sizes, _displacements,
                   MPI_DOUBLE, MPI_COMM_WORLD);
    //cout << "done gathering function values. Rank: " << _rank <<endl;
    // czy to jest ok?
    // delete[] function_value;
    //cout << "done gathering function values. Deleted function_value. Rank: " << _rank <<endl;
    function_value = function_buffer;
    //cout << "done gathering function values. Assigned new function_value. Rank: " << _rank <<endl;
    //cout << "new function_value:"<<endl;
//    for(int i=0;i<8;++i)
//        cout<<"["<<i<<"]"<<function_value[i]<<" ";
//    cout<<endl;
}

// todo: skladanie macierzy z pozycjami musi jednak wygladac inaczej
// https://stackoverflow.com/questions/9507834/how-to-combine-subarrays-of-different-widths-using-only-one-array-for-send-and-r
void ParallelSwarm::gather_positions() {
    //cout<< "step="<<step<<". Rank="<<_rank<< ". gather_positions";
    //double** buffer_positions = Allocator<double>::alloc_2d_oneblock(robots, dimensions);
    double* buffer_positions = (double*) malloc(sizeof(double)*robots*dimensions);

    // &position[_start][0] - czy ja moge brac tak po kawalku z tabeli
    MPI_Allgatherv(_position + (_start * dimensions), (_end-_start)*dimensions, MPI_DOUBLE, buffer_positions,
        _sizes_positions,_displacements_positions, MPI_DOUBLE, MPI_COMM_WORLD);

    _position = buffer_positions;
}

void ParallelSwarm::run(int steps) {
    for (int step = 0; step < steps; step++) {
        this->step++;
        evaluate_function();
        gather_function_values();
        if(step > 0)
            // todo wewnatrz petli to musi byc allgatherv, ale na potrzeby prezentacji wystarczy gatherv
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
        MPI_Bcast(_position, robots*dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    else{
        double *position_buffer = (double*) malloc(sizeof(double) * robots * dimensions);
        MPI_Bcast(position_buffer, robots*dimensions, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        free(_position);
        _position=position_buffer;
    }

    //cout << "after MPI_Bcast. " << "rank="<<_rank<<" step="<<step<<endl;
}
void ParallelSwarm::before_get_position() {
    gather_positions();
}
void ParallelSwarm::set_position( int dimension, int robot, double position ) {
    //this->position[robot][dimension] = position;
    this->_position[robot * dimension] = position;
}
double ParallelSwarm::get_position( int robot, int dimension )  {
    //return position[robot][dimension];
    return this->_position[robot * dimension];
}



