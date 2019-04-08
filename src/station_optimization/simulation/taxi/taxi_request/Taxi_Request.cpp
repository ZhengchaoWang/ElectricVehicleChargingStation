//
// Created by ZhengchaoWANG on 19.07.18.
//

#include "station_optimization/simulation/taxi/taxi_request/Taxi_Request.h"


double Taxi_Request::computeDistance(array<double, 2> pos1, array<double, 2> pos2) {

    double dlat = (pos2[0]-pos1[0])/M_PI;
    double dlon = (pos2[1]-pos1[0])/M_PI;
    double a = pow(sin(dlat/2),2) + cos(pos1[0]/M_PI)*cos(pos2[0]*M_PI/180)*pow(sin(dlon/2),2);
    double distance = 2*atan2(sqrt(a),sqrt(1.0-a)) * 6378.135;

    return distance;
}

void Taxi_Request::computeDirectlength() {

    this->length = computeDistance(this->origin, this->destination);
}

void Taxi_Request::requestServedTaxiID(int taxiID) {
    this->serviceprovidetaxiID = taxiID;
}
