/* Check the function constructors, getters, and setters*/

 /* Charging_Station.h */

#ifndef CHARGING_FACILITY_H
#define CHARGING_FACILITY_H

#include <vector>
#include <list>
#include <deque>
#include <map>
#include <array>


using namespace std;

/* Agents */
class Charging_Station{

private:
    int                                          id_; // The id of charging station
    array<double,2>                              pos;

    double                                      rate; // charge rate of each station. how to have the case that there are multiple charging rate at the station.

    int                                          cap; // number of slots in the charging station

    int                                   ocupslot=0; // number of slots occupied by vehicle

    deque <int>                             waitlist; // waiting list of vehicles (id) at each station. We use this to visit the vehicle.
    deque<double>                           waitutil; // the plantime utill which the waiting vehicle could charge.


    vector<int>                            chargeend; // the plantime by which the charging is finished for each vehicle.

    vector<array<int,2>>                   slotutili; // Store the utilization of charging station with respect to plantime. The first int is the timestamp while the second int is the number of slots used.

    vector<array<int,2>>                  waitnumber; // the first is the timeStamp while the second is the length of the waitlist.

    vector<array<int,2>>        unchargedeletenumber; /* To store the number of taxis that are deleted due to out of tracking time at time t.
                                                 * The first element is the timeStamp. */

    vector<double>                       takenenergy; // The energy that is taken during the time.
    vector<double>                        energycost; // The cost of the energy taken from the grid.
    double                         totalenergycost=0; // total energy cost

    double                             service_price; // The price is given as CHF/s
    vector<double>                     chargerevenue; // The revenue of providing charging service.
    double                            totalrevenue=0;

public:

    Charging_Station();

    Charging_Station(int id_, array<double,2> pos, double rate, int cap):id_(id_),pos(pos),rate(rate),cap(cap){};

    /* Setter  */
    void set(int cap,  double rate);

    void addOccupied(int timeStamp);

    void departOccupied(int timeStamp);
    
    /* Actions */
    
    Charging_Station clone();

    void addWaitList(const int vehicleID, const int timestamp);

    void departWaitList(const int VehicleID, const int currenttime);

    void addChargeEnd(const int addchargeend);

    void deleteChargeEnd(const int addChargeend);

    void addUnchargedeletenumber(int time, int number);

    void computeChargePrice();
    /* Getter and Setter*/
    int getId_() const {
        return id_;
    }

    void setId_(int id_) {
        Charging_Station::id_ = id_;
    }

    const array<double,2> getPos() const {
        return pos;
    }

    double getRate() const {
        return rate;
    }

    void setRate(double rate) {
        Charging_Station::rate = rate;
    }

    int getCap() const {
        return cap;
    }

    void setCap(int cap) {
        Charging_Station::cap = cap;
    }

    int getOcupslot() const {
        return ocupslot;
    }

    void setOcupslot(int ocupslot) {
        Charging_Station::ocupslot = ocupslot;
    }

    deque<int> &getWaitlist(){
        return waitlist;
    }

    void setWaitlist(const deque<int> & waitlist) {
        Charging_Station::waitlist = waitlist;
    }

    void addWaitlist(const int & newwait){
        Charging_Station::waitlist.push_back(newwait);
    }

    vector<array<int,2>> &getSlotutili() {
        return slotutili;
    }

    void setSlotutili(const vector<array<int,2>> &slotutili) {
        Charging_Station::slotutili = slotutili;
    }

    void initializeSlotutili(const array<int,2> & newslotutili){
        Charging_Station::slotutili.push_back(newslotutili);
    }

    void setPos(const array<double, 2> &pos) {
        Charging_Station::pos = pos;
    }

    const vector<int> &getChargeend() const {
        return chargeend;
    }

    const deque<double> &getWaitutil() const {
        return waitutil;
    }

    const vector<array<int, 2>> &getWaitnumber() const {
        return waitnumber;
    }

    const vector<array<int, 2>> &getUnchargedeletenumber() const {
        return unchargedeletenumber;
    }

    const vector<double> &getTakenenergy() const {
        return takenenergy;
    }

    const vector<double> &getEnergycost() const {
        return energycost;
    }

    double getTotalenergycost() const {
        return totalenergycost;
    }

    const vector<double> &getChargerevenue() const {
        return chargerevenue;
    }

    double getTotalrevenue() const {
        return totalrevenue;
    }

    void initializeWaitindicator(const array<int, 2> &waitindicator) {
        Charging_Station::waitnumber.push_back(waitindicator);
    }

    void initiateboth(const array<int,2> & initializer){
        Charging_Station::slotutili.push_back(initializer);
        Charging_Station::waitnumber.push_back(initializer);
    }


};

#endif
