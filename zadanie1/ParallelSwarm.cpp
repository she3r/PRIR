//
// Created by User on 19.11.2024.
//
#include "ParallelSwarm.h"
#include "MathHelper.h"
#include "Swarm.h"
#include "consts.h"
#include <iostream>

using namespace std;

ParallelSwarm::ParallelSwarm(int robots, Antenna *antenna, Function *function) : Swarm(robots, antenna, function) {}

void ParallelSwarm::run(int steps) {
}

void ParallelSwarm::before_first_run() {
}

void ParallelSwarm::before_get_position() {
}

void ParallelSwarm::set_position( int dimension, int robot, double position ) {

}

double ParallelSwarm::get_position(int robot, int dimension) {

}

