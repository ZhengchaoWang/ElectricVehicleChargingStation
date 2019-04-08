//
// Created by ZhengchaoWANG on 03.08.18.
//

#ifndef CHARING_LOCAL_SEARCH_H
#define CHARING_LOCAL_SEARCH_H

#include <vector>
#include <array>
#include <string>
#include <limits>
#include <iostream>
#include <cmath>
#include <numeric>    // for sum up function accumulate

#include "station_optimization/optimization/local_search/Local_Search_Simulation.h"

#include "station_optimization/optimization/heuristic/Heuristics.h"
using namespace std;
//using dist_t = uniform_int_distribution<>;
//using param_t = dist_t::param_type;

class Local_Search final : public Heuristics{

private:
    /* variables for the method */

    vector<int>                                                  queue_length;
    vector<int>                                               total_wait_time;

    vector<vector<double>>                          all_utilization_time_rate;
    vector<double>                                           UR_max_threshold;      // need to design this value. The larger the average charge rate, the more we want it to be utilized.
    vector<double>                                     average_wait_frequency;
    vector<double>                                          AWF_max_threshold;      // need to design this value. The more the charging piles, the less we want the waiting list is.
    vector<double>                                          average_wait_time;
    vector<double>                                          AWT_max_threshold;      // need to can calculate from the average charging time of one vehicle.
    bool                                                      envolve = false;
    vector<double>                                          default_threshold = {0.2,1200,600};

    vector<double>                                         best_coverage_ratio;
    vector<double>                                       desired_utilize_ratio;

    int                                                         begin_time = 0;
    int                                                       end_time = 86400;
    int                                                    objectivechoice = 0;

    /* For generating some neighbors */
    void generateNeighbor() override ;

    void increaseSignificant(int stationID);

    void increaseSlightly(int stationID);

    void increaseNeighbor(vector<array<double,2>> & utilization_ratio_vec, int k);

    void decreaseConfig(vector<array<double,2>> & utilization_ratio_vec, vector<array<double,2>> &history_utilization_records, int k);
/*  Helper functions */
    int findNeighborStation(int i);

    double computeUsedBudget(vector<vector<int>> & config);

    using Heuristics::computeUsedBudget;

    void getThePerformances(Local_Search_Simulation & this_simulation);

    void emptyThePerformances();

    void matchThePerformanceFromSimulation(Local_Search_Simulation & this_simulation);

    void pushBackPerformances(vector<double> & pile_utilization_ratio,int qlength, int totwaittime, double averagewaittime, double averagewaitfrequency);

    void pushBackThresholds(double UR_threshold, double AWF_threshold, double AWT_threshold);

    double computeURT(vector<int> & station_config);

    double computeAWFT(vector<int> & station_config);

    double computeAWTT(vector<int> & station_config);

    double computeAverageChargeRate(vector<int> & station_config);

    void emptyPerformanceThresholds();
public:

    using Heuristics::Heuristics;

    void defineTheThresholds();
    void initializeDesiredUtilizationRatio(double desiredratio);

    void setObjectivechoice(int objectivechoice);

    void run() override ;

    /* Setters and Getters */


};


#endif //CHARING_LOCAL_SEARCH_H
