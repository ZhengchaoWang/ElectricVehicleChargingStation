//
// Created by ZhengchaoWANG on 14.06.18.
//

#ifndef CHARING_TAXI_DEMAND_H
#define CHARING_TAXI_DEMAND_H

#include <array>

using namespace std;

class Taxi_Demand{
private:
    array<int,2>         time;  // The beginning and end time of the taxi ride.
    double               length; // The length of the ride demand
    array<double,2>      sPosition; // start planposition of the taxi ride
    array<double,2>      ePosition; // End planposition of the taxi ride
    bool                  served; // A bool value to represent if the taxi demand is served.

public:

    /* Constructors and distructors  */
    Taxi_Demand(const array<int, 2> &time, double length, const array<double, 2> &sPosition,
                const array<double, 2> &ePosition, const bool & served) : time(time), length(length), sPosition(sPosition),
                                                     ePosition(ePosition), served(served) {}

    Taxi_Demand(const array<int, 2> &time, double length) : time(time), length(length) {}

    //~Taxi_Demand(){}

    /* Setters and Getters */

    const array<int, 2> &getTime() const {
        return time;
    }

    void setTime(const array<int, 2> &time) {
        Taxi_Demand::time = time;
    }

    double getLength() const {
        return length;
    }

    void setLength(double length) {
        Taxi_Demand::length = length;
    }

    const array<double, 2> &getSPosition() const {
        return sPosition;
    }

    void setSPosition(const array<double, 2> &sPosition) {
        Taxi_Demand::sPosition = sPosition;
    }

    const array<double, 2> &getEPosition() const {
        return ePosition;
    }

    void setEPosition(const array<double, 2> &ePosition) {
        Taxi_Demand::ePosition = ePosition;
    }

    bool isServed() const {
        return served;
    }

    void setServed(bool served) {
        Taxi_Demand::served = served;
    }
};


#endif //CHARING_TAXI_DEMAND_H
