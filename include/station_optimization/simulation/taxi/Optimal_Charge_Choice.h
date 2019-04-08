/*
//
// Created by ZhengchaoWANG on 04.07.18.
//

*/
/* TO DO:
 *
 * Set starttripindex in intializeFromData().
 *
 *//*



#ifndef CHARING_OPTIMAL_CHARGE_CHOICE_H
#define CHARING_OPTIMAL_CHARGE_CHOICE_H

#include <vector>
#include <algorithm> // for min/max function
#include <cmath>
#include <iostream>
#include <sstream>
#include <ilcplex/ilocplex.h>

#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi.h"
#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_Tour.h"
#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_Trip.h"
#include "station_optimization/simulation/charging_facility/Charging_Station_Manager.h"

using namespace std;

class  Optimal_Charge_Choice{
private:
    // Charge choices
    vector<bool>                 tripiscarriedout;
    vector<int>                  chargefacilityID; // each element represent the ID of charging facility used in trip.
    vector<double>               chargedenergy;

    // For reward (objective function)
    vector<int>                 tripfixreward;
    vector<double>              tripdistancereward;
    vector<double>              emptytripreward;
    double                      alphapenalty = 10000000;

    // Necessary information of future trips
    vector<double>              tripdistances;
    vector<int>                 tripduration;
    vector<bool>                tripcustags;
    vector<vector<double>>      pointtocfdistance; // # of trips+1 * # of charging station
    vector<vector<double>>      cftocfdistance;  // # of charging station * # of charging station
    vector<double>              usedchargerate; // equal to number of charging station
    vector<vector<int>>         waittimeatfacilities; // this is the wait duration if the taxi arrive at that time.

    // Some properties of the taxi.
    double                      drivespeed = 1.0/60.0;
    double                      consumerate = 0.2;
    double                      batterycapacity = 24.0;
    double                      initialenergy = 24.0;

    double                      fixreward = 15.0;
    double                      driveprice = 1.5;
    double                      emptyprice = 0.4;

    int                         foresight = 86400;  // A threshold to define how far that the taxi is going to look forward and make his charge plan.


public:
    */
/******************************************* Constructors or Initializers *****************************************//*

    Optimal_Charge_Choice() = default;

    Optimal_Charge_Choice(unsigned long i, unsigned long j){
        tripiscarriedout = vector<bool> (i,true);
        chargefacilityID = vector<int> (i,-1);

        tripdistances = vector<double> (i,0.0);
        pointtocfdistance = vector<vector<double>> (i+1,vector<double>(j,0.0));
        cftocfdistance = vector<vector<double >> (j,vector<double>(j,0.0));
    }

    void initializeWithTripSize(const unsigned long tripsize, const unsigned long chargestationsize);

    void intializeFromData(ReProduce_Taxi & mytaxi, Charging_Station_Manager & chargefacilities);

    void getOptimalChargeChoice();

    vector<double> pointDistancesToChargingFacilities(array<double, 2> point,
                                                      Charging_Station_Manager &chargefacilities);


    */
/******************************************** Math Helper functions ***********************************************//*


    double twoPointsDistance(array<double,2> point1, array<double,2> point2);

    void computeReward();

    */
/********* Setters and Getters *****************//*

    const vector<bool> &getTripiscarriedout() const;

    void setTripiscarriedout(const vector<bool> &tripiscarriedout);

    const vector<int> &getChargefacilityID() const;

    void setChargefacilityID(const vector<int> &chargefacilityID);

    const vector<double> &getTripdistances() const;

    void setTripdistances(const vector<double> &tripdistances);

    const vector<vector<double>> &getPointtocfdistance() const;

    void setPointtocfdistance(const vector<vector<double>> &pointtocfdistance);

    const vector<vector<double>> &getCftocfdistance() const;

    void setCftocfdistance(const vector<vector<double>> &cftocfdistance);

    const vector<vector<int>> &getWaittimeatfacilities() const;

    void setWaittimeatfacilities(const vector<vector<int>> &waittimeatfacilities);

    const vector<double> &getChargedenergy() const;

    void setChargedenergy(const vector<double> &chargedenergy);

    const vector<int> &getTripfixreward() const;

    void setTripfixreward(const vector<int> &tripfixreward);

    const vector<double> &getTripdistancereward() const;

    void setTripdistancereward(const vector<double> &tripdistancereward);

    const vector<double> &getEmptytripreward() const;

    void setEmptytripreward(const vector<double> &emptytripreward);

    double getAlphapenalty() const;

    void setAlphapenalty(double alphapenalty);

    const vector<int> &getTripduration() const;

    void setTripduration(const vector<int> &tripduration);

    const vector<bool> &getTripcustags() const;

    void setTripcustags(const vector<bool> &tripcustags);

    const vector<double> &getUsedchargerate() const;

    void setUsedchargerate(const vector<double> &usedchargerate);

    double getDrivespeed() const;

    void setDrivespeed(double drivespeed);

    double getConsumerate() const;

    void setConsumerate(double consumerate);

    double getBatterycapacity() const;

    void setBatterycapacity(double batterycapacity);

    double getInitialenergy() const;

    void setInitialenergy(double initialenergy);

    double getFixreward() const;

    void setFixreward(double fixreward);

    double getDriveprice() const;

    void setDriveprice(double driveprice);

    double getEmptyprice() const;

    void setEmptyprice(double emptyprice);

    int getForesight() const;

    void setForesight(int foresight);

};



#endif //CHARING_OPTIMAL_CHARGE_CHOICE_H
*/
