#include "ParallelSwarm.h"
#include "MathHelper.h"
#include "Swarm.h"
#include "consts.h"
#include <cmath>

using namespace std;

ParallelSwarm::ParallelSwarm(int robots, Antenna *antenna,
                                 Function *function)
    : Swarm(robots, antenna, function) {
  step = 0;
  ParallelSwarm::allocate_memory();
  ParallelSwarm::initialize_antennas();
}

void ParallelSwarm::run(const int steps) {
  for (int step = 0; step < steps; step++)
    single_step();
}

void ParallelSwarm::single_step() {
  step++;
  evaluate_function();
  find_neighbours_and_remember_best();
  move();
  fit_antenna_range();
}

void ParallelSwarm::evaluate_function() {
#pragma omp parallel for schedule(static)
  for (int robot = 0; robot < robots; robot++) {
    function_value[robot] = function->value(position[robot]);
  }
}

void ParallelSwarm::find_neighbours_and_remember_best() {
#pragma omp parallel for schedule(static)
  for (int robot = 0; robot < robots; robot++) {
    int* best_id = new int;
    *best_id = robot;
    nearest_neighbours[robot] = 0;

    double* best_function_value = new double;
    *best_function_value = function_value[robot];
    double* my_antenna_range_sq = new double;
    *my_antenna_range_sq = antenna_range_sq[robot];
    double* my_position;
    my_position = position[robot];

    for (int other_robot = 0; other_robot < robot; other_robot++) {
      compare_with_other_robot(robot, other_robot, my_position, my_antenna_range_sq, best_id, best_function_value);
    }
    for (int other_robot = robot+1; other_robot < robots; other_robot++) {
      compare_with_other_robot(robot, other_robot, my_position, my_antenna_range_sq, best_id, best_function_value);
    }

    neighbour_id[robot] = *best_id;
    delete best_function_value;
    delete my_antenna_range_sq;
    delete best_id;
  }
}

void ParallelSwarm::compare_with_other_robot(const int robot, const int other_robot, double* my_position,
                                             const double* my_antenna_range_sq, int* best_id, double* best_function_value) const {

  if (MathHelper::distanceSQ(my_position, position[other_robot], dimensions) <
      *my_antenna_range_sq) {
    // inny robot jest w zasiegu anteny
    nearest_neighbours[robot]++;

    if (*best_function_value < function_value[other_robot]) {
      // w dodatku ma lepsza wartosc funkcji
      *best_function_value = function_value[other_robot];
      *best_id = other_robot;
    }
  }
}

void ParallelSwarm::move() const {
#pragma omp parallel for schedule(static)  // potencjalne chunkowanie?
  for (int robot = 0; robot < robots; robot++)
    MathHelper::move(position[robot], position[neighbour_id[robot]],
                     new_position[robot], dimensions, STEP_SIZE / sqrt(step));

#pragma omp parallel for collapse(2) schedule(static) // potencjalne chunkowanie?
  for (int robot = 0; robot < robots; robot++)
    for (int d = 0; d < dimensions; d++)
      position[robot][d] = new_position[robot][d];
}

void ParallelSwarm::distributionOfRobots( int *histogramD0, int *histogramD1, const int size, const double d2idx ) {
	this->histogramD0 = histogramD0;
	this->histogramD1 = histogramD1;
#pragma omp parallel for schedule(static)
	for ( int robotA = 0; robotA < robots; robotA++ )
		distributionForRobot( robotA, d2idx, size);
}

void ParallelSwarm::distributionForRobot(const int robotA, const double d2idx, const int histogram_size) {
	double dxMin = 1000000000000000000000.0;
	double dyMin = 1000000000000000000000.0;
  for ( int robotB = 0; robotB < robots; robotB++ ) {
		if ( robotA == robotB ) continue;
		const double dx = fabs(position[robotA][0] - position[robotB][0]);
		const double dy = fabs(position[robotA][1] - position[robotB][1]);

		if ( dx < dxMin ) dxMin = dx;
		if ( dy < dyMin ) dyMin = dy;
	}

	const int idxD0 = dxMin * d2idx;
	const int idxD1 = dyMin * d2idx;

	if ( idxD0 < histogram_size )
#pragma omp atomic
		histogramD0[ idxD0 ]++;
	if ( idxD1 < histogram_size )
#pragma omp atomic
		histogramD1[ idxD1 ]++;
}



void ParallelSwarm::allocate_memory() {
  position = new double *[robots];
  new_position = new double *[robots];
#pragma omp parallel for
  for (int i = 0; i < robots; i++) {
    position[i] = new double[dimensions];
    new_position[i] = new double[dimensions];
  }

  neighbour_id = new int[robots];
  nearest_neighbours = new int[robots];
  function_value = new double[robots];
  antenna_range_sq = new double[robots];
}

void ParallelSwarm::fit_antenna_range() {
#pragma omp parallel for schedule(static)  // potencjalne chunkowanie?
  for (int robot = 0; robot < robots; robot++) {
    const double range = antenna->range(sqrt(antenna_range_sq[robot]),
                                  nearest_neighbours[robot]);
    antenna_range_sq[robot] = range * range;
  }
}

void ParallelSwarm::initialize_antennas() {
  double vSQ = antenna->initial_range();
  vSQ *= vSQ;
#pragma omp parallel for  schedule(static)// potencjalne chunkowanie?
  for (int r = 0; r < robots; r++) {
    antenna_range_sq[r] = vSQ;
  }
}

void ParallelSwarm::set_position(const int dimension, const int robot, const double position) {
  this->position[robot][dimension] = position;
}

double ParallelSwarm::get_position(const int robot,const int dimension) {
  return position[robot][dimension];
}
