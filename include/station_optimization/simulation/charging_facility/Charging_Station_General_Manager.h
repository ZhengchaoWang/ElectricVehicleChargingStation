//
// Created by ZhengchaoWANG on 29.07.18.
//

#ifndef CHARING_CHARGING_FACILITY_MULTI_TYPES_MANAGER_H
#define CHARING_CHARGING_FACILITY_MULTI_TYPES_MANAGER_H

#include <iostream>
#include <fstream>
#include <string>

#include "Charging_Station_General.h"
#include <map>

class Charging_Station_General_Manager{
private:

    int                                      id{};
    map<int, Charging_Station_General>      cfset; // set of the charging facility under the management of the manager.

public:
    Charging_Station_General_Manager() = default;

    explicit Charging_Station_General_Manager(int id_):id(id_){};

    Charging_Station_General_Manager(int id_, map<int, Charging_Station_General> & newcfset): id(id_), cfset(newcfset){}

    int getId() const {
        return id;
    }

    void setId(int id) {
        Charging_Station_General_Manager::id = id;
    }

    map<int, Charging_Station_General> &getCfset() {
        return cfset;
    }

    void setCfset(const map<int, Charging_Station_General> &cfset) {
        Charging_Station_General_Manager::cfset = cfset;
    }

    /*
     * Output station information.
     */
    void writeStationInfoToTxt(string & path);

    void writeStationUtilizationToTxt(string & path);

    void writeStationDemandToTxt(string &path);

    void writeStationQueueInfoToTxt(string & path);

    void writeStationEnergyInfoToTxt(string & path);

    void writeStationUnchargedNum(string &path);



};

#endif //CHARING_CHARGING_FACILITY_MULTI_TYPES_MANAGER_H
