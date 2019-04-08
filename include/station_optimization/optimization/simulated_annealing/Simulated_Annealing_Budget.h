//
// Created by ZhengchaoWANG on 20.07.18.
//

#ifndef CHARING_SIMULATED_ANNEALING_BUDGET_H
#define CHARING_SIMULATED_ANNEALING_BUDGET_H

#include <utility>
#include <vector>
#include <array>
#include <limits>
#include <iostream>
#include <cmath>
#include <time.h>
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

class Simulated_Annealing_Budget final: public  Heuristics{

private:

    /* Algorithm related values - Generating neighbor related */
    int                                  generate_neighbor_times = 0;
    bool                                    neighbor_changed = false;
    unsigned int                 neighbor_generate_seed = 1631682341; //124613654
    mt19937                                          neighbor_engine;
    array<int,2>                     neighbor_add_swap_ratio = {1,1};
    array<int,2>                   neighbor_add_delete_ratio = {2,1};
    array<int,2>            neighbor_inner_mutual_swap_ratio = {1,2};

    /* Algorithm related values - adopting neighbor related */
    double                                      temperature = 1000.0;  // A temperature value for simulated annealing.
    double                                    decrease_ratio = 0.995;
    double                                 stop_temperature = 0.0001;
    int                                 one_loop_searching_times = 5;  // How many times to search for neightborhood for one temperature.
    unsigned int                           adoption_seed = 733631680; //134683146
    mt19937                                          adoption_engine;
    std::uniform_real_distribution<>               real_distribution;

    /******************************** climb hill algorithm related helpers ********************************/

    double adoptionProbability();

    bool compareThePerformance(double performance1, double performance2);

    bool isadopted();

    /******************************** generate neighbors related ********************************/
    void generateNeighbor() override ;

    void addpilenumber();

    void decreasepilenumber();

    void mutualSwapPileNumber();

    void innerSwapPileNumber();

public:

    /******************************** constructors ********************************/
    Simulated_Annealing_Budget() = default;

    using Heuristics::Heuristics;

    Simulated_Annealing_Budget(vector<array<double, 2>> &station_locations,  vector<vector<int>> &current_station_config,
                               vector<double> &pile_charging_rate,  vector<double> &pile_price_by_type):Heuristics(station_locations, current_station_config,
                                                                                                                   pile_charging_rate, pile_price_by_type) {};

    /******************************** Actions ********************************/

    void defaultClassInitialization(bool pilenumber, bool MP, bool MPNS, bool SMIDV, vector<bool> singletype, bool budgetcompute = false ) override;

    void initializeRandomEngineDistributors() override;

    void getPerformance(vector<ReProduce_Taxi_General> this_fleet);

    void run() override ;

    /******************************** Setters and getters ********************************/

    void setGenerateNeighborsParameters(unsigned int neighbor_generate_seed, array<int, 2> add_swap_ratio,
                                        array<int, 2> add_delete_ratio);

    void setSeeds(unsigned int initial_seeds,unsigned int neighbor_generate_seed,unsigned int adoption_seed);

    void setTemperatureRalatedParameters(double temperature, double decrease_ratio, double stop_temperature);

    void setMaximumPileNumberInStations(const vector<int> &my_maximum_pile_number_in_stations);

    int getOneLoopSearchingTimes() const;

    void setOneLoopSearchingTimes(int one_loop_searching_times);

    void setDecreaseRatio (double ratio);

};


#endif //CHARING_SIMULATED_ANNEALING_BUDGET_H
