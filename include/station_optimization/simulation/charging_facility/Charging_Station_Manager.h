//
// Created by ZhengchaoWANG on 13.06.18.
//

#ifndef CHARING_CHARGING_FACILITY_MANAGER_H
#define CHARING_CHARGING_FACILITY_MANAGER_H

#include <iostream>
#include <fstream>

#include "Charging_Station.h"
#include <map>

using namespace std;


class Charging_Station_Manager{
    /*
     * For this can be regarded as a set of charging facility.
     * If we want to know the state of the charging facility, we can visit this set and find the desired charging facility.
     *
     * In the future, if we want to implement the competitive game between different charging station operators, we can
     * also regard this as a class of charge facility company
     */

private:
    int                                id_;  // id of this charging facility manager
    map<int, Charging_Station>      cfset; // set of the charging facility under the management of the manager.


public:

    /*  Constructor and destructor*/
    Charging_Station_Manager(int id_, const map<int, Charging_Station> & newcfset): id_(id_), cfset(newcfset){}

    Charging_Station_Manager(int id_) : id_(id_) {}

    //~Charging_Station_Manager(){}

    /* Setter and getters */

/*    const int getId_() const {
        return id_;
    }

    void setId_(int id_) {
        Charging_Station_Manager::id_ = id_;
    }*/

    map<int, Charging_Station> & getCfset() {
        return cfset;
    }

    void setCfset(const  map<int,Charging_Station> &cfset) {
        Charging_Station_Manager::cfset = cfset;
    }

    void outputWaitlist(string filepath);

};



#endif //CHARING_CHARGING_FACILITY_MANAGER_H


