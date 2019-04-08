//
// Created by ZhengchaoWANG on 27.07.18.
//

#ifndef CHARING_TAXI_FLEET_H
#define CHARING_TAXI_FLEET_H

#include <sys/types.h>
#include <dirent.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <vector>
#include <string>
#include <array>


#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.h"
#include "Taxi_History.h"

using  namespace std;

namespace fs = boost::filesystem;

class ReProduce_Taxi_Fleet{
private:
    int                                     id;
    vector<ReProduce_Taxi_General>          my_fleet;
    string                                  path;
    double                                  total_distance=0;
    bool                                    initialized = false;

public:
    /* Constructors */
    ReProduce_Taxi_Fleet( int id, string & my_path ):path(my_path),id(id){};

    /*
     * Initiate all the fleets from files
     */

    void initiate_from_files(){
        readMyFleetFromFiles();
    };

    vector<string> get_filenames_in( fs::path datapath ){
        boost::filesystem::path full_path( boost::filesystem::current_path() );
        //std::cout << "Current path is : " << full_path << std::endl;
        vector<string> filenames;

        //cout << datapath << std::endl;
        string myextension = ".txt";
        for (fs::directory_iterator itr(datapath); itr!=fs::directory_iterator(); ++itr)
        {
            if( itr->path().filename().extension() == myextension){
                filenames.push_back(itr->path().filename().string());
            }
        }
        return filenames;
    };

    void readMyFleetFromFiles(const int chargesimulationalgorithm = 2){

        vector<string> filenames =  get_filenames_in(path);
        my_fleet.reserve(filenames.size());
        for(int i = 0; i < filenames.size(); i++){
//            my_fleet.push_back(ReProduce_Taxi_General());
            my_fleet.emplace_back();
        }

        int i = 0;
        for(const auto & name : filenames){
            /*string number = name.substr(0, name.find('.'));
            int i = stoi(number);*/

            ifstream myfile(path+name);
            if(myfile.is_open()){
                vector<array<double,2>> position;
                position.reserve(10000);

                vector<int> timevector;
                timevector.reserve(10000);

                vector<bool> custagvector;
                custagvector.reserve(10000);

                vector<double> distancevector;
                distancevector.reserve(10000);

//                vector<double> speedvector;
//                speedvector.reserve(10000);

                double lat, lon, onedistance;
                int  intime;
                bool cus;

                while(myfile>>lat >> lon >> cus >> intime >> onedistance){
                    if(intime <86400){
                        array<double,2> positi = {lat,lon};
                        position.push_back(positi);

                        custagvector.push_back(cus);

                        timevector.push_back(intime);
                        distancevector.push_back(onedistance);
                    }
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
                thistaxi.findAllDemandNumber();

                thistaxi.findAllDemandTrips();

                thistaxi.setTrackend(myhistory.getTime().back());

                thistaxi.setChargesimulationalgorithm(chargesimulationalgorithm);

                if(chargesimulationalgorithm ==2){
                    thistaxi.generateTaxiTour();
                }
                my_fleet[i] = thistaxi;
            }
            i++;
        }
    }

    void computeTotalDistance(){
        total_distance = 0;
        for(auto & my_taxi : my_fleet){
            double tracking_distance = 0.0;
            for(auto & my_trip : my_taxi.getMytour().getTrips()){
                tracking_distance += my_trip.getTriplength();
            }
            total_distance += tracking_distance;
            my_taxi.setMy_tracking_distance(tracking_distance);
        }
    }

    void findThelongestDistanceDays(){
        int taxiID = 0;
        for(auto & my_taxi : my_fleet){

            double  distance = 0;
            int current_last_index = 0;

            double longest_distance = 0;
            int longest_day_begin_index_in_data = 0, longest_day_end_index_in_data = 0;

            bool second_time_end = false;

            if(my_taxi.getId_() == 207){
                int bala = 0;
            }
            for(auto & my_trip : my_taxi.getMytour().getTrips()){

                while(current_last_index<my_taxi.getMytour().getTrips().size()){
                    if((my_taxi.getMytour().getTrips()[current_last_index].getBegintime()-my_trip.getBegintime() <86400) &
                            (my_taxi.getMytour().getTrips()[current_last_index].getEndtime()-my_trip.getBegintime() < 95000 )){
                        distance+=my_taxi.getMytour().getTrips()[current_last_index].getTriplength();
                        current_last_index++;
                    } else {
                        break;
                    }
                }
                if((current_last_index ==my_taxi.getMytour().getTrips().size()) & !second_time_end){
                    second_time_end = true;
                } else if ((current_last_index ==my_taxi.getMytour().getTrips().size()) & second_time_end) {
                    break;
                }
                if(distance>longest_distance){
                    longest_distance = distance;
                    longest_day_begin_index_in_data = my_trip.getBeginendindex().front();
                    longest_day_end_index_in_data = my_taxi.getMytour().getTrips()[current_last_index-1].getBeginendindex().back();
                }
                distance -= my_trip.getTriplength();
            }
            outputLongestDayData(longest_day_begin_index_in_data, longest_day_end_index_in_data, taxiID);
            taxiID++;
        }
    }

     vector<ReProduce_Taxi_General> & getMy_fleet() {
        return my_fleet;
    }

    void outputLongestDayData(int begin_index, int end_index, int taxiID){

        string outputfilename = path + "onedaydatalongestdistance/" + to_string(taxiID) +".txt";
        ofstream outputfile(outputfilename);
        if(outputfile.is_open()){

            outputfile.precision(8);
            outputfile.setf(ios::fixed);
            outputfile.setf(ios::showpoint);

            int start_time = my_fleet[taxiID].getMyhistory().getTime()[begin_index];
            for(int i = begin_index; i <= end_index; i++){

                outputfile << setprecision(5)  << my_fleet[taxiID].getMyhistory().getPoss()[i][0] << "\t" << setprecision(5) << my_fleet[taxiID].getMyhistory().getPoss()[i][1] << "\t" <<
                          my_fleet[taxiID].getMyhistory().getCustags()[i] <<"\t" << my_fleet[taxiID].getMyhistory().getTime()[i]-start_time <<"\t"
                          << setprecision(8) << my_fleet[taxiID].getMyhistory().getDistance()[i]<<endl;
            }
        } else{
            printf("file open failed");
        }
        outputfile.close();
    }

    double getTotal_distance() const {
        return total_distance;
    }

    bool isInitialized() {
        return initialized;
    }

    void setInitialized(bool initialized) {
        ReProduce_Taxi_Fleet::initialized = initialized;
    }
};

#endif //CHARING_TAXI_FLEET_H
