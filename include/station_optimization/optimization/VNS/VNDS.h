//
// Created by ZhengchaoWANG on 30.11.18.
//

#ifndef CHARING_VNDS_H
#define CHARING_VNDS_H

#include <utility>
#include <vector>
#include <array>
#include <limits>
#include <iostream>
#include <cmath>
//#include <cstdlib>  // For srand() and rand()
#include <random>
#include <numeric>    // for sum up function accumulate

#include "station_optimization/simulation/helpers/One_Simulation.h"
#include "station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h"
#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.h"
#include "station_optimization/optimization/heuristic/Heuristics.h"

using namespace std;
using dist_t = uniform_int_distribution<>;
using param_t = dist_t::param_type;

/*
 * In the vector<vector<int>> related variables, we set the outter vector related to stations, the inner vector related
 * to charging piles. We put the faster one in the back.
 */

class VNDS : public  Heuristics {
private:
    vector<vector<int>>         original_solution;
    vector<vector<int>>         first_neighbor;
    vector<vector<int>>         shaking_space;
    vector<vector<int>>         shaking_space_neighbor;

    vector<vector<int>>         dont_touch_space;
    vector<int>                 shaking_space_position;

    int                         max_k; // number of total neighbor
    int                         number_type;

public:
    void generateNeighbor() override;
    void generateShakingSpace();         // From the neighborhood to generate the shaking space
    void generateShakingSpaceNeighbor(); // Generate the neighborhood of the shaking space
    void makeupWholeSpace();             // make the whole solution for the favor of simulation
    void oneSimulation();
    bool compareResult();
    void originalLocalSearch();

    void run() override;


};

#endif //CHARING_VNDS_H
