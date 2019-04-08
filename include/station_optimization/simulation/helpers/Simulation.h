//
// Created by ZhengchaoWANG on 07.08.18.
//

#ifndef CHARING_SIMULATION_H
#define CHARING_SIMULATION_H

#include <vector>

#include <sys/types.h>
#include <dirent.h>
#include <boost/filesystem.hpp>

#include "station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h"
#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.h"
#include "Taxi_Performance_General.h"

using namespace std;

namespace fs = boost::filesystem ;

class Simulation {

protected:
    vector<ReProduce_Taxi_General>                                         my_fleet;
    Charging_Station_General_Manager                                    my_stations;

    Taxi_Performance_General                            this_simulation_performance;

    int                                                                  begin_time;
    int                                                                    end_time;

    bool                                       output_one_simulation_result = false;
    string                                                              output_path;
    int                                                 objectivefunctionchoice = 0;

public:
    /* Constructors and distructors */


    Simulation() = default;

    Simulation(vector<ReProduce_Taxi_General> & fleets, Charging_Station_General_Manager & stations, int begin_time, int end_time):
            my_fleet(fleets),my_stations(stations), begin_time(begin_time), end_time(end_time){};

    Simulation( vector<ReProduce_Taxi_General> &my_fleet,  Charging_Station_General_Manager &my_stations,
               int begin_time, int end_time, bool output_one_simulation_result,  string &output_path) : my_fleet(
            my_fleet), my_stations(my_stations), begin_time(begin_time), end_time(end_time), output_one_simulation_result(
                    output_one_simulation_result), output_path(output_path) {};

    Simulation( vector<ReProduce_Taxi_General> &my_fleet,  Charging_Station_General_Manager &my_stations,
               int begin_time, int end_time,  string &output_path) : my_fleet(my_fleet), my_stations(my_stations),
                                                                          begin_time(begin_time), end_time(end_time),
                                                                          output_path(output_path) {}

    virtual  ~Simulation() = default;
    /* Actions */


    virtual void stationAction(int current_time){};

    virtual void taxiAction(int current_time){};

    virtual void run(){};

    virtual void outputSimulationResult(){};

    void setTaxiAndStation(vector<ReProduce_Taxi_General>& myfleets, Charging_Station_General_Manager & mystations){
        this->my_fleet = myfleets;
        this->my_stations = mystations;
    }

    /* Setters and Getters */

    vector<ReProduce_Taxi_General> & getMy_fleets()  {
        return my_fleet;
    }

    bool isOutput_one_simulation_result() const {
        return output_one_simulation_result;
    }

    void setOutput_one_simulation_result(bool output_one_simulation_result, const string &output_path) {
        Simulation::output_one_simulation_result = output_one_simulation_result;
        Simulation::output_path = output_path;
    }

    const string &getOutput_path() const {
        return output_path;
    }

    void setObjectivefunctionchoice(int objectivefunctionchoice) {
        Simulation::objectivefunctionchoice = objectivefunctionchoice;
    }
};

#endif //CHARING_SIMULATION_H
