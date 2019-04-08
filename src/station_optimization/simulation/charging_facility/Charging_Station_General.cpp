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


#include "station_optimization/simulation/charging_facility/Charging_Station_General.h"

/* Constructors */
Charging_Station_General::Charging_Station_General() {
    Charging_Station_General::id_ = 0;
    Charging_Station_General::pos[0] = 0.0;
    Charging_Station_General::pos[1] = 0.0;
    Charging_Station_General::rate_vector = {0.0};
    Charging_Station_General::cap = {0};
    Charging_Station_General::ocupslot = {0};
}

void Charging_Station_General::addOccupied(int timeStamp, int type) {

    ocupslot[type]++;
    if (slotutili[type].back()[0] == timeStamp){
        slotutili[type].back()[1] = ocupslot[type];
    }else{
        slotutili[type].push_back({timeStamp,ocupslot[type]});
    }
    // These commented code should be kept.!!!!!!!
    double used_rate = takenenergy.back()[1] + rate_vector[type];
    double revenue = chargerevenue.back()[1] + service_price[type];
    if(takenenergy.back()[0] == double(timeStamp)){
        takenenergy.back()[1] = used_rate;
        energycost.back()[1] = used_rate*current_electricity_price;
        chargerevenue.back()[1] = revenue;
    } else {
        takenenergy.push_back({double(timeStamp),used_rate});
        energycost.push_back({double(timeStamp), used_rate*current_electricity_price});
        chargerevenue.push_back({double(timeStamp), revenue});
    }
}

void Charging_Station_General::departOccupied(int timeStamp, int type) {

    ocupslot[type]--;
    if (slotutili[type].back()[0] == timeStamp){
        slotutili[type].back()[1] = ocupslot[type];
    }else{
        array<int,2> chargeinfo = {timeStamp,ocupslot[type]};
        slotutili[type].push_back(chargeinfo);
    }

    double used_rate = takenenergy.back()[1] - rate_vector[type];
    double revenue = chargerevenue.back()[1] - service_price[type];
    if(takenenergy.back()[0] == double(timeStamp)){
        takenenergy.back()[1] = used_rate;
        energycost.back()[1] = used_rate*current_electricity_price;
        chargerevenue.back()[1] = revenue;
    } else{
        takenenergy.push_back({double(timeStamp),used_rate});
        energycost.push_back({double(timeStamp), used_rate*current_electricity_price});
        chargerevenue.push_back({double(timeStamp), revenue});
    }
}

void Charging_Station_General::addWaitList(const int vehicleID, const int timestamp) {

    waitlist.push_back(vehicleID);
    startwaittime.push_back(timestamp);
    if (waitnumber.back()[0] == timestamp){
        waitnumber.back()[1] = int(waitlist.size());
    }else{
        array<int,2> waitinfo = {timestamp, int(waitlist.size())};
        waitnumber.push_back(waitinfo);
    }
}

void Charging_Station_General::departWaitList(int VehicleID, int currenttime, int delete_index,  bool direct_delated) {
    if (direct_delated){
        if (*(waitlist.begin()+delete_index) == VehicleID){
            waitlist.erase(waitlist.begin()+delete_index);
            waitduration.push_back({startwaittime[delete_index], (currenttime-startwaittime[delete_index])});
            startwaittime.erase(startwaittime.begin()+delete_index);
        } else{
            cout <<"There is a problem with the delete index from stationAction()" <<endl;
        }
    } else{
        waitlist.erase(find(waitlist.begin(), waitlist.end(),VehicleID));
    }

    if(waitnumber.back()[0]==currenttime){
        waitnumber.back()[1] = int(waitlist.size());
    }else{
        array<int,2> waitinfo = {currenttime, int(waitlist.size())};
        waitnumber.push_back(waitinfo);
    }
}

void Charging_Station_General::addChargeEnd(const int addchargeend, int type) {

    bool inserted = false;
    for(auto iter = chargeend[type].begin(); iter != chargeend[type].end(); iter++){
        if (*iter>addchargeend){
            chargeend[type].insert(iter,addchargeend);
            inserted = true;
            break;
        }
    }
    if(!inserted){
        chargeend[type].push_back(addchargeend);
    }
}

void Charging_Station_General::deleteChargeEnd(const int deletechargeend, int type ) {
    // This tries to delete the chargeend as the vehicle depart from the station.

    if(*(this->chargeend[type].begin()) == deletechargeend){
        this->chargeend[type].erase(this->chargeend[type].begin());
    }else{
        /*for(auto i : this->chargeend[type]){
            cout << i <<endl;
        }*/
        while(*(this->chargeend[type].begin()) != deletechargeend){
            this->chargeend[type].erase(this->chargeend[type].begin());
        }
        this->chargeend[type].erase(this->chargeend[type].begin());
        //printf("This is strenge as the first chargeend should be equal to the one that will be deleted \n" );
    }
}

void Charging_Station_General::addUnchargedeletenumber(int time, int number) {

    array<int,2> thispair = {time, number};
    this->unchargedeletenumber.push_back(thispair);
}

void Charging_Station_General::initiateDefaultElements(int time_stamp) {
    vector<array<int, 2>> myvector;
    vector<int> myvector1;
    myvector.push_back({time_stamp, 0});
    for(int i=0; i < static_cast<int>(rate_vector.size()); i++) {
        this->ocupslot.push_back(0);
        this->slotutili.push_back(myvector);
        this->chargeend.push_back(myvector1);
        for(int j= 0; j < cap[i];j++){
            integrate_rate_vec.push_back(rate_vector[i]);
            charger_occupation.push_back(myvector);
        }
    }
    this->takenenergy.push_back({double(time_stamp),0.0});
    this->energycost.push_back({double(time_stamp), 0.0});
    this->chargerevenue.push_back({double(time_stamp), 0.0});
    this->waitnumber = myvector;
    this->station_charging_demand = myvector;
    this->total_charger_num = accumulate(cap.begin(),cap.end(),0);

    computeChargePrice();
}

void Charging_Station_General::computeChargePrice() {

    double pertol_price = 1.61, gas_per_100km = 7.0, electricity_per_100km = 20.0;
    // In usa, gasoline 0.83 $ per liter

    for(auto i : rate_vector){
        int case_num = 0;
        if(i > 60.0/3600.0) {
            case_num = 1;
        } else if(i>20.0/3600){
            case_num = 2;
        } else {
            case_num = 3;
        }
        switch(case_num){
            case(1):{
                service_price.push_back(pertol_price*gas_per_100km/2/(electricity_per_100km/i)*1.5);
                break;
            }
            case(2):{
                service_price.push_back(pertol_price*gas_per_100km/2/(electricity_per_100km/i)*1.2);
                break;
            }
            case(3):{
                service_price.push_back(pertol_price*gas_per_100km/2/(electricity_per_100km/i));
                break;
            }
        }
    }
}

void Charging_Station_General::addChargingArrival(int timeStamp) {
    if(station_charging_demand.back().front() == timeStamp){
        station_charging_demand.back().back()++;
    } else {
        station_charging_demand.push_back({timeStamp,1});
    }
}

void Charging_Station_General::addChargerOccupation(int charger_ID, array<int, 2> charge_end) {
    charger_occupation[charger_ID].push_back(charge_end);
}










