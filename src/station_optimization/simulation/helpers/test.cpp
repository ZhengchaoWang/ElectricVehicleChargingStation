//
// Created by ZhengchaoWANG on 08.08.18.
//

#include <iostream>
#include <fstream>


#include<vector>
#include<string>
#include <array>
#include<deque>
#include <list>

// For time record
#include <ctime>

#include <limits>    // For large value
#include <algorithm> // for min function

#include <sys/types.h>
#include <dirent.h>
#include <boost/filesystem.hpp> // http://www.boost.org/doc/libs/1_61_0/libs/filesystem/doc/index.htm

// self defined class
#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi.h"
#include "station_optimization/simulation/taxi/Taxi_History.h"
#include "station_optimization/simulation/taxi/Taxi_Demand.h"
#include "station_optimization/simulation/taxi/Optimal_Charge_Choice.h"

#include "station_optimization/simulation/charging_facility/Charging_Station.h"
#include "station_optimization/simulation/charging_facility/Charging_Station_Manager.h"

#include "station_optimization/simulation/helpers/Taxi_Performance.h"
//#include "station_optimization/simulation/taxi_helpers/One_Simulation.h"
#include "station_optimization/simulation/helpers/Stations_Piles_Info.h"


using namespace std;

namespace fs = boost::filesystem ;

