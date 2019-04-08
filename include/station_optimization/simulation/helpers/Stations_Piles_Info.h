//
// Created by ZhengchaoWANG on 31.07.18.
//

#ifndef CHARING_STATIONS_PILES_INFO_H
#define CHARING_STATIONS_PILES_INFO_H

#include <vector>
#include <array>
#include <string>

#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <dirent.h>
#include <boost/filesystem.hpp>

#include "station_optimization/simulation/taxi/Taxi_History.h"
#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.h"
#include "Initial_Info.h"
using namespace std;
namespace fs = boost::filesystem ;

class Stations_Piles_Info: public Initial_Info{
private:
    vector<ReProduce_Taxi_General>      my_fleet;

public:
    using Initial_Info::Initial_Info;

    void initiateFromFiles() override ;

    void readTaxiFleetFromFiles(int chargesimulationalgorithm = 2);

    /*  Setters and Getters */

    vector<ReProduce_Taxi_General> & getMy_fleet();
};



#endif //CHARING_STATIONS_PILES_INFO_H
