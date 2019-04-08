/* Vehicle.cpp */

#include <iostream>
#include <limits>
#include "station_optimization/simulation/taxi/Vehicle.h"


/* Actions */

double Vehicle::twoPointsDistance(array<double, 2> pos1, array<double, 2> pos2) {

    double dlat = (pos2[0]-pos1[0]) * M_PI/180;
    double dlon = (pos2[1]-pos1[1]) * M_PI/180;
    double a = pow(sin(dlat/2),2) + cos(pos1[0]*M_PI/180)*cos(pos2[0]*M_PI/180)*pow(sin(dlon/2),2);
    double distance = 2*atan2(sqrt(a),sqrt(1.0-a)) * 6378.135;

    return distance;
}

Charge_Choice Vehicle::chargeStationChoice(Charging_Station_General_Manager &cfm, int decision_time) {
        // Initiate a charge_choice instance.

        double distance_to_chosen_station = numeric_limits<double>::infinity();;
        int   chargingstationID = 0;
        array<double,2>  chargingstationpos ={0.0,0.0};

        switch(CFchoicealgorithm){
            case(1):{
                // This is to choose the closest charging station from current planposition to charge.
                for(auto const & it : cfm.getCfset()){
                    double distance = twoPointsDistance(it.second.getPos(), pos);
                    if (distance < distance_to_chosen_station){
                        chargingstationID = it.second.getId_();
                        chargingstationpos = it.second.getPos();
                        distance_to_chosen_station = distance;
                    }
                }
                break;
            }
            case(2):{

                /* Choose the charging station which allows the vehicle to finish charging the most earily.
                 * For every station, compute the distance to station, then the time to station.
                 * Find the corresponding charging rate for the arrived vehicle, and compute the charging time.
                 * Compute the waiting duration in the charging station.
                 * Find the station with the smallest end time.
                 *
                 * In this charging station choice algorithm, we have one limitation is that the vehicle will begin to charge as
                 * long as there is an empty charger. However, in some cases, vehicle can wait for a while and get a fast charger
                 * so that the final charge end is even eariler.
                 */

                int predictedchargeend = INT_MAX;
                int predictedchargebegin = 0;
                int thischargeend, final_charger_ID = 0;
                for(auto  & it : cfm.getCfset()){
                    double distance_to_station = twoPointsDistance(it.second.getPos(), pos);
                    int arrival_time = decision_time + int(distance_to_station/drivespeed);

                    auto pile_utilized_info = it.second.getCharger_occupation();
                    vector<int> wait_duration(pile_utilized_info.size());
                    int i = 0;
                    for (auto &one_pile_iter : pile_utilized_info) {
                        wait_duration[i] = max(one_pile_iter.back().back()-arrival_time,0);
                        i++;
                    }
                    auto minimal_iter = min_element(wait_duration.begin(),wait_duration.end());
                    int position = static_cast<int>(distance(wait_duration.begin(), minimal_iter));

                    thischargeend= arrival_time + wait_duration[position] + int((distance_to_station*consumrate + (chargeTo - soc)*capf)/it.second.getIntegrate_rate_vec()[position]);

                    if(predictedchargeend > thischargeend){
                        predictedchargeend = thischargeend;
                        predictedchargebegin = arrival_time + wait_duration[position];
                        chargingstationID = it.second.getId_();
                        chargingstationpos = it.second.getPos();
                        distance_to_chosen_station = distance_to_station;
                        final_charger_ID = position;
                    }
                }
                cfm.getCfset()[chargingstationID].addChargerOccupation(final_charger_ID,{predictedchargebegin, predictedchargeend});
                break;
            }
            case(3):{
                /*
                 * This charging choice strategy is an extension of case 2. In the above, a vehicle is chosen a charger
                 * so that it can charge as soon as possible. However, as mentioned above, a taxi might sacrifice a bit of
                 * waiting time and get charge as fast as possible. This algorithm implement this choice strategy.
                 *
                 * To use this charging choice, we need to change the charging station behavior to allows this kind of
                 * charging  service reservation.
                 */
                int predictedchargeend = INT_MAX, predictedchargebegin = 0;
                int final_charger_ID = 0;
                for(auto  & it : cfm.getCfset()){
                    double distance_to_station = twoPointsDistance(it.second.getPos(), pos);
                    int arrival_time = decision_time + int(distance_to_station/drivespeed);

                    auto pile_utilized_info = it.second.getCharger_occupation();

                    int i = 0, position = 0, wait_duration = 0, thischargeend = INT_MAX;
                    for (auto &one_pile_iter : pile_utilized_info) {
                        int charge_end = max(one_pile_iter.back().back()-arrival_time,0) +
                                int((distance_to_station*consumrate + (chargeTo - soc)*capf)/it.second.getIntegrate_rate_vec()[i]);
                        if(charge_end < thischargeend){
                            position = i;
                            thischargeend = charge_end;
                            wait_duration = max(one_pile_iter.back().back()-arrival_time,0);
                        }
                        i++;
                    }
                    if(predictedchargeend > thischargeend){
                        predictedchargeend = thischargeend;
                        predictedchargebegin = arrival_time + wait_duration;
                        chargingstationID = it.second.getId_();
                        chargingstationpos = it.second.getPos();
                        distance_to_chosen_station = distance_to_station;
                        final_charger_ID = position;
                    }
                }
                cfm.getCfset()[chargingstationID].addChargerOccupation(final_charger_ID,{predictedchargebegin, predictedchargeend});
                break;
            }
            default:
                break;
        }
        return Charge_Choice(chargingstationpos, chargingstationID,distance_to_chosen_station);
}

map<int, Charging_Station_General>::iterator
Vehicle::findTheChargeStaion(Charging_Station_General_Manager &cfm, array<double, 2> pos) {
        auto chargefacilityiter = cfm.getCfset().begin();

        for(auto iter = cfm.getCfset().begin(); iter!=cfm.getCfset().end(); iter++){
            if (iter->second.getPos() == pos){
                chargefacilityiter = iter;
                break;
            }
        }
        return chargefacilityiter;
}

int Vehicle::estimateWaitDuration(Charging_Station_General &cf) {

        vector<int> charge_end =cf.getChargeend()[0];
        for (const auto &i : cf.getChargeend()) {
            for(int value : i){
                charge_end.push_back(value);
            }
        }
        sort(charge_end.begin(),charge_end.end());
        return min(charge_end[cf.getWaitlist().size()]-this->time, 0);
}

 One_Charge &Vehicle::getCurrentcharge()  {
    return currentcharge;
}

void Vehicle::addACharge() {
 this->taxicharges.push_back(currentcharge);
}

/*Charge_Choice Vehicle::chargeStationChoice(Charging_Station_General_Manager &cfm) {
    double minimaldistance = numeric_limits<double>::infinity();

    int   chargingstationID = 0;
    array<double,2>  chargingstationpos ={0.0,0.0};

    for(auto const & it : cfm.getCfset()) {
        // This is to choose the closest charging station from current planposition to charge.
        double distance = twoPointsDistance(it.second.getPos(), pos);
        if (distance < minimaldistance) {
            chargingstationID = it.second.getId_();
            chargingstationpos = it.second.getPos();
            minimaldistance = distance;
        }
    }
    return Charge_Choice(chargingstationpos, chargingstationID,minimaldistance);
}*/
