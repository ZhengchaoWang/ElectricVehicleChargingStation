//
// Created by ZhengchaoWANG on 07.09.18.
// Here, I change a mini_distance from INT_MAX to 100000000.0;
//

#ifndef CHARING_REQUEST_ASSIGNMENT_H
#define CHARING_REQUEST_ASSIGNMENT_H

#include <vector>

#include "Request_Set.h"
#include "station_optimization/simulation/taxi/Vehicle_Fleet.h"
#include "station_optimization/simulation/taxi/normal_taxi/Taxi.h"
class Request_Assignment{
private:
    Request_Set                                                                  unassigned_request;
    Vehicle_Fleet                                                                available_taxis;
    Request_Set                                                                  assigned_request;
    Vehicle_Fleet                                                                used_taxis;

    int                                                                        assignmentalgorithm = 0;

    double                                                                     mini_distance_threshold = 0.1; // km

    double twoPointsDistance(array<double, 2> pos1, array<double, 2> pos2);

public:
    /********************************************* Constructors ***********************************************/

    Request_Assignment( Request_Set &unassigned_request,  Vehicle_Fleet &available_taxis,Request_Set &assigned_request,  Vehicle_Fleet &used_taxis,
                        int assignmentalgorithm) : unassigned_request(unassigned_request), available_taxis(available_taxis),
                                                   assigned_request(assigned_request), used_taxis(used_taxis),
                                                   assignmentalgorithm(assignmentalgorithm) {};
    Request_Assignment(Request_Set & unassigned_request, Vehicle_Fleet & available_taxis,Request_Set &assigned_request,  Vehicle_Fleet &used_taxis):
            unassigned_request(unassigned_request), available_taxis(available_taxis),assigned_request(assigned_request), used_taxis(used_taxis){};
    /********************************************* Algorithms ***********************************************/
    void assignRequest();

    /********************************************* Getters and setters ***********************************************/
    Request_Set &getUnassigned_request() {
        return unassigned_request;
    }

    void setUnassigned_request( Request_Set &unassigned_request) {
        Request_Assignment::unassigned_request = unassigned_request;
    }

    Vehicle_Fleet &getAvailable_taxis() {
        return available_taxis;
    }

    void setAvailable_taxis( Vehicle_Fleet &available_taxis) {
        Request_Assignment::available_taxis = available_taxis;
    }

    void setRequestAndTaxis(Request_Set &unassigned_request,Vehicle_Fleet &available_taxis,Request_Set &assigned_request,  Vehicle_Fleet &used_taxis){
        Request_Assignment::unassigned_request = unassigned_request;
        Request_Assignment::available_taxis = available_taxis;
        Request_Assignment::assigned_request = assigned_request;
        Request_Assignment::used_taxis = used_taxis;
    }

    Request_Set &getAssigned_request() {
        return assigned_request;
    }

    Vehicle_Fleet &getUsed_taxis() {
        return used_taxis;
    }
};

void Request_Assignment::assignRequest() {
    if(unassigned_request.isRequestLeft()){
        for(auto request_iter = unassigned_request.getLeft_request_set().begin(); request_iter != unassigned_request.getLeft_request_set().end(); request_iter++){
            double trip_distance = twoPointsDistance((*request_iter)->getOrigin(), (*request_iter)->getDestination());
            double mini_distance = 100000000.0;
            vector<shared_ptr<Taxi>>::iterator cloest_taxi;
            for(auto taxi_iter = available_taxis.getMy_fleet().begin(); taxi_iter!= available_taxis.getMy_fleet().end(); taxi_iter++){
                double taxi_to_request = twoPointsDistance((*taxi_iter)->getPos(), (*request_iter)->getOrigin());
                double estimated_left_soc = (*taxi_iter)->getSoc() - (taxi_to_request + trip_distance)*(*taxi_iter)->getConsumrate()/(*taxi_iter)->getCapf();
                if ((mini_distance > taxi_to_request) & (estimated_left_soc > (*taxi_iter)->getMinSoc())){
                    mini_distance = taxi_to_request;
                    cloest_taxi = taxi_iter;
                }
            }
            if(mini_distance < mini_distance_threshold){
                unassigned_request.eraseARequest(request_iter);
                available_taxis.erase_member(cloest_taxi);
                (*cloest_taxi)->setMy_future_request((*request_iter));
                (*cloest_taxi)->setCustomer(true);
            }
        }
    }
}

double Request_Assignment::twoPointsDistance(array<double, 2> pos1, array<double, 2> pos2) {

    double dlat = (pos2[0]-pos1[0]) * M_PI/180;
    double dlon = (pos2[1]-pos1[1]) * M_PI/180;
    double a = pow(sin(dlat/2),2) + cos(pos1[0]*M_PI/180)*cos(pos2[0]*M_PI/180)*pow(sin(dlon/2),2);
    double distance = 2*atan2(sqrt(a),sqrt(1.0-a)) * 6378.135;

    return distance;
}

#endif //CHARING_REQUEST_ASSIGNMENT_H
