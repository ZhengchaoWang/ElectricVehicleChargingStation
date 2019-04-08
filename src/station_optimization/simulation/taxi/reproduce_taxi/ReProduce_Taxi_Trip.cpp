//
// Created by ZhengchaoWANG on 04.07.18.
//

#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_Trip.h"

const array<double, 2> &ReProduce_Taxi_Trip::getBeginposition() const {
    return beginposition;
}

const array<double, 2> &ReProduce_Taxi_Trip::getEndposition() const {
    return endposition;
}

double ReProduce_Taxi_Trip::getTriplength() const {
    return triplength;
}

int ReProduce_Taxi_Trip::getBegintime() const {
    return begintime;
}

int ReProduce_Taxi_Trip::getEndtime() const {
    return endtime;
}

bool ReProduce_Taxi_Trip::getCustag() const {
    return custag;
}

void ReProduce_Taxi_Trip::setCustag(bool state) {
    this->custag = state;
}

double ReProduce_Taxi_Trip::getConsumedenergy() const {
    return consumedenergy;
}

bool ReProduce_Taxi_Trip::isServed() const {
    return served;
}

void ReProduce_Taxi_Trip::setServed(bool served) {
    ReProduce_Taxi_Trip::served = served;
}
