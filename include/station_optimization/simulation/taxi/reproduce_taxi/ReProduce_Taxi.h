/* ReProduce_Taxi.h */
// Created by ZhengchaoWANG on 13.06.18.
//

#ifndef CHARING_TAXI_H
#define CHARING_TAXI_H

// base class
#include "station_optimization/simulation/taxi/Vehicle.h"

//dependent class
#include "station_optimization/simulation/helpers/Plan.h"
#include "station_optimization/simulation/taxi/Taxi_History.h"
#include "station_optimization/simulation/taxi/Taxi_Demand.h"
#include "ReProduce_Taxi_Tour.h"

#include "math.h"
#include <array>
#include <string>
#include <numeric> // for the sum of vector
#include <algorithm> // for min function
#include <iostream>

using namespace std;

class ReProduce_Taxi : public Vehicle {
private:
    // State of the taxi, including pos, soc, drivespeed, and.
    int                              cus; // A bool value to indicate whether passengers on board.
    int          mycurrentstateinhistory; // To shychronize with the measure history.
    int                      tripindex=0;
    /******* Properties of this type of taxi. *******************/
    // The charge simulation algorithm. Thereshold based, or operation based
    int    chargesimulationalgorithm = 1; // To control which method is used to generate Plan.
    // Charging duration choice algorithm, more related Threshold based simulation algorithm
    int      chargedurationalgorithm = 1; // 1, charge to a certain level. 2, optimize the charge level (might be hard to decide).
    // To compute the revenue of each trips
    double        fixedtripcharge = 15.0; // If a trip is taken, then this taxi can receive this amount of money for the first "minimumdistance".
    double         minimumdistance = 3.0; // The first 3.0 km will not be charged additional cost.
    double        chargeperdistance= 1.5; // 1.5 per km after 3.0 km.

    int                  largegap = 3600; // This is a large value than which the neighboring points is larger, they will try to search for charging possibility.
    int                 foresight = 3600; // this is a threshold of vehicle foresight to see the future.
    // the following trajectories of this type of taxi.
    int                     trackend = 0; // The end of the tracking data so that we can know the end of the simulation.
    int                 demandnumber = 0; // Total taxi demand of this taxi in the tracking.
    vector<array<int,2>>       demandID;   // For test.
    Taxi_History               myhistory; // to remember the history of myself.
    ReProduce_Taxi_tour           mytour; // a vector of trips that the taxi takes out.


    // performance indicator of this type of taxi.
    vector<Taxi_Demand>               td;  // The total demand of a taxi. We can ignore this, if we get a mytour elements


public:

    ReProduce_Taxi () {}

    ReProduce_Taxi (int id_, array<double, 2> pos, double soc, int cus, int time, double chargerate, double capf,
                    double ra, double minSoc, double chargeTo, int trackend, Taxi_History(myhistory), int myCSH) :
                            Vehicle(id_, pos, chargerate, soc, time, ra, minSoc, chargeTo), cus(cus),
                            trackend(trackend), myhistory(myhistory), mycurrentstateinhistory(myCSH) {}


    /******************************************** Action methods  *****************************************************/

    //generate a plan for taxi to tell them what to do next
    void generatePlan (Charging_Station_Manager &cfm, const int & threshold = 3600);

    // After getting a plan, then carrying out the plan.
    void move (Charging_Station_Manager & mychargestations);

    /*********************************** charging related method (station choice, duration) ***************************/

    // find the desired charging station for taxi.
    Charge_Choice chargeStationChoice(Charging_Station_Manager &cfm);
    Charge_Choice chargeStationChoice(Charging_Station_Manager &cfm, const int & method);
    // decide the duration of charging
    int computeChargeDuration (double distance, double chargerate);

    int computeChargeduration(double chargerate);

    // If the taxi cannot charge when he arrive at the charging station, he uses this method to decide how long he needs to wait.
    int exactWaitDuration(Charging_Station_Manager &cfm); // First in first out rule, and the charge duration of former taxi will not change.

    int estimateWaitDuration( Charging_Station & cf);

    /*********************************** For trip based charging simulation method ***************************/
    void keepRunningBeforeCharging();

    void updateTaxiState(int currenttripindex);

    int findBeginPositionToCharge(int currenttripindex);

    void setTheChargePlan(int chargepoint);
    /****************************************** Performance indicator aid method **************************************/
    void tdAdd (Taxi_Demand & newDemand);

    int findIndexInFuture (int needfoundtime);

    // the might miss some demand due to charge. This method helps recording the missed demand.
    void getMissedDrive (int index);

