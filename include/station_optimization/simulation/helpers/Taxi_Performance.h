//
// Created by ZhengchaoWANG on 22.06.18.
//

#ifndef CHARING_TAXI_PERFORMANCE_H
#define CHARING_TAXI_PERFORMANCE_H
//basic class
#include <vector>

#include <iostream>
#include <fstream>
#include <algorithm>

// my class
#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi.h"
#include "station_optimization/simulation/charging_facility/Charging_Station_Manager.h"

using namespace std;

class Taxi_Performance{
private:
    vector<int>                               taxiID;

    // taxi request service related
    vector<int>                         atotaldemand;      // Actual total demand.
    vector<int>                          totaldemand;      // The total number of demand of the taxi.
    vector<int>                         serveddemand;      // Number of demand that is served by the taxi.

    vector<double>                     totaldistance;      // This can be used as an indicator of the total distance in stochatistic case, while in this reproduction period,this is the total distance of demand.
    vector<double>                    serveddistance;      // distance of demand that is served by the taxi.

    vector<int>                            totaltime;
    vector<int>                           servedtime;      // served time of the taxi.

    // taxi income replated
    vector<double>                       taxirevenue;      // the received money of the taxi.
    vector<double>                       taxiprofits;      // The profits of taxis

    vector<vector<int>>            taxiswaitduration;      // the waitduration get from taxis.
    vector<vector<int>>          taxischargeduration;      // charge duration of the taxis.

public:

    Taxi_Performance(){}

    void computeTaxiRevenueProfit(vector<ReProduce_Taxi> & mytaxifleet);

    void getTaxiPerformance(vector<ReProduce_Taxi> &mytaxifleet);

    void addValueToVector(const int & id, const int & totaldemand, const int & serveddemand, const double & totaldistance,
                          const double & serveddistance, const int & totaltime, const int & servedtime, const int & thistaxidemand);

    void addTaxiChargeInfo(const vector<int> & thiswaitduration, const vector<int> & thischargeduration);

    void settotalValue(vector<int> & totaldemand){
        this->atotaldemand = totaldemand;
    }

    void writeTaxiDemandIndicatorToTxt(const string & filename);

    /* Setter and Getters */

    const vector<int> &getTaxiID() const {
        return taxiID;
    }

    void setTaxiID(const vector<int> &taxiID) {
        Taxi_Performance::taxiID = taxiID;
    }

    const vector<int> &getTotaldemand() const {
        return totaldemand;
    }

    void setTotaldemand(const vector<int> &totaldemand) {
        Taxi_Performance::totaldemand = totaldemand;
    }

    const vector<int> &getServeddemand() const {
        return serveddemand;
    }

    void setServeddemand(const vector<int> &serveddemand) {
        Taxi_Performance::serveddemand = serveddemand;
    }

    const vector<double> &getTotaldistance() const {
        return totaldistance;
    }

    void setTotaldistance(const vector<double> &totaldistance) {
        Taxi_Performance::totaldistance = totaldistance;
    }

    const vector<double> &getServeddistance() const {
        return serveddistance;
    }

    void setServeddistance(const vector<double> &serveddistance) {
        Taxi_Performance::serveddistance = serveddistance;
    }

    const vector<int> &getTotaltime() const {
        return totaltime;
    }

    void setTotaltime(const vector<int> &totaltime) {
        Taxi_Performance::totaltime = totaltime;
    }

    const vector<int> &getServedtime() const {
        return servedtime;
    }

    void setServedtime(const vector<int> &servedtime) {
        Taxi_Performance::servedtime = servedtime;
    }

    const vector<double> &getRevenue() const {
        return taxirevenue;
    }

    void setRevenue(const vector<double> &revenue) {
        Taxi_Performance::taxirevenue = revenue;
    }

    const vector<double> &getTaxirevenue() const {
        return taxirevenue;
    }

    void setTaxirevenue(const vector<double> &taxirevenue) {
        Taxi_Performance::taxirevenue = taxirevenue;
    }

};


#endif //CHARING_TAXI_PERFORMANCE_H
