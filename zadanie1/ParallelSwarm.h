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

public:
    ParallelSwarm( int robots, Antenna *antenna, Function *function  );
    void run( int steps ) override;
    void before_first_run() override;
    void before_get_position() override;
    void set_position( int dimension, int robot, double position ) override;
    double get_position( int robot, int dimension ) override;
};
#endif //PARALLELSWARM_H
