//
// Created by ZhengchaoWANG on 10.09.18.
//

//
// Created by ZhengchaoWANG on 31.07.18.
//

#include "station_optimization/simulation/helpers/Initial_Info.h"

void Initial_Info::readPilesInfoFromFile() {
    ifstream myfile(pile_file_path_name);
    if (myfile.is_open()) {
        double price, rate;
        int type;

        while (myfile >> type >> rate >> price) {
            pile_price.push_back(price);
            pile_rate.push_back(rate/3600);
        }
    } else {
        cout <<"fail to open the pile infomation file" << endl;
    }
}

void Initial_Info::readChargingStationLocationFromFile() {
    ifstream myfile(station_file_path_name);

    vector<array<double,2>> locations;
    if (myfile.is_open()) {

        double lat, lon;
        int id;

        while (myfile >> id >> lat >> lon) {
            station_locations.push_back({lat, lon});
        }
    } else {
        cout << "fail to open the station location files" << endl;
    }
}

void Initial_Info::readCurrentStationsConfig() {
    ifstream myfile(station_initial_file_name);

    if(myfile.is_open()){
        int id, config1, config2, config3;
        double lati, lon;
        myfile.ignore(numeric_limits<streamsize>::max(),'\n');
        while(myfile >>id>> lati >> lon >> config1 >> config2 >> config3){
            stations_initial_config.push_back({config1,config2,config3});
        }
    } else {
        cout << "fail to open the station configuaration files" << endl;
    }
}

void Initial_Info::initiateFromFiles(){
    readPilesInfoFromFile();
    readChargingStationLocationFromFile();
    readCurrentStationsConfig();
    //readTaxiFleetFromFiles();
}

/*  Helpers */
vector<string> Initial_Info::get_filenames_in(fs::path newpath) {
    boost::filesystem::path full_path( boost::filesystem::current_path() );
    std::cout << "Current path is : " << full_path << std::endl;
    vector<string> filenames;

    cout << newpath << std::endl;
    string myextension = ".txt";
    for (fs::directory_iterator itr(newpath); itr!=fs::directory_iterator(); ++itr)
    {
        if( itr->path().filename().extension() == myextension){
            filenames.push_back(itr->path().filename().string());
        }
    }
    return filenames;
};


Charging_Station_General_Manager Initial_Info::getChargingFacilityManager(int starttime) {
    Charging_Station_General_Manager mychargestations = Charging_Station_General_Manager(0);
    map<int, Charging_Station_General> cfset;
    int used_station = 0, index = 0;

    vector<vector<int>>* station_config;
    station_config = & stations_initial_config;
    for (auto &i : *station_config) {
        if(accumulate(i.begin(), i.end(), 0) > 0){
            vector<double> this_rate = findUsedPileRate(i);
            vector<int> this_types = findInstalledType(i);
            Charging_Station_General mychargestation(used_station, station_locations[index], this_rate, this_types);
            mychargestation.initiateDefaultElements(starttime);
            cfset.insert(std::pair<int, Charging_Station_General>(used_station, mychargestation));
            used_station++;
        }
        index++;
    }
    mychargestations.setCfset(cfset);
    return mychargestations;
}

/* Setters and Getters */

void Initial_Info::setAllFilesPath(const string &station, const string &pile, const string & station_config, const string &taxi) {
    setStation_file(station);
    setPile_file(pile);
    setStation_initial_file_name(station_config);
    setTaxi_file_path(taxi);
}

void Initial_Info::setStation_file(const string & station_file) {
    Initial_Info::station_file_path_name = station_file;
}

void Initial_Info::setPile_file(const string & pile_file) {
    Initial_Info::pile_file_path_name = pile_file;
}

void Initial_Info::setTaxi_file_path(const string &taxi_file_path) {
    Initial_Info::taxi_file_path_name = taxi_file_path;
}

