/* Vehicle.h */

#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <array>
#include <string>
#include <cmath>
#include <climits>
#include <algorithm>

#include "station_optimization/simulation/helpers/Plan.h"
#include "station_optimization/simulation/charging_facility/Charging_Station.h"
#include "station_optimization/simulation/charging_facility/Charging_Station_Manager.h"

#include "station_optimization/simulation/charging_facility/Charging_Station_General.h"
#include "station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h"

#include "station_optimization/simulation/helpers/Charge_Choice.h"
#include "station_optimization/simulation/helpers/One_Charge.h"
/* Agents */

using namespace std;

class Vehicle{

protected:
    int                                                                   id_; // The id of charging station

    // Properties of vehicle
    double                                              chargerate = 1.0/30.0; // charge rate of each station
    double                                                            capf=24; // capacity of the battery or fuel tank.
    double                                                     consumrate=0.2; // consumption rate of the vehicle per km.

    //State of the vehicle
    int                                                                  time; // current time of vehicles
    array<double,2>                                                       pos; // planposition of charing station
    double                                                                soc; // state of charge at a time step
    double                                               drivespeed= 1.0/60.0; // this is a speed chosen by the driver. By now, we set as 60km/h.

    double                                                                 ra; // range anxiety of taxi drivers. Below this value, the taxi will start to charge.
    double                                                             minSoc; // the minimum state of charge that a vehicle wants to have after sending a passenger.
    double                                                           chargeTo; // the level of soc that the vehicle wants to charge to when charge.

    // Charging station choice algorithm
    int                                                 CFchoicealgorithm = 1; // 1, cloest, 2, the station allows less wait time. etc

    vector<array<int,2>>                                         waitduration;  // to indicate the wait duration of the vehicle at charging station id.
    One_Charge                                                  currentcharge;
    vector<One_Charge>                                            taxicharges;  // the charges that the taxi will have during the tracking trajectories.

    Plan                                                                 plan; // A plan of future.

public:

    Vehicle() = default;

    Vehicle(int id_, array<double,2> pos,int time, double chargerate, double soc, double speed, Plan &plan) : id_(id_), pos(pos), time(time),
                                                                                     chargerate(chargerate), soc(soc), drivespeed(speed), plan(plan) {};

    Vehicle(int id_, array<double,2> pos, double chargerate, double soc, int time, double ra, double minsoc, double chargeTo):
            id_(id_), pos(pos), chargerate(chargerate),  soc(soc), time(time),ra(ra),minSoc(minsoc),chargeTo(chargeTo) {}

     virtual ~Vehicle() = default;

    /* Actions */

    virtual void move(Charging_Station_General_Manager & mychargestations){};


    /*********************************** charging related method (station choice, duration) ***************************/

//    Charge_Choice chargeStationChoice(Charging_Station_General_Manager &cfm);

    virtual Charge_Choice chargeStationChoice(Charging_Station_General_Manager &cfm, int decision_time);

    void addACharge();

    /* Helper functions */
    virtual map<int,Charging_Station_General>::iterator findTheChargeStaion(Charging_Station_General_Manager & cfm, array<double,2> pos);

    virtual int estimateWaitDuration( Charging_Station_General & cf);

    double twoPointsDistance(array<double, 2> pos1, array<double, 2> pos2);

    /* Getters specific to this kind of Agent */
    int getId_() const {
        return id_;
    }

    const array<double,2> & getPos() const {
        return pos;
    }

    double getChargerate() const {
        return chargerate;
    }

    double getSoc() const {
        return soc;
    }

    void setPlan(const Plan & plan) {
        Vehicle::plan = plan;
    }

    double getCapf() const {
        return capf;
    }

    double getConsumrate() const {
        return consumrate;
    }

    double getDrivespeed() const {
        return drivespeed;
    }

    int getTime () const {
        return time;
    }

    void setTime (int time) {
        Vehicle::time = time;
    }

    double getRa () const {
        return ra;
    }

    double getMinSoc () const {
        return minSoc;
    }

    void setRa (double ra) {
        Vehicle::ra = ra;
    }

    One_Charge &getCurrentcharge();
};

#endif
