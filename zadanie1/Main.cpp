/*
 * Main.cpp
 */

#include "Antenna.h"
#include "Function.h"
#include "MathHelper.h"
#include "SimpleAntenna.h"
#include "SimpleExpFunction.h"
#include "Swarm.h"
#include "consts.h"
#include "ParallelSwarm.h"

#include <iostream>
#include <stdlib.h>

#define MPI_ON

#ifdef MPI_ON
#include <mpi.h>
#endif

//sleep
#include <thread>
#include <time.h>
#include <unistd.h> // For sleep function
#include <fstream>

using namespace std;

double rnd() { return rand() % 1000000 / 1000000.0; }

void initialize_swarm(Swarm *swarm, Function *function, int robots, int rank) {
  //cout << "initalizing swarm. Rank: " << rank<<endl;
  int dims = function->dimensions();
  //if(dims<=0) cout << "dims<=0"<<endl;
  //double *size = new double[(int)dims];
  double *size = (double*)malloc(sizeof(double) * dims);
    //cout << "initalizing swarm. Done weird double array Rank: " << rank<<endl;
  for (int d = 0; d < dims; d++)
    size[d] = function->get_max_position()[d] - function->get_min_position()[d];

  //cout << "initalizing swarm. Done init size values. Rank: " << rank<<endl;
  srand(SEED);

  for (int robot = 0; robot < robots; robot++) {
    for (int d = 0; d < dims; d++) {
        double rand = rnd();
        swarm->set_position(d, robot,
                            function->get_min_position()[d] + size[d] * rand);
        //cout << rand << " ";
    }
    //cout << endl;
  }
    //cout << "done initalizing swarm. Rank: " << rank<<endl;
  //delete[] size;
    //free(size);
    //cout << "done initalizing swarm. Delted size. Rank: " << rank <<endl;
}

Function *initialize_function() {
  int DIMS = 3;
  SimpleExpFunction *f = new SimpleExpFunction(DIMS, 4);

    f->set_exp_params(0, 2.0, 0.25);
    f->set_exp_params(1, 2.0, 0.3);
    f->set_exp_params(2, 2.0, 0.4);
    f->set_exp_params(3, 2.0, 0.3);

    f->set_exp_location(0, 0, 1);
    f->set_exp_location(0, 1, 1);
    f->set_exp_location(0, 2, 0.5);

    f->set_exp_location(1, 0, 4);
    f->set_exp_location(1, 1, 4);
    f->set_exp_location(1, 2, 0.5);

    f->set_exp_location(2, 0, 1);
    f->set_exp_location(2, 1, 4);
    f->set_exp_location(2, 2, 0.5);

    f->set_exp_location(3, 0, 4);
    f->set_exp_location(3, 1, 1);
    f->set_exp_location(3, 2, 0.5);

  return f;
}

void show_report(int step, Swarm *swarm, int robots, int dims) {
    for (int r = 0; r < robots; r++) {
        cout << step << " " << r << " ";
        for (int d = 0; d < dims; d++) {
            cout << swarm->get_position(r, d) << " ";
        }
        cout << endl;
    }
}
std::string timeToString(std::time_t time) {
    std::tm* tm_ptr = std::localtime(&time);
    char buffer[100];
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", tm_ptr)) {
        return std::string(buffer);
    } else {
        throw std::runtime_error("strftime failed");
    }
}

string get_filename(int num_process, int rank){
    // Get the current time as a time_t object
    std::time_t now = std::time(nullptr);

    // Convert the time_t object to a string
    std::string timeStr = timeToString(now);
    return (string) "output-p-" + std::to_string(num_process) + "r-" + std::to_string(rank) + "-" + timeStr + ".txt";
}

int main(int argc, char **argv) {
    clock_t startclock, endclock;
    double start;
  int procs, rank = 0;

#ifdef MPI_ON
  MPI_Init(&argc, &argv);
  //cout << "MPI version " << MPI_VERSION << endl;
  MPI_Comm_size(MPI_COMM_WORLD, &procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
    std::ofstream* outFile = nullptr;
    std::streambuf* coutBuf = std::cout.rdbuf();
    if(!rank) {
        // Open a file in write mode.
        outFile = new std::ofstream(get_filename(procs, rank));
        // Check if the file is open
        if (!outFile->is_open()) {
            std::cerr << "Failed to open file for output." << std::endl;
            return 1;
        }
        std::cout.rdbuf(outFile->rdbuf());
        //sleep(10);

        startclock = clock();
    }

  Function *function = initialize_function();
  double max_distance = MathHelper::distance(function->get_max_position(),
                                             function->get_min_position(),
                                             function->dimensions());

  Antenna *antenna = new SimpleAntenna(
      TARGET_NEIGHBOURS, max_distance / ANTENNA_MIN_RANGE_DIV,
      max_distance / ANTENNA_MAX_RANGE_DIV, ANTENNA_RANGE_MODIFIER);
  Swarm *swarm = new ParallelSwarm(ROBOTS, antenna, function);

  if (!rank)
    initialize_swarm(swarm, function, ROBOTS, rank);    // zainicjalizowany swarm tylko w parencie, trzeba inicjowac/przeslac

  swarm->before_first_run();  // przeslanie inicjalizacji do wszystkich pozostalych procesow
  int step = 0;
  if(rank == 1 || rank == 0)
    show_report(step, swarm, ROBOTS, function->dimensions());
  do {
//      if(rank == 0 && step==0)
    swarm->run(REPORT_PERIOD);
    // todo: to nie musi juz byc allgatherv, tylko gatherv
    swarm->before_get_position();
    step += REPORT_PERIOD;
    if (!rank) {
          show_report(step, swarm, ROBOTS, function->dimensions());
    }
  } while (step < STEPS);
    if(!rank) {
        endclock = clock();
        double duration = (double) (endclock - startclock) / CLOCKS_PER_SEC;
        // Restore the original buffer to cout
        std::cout.rdbuf(coutBuf);
        // Close the file
        outFile->close();
        std::cout << "Step: " << step << " Execution time: " << duration << std::endl;
    }
#ifdef MPI_ON
  MPI_Finalize();
#endif

  return 0;
}