//Read files
vector<string> get_filenames_in( fs::path newpath ){
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

vector<ReProduce_Taxi>  getTaxiFromData(const string & newpath, const int chargesimulationalgorithm = 2){

    vector<string> filenames =  get_filenames_in(newpath);
    vector<ReProduce_Taxi> mytaxivector(filenames.size());

    int i = 0;
    for(const auto & name : filenames){
        ifstream myfile(newpath+name);
        if(myfile.is_open()){
            vector<array<double,2>> position;
            position.reserve(10000);

            vector<int> timevector;
            timevector.reserve(10000);

            vector<bool> custagvector;
            custagvector.reserve(10000);

            vector<double> distancevector;
            distancevector.reserve(10000);

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
            ReProduce_Taxi thistaxi = *new ReProduce_Taxi(i, position[0], 1.0, custagvector[0], timevector[0], 1.0/12.0, 25, 0.2, 0.1, 1, timevector.back(), myhistory, 0);
            thistaxi.findDemandNumber();

            thistaxi.findDemandsBeginEnd();

            thistaxi.setTrackend(myhistory.getTime().back());

            thistaxi.setChargesimulationalgorithm(chargesimulationalgorithm);

            if(chargesimulationalgorithm ==2){
                thistaxi.generateTaxiTour();
            }

            mytaxivector[i] = thistaxi;
            i++;
        }
    }
    return mytaxivector;
}

Charging_Station_Manager getChargeStationFromData(const string & filename, const int beginingtime) {

    Charging_Station_Manager mychargestations = *new Charging_Station_Manager(0);

    array<int,2> initialarr = {beginingtime,0};

    ifstream myfile(filename);

    if (myfile.is_open()) {

        map<int, Charging_Station> cfset;

        double lat, lon, rate;
        int id, cap;

        while (myfile >> id >> lat >> lon >> cap >> rate) {

            array<double, 2> positi = {lat, lon};

            Charging_Station mychargestation = *new Charging_Station(id, positi, rate/3600.0, cap);
            mychargestation.initiateboth(initialarr);

            cfset.insert(std::pair<int, Charging_Station>(id, mychargestation));
        }
        mychargestations.setCfset(cfset);
    }
    return mychargestations;
}

template <class T> array<int,2> findtimelimit(vector<T> & mytaxifleet){

    int minimumtime = std::numeric_limits<int>::max();
    int maximumtime = 0;
    for(auto iter = mytaxifleet.begin(); iter != mytaxifleet.end(); iter++){
        const int & currentbegin = iter->getTime();
        if (currentbegin < minimumtime){
            minimumtime = currentbegin;
        }

        const int & currentend = iter->getMyhistory().getTime().back();
        if(currentend>maximumtime){
            maximumtime = currentend;
        }
    }
    return {minimumtime,maximumtime};
}


void chargeFacilityAction(vector<ReProduce_Taxi> & mytaxifleet, Charging_Station_Manager & mychargestations, int timeStamp){
    // check if taxi in the waitlist is already at the end of tracking. if is, delete the wait taxi.

    for (auto &cfiter : mychargestations.getCfset()) {

        int totaldeletenumber= 0;
        for(int i = 0; i< cfiter.second.getWaitlist().size();i++){
            int taxiID = cfiter.second.getWaitlist()[i];
            if(mytaxifleet[taxiID].getTrackend() == timeStamp) {
                cfiter.second.departWaitList(taxiID, timeStamp);
                totaldeletenumber++;
                //mytaxifleet[taxiID].getMissedDrive(mytaxifleet[taxiID].getMyhistory().getTime().size() - mytaxifleet[taxiID].getMycurrentstateinhistory());
            }
        }

        if(totaldeletenumber > 0){
            cfiter.second.addUnchargedeletenumber(timeStamp,totaldeletenumber);
        }

        // find the minimum of left charging slots and waitlist;
        int freeslotnum = cfiter.second.getCap()- cfiter.second.getOcupslot();
        int loopnumber  = min(freeslotnum, int(cfiter.second.getWaitlist().size()));

        for(int i = 0; i < loopnumber; i++){
            int taxiID = cfiter.second.getWaitlist().front();
            // If these car already ended, delete it. otherwise, start to charge it from now and try to get next state.
            if(mytaxifleet[taxiID].getTrackend()>=timeStamp){
                // add the wait duration to its vector.
                // How could this happen? the time and current timestamp is the same.
                mytaxifleet[taxiID].addWaitDuration(cfiter.second.getId_(), timeStamp - mytaxifleet[taxiID].getTime());

                // Set the plan and move. This can be done in the vehicle action step. But it will cause two times of
                // getmisseddrive(). Therefore, put it here by now.
                int index = mytaxifleet[taxiID].findIndexInFuture(timeStamp);
                mytaxifleet[taxiID].setThePlan(index,4,0,timeStamp);
                cfiter.second.departWaitList(taxiID,timeStamp);
                mytaxifleet[taxiID].setTime(timeStamp);
            }
        }
    }
}

void taxiAction(vector<ReProduce_Taxi> & mytaxifleet, Charging_Station_Manager & mychargestations, int timeStamp){

    for (auto &taxiiter : mytaxifleet) {

        if(taxiiter.getTime() <= taxiiter.getMyhistory().getTime().back()){
            if (taxiiter.getTime() == timeStamp){
                taxiiter.generatePlan(mychargestations);
                taxiiter.move(mychargestations);
            }
        }
    }
}

/***********************************************************************************
 * Main function
 ***********************************************************************************/
// Main for the point based fixed assignment simulation

void fixedAssignmentSimulation(){
    vector<int> maxcasenumber = {1}; //,5,6

    //string test_directory = "/Volumes/Documents HD/Research/PhD/Data/SFTaxi/testcorrecteddata/";
    string test_directory = "/Volumes/Documents HD/Research/PhD/Data/SFTaxi/spaceSeperatedCorrectedData/";
    //string test_directory = "/Volumes/Documents HD/Research/PhD/Data/SFTaxi/cabspottingdataOriginal " ;

    //vector<ReProduce_Taxi> mytaxifleet = getTaxiFromData(test_directory);

    for (int casenumber : maxcasenumber) {
        vector<ReProduce_Taxi> mytaxifleet = getTaxiFromData(test_directory);
        array<int,2> timelimit = findtimelimit(mytaxifleet);
        string chargestationfile = "/Volumes/Documents HD/Research/PhD/Data/SFTaxi/stationposition/stationposition"+to_string(casenumber)+".txt";
        Charging_Station_Manager mychargestations = getChargeStationFromData(chargestationfile, timelimit[0]);

        /********************************
         * Begin simulation
         ********************************/

        time_t start,timeend;
        time(&start);
        for(int timeStamp = timelimit[0]; timeStamp <= timelimit[1];timeStamp++){

            chargeFacilityAction( mytaxifleet, mychargestations, timeStamp);

            taxiAction(mytaxifleet, mychargestations,timeStamp);
        }

        Taxi_Performance thisperformance;
        thisperformance.getTaxiPerformance(mytaxifleet);
        string outputfile = "/Users/zhengchaowang/Code/C++/resulttour/case"+to_string(casenumber)+"/test.txt";
        thisperformance.writeTaxiDemandIndicatorToTxt(outputfile);

        string outputfilepath = "/Users/zhengchaowang/Code/C++/resulttour/case"+to_string(casenumber)+"/";
        mychargestations.outputWaitlist(outputfilepath);

        time(&timeend);
        double dif = difftime(timeend,start);
        printf("Elasped time is %.2lf seconds", dif);
    }
}

/*
void fixedOptimalSchedule(){
    // this is a toy example to check the correctness of optimal_Charge_Choice.cpp.

    Optimal_Charge_Choice myocc;
    vector<double>     tripdistances = {20,20,2,20,20};
    myocc.setTripdistances(tripdistances);
    vector<int>        tripduration = {3600, 3600, 7200, 3600, 3600};
    myocc.setTripduration(tripduration);

    vector<vector<int>> waitduration;
    vector<int> waitduration1 = {0,0,0,0,0};
    vector<int> waitduration2 = {0,0,4800,0,0};

    for(int i = 0; i < waitduration1.size();i++){
        vector<int> poids = {waitduration1[i],waitduration2[i]};
        waitduration.push_back(poids);
    }
    myocc.setWaittimeatfacilities(waitduration);

    vector<int>                 tripfixreward;
    vector<double>              tripdistancereward;
    vector<double>              emptytripreward;

    vector<bool>                tripcustags = {true, true, false, true, true};
    myocc.setTripcustags(tripcustags);
    vector<vector<double>>      pointtocfdistance; // # of trips+1 * # of charging station
    vector<double> p2cf = {2,2,1,1,2,2};
    vector<double> p2cf1 = {2,2,2,2,2,2};
    for(int i = 0; i < p2cf.size();i++){
        vector<double> poids = {p2cf[i],p2cf1[i]};
        pointtocfdistance.push_back(poids);
    }
    myocc.setPointtocfdistance(pointtocfdistance);

    vector<vector<double>>      cftocfdistance;  // # of charging station * # of charging station
    vector<double> cf2cf = {0, 2};
    cftocfdistance.push_back(cf2cf);
    vector<double> cf2cf1 = {2.0, 0.0};
    cftocfdistance.push_back(cf2cf1);
    myocc.setCftocfdistance(cftocfdistance);

    vector<double>              usedchargerate = {1.0/60.0, 1.0/180.0}; // equal to number of charging station
    myocc.setUsedchargerate(usedchargerate);

    myocc.computeReward();
    myocc.getOptimalChargeChoice();
};*/
