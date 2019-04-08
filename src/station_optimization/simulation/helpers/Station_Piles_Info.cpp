//
// Created by ZhengchaoWANG on 31.07.18.
//

#include "station_optimization/simulation/helpers/Stations_Piles_Info.h"


void Stations_Piles_Info::readTaxiFleetFromFiles(const int chargesimulationalgorithm) {
    vector<string> filenames =  get_filenames_in(taxi_file_path_name);

    this->my_fleet.reserve(filenames.size());
    int i =0;
    for(const auto & name : filenames){
        ifstream myfile(taxi_file_path_name+name);
        if(myfile.is_open()){
            vector<array<double,2>> position;  position.reserve(10000);

            vector<int> timevector;  timevector.reserve(10000);

            vector<bool> custagvector;  custagvector.reserve(10000);

            vector<double> distancevector;  distancevector.reserve(10000);

            double lat, lon, onedistance, speed;
            int  intime;
            bool cus;

            while(myfile>>lat >> lon >> cus >> intime >> onedistance >> speed){
                array<double,2> positi = {lat,lon};
                position.push_back(positi);

                custagvector.push_back(cus);

                timevector.push_back(intime-1200000000);
                distancevector.push_back(onedistance);
            }
            Taxi_History myhistory = *new Taxi_History(position, custagvector, timevector, distancevector);

            /*
             *The order of inputs is int id_, array<double,2> pos,  double  soc, int cus, int plantime, double chargerate,
             * double capf, double ra, double minSoc, double chargeTo, Taxi_History(myhistory), int myCSH
             *
             * Here, we initiate a taxi with maximum charge rate of 300 kw, a battery capacity of 25 kwh, range anxiety of
             * 0.2, and a minimal soc of 0.1 if the taxi is used to drive a customer. When the taxi tries to charge, he tries
             * to charge to full. The final 0 means that I am at point 0;
             */
            ReProduce_Taxi_General thistaxi = *new ReProduce_Taxi_General(i, position[0], 1.0, custagvector[0], timevector[0], 1.0/12.0, 25, 0.2, 0.1, 1, timevector.back(), myhistory, 0);
            i++;
            thistaxi.findAllDemandNumber();

            thistaxi.findAllDemandTrips();

            thistaxi.setTrackend(myhistory.getTime().back());

            thistaxi.setChargesimulationalgorithm(chargesimulationalgorithm);

            if(chargesimulationalgorithm ==2){
                thistaxi.generateTaxiTour();
            }

            my_fleet.push_back(thistaxi);
        }
    }
}

void Stations_Piles_Info::initiateFromFiles() {
    readPilesInfoFromFile();
    readChargingStationLocationFromFile();
    readCurrentStationsConfig();
    //readTaxiFleetFromFiles();
}


 vector<ReProduce_Taxi_General> & Stations_Piles_Info::getMy_fleet()  {
    return my_fleet;
}