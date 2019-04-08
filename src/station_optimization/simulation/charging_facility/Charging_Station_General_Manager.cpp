

#include "station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h"

void Charging_Station_General_Manager::writeStationInfoToTxt(string &path) {
    writeStationUtilizationToTxt(path);
    writeStationQueueInfoToTxt(path);
    writeStationEnergyInfoToTxt(path);
    writeStationUnchargedNum(path);
    writeStationDemandToTxt(path);
}

void Charging_Station_General_Manager::writeStationUtilizationToTxt(string &path) {
    int i = 0;
    for(auto &station : cfset){
        string filename = path + "station" + to_string(i) +"_utilization.txt";
        ofstream outputfile(filename);
        if(outputfile.is_open()){

            for(auto &vec : station.second.getSlotutili()){
                for (auto & value : vec){
                    outputfile << value[0] << "\t" << value[1] << endl;
                }
                outputfile << "another" << "\t" << "another" << endl;
            }
        }else{
            printf("file open failed");
        }
        outputfile.close();
    }
}

void Charging_Station_General_Manager::writeStationQueueInfoToTxt(string &path) {
    int i = 0;
    for(auto &station : cfset){
        string filename = path + "station" + to_string(i) +"_queue.txt";
        ofstream outputfile(filename);
        if(outputfile.is_open()){
            for(auto &vec : station.second.getWaitnumber()){
                for (auto & value : vec){
                    outputfile << value << "\t";
                }
                outputfile << endl;
            }
        }else{
            printf("file open failed");
        }
        outputfile.close();
        i++;
    }
}

void Charging_Station_General_Manager::writeStationEnergyInfoToTxt(string &path) {
    int i = 0;
    for(auto &station : cfset){
        string filename = path + "station" + to_string(i) +"_energy_revenue_info.txt";
        ofstream outputfile(filename);
        if(outputfile.is_open()){
            outputfile << "time" << "\t"<< "energy" << "\t" << "cost" << "\t" << "revenue" << endl;
            for(int j = 0; j < station.second.getEnergycost().size(); j++){
                outputfile << station.second.getTakenenergy()[j].front() << "\t" <<station.second.getTakenenergy()[j].back()
                           << "\t" << station.second.getEnergycost()[j].back() << "\t" << station.second.getChargerevenue()[j].back() <<  endl;
            }
        } else {
            printf("file open failed");
        }
        outputfile.close();
        i++;
    }
}

void Charging_Station_General_Manager::writeStationUnchargedNum(string &path) {
    string filename = path + "station_uncharged_slot.txt";
    ofstream outputfile(filename);
    if(outputfile.is_open()){
        for(auto &station : cfset) {
            int total_uncharged_num = 0;
            for(auto &array_num : station.second.getUnchargedeletenumber()){
                total_uncharged_num += array_num.back();
            }
            outputfile << station.second.getId_() << "\t" << total_uncharged_num << endl;
        }
    }else{
        printf("file open failed");
    }
    outputfile.close();
}

void Charging_Station_General_Manager::writeStationDemandToTxt(string &path) {
    int i = 0;
    for(auto &station : cfset){
        string filename = path + "station" + to_string(i) +"charging_demand.txt";
        ofstream outputfile(filename);
        if(outputfile.is_open()){
            outputfile << "time" << "\t"<< "demand number" << endl;
            for(auto & one_demand : station.second.getStation_charging_demand()){
                outputfile << one_demand.front() << "\t" <<one_demand.back() <<  endl;
            }
        } else {
            printf("file open failed");
        }
        outputfile.close();
        i++;
    }
}

