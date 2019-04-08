//
// Created by ZhengchaoWANG on 04.07.18.
//

#ifndef CHARING_REPRODUCE_TAXI_TOUR_H
#define CHARING_REPRODUCE_TAXI_TOUR_H

#include <vector>


#include "Taxi_History.h"
#include "ReProduce_Taxi_Trip.h"

using namespace std;

class ReProduce_Taxi_tour{
private:
    int                              tourID = 0;
    vector<ReProduce_Taxi_Trip>      trips;

    int                              threshold = 1200;
    double                           threshspeed = 1.0/120.0;

public:

    ReProduce_Taxi_tour() = default;

    ReProduce_Taxi_tour(int ID, vector<ReProduce_Taxi_Trip> & mytrips):tourID(ID), trips(mytrips){}

    void generateStopTourFromTrackingData(Taxi_History &myhistory);

    void generateTourFromTrackingData(Taxi_History &myhistory);

    void appendNewTripToTour(const ReProduce_Taxi_Trip &newtrip);

    /************************* Getters and Setters **********************************/

    vector<ReProduce_Taxi_Trip> &getTrips() ;


};

#endif //CHARING_REPRODUCE_TAXI_TOUR_H
