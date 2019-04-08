//
// Created by ZhengchaoWANG on 01.07.18.
//

#include "station_optimization/simulation/charging_facility/Charging_Station_Manager.h"

void Charging_Station_Manager::outputWaitlist(string filepath) {
    int i = 0;
    for(auto iter = cfset.begin(); iter !=cfset.end(); iter++){
        string filename = filepath + "chargingstationwaitlist" + to_string(i) + ".txt";
        ofstream outputfile(filename);
        if(outputfile.is_open()){
            outputfile <<"charging station ID" << "\t" << "station configuration" <<endl;
            outputfile << iter->second.getId_() << "\t" << cfset.size() <<endl;
            outputfile << "time" << "\t" << "waitnumber" << endl;

            for(int j = 0; j < iter->second.getWaitnumber().size(); j++){
                outputfile<< iter->second.getWaitnumber()[j][0] << "\t" << iter->second.getWaitnumber()[j][1]  <<endl;
            }
        } else{
            printf("file open failed");
        }
        outputfile.close();

        string filenamechargelist = filepath + "chargingstationchargelist" + to_string(i) + ".txt";
        ofstream outputfilecharge(filenamechargelist);
        if(outputfilecharge.is_open()){
            outputfilecharge <<"charging station ID" << "\t" << "station configuration" <<endl;
            outputfilecharge << iter->second.getId_() << "\t" << cfset.size() <<endl;
            outputfilecharge << "time" << "\t" << "utilize number" << endl;

            for(int j = 0; j < iter->second.getSlotutili().size(); j++){
                outputfilecharge<< iter->second.getSlotutili()[j][0] << "\t" << iter->second.getSlotutili()[j][1]  <<endl;
            }
        } else{
            printf("file open failed");
        }
        outputfilecharge.close();
        i++;
    }
}


