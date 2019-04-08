/* To Do:
 * Avoid the confliction of the performance slotutili updation for the same plantime.
 * Finish chargeVehicle method
 * Finish updateWaitlist method
 * There might be problem with Clone method
 */

/* Charging_Station.cpp */

/*#include <vector>
#include <map>
#include <boost/mpi.hpp>
#include <array>*/


#include "station_optimization/simulation/charging_facility/Charging_Station.h"

/* Constructors */
Charging_Station::Charging_Station() {
    id_ = 0;
    pos[0] = 0.0;
    pos[1] = 0.0;
    rate = 0.0;
    cap = 0;
    ocupslot = 0;
}

/* Setter */

void Charging_Station::set(int cap, double rate) {
    this->cap = cap;
    this->rate = rate;
}

/* Action */
Charging_Station Charging_Station::clone(){
    return *this;
}

void Charging_Station::addOccupied(int timeStamp) {
    ocupslot++;
    if (slotutili.back()[0] == timeStamp){
        slotutili.back()[1] = ocupslot;
    }else{
        array<int,2> chargeinfo = {timeStamp,ocupslot};
        slotutili.push_back(chargeinfo);
    }
}

void Charging_Station::departOccupied(int timeStamp) {
    ocupslot--;
    if (slotutili.back()[0] == timeStamp){
        slotutili.back()[1] = ocupslot;
    }else{
        array<int,2> chargeinfo = {timeStamp,ocupslot};
        slotutili.push_back(chargeinfo);
    }
}

void Charging_Station::addWaitList(const int vehicleID, const int timestamp) {

    waitlist.push_back(vehicleID);

    if (waitnumber.back()[0] == timestamp){
        waitnumber.back()[1] = int(waitlist.size());
    }else{
        array<int,2> waitinfo = {timestamp, int(waitlist.size())};
        waitnumber.push_back(waitinfo);
    }
}

void Charging_Station::departWaitList(int VehicleID, int currenttime) {

    for(auto iter = waitlist.begin(); iter !=waitlist.end(); iter++){
        if(*iter ==VehicleID){
            waitlist.erase(iter);
            break;
        }
    }

    if(waitnumber.back()[0]==currenttime){
        waitnumber.back()[1] = int(waitlist.size());
    }else{
        array<int,2> waitinfo = {currenttime, int(waitlist.size())};
        waitnumber.push_back(waitinfo);
    }
}

void Charging_Station::addChargeEnd(const int addchargeend) {
    bool inserted = false;
    for(auto iter = chargeend.begin(); iter != chargeend.end(); iter++){
        if (*iter>addchargeend){
            chargeend.insert(iter,addchargeend);
            inserted = true;
            break;
        }
    }
    if(!inserted){
        chargeend.push_back(addchargeend);
    }
}

void Charging_Station::deleteChargeEnd(const int deletechargeend) {
    // This tries to delete the chargeend as the vehicle depart from the station.

    if(*(this->chargeend.begin()) == deletechargeend){
        this->chargeend.erase(this->chargeend.begin());
    }else{
        printf("This is strenge as the first chargeend should be equal to the one that will be deleted \n" );
    }
}

void Charging_Station::addUnchargedeletenumber(int time, int number) {

    array<int,2> thispair = {time, number};
    this->unchargedeletenumber.push_back(thispair);
}

void Charging_Station::computeChargePrice() {
    double pertol_price = 1.61, gas_per_100km = 7.0, electricity_per_100km = 20.0;
    service_price = pertol_price*gas_per_100km/2/(electricity_per_100km/rate);
}









