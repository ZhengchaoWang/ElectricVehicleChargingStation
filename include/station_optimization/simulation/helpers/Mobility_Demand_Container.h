//
// Created by ZhengchaoWANG on 10.09.18.
//

#ifndef CHARING_MOBILITY_DEMAND_CONTAINER_H
#define CHARING_MOBILITY_DEMAND_CONTAINER_H

#include "station_optimization/simulation/taxi/taxi_request/Taxi_Request.h
#include "station_optimization/simulation/taxi/Vehicle_Fleet.h"
#include "Initial_Info.h"

class Mobility_Demand_Container:public Initial_Info{
private:
    vector<Taxi_Request>                                                         taxi_requests;
    Vehicle_Fleet                                                                 my_fleet;
};

#endif //CHARING_MOBILITY_DEMAND_CONTAINER_H
