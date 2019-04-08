//
// Created by ZhengchaoWANG on 27.07.18.
//

#ifndef CHARING_REPRODUCE_TAXI_MULTI_CHARGING_TYPES_H
#define CHARING_REPRODUCE_TAXI_MULTI_CHARGING_TYPES_H

#include "station_optimization/simulation/helpers/Plan.h"
#include "station_optimization/simulation/taxi/Vehicle.h"
#include "station_optimization/simulation/helpers/One_Charge.h"
#include "station_optimization/simulation/taxi/Taxi_History.h"
#include "station_optimization/simulation/taxi/Taxi_Demand.h"
#include "ReProduce_Taxi_Tour.h"
/*
 * #include "station_optimization/simulation/taxi_helpers/Charge_Choice.h"
 * #include "station_optimization/simulation/charging_facility/Charging_Station_General.h"
 * #include "station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h"*/

#include <cmath>
#include <array>
#include <string>
#include <numeric>      // for the sum of vector
#include <algorithm>    // for min function
using namespace std;

class ReProduce_Taxi_General : public Vehicle {

private:
    // State of the taxi, including pos, soc, drivespeed, and.
    int                                                                 cus; // A bool value to indicate whether passengers on board.
    int                                             mycurrentstateinhistory; // To shychronize with the measure history.
    int                                                         tripindex=0;
    /******* Properties of this type of taxi. *******************/
    // The charge simulation algorithm. Thereshold based, or operation based
    int                                        chargesimulationalgorithm = 1; // To control which method is used to generate Plan.
    // Charging duration choice algorithm, more related Threshold based simulation algorithm
    int                                          chargedurationalgorithm = 1; // 1, charge to a certain level. 2, optimize the charge level (might be hard to decide).
    // To compute the revenue of each trips
    double                                              fixedtripcharge = 3.5; // If a trip is taken, then this taxi can receive this amount of money for the first "minimumdistance".
    double                                              minimumdistance =   0; // The first 0 km will not be charged additional cost.
    double                                             chargeperdistance=1.71; // 1.71$ per km after 3.0 km.

    int                                                       largegap = 3600; // This is a large value than which the neighboring points is larger, they will try to search for charging possibility.
    int                                                      foresight = 3600; // this is a threshold of vehicle foresight to see the future.
    // the following trajectories of this type of taxi.
    int                                                          trackend = 0; // The end of the tracking data so that we can know the end of the simulation.
    int                                                      demandnumber = 0; // Total taxi demand of this taxi in the tracking.
    vector<array<int,2>>                                             demandID;   // For test.
    Taxi_History                                                    myhistory; // to remember the history of myself.
    ReProduce_Taxi_tour                                                mytour; // a vector of trips that the taxi takes out.
    bool                                              longstopseperate = true;
    double                                           my_tracking_distance = 0;
    // performance indicator of this type of taxi.
    vector<Taxi_Demand>                                                    td;  // The total demand of a taxi. We can ignore this, if we get a mytour elements

public:

    ReProduce_Taxi_General() = default;

    ReProduce_Taxi_General (int id_, array<double, 2> pos, double soc, int cus, int time, double chargerate, double capf,
                    double ra, double minSoc, double chargeTo, int trackend, Taxi_History & myhistory, int myCSH) :
            Vehicle(id_, pos, chargerate, soc, time, ra, minSoc, chargeTo), cus(cus),
            trackend(trackend), myhistory(myhistory), mycurrentstateinhistory(myCSH) {}

    /******************************************** Action methods  *****************************************************/
    //generate a plan for taxi to tell them what to do next
    void generatePlan (Charging_Station_General_Manager &cfm, const int & threshold = 3600);

    // After getting a plan, then carrying out the plan.
    void move (Charging_Station_General_Manager & mychargestations) override;

    /*********************************** For trip based charging simulation method ***************************/
    void keepRunningBeforeCharging();

    void updateTaxiState(int currenttripindex);

    int findBeginPositionToCharge(int currenttripindex);

    void setTheChargePlan(int chargepoint);
    /****************************************** Performance indicator aid method **************************************/
    void tdAdd (Taxi_Demand & newDemand);

    int findIndexInFuture (int needfoundtime);

    void getMissedDrive (int index);

    void addWaitDuration(int station_id, int wait_duration);
    /******************************************** helping methods  ****************************************************/

    void generateTaxiTour();

//    array<double,2> myarray = {0,0};
    void setThePlan(const int & index, const int & nextstate, const double & myenergy, const int & mytime = -999,
            const array<double,2> & position = array<double,2> {0,0}, const double & charge_rate =0.0, const int & chargetype = 0);

    void generateAddTaxiDemand(int index, double distance, bool success);

    void findAllDemandNumber();

    void findAllDemandTrips();

    int computetripindex(int futuretime);
    /*********************************************************************************
    // Getters and setters //
    *********************************************************************************/

    Taxi_History &getMyhistory () {
        return myhistory;
    }

    ReProduce_Taxi_tour &getMytour() {
        return mytour;
    }

    void setChargesimulationalgorithm(int chargesimulationalgorithm) {
        ReProduce_Taxi_General::chargesimulationalgorithm = chargesimulationalgorithm;
    }

    int getTrackend() const {
        return trackend;
    }

    void setTrackend(int trackend) {
        ReProduce_Taxi_General::trackend = trackend;
    }

    void setMy_tracking_distance(double my_tracking_distance) {
        ReProduce_Taxi_General::my_tracking_distance = my_tracking_distance;
    }

    // These are used in template.
    int getChargedurationalgorithm() const {
        return chargedurationalgorithm;
    }

    double getFixedtripcharge() const {
        return fixedtripcharge;
    }

    double getMinimumdistance() const {
        return minimumdistance;
    }

    double getChargeperdistance() const {
        return chargeperdistance;
    }

    const vector<One_Charge> &getTaxicharges() const {
        return taxicharges;
    }

    int getChargesimulationalgorithm() const {
        return chargesimulationalgorithm;
    }

    int getDemandnumber() const {
        return demandnumber;
    }

    const vector<Taxi_Demand> &getTd() const {
        return td;
    }

    const vector<array<int, 2>> &getDemandID() const {
        return demandID;
    }


};

#endif //CHARING_REPRODUCE_TAXI_MULTI_CHARGING_TYPES_H