void Initial_Info::setStation_initial_file_name(const string &station_initial_file_name) {
    Initial_Info::station_initial_file_name = station_initial_file_name;
}

const vector<array<double, 2>> &Initial_Info::getStation_locations() const {
    return station_locations;
}

const vector<double> &Initial_Info::getPile_rate() const {
    return pile_rate;
}

const vector<double> &Initial_Info::getPile_price() const {
    return pile_price;
}

const vector<vector<int>> &Initial_Info::getStations_initial_config() const {
    return stations_initial_config;
}

/*void Initial_Info::outputMostUsedDayInfo() {
    string path = "/Volumes/Documents HD/Research/PhD/Data/SFTaxi/oneDayData/";

    double best_length = 0.0;
    vector<int> best_begin_index;
    vector<int> best_end_index;

    double total_distance;
    int total_trips;
    vector<int> current_index(my_fleet.size(),0);
    vector<int> current_end_index(my_fleet.size(),0);

    array<int, 2> timelimit =findTimeLimit();
    for(int i = timelimit[0]; i <= timelimit[1] - 24*3600; i++ ){
        total_distance = 0;
        total_trips = 0;
        for( int j = 0; j< my_fleet.size(); j++){
            if(my_fleet[j].getMytour().getTrips().back().getBegintime() >=i){
                for(int k = current_index[j]; k < my_fleet[j].getMytour().getTrips().size(); k++){
                    if(my_fleet[j].getMytour().getTrips()[k].getBegintime() < i){
                        continue;
                    } else if((my_fleet[j].getMytour().getTrips()[k-1].getBegintime() < i) & (my_fleet[j].getMytour().getTrips()[k].getBegintime() >= i)){
                        total_distance += my_fleet[j].getMytour().getTrips()[k].getTriplength();
                        total_trips ++;
                        current_index[j] = k;
                        if(k == (my_fleet[j].getMytour().getTrips().size() -1)){
                            current_end_index[j] = k;
                        }
                    } else if(my_fleet[j].getMytour().getTrips()[k].getBegintime() < (i + 86400)){
                        total_distance += my_fleet[j].getMytour().getTrips()[k].getTriplength();
                        total_trips ++;
                        if(k == (my_fleet[j].getMytour().getTrips().size() -1)){
                            current_end_index[j] = k;
                        }
                    } else if (my_fleet[j].getMytour().getTrips()[k].getBegintime() > (i + 86400)){
                        current_end_index[j] = k-1;
                        break;
                    }
                }
            } else {
                current_index[j] = INT_MAX;
                current_end_index[j] = INT_MAX;
            }

        }
        if(total_distance > best_length){
            best_length = total_distance;
            best_begin_index = current_index;
            best_end_index = current_end_index;
        }
    }

    ofstream outputfile(path+"onedaydataindex.txt");
    if(outputfile.is_open()){
        for(int i = 0; i<best_begin_index.size();i++){
            if(best_begin_index[i] != INT_MAX){
                outputfile<< i << "\t" << my_fleet[i].getMytour().getTrips()[best_begin_index[i]].getBeginendindex()[0]
                          <<"\t" << my_fleet[i].getMytour().getTrips()[best_end_index[i]].getBeginendindex()[1] <<"\t"
                          << best_begin_index[i] << "\t" << best_end_index[i] <<endl;
            }
        }
    } else{
        printf("file open failed");
    }
    outputfile.close();
}*/

vector<double> Initial_Info::findUsedPileRate(vector<int> & this_config) {
    vector<double> this_vector;

    for(int i =0 ; i<this_config.size(); i++){
        if(this_config[i]>0){
            this_vector.push_back(pile_rate[i]);
        }
    }
    return this_vector;
}

vector<int> Initial_Info::findInstalledType(vector<int> &this_config) {
    vector<int> this_vector;
    for (int i : this_config) {
        if(i >0){
            this_vector.push_back(i);
        }
    }
    return this_vector;
}


