//
// Created by ZhengchaoWANG on 10.09.18.
//

#ifndef CHARING_INITIAL_INFO_H
#define CHARING_INITIAL_INFO_H
#include <vector>
#include <array>
#include <string>
#include <numeric>

#include <iostream>
#include <fstream>

#include <sys/types.h>
#include <dirent.h>
#include <boost/filesystem.hpp>

#include "station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h"

using namespace std;
namespace fs = boost::filesystem ;

class Initial_Info{
private:
    vector<array<double,2>>             station_locations;
    vector<vector<int>>                 stations_initial_config;
    vector<double>                      pile_rate;
    vector<double>                      pile_price;


    string                              station_file_path_name;
    string                              pile_file_path_name;
    string                              station_initial_file_name;

    /* Helpers */
protected:
    string                              taxi_file_path_name;

    vector<string> get_filenames_in( fs::path newpath );
    vector<double> findUsedPileRate(vector<int> & this_config);
    vector<int> findInstalledType(vector<int> &this_config);

public:
    Initial_Info() = default;
    Initial_Info(vector<array<double,2>> & stationlocation, vector<double> & pilerate, vector<double> & pileprice):
            station_locations(stationlocation), pile_rate(pilerate),pile_price(pileprice){};

    virtual void initiateFromFiles();

    void readPilesInfoFromFile();

    void readChargingStationLocationFromFile();

    void readCurrentStationsConfig();

    Charging_Station_General_Manager  getChargingFacilityManager(int starttime);

//    void outputMostUsedDayInfo();

    /*  Setters and Getters */

    void setAllFilesPath (const string & station, const string & pile, const string & station_initial_file, const string & taxi = " ");

    void setStation_file(const string &station_file);

    void setPile_file(const string &pile_file);

    void setTaxi_file_path(const string &taxi_file_path);

    void setStation_initial_file_name(const string &station_initial_file_name);

    const vector<array<double, 2>> &getStation_locations() const;

    const vector<double> &getPile_rate() const;

    const vector<double> &getPile_price() const;

    const vector<vector<int>> &getStations_initial_config() const;
};
#endif //CHARING_INITIAL_INFO_H