    void addWaitDuration(int cfid, int duration);

    /******************************************** helping methods  ****************************************************/

    void generateTaxiTour();

    array<double,2> myarray = {0,0};
    void setThePlan(const int & index, const int & nextstate, const double & myenergy, const int & mytime = -999, const array<double,2> & position = array<double,2> {0,0});

    // find the iterator of charging station according to their position
    map<int,Charging_Station>::iterator findTheChargeStaion(Charging_Station_Manager & cfm, array<double,2> pos);

    void generateAddTaxiDemand(int index, double distance, bool success);
    // Total demand in the tracking data.
    void findDemandNumber();
    // The beginnings and ends of all the demands.
    void findDemandsBeginEnd();

    int computetripindex(int futuretime);
    /*********************************************************************************
    // Getters and setters //
    *********************************************************************************/

    int getCus () const {
        return cus;
    }

    void setCus (int cus) {
        ReProduce_Taxi::cus = cus;
    }

    vector<Taxi_Demand> &getTd () {
        return td;
    }

    void setTd (const vector<Taxi_Demand> &td) {
        ReProduce_Taxi::td = td;
    }

    void setMinSoc (double minSoc) {
        ReProduce_Taxi::minSoc = minSoc;
    }

    double getChargeTo () const {
        return chargeTo;
    }

    void setChargeTo (double chargeTo) {
        ReProduce_Taxi::chargeTo = chargeTo;
    }

    Taxi_History &getMyhistory () {
        return myhistory;
    }

    void setMyhistory (const Taxi_History &myhistory) {
        ReProduce_Taxi::myhistory = myhistory;
    }

    int getMycurrentstateinhistory () const {
        return mycurrentstateinhistory;
    }

    void setMycurrentstateinhistory (int mycurrentstateinhistory) {
        ReProduce_Taxi::mycurrentstateinhistory = mycurrentstateinhistory;
    }

    int getDemandnumber() const {
        return demandnumber;
    }

    void setDemandnumber(int demandnumber) {
        ReProduce_Taxi::demandnumber = demandnumber;
    }

    const vector<array<int, 2>> &getDemandID() const {
        return demandID;
    }

    void setDemandID(const vector<array<int, 2>> &demandID) {
        ReProduce_Taxi::demandID = demandID;
    }

    ReProduce_Taxi_tour &getMytour() {
        return mytour;
    }

    void setMytour(const ReProduce_Taxi_tour &mytour) {
        ReProduce_Taxi::mytour = mytour;
    }

    int getChargesimulationalgorithm() const {
        return chargesimulationalgorithm;
    }

    void setChargesimulationalgorithm(int chargesimulationalgorithm) {
        ReProduce_Taxi::chargesimulationalgorithm = chargesimulationalgorithm;
    }

    int getCFchoicealgorithm() const {
        return CFchoicealgorithm;
    }

    void setCFchoicealgorithm(int CFchoicealgorithm) {
        ReProduce_Taxi::CFchoicealgorithm = CFchoicealgorithm;
    }

    int getChargedurationalgorithm() const {
        return chargedurationalgorithm;
    }

    void setChargedurationalgorithm(int chargedurationalgorithm) {
        ReProduce_Taxi::chargedurationalgorithm = chargedurationalgorithm;
    }

    const vector<array<int, 2>> &getWaitduration() const {
        return waitduration;
    }

    void setWaitduration(const vector<array<int, 2>> &waitduration) {
        ReProduce_Taxi::waitduration = waitduration;
    }

    const vector<One_Charge> &getTaxicharges() const {
        return taxicharges;
    }

    void setTaxicharges(const vector<One_Charge> &taxicharges) {
        ReProduce_Taxi::taxicharges = taxicharges;
    }

    int getTrackend() const {
        return trackend;
    }

    void setTrackend(int trackend) {
        ReProduce_Taxi::trackend = trackend;
    }

    double getFixedtripcharge() const {
        return fixedtripcharge;
    }

    void setFixedtripcharge(double fixedtripcharge) {
        ReProduce_Taxi::fixedtripcharge = fixedtripcharge;
    }

    double getMinimumdistance() const {
        return minimumdistance;
    }

    void setMinimumdistance(double minimumdistance) {
        ReProduce_Taxi::minimumdistance = minimumdistance;
    }

    double getChargeperdistance() const {
        return chargeperdistance;
    }

    void setChargeperdistance(double chargeperdistance) {
        ReProduce_Taxi::chargeperdistance = chargeperdistance;
    }
};

#endif //CHARING_TAXI_H