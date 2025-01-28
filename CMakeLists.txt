cmake_minimum_required(VERSION 3.30)
project(PRIR)

set(CMAKE_CXX_STANDARD 14)

include_directories(zadanie3)

add_executable(PRIR
    zadanie3/Antenna.cpp
    zadanie3/Antenna.h
    zadanie3/consts.h
    zadanie3/Function.cpp
    zadanie3/Function.h
    zadanie3/Main.cpp
    zadanie3/MathHelper.cpp
    zadanie3/MathHelper.h
    zadanie3/ParallelSwarm.cpp
    zadanie3/ParallelSwarm.h
    zadanie3/SequentialSwarm.cpp
    zadanie3/SequentialSwarm.h
    zadanie3/SimpleAntenna.cpp
    zadanie3/SimpleAntenna.h
    zadanie3/SimpleExpFunction.cpp
    zadanie3/SimpleExpFunction.h
    zadanie3/Swarm.cpp
    zadanie3/Swarm.h)
