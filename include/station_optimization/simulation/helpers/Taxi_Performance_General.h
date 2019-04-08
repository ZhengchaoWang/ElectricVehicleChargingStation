//
// Created by ZhengchaoWANG on 30.07.18.
//

#ifndef CHARING_TAXI_PERFORMANCE_GENERAL_H
#define CHARING_TAXI_PERFORMANCE_GENERAL_H

#include <vector>

#include <iostream>
#include <fstream>
#include <algorithm>

// my class
#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.h"
#include "station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h"
#include "station_optimization/simulation/helpers/One_Charge.h"

using namespace std;

class Taxi_Performance_General{
private:
    vector<int>                               taxiID;
    int                  objectivefunctionchoice = 0;
    // taxi request service related
    vector<int>                          totalnumber;      // Total trips travelled by a taxi during one day.
    vector<int>                         demandnumber;      // The total number of demand of the taxi.
    vector<int>                   serveddemandnumber;      // Number of demand that is served by the taxi.

    vector<double>                     totaldistance;      // The total distance traveled by one taxi during the day
    vector<double>               totaldemanddistance;      // This can be used as an indicator of the total distance in stochatistic case, while in this reproduction period,this is the total distance of demand.
    vector<double>                    serveddistance;      // distance of demand that is served by the taxi.

    vector<int>                            totaltime;      // Total time a taxi is on tracking
    vector<int>                      totaldemandtime;
    vector<int>                           servedtime;      // served time of the taxi.

    // taxi income replated
    vector<double>                   originalrevenue;
    vector<double>                       taxirevenue;      // the received money of the taxi.
    vector<double>                       taxiprofits;      // The profits of taxis
    double                             revenue_ratio;

    vector<vector<int>>            taxiswaitduration;      // the waitduration get from taxis.
    vector<vector<int>>          taxischargeduration;      // charge duration of the taxis.

    double                               alpha = 0.4;      // for the missed demand time. and for the variables.
    double                            beta = 1.0/2.0;      // for the waiting time.
    double                           gamma = 1.0/8.0;      // for the charging time.
    double                             performance=0;

    // Charging station performance related
    vector<int>                     max_queue_length;
    vector<int>                      total_wait_time;
    vector<int>                    total_charge_time;
    vector<double>                 average_wait_time;      // How long a vehicle wait in the stations.
    vector<int>                    total_wait_number;      // How long there is a new wait vehicle comes
    vector<vector<double>> all_utilization_time_rate;      // The area under the used number and

    vector<One_Charge>                   most_charge;
    /* Some self used functions */


    template<typename T1, size_t T2>
    bool arraiesComare(array<T1,T2> & my_array1, array<T1,T2> & my_array2);

    int findMaximumQueueLength(const vector<array<int,2>> & my_queue_series);

    void addValueToVector(const int & id, const int & demandnumber, const int & serveddemandnumber, const double & demanddistance,
                          const double & serveddistance, const int & demandtime, const int & servedtime,
                          const int& totaltripnumber, const double& totaldistance, const int& totaltime);

    void addTaxiChargeInfo(const vector<int> & thiswaitduration, const vector<int> & thischargeduration);

public:

    /* Constructors */
    Taxi_Performance_General() = default;

    /* Actions */
    void computePerformance(vector<ReProduce_Taxi_General>& mytaxifleet);

    template<class T> void computeTaxiRevenueProfit(vector<T> & mytaxifleet);

    template<class T> void getTaxiPerformance(vector<T> &mytaxifleet);

    void computeChargingStationIndex(Charging_Station_General_Manager & mystations, int simulation_span);

    template <class T>
    void computeStationUtilizationRatio(T & my_stations, int simulation_span);

    template <class T>
    void computeStationWaitIndex(T & my_stations);

    template <class T>
    void computeStationRevenue( T & my_stations);


    void writeTaxiDemandIndicatorToTxt(const string & filename);

    void writeStationSummaryIndicatorToTxt(const string & filename);

    void writeTaxiChargingDemandsToTxt(const string &filename);

    /* Setter and Getters */

    double getPerformance() const {
        return performance;
    }

    const vector<int> &getQueue_length() const {
        return max_queue_length;
    }

    const vector<int> &getTotal_wait_time() const {
        return total_wait_time;
    }

    const vector<vector<double>> &getAll_utilization_time_rate() const {
        return all_utilization_time_rate;
    }

    const vector<int> &getTotaldemand() const {
        return demandnumber;
    }

    const vector<int> &getServeddemand() const {
        return serveddemandnumber;
    }

    const vector<double> &getTotaldistance() const {
        return totaldemanddistance;
    }

    const vector<double> &getServeddistance() const {
        return serveddistance;
    }

    const vector<int> &getTotaltime() const {
        return totaldemandtime;
    }

    const vector<int> &getServedtime() const {
        return servedtime;
    }

    const vector<double> &getAverage_wait_time() const {
        return average_wait_time;
    }

    const vector<int> &getTotal_wait_number() const {
        return total_wait_number;
    }

    void setObjectivefunctionchoice(int objective_function) {
        Taxi_Performance_General::objectivefunctionchoice = objective_function;
    }

    void setAlpha(double alpha) {
        Taxi_Performance_General::alpha = alpha;
    }
};

#endif //CHARING_TAXI_PERFORMANCE_GENERAL_H
