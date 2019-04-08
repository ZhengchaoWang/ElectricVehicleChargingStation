//
// Created by ZhengchaoWANG on 10.09.18.
//

#ifndef CHARING_SIMULATION_GENERAL_H
#define CHARING_SIMULATION_GENERAL_H

#include <vector>
#include <string>
#include "station_optimization/simulation/helpers/Taxi_Performance_General.h"
#include "station_optimization/simulation/taxi/taxi_request/Taxi_Request.h"
#include "station_optimization/simulation/taxi/taxi_request/Request_Generator.h"

#include "station_optimization/simulation/taxi/taxi_request/Request_Assignment.h"
#include "station_optimization/simulation/taxi/taxi_request/Request_Set.h"
#include "station_optimization/simulation/taxi/Vehicle_Fleet.h"
#include "station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h"

class Simulation_General {
protected:
    Request_Set                                                            served_request_set;
    Request_Set                                                                   request_set;
    Vehicle_Fleet                                                                     all_fleet;
    Vehicle_Fleet                                                              available_fleet;
    Vehicle_Fleet                                                             unavailable_fleet;
    Charging_Station_General_Manager                                                   my_stations;

    Request_Generator                                             request_generator;
    Request_Assignment                                            request_assignment;
    Taxi_Performance_General                            this_simulation_performance;

    int                                                                  begin_time;
    int                                                                    end_time;

    bool                                       output_one_simulation_result = false;
    string                                                              output_path;

public:
    /********************************************* Constructors ***********************************************/
    Simulation_General(Request_Set & request_set, Request_Set & served_request_set, Vehicle_Fleet & my_fleet, Vehicle_Fleet & used_fleets, Charging_Station_General_Manager & stations, int begin_time, int end_time):
        request_set(request_set),served_request_set(served_request_set),all_fleet(my_fleet),available_fleet(my_fleet),unavailable_fleet(used_fleets),
        my_stations(stations), begin_time(begin_time), end_time(end_time), request_assignment(request_set, my_fleet, served_request_set,used_fleets){};

    Simulation_General( Request_Set & request_set, Request_Set & served_request_set, Vehicle_Fleet &my_fleet, Vehicle_Fleet & used_fleets,  Charging_Station_General_Manager &my_stations,
    int begin_time, int end_time, bool output_one_simulation_result,  string &output_path) : request_set(request_set),
            served_request_set(served_request_set), all_fleet(my_fleet),available_fleet(my_fleet), unavailable_fleet(used_fleets), my_stations(my_stations),
            begin_time(begin_time), end_time(end_time), output_one_simulation_result(output_one_simulation_result),
            output_path(output_path),request_assignment(request_set,my_fleet,served_request_set,used_fleets) {};

    Simulation_General( Request_Set & request_set, Request_Set & served_request_set, Vehicle_Fleet &my_fleet, Vehicle_Fleet & used_fleets, Charging_Station_General_Manager &my_stations,
    int begin_time, int end_time,  string &output_path) :request_set(request_set), served_request_set(served_request_set),
                    all_fleet(my_fleet),available_fleet(my_fleet),unavailable_fleet(used_fleets), my_stations(my_stations), begin_time(begin_time),
                    end_time(end_time), output_path(output_path), request_assignment(request_set,my_fleet,served_request_set,used_fleets) {};

    virtual  ~Simulation_General() = default;

    /********************************************* Actions ***********************************************/

    virtual void run();

    virtual void outputSimulationResult();

    void setTaxiAndStation(Vehicle_Fleet& myfleets, Charging_Station_General_Manager & mystations){
        this->available_fleet = myfleets;
        this->my_stations = mystations;
    }

    void setVariables(Request_Set &served_requests, Request_Set &request_set, Vehicle_Fleet &available_fleet, Vehicle_Fleet& unavailable_fleet){
        this->served_request_set = served_requests;
        this->request_set = request_set;
        this->available_fleet = available_fleet;
        this->unavailable_fleet = unavailable_fleet;
    }

    /********************************************* Setters and Getters ***********************************************/
    Vehicle_Fleet & getMy_fleets()  {
        return available_fleet;
    }

    bool isOutput_one_simulation_result() const {
        return output_one_simulation_result;
    }

    void setOutput_one_simulation_result(bool output_one_simulation_result, const string &output_path) {
        Simulation_General::output_one_simulation_result = output_one_simulation_result;
        Simulation_General::output_path = output_path;
    }

    const string &getOutput_path() const {
        return output_path;
    }
};

void Simulation_General::run() {

    int current_time = begin_time;
    while (end_time > current_time){
        //use generator to generate new request.
        Request_Set newrequest;
        request_set.addNewRequestVec(newrequest.getLeft_request_set());

        request_assignment.setRequestAndTaxis(request_set,available_fleet,served_request_set,unavailable_fleet);
        request_assignment.assignRequest();
        this->setVariables(request_assignment.getAssigned_request(),request_assignment.getUnassigned_request(),request_assignment.getAvailable_taxis(),request_assignment.getUsed_taxis());
        for(auto &vehicle:all_fleet.getMy_fleet()){
            if (vehicle->getTime() == current_time){
                vehicle->run();
            }
        }
    }
}

//template<class R, class T, class S>  Simulation_General<R, T, S>
void Simulation_General::outputSimulationResult() {

}


#endif //CHARING_SIMULATION_GENERAL_H
