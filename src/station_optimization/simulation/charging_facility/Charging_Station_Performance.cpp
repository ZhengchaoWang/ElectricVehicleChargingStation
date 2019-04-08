//
// Created by ZhengchaoWANG on 21.07.18.
//

#include "station_optimization/simulation/charging_facility/Charging_Station_Performance.h"

void Charging_Station_Performance::facilityRevenueProfit(Charging_Station_Manager &mychargestations) {

    stationrevenue.reserve(mychargestations.getCfset().size());
    stationprofits.reserve(mychargestations.getCfset().size());

    int i = 0;
    for(auto stationiter = mychargestations.getCfset().begin(); stationiter != mychargestations.getCfset().end(); stationiter++){
        stationrevenue[i] = stationiter->second.getTotalrevenue();
        stationprofits[i] = stationrevenue[i] - stationiter->second.getTotalenergycost();
        i++;
    }

}

void Charging_Station_Performance::getFacilityPerformance(Charging_Station_Manager &mychargestations) {
    /*
     * By now, this function is not correct. the result gets does not make sense.
     */
    for(auto stationiter = mychargestations.getCfset().begin(); stationiter != mychargestations.getCfset().end(); stationiter++) {

        int thistotalqueue = 0;
        int maxqueuenumber = 0;

        for (auto waitlistiter = stationiter->second.getWaitnumber().begin();
             waitlistiter != stationiter->second.getWaitnumber().end(); waitlistiter++){
            thistotalqueue += waitlistiter->back();
            maxqueuenumber = max(waitlistiter->back(), maxqueuenumber);
        }
        this->totalqueuelength.push_back(thistotalqueue);
        this->maxqueuelength.push_back(maxqueuenumber);
    }
}