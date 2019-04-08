//
// Created by ZhengchaoWANG on 03.08.18.
//

#ifndef CHARING_HEURISTIC_H
#define CHARING_HEURISTIC_H


#include <sys/types.h>
#include <dirent.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <vector>
#include <array>
#include <string>
#include <limits>
#include <cmath>
#include <random>
#include <numeric>    // for sum up function accumulate

#include "station_optimization/simulation/taxi/ReProduce_Taxi_Fleet.h"
#include "station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h"


using namespace std;
using dist_t = uniform_int_distribution<>;
using param_t = dist_t::param_type;

class Heuristics{

protected:
    vector<array<double,2>>                        station_locations;     // location of charge stations. in the format of latitude and longitude. Maybe we can use 3-dimension array to remember the type of station or lanes.

    vector<vector<int>>                         best_stations_config;     // number of different charging piles in different stations. We can also regard it as the best configuration that come accorss.
    vector<vector<int>>                       current_station_config;     // current station
    vector<vector<int>>                             neighbors_config;     // current station
    vector<double>                                pile_charging_rate;

    unsigned int                       initialize_seeds = 1357313476;     // A seeds use to generate random number in case of random initialization.
    mt19937                                           initial_engine;
    std::uniform_int_distribution<>             integer_distribution;

    /* Constraints related */
    vector<bool>                             single_type_in_stations;
    vector<int>                      maximum_pile_number_in_stations;     // Maximum allowable pile numbers can be installed in the station. this vector is same dimension as locations
    vector<double>                                pile_price_by_type;    // The faster one is in the back.
    double                                            budget = 290.0;     // In case the charging station is limited by money budget.
    double                                        budget_ratio = 1.0;
    double                                           used_budget=0.0;     // how much have been used?
    double                         minimal_price = 100000000000000.0;
    vector<vector<int>>    stations_maximal_increase_decrease_number;
    /* performance related */
    double                         current_performance = -10000000.0;         // An integrated factor to evaluate the goodness of scenario.
    double                        neighbor_performance = -10000000.0;
    double                        best_performance = -100000000000.0;
    vector<double>                           performance_vector = {};
    vector<double>                    neighbor_performance_vect = {};
    vector<double>                        best_performance_vect = {};
    double                                     objective_alpha = 0.4;
    int                                  objectivefunctionchioce = 0;

    ReProduce_Taxi_Fleet *                          my_fleet_pointer;

    array<int,2>                                     simulation_span;

    string                                               output_path;
    /************************  Related to find existed piles in current config **********************/

    vector<int> findInstalledType(int stationnode);

    vector<int> findInstalledType(vector<int> &this_config);

    vector<double> findUsedPileRate(vector<int> & this_config);

    void nonEmptyStations(vector<int> & non_empty_stations);

    /************************  Helper functions **********************/
    template <typename T>
    vector<size_t> sort_indexes(const vector<T> &v);

    void generateSequece(vector<int> & target_vector, int begin, int end);

    void generateSequenceWithErase(vector<int> & target_vector, int begin, int end, vector<int> eraser);

    template<class bidiiter> bidiiter randomUnique(bidiiter begin, bidiiter end, int numrandom);

    double twoPointsDistance(array<double,2> pos1, array<double,2> pos2);

    void computeUsedBudget();

    template<class T>
    array<int, 2> findtimelimit(vector<T> &mytaxifleet) {
        int minimumtime = std::numeric_limits<int>::max();
        int maximumtime = 0;

        for(auto & my_taxi : mytaxifleet){
            if(my_taxi.getMyhistory().getTime()[0] <minimumtime ){
                minimumtime = my_taxi.getMyhistory().getTime()[0];
            }
            if(my_taxi.getMyhistory().getTime().back() > maximumtime){
                maximumtime = my_taxi.getMyhistory().getTime().back();
                if(maximumtime > 95000){
                    cout << my_taxi.getId_();
                }
            }
        }
        return {minimumtime,maximumtime};
    }

    /************************************************************************************/
    /************************  Initialization functions **********************/
    /************************************************************************************/
    void initiateCurrentStationConfig();

    void initializeStationsUsingSeeds();

    virtual void initializeRandomEngineDistributors();

    void initializeSingleTypeInStations(bool singletype);

    void initializeSingleTypeInStations(vector<bool> & singletypes);

    void initializeMaximumPileNumberInStations();

    void initializeMinimalPrice();

    void initializeStationsMaximalIncreaseDecreaseValue();

    virtual void setupBudget();

    void outputStationConfig(string & filename);

    void outputPerformanceHistory(string&  filename);

    double computeTheoreticalValue();

public:

    /************************************ Constructors and distructors ****************************************/
    Heuristics() = default;

    Heuristics(const vector<array<double, 2>> &station_locations, const vector<vector<int>> &current_station_config,
               const vector<double> &pile_charging_rate, const vector<double> &pile_price_by_type) : station_locations(
            station_locations), current_station_config(current_station_config), pile_charging_rate(pile_charging_rate), pile_price_by_type(pile_price_by_type) {};

    Heuristics(const vector<array<double, 2>> &station_locations, const vector<double> &pile_charging_rate, const vector<double> &pile_price_by_type) : station_locations(
            station_locations), pile_charging_rate(pile_charging_rate), pile_price_by_type(pile_price_by_type) {};

    Heuristics(const vector<array<double,2>> & locations, const vector<vector<int>> & slotnumbers, const vector<double> & price):
    station_locations(locations), current_station_config(slotnumbers), pile_price_by_type(price){};

    Heuristics(const vector<array<double, 2>> & locations, unsigned int seed, const vector<double> & price,
               const vector<double> & rate): station_locations(locations), initialize_seeds(seed),  pile_price_by_type(price), pile_charging_rate(rate){};

    Heuristics(const vector<array<double,2>> & locations, unsigned int seed,const vector<double> & price, const vector<int> & maximumpilenumber, double budget = 100000000000.0 ):
    station_locations(locations), initialize_seeds(seed), pile_price_by_type(price), maximum_pile_number_in_stations(maximumpilenumber),budget(budget){};

    virtual  ~Heuristics() = default;
    /**************************************************** Actions *************************************************/

    void setMy_fleet_pointer(ReProduce_Taxi_Fleet *my_fleet_pointer);

    Charging_Station_General_Manager getChargingFacilityManager(int starttime, bool want_neighbor = true);

    virtual void defaultClassInitialization(bool pilenumber, bool MP, bool MPNS, bool SMIDV, vector<bool> singletype, bool budgetcompute);

    virtual void generateNeighbor(){};

    virtual void run(){};

    void outputResult();

    /* Getters */

    const array<int, 2> &getSimulation_span() const {
        return simulation_span;
    }

    void setOutput_path(const string &output_path) {
        Heuristics::output_path = output_path;
    }

    void setBudget_ratio(double budget_ratio);

    void setObjective_alpha(double objective_alpha) {
        Heuristics::objective_alpha = objective_alpha;
    }

    const vector<double> &getBest_performance_vect() const;

    void setObjectivefunctionchioce(int objectivefunctionchioce) {
        Heuristics::objectivefunctionchioce = objectivefunctionchioce;
    }

    double getBest_performance() const;



};


#endif //CHARING_HEURISTIC_H
