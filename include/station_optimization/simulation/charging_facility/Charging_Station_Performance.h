//
// Created by ZhengchaoWANG on 21.07.18.
//

#ifndef CHARING_CHARGE_FACILITY_PERFORMANCE_H
#define CHARING_CHARGE_FACILITY_PERFORMANCE_H

#include <vector>
#include "Charging_Station_Manager.h"
using namespace std;

class Charging_Station_Performance{
    vector<double>                    stationrevenue;      // revenue of the charging station.
    vector<double>                    stationprofits;      // profits of stations

    vector<int>                     totalqueuelength;      // the waitduration get from the charging facility.
    vector<int>                       maxqueuelength;


public:


    void getFacilityPerformance(Charging_Station_Manager &mychargestations);

    void facilityRevenueProfit(Charging_Station_Manager &mychargestations);



    /* Setters and Getters */



    void setStationrevenue(const vector<double> &stationrevenue) {
        Charging_Station_Performance::stationrevenue = stationrevenue;
    }

    const vector<double> &getStationrevenue() const {
        return stationrevenue;
    }
};

#endif //CHARING_CHARGE_FACILITY_PERFORMANCE_H
