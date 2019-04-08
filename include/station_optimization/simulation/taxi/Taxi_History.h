//
// Created by ZhengchaoWANG on 14.06.18.
//

#ifndef CHARING_VEHICLE_FUTURE_H
#define CHARING_VEHICLE_FUTURE_H

#include <array>
#include <vector>

using namespace std;

class Taxi_History{
private:
    vector<array<double,2>>                   poss; // This represent the future positions of vehicle
    vector<bool>                           custags; // Customer tags of the vehicles
    vector<int>                               time; // plantime of each step
    vector<double>                        distance; // The distance between two planposition
    vector<double>                        speed;

public:

    /* Constructors and Distructors*/

    Taxi_History( vector<array<double, 2>> & poss,  vector<bool> &custags,  vector<int> &time,
        vector<double> & distance) : poss(poss), custags(custags), time(time), distance(distance) {}

    Taxi_History( vector<array<double, 2>> & poss,  vector<bool> &custags,  vector<int> &time,
    vector<double> & distance,vector<double> & speed) : poss(poss), custags(custags), time(time), distance(distance), speed(speed) {}

    Taxi_History( vector<array<double, 2>> & poss,  vector<int> &time,  vector<double> & distance)
            : poss(poss), time(time), distance(distance) {}

    Taxi_History() {}

    //~Taxi_History() {}


    /*  Actions  */
    void updateFuture(Taxi_History & future){
        this->poss = future.poss;
        this->custags = future.custags;
        this->time = future.time;
    }

    void updateFuture(vector<array<double,2>> & newposs, vector<bool> & newcustags, vector<int> & newtime){
        this->poss = newposs;
        this->custags = newcustags;
        this->time = newtime;
    }


    /* Getters and Setters */

    const vector<array<double, 2>> &getPoss() const {
        return poss;
    }

    void setPoss(const vector<array<double, 2>> &poss) {
        Taxi_History::poss = poss;
    }

    vector<bool> & getCustags() {
        return custags;
    }

    void setCustags(const vector<bool> &custags) {
        Taxi_History::custags = custags;
    }

    const vector<int> &getTime() const {
        return time;
    }

    void setTime(const vector<int> &time) {
        Taxi_History::time = time;
    }

    const vector<double> &getDistance() const {
        return distance;
    }

    void setDistance(const vector<double> &distance) {
        Taxi_History::distance = distance;
    }
};



#endif //CHARING_VEHICLE_FUTURE_H
