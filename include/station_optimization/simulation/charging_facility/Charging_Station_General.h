/* Check the function constructors, getters, and setters*/

 /* Charging_Station.h */

#ifndef CHARGING_FACILITY_MULTI_TYPES_H
#define CHARGING_FACILITY_MULTI_TYPES_H

#include <vector>
#include <list>
#include <vector>
#include <map>
#include <iostream>
//#include <boost/mpi.hpp>
#include <array>
#include <numeric>
#include <algorithm>

using namespace std;

/* Agents */
class Charging_Station_General{

private:
    int                                                 id_; // The id of charging station
    array<double,2>                                     pos;
    vector<double>                              rate_vector; // charge rate of each station. how to have the case that there are multiple charging rate at the station. the unit is equal to (kwh/s).
    vector<int>                                         cap; // number of slots in the charging station.
    vector<double>                       integrate_rate_vec; // charger rate with a fast to slow order.
    int                                   total_charger_num; // total charger number in the station.

    vector<array<int,2>>            station_charging_demand; // number of charging demands through the simulation period

    vector<int>                                     ocupslot; // number of slots occupied by vehicle
    vector<vector<int>>                            chargeend; // the plantime by which the charging is finished for each vehicle.
    vector<vector<array<int,2>>>                   slotutili; // Store the utilization of charging station with respect to plantime. The first int is the timestamp while the second int is the number of slots used.
    vector<vector<array<int,2>>>          charger_occupation; // Each charger has a vector of utilized time slot. for example ([0, 100], [120,217] ...). It represents that this charger is occupied at these time. The gap between two array is the empty time.

    vector <int>                                     waitlist; // waiting list of vehicles (id) at each station. We use this to visit the vehicle.
    vector <int>                                startwaittime; // the start time of the vehicles at each station, use to record the waiting time for the charging station.
    vector<array<int,2>>                  estimated_end_rate; // Estimated end of charging end and charger type of all the vehicles in the charging station.
    vector<array<int,2>>                          waitnumber; // the first is the timeStamp while the second is the length of the waitlist.
    vector<array<int,2>>                        waitduration; // unit is second.
    vector<array<int,2>>                unchargedeletenumber; /* To store the number of taxis that are deleted due to out of tracking time at time t.
                                                        * The first element is the timeStamp. */

    vector<array<double,2>>                       takenenergy; // The energy that is taken during the time. This value is equal to (kwh/s)
    vector<array<double,2>>                        energycost; // The cost of the energy taken from the grid. this value is equal to (chf/s)
    double                    current_electricity_price = 0.2; // This is how much chf per kwh energy. For example, (0.2 chf/kwh).
    double                                  totalenergycost=0; // total energy cost

    vector<double>                              service_price; // This is how much for the using piles (chf/s).
    vector<array<double,2>>                     chargerevenue; // The revenue of providing charging service. this has a unit of (chf/s).
    double                                     totalrevenue=0; //

public:

    Charging_Station_General();

    Charging_Station_General(int id_, array<double,2> pos, vector<double> &rate, vector<int> &cap):id_(id_),pos(pos),rate_vector(rate),cap(cap){};

    /* Actions */
    void addChargerOccupation(int charger_ID, array<int,2> charge_end);

    void addChargingArrival(int timeStamp);

    void addOccupied(int timeStamp, int type);

    void departOccupied(int timeStamp, int type);

    void addWaitList( int vehicleID,  int timestamp);

    void departWaitList( int VehicleID,  int currenttime, int delete_index = 0, bool direct_delated = false);

    void addChargeEnd( int addchargeend, int type );

    void deleteChargeEnd( int addChargeend, int type);

    void addUnchargedeletenumber(int time, int number);

    void computeChargePrice();

    /* Useful initializer for some elements */
    void initiateDefaultElements(int time_stamp);

    void initiateboth(const array<int,2> & initializer){
        vector<array<int,2>> use_time_slot;
        use_time_slot.push_back(initializer);
        for(int i =0; i<cap.size(); i++){
            Charging_Station_General::slotutili[i].push_back(initializer);
            for(int j = 0; j < cap[i];j++){
                Charging_Station_General::charger_occupation.push_back(use_time_slot);
            }
        }
        Charging_Station_General::waitduration.push_back(initializer);
        Charging_Station_General::waitnumber.push_back(initializer);
        Charging_Station_General::station_charging_demand.push_back(initializer);
    }

    /* Getter and Setter*/
    int getId_() const {
        return id_;
    }

    const array<double,2> getPos() const {
        return pos;
    }

    vector<double>& getRate() {
        return rate_vector;
    }

    vector<int> & getCap()  {
        return cap;
    }

    vector<int> & getOcupslot()  {
        return ocupslot;
    }

    vector<int> &getWaitlist(){
        return waitlist;
    }

    vector<vector<array<int,2>>> &getSlotutili() {
        return slotutili;
    }

    const vector<vector<int>> &getChargeend() const {
        return chargeend;
    }

    const vector<array<int, 2>> &getWaitnumber() const {
        return waitnumber;
    }

    const vector<array<int, 2>> &getUnchargedeletenumber() const {
        return unchargedeletenumber;
    }

    const vector<array<double,2>> &getTakenenergy() const {
        return takenenergy;
    }

    const vector<array<double,2>> &getEnergycost() const {
        return energycost;
    }

    double getTotalenergycost() const {
        return totalenergycost;
    }

    const vector<array<double,2>> &getChargerevenue() const {
        return chargerevenue;
    }

    double getTotalrevenue() const {
        return totalrevenue;
    }

    const vector<array<int, 2>> &getWaitduration() const {
        return waitduration;
    }

    const vector<double> &getService_price() const {
        return service_price;
    }

    const vector<array<int, 2>> &getStation_charging_demand() const {
        return station_charging_demand;
    }

    const vector<vector<array<int, 2>>> &getCharger_occupation() const {
        return charger_occupation;
    }

    const vector<array<int, 2>> &getEstimated_end_rate() const {
        return estimated_end_rate;
    }

    int getTotal_charger_num() const {
        return total_charger_num;
    }

    const vector<double> &getIntegrate_rate_vec() const {
        return integrate_rate_vec;
    }
};

#endif
