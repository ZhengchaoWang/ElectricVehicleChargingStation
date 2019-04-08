/* Charging_Station_Agent.h */

#ifndef PRIVATE_VEHICLE_H
#define PRIVATE_VEHICLE_H

#include<vector>
#include "simulation/taxi/Vehicle.h"

/* Agents */
class Private_vehicle : public Vehicle {
	
private:
    int               id_; // The id of charging station
    double         pos[2]; // planposition of charing station

    double     chargerate; // charge rate of each station
    
	
    int               cap; 
    int          ocupslot; // number of slots occupied by vehicle
    vector<int>  waitlist; // waiting list of vehicles (id) at each station
    
    list<int>    slotutil; // Store the utilization of charging station
    
    
public:
    Private_vehicle(int id);
    ~Private_vehicle(){}
    Private_vehicle(int id, double pos[2], double chargerate, int cap);

	
    /* Required Getters */
    virtual int& getId(){                   return id_;    }
    virtual const int& getId() const {      return id_;    }
	
    /* Getters specific to this kind of Agent */
   // double getPos(){                                      return pos;        }
    double getChargerate(){                               return chargerate; }
    int getCap(){                                         return cap;        }
    int getOcupslot(){                                    return ocupslot;   }
    vector<int> getWaitlist(){                            return waitlist;   }
    
    /* Setter & updater */
    void set(double pos[2], int cap,  int type);

    void updateWaitlist(vector<int> newVehicles, int numberleft);
    
    /* Actions */
    void receiveVehicle(int vehicleID);
    
    void departVehicle(int vehicleID);
    
    void chargeVehicle(int vehicleID);
    
};

#endif
