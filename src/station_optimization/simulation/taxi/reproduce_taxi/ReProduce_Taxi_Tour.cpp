//
// Created by ZhengchaoWANG on 04.07.18.
//

#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_Tour.h"

void ReProduce_Taxi_tour::generateStopTourFromTrackingData(Taxi_History &myhistory) {

    /*
     * We try to divide the tracking data into trips, and connect the trips to form a tour.
     *
     * Here, we divide trip into 3 different class: trip with passenger, trip without passenger, and stop.
     *
     * A stop is defined as the gap between two points which is seperated by long time. and the distance tween the two
     * points is smaller than 10km/h * time gap.
     *
     * Here, we cannot find out the cases 1110 where from 1 to 0, it is a huge gap or 11   11 where 1    1 has a huge gap.
     */

    int tripID = 0;
    int i = 0;

    while(i< (myhistory.getTime().size()-1)){

        bool state = myhistory.getCustags()[i];

        int j = 1;
        double triplength = 0;
        bool longstop = false;
        // Find one trip
        if(!state){
            while((i+j < myhistory.getTime().size()) & (myhistory.getCustags()[i+j] ==state)){
                int timediff = myhistory.getTime()[i+j] - myhistory.getTime()[i+j-1];
                if((timediff > threshold) & (myhistory.getDistance()[i+j] < threshspeed * timediff )){
                    longstop = true;
                    break;
                } else{
                    triplength += myhistory.getDistance()[i+j];
                    j++;
                }
            }
        } else {
            while((i+j < myhistory.getTime().size()) & (myhistory.getCustags()[i+j] ==state)){
                triplength += myhistory.getDistance()[i+j];
                j++;
            }
        }
        //add one trip.
        if(longstop){
            if(j!=1){
                ReProduce_Taxi_Trip mytrip(tripID, {i,i+j-1}, myhistory.getPoss()[i], myhistory.getPoss()[i+j-1], triplength, myhistory.getTime()[i],
                                           myhistory.getTime()[i+j-1], state);
                appendNewTripToTour(mytrip);
                i+=j-1;
                tripID++;
            } else {
                triplength = myhistory.getDistance()[i+j];
                ReProduce_Taxi_Trip mytrip(tripID, {i,i+j}, myhistory.getPoss()[i], myhistory.getPoss()[i+j], triplength, myhistory.getTime()[i],
                                           myhistory.getTime()[i+j], false); // try to initialize as int.
                appendNewTripToTour(mytrip);
                i++;
                tripID++;
            }
        } else{
            if((i+j) >= myhistory.getTime().size()){
                j = int(myhistory.getTime().size()-i -1);
            }
            ReProduce_Taxi_Trip mytrip(tripID, {i,i+j}, myhistory.getPoss()[i], myhistory.getPoss()[i+j], triplength, myhistory.getTime()[i],
                                       myhistory.getTime()[i+j], state);
            appendNewTripToTour(mytrip);
            i +=j;
            tripID++;
        }
    }
}

void ReProduce_Taxi_tour::generateTourFromTrackingData(Taxi_History &myhistory) {
    int tripID = 0;
    int i = 0;

    while(i< (myhistory.getTime().size()-1)) {

        bool state = myhistory.getCustags()[i];

        int j = 1;
        double triplength = 0;
        while((i+j < myhistory.getTime().size()) & (myhistory.getCustags()[i+j] ==state)){
            triplength += myhistory.getDistance()[i+j];
            j++;
        }
        if((i+j)>=myhistory.getTime().size()){
            j = int(myhistory.getTime().size())-i-1;
        }
        ReProduce_Taxi_Trip mytrip(tripID, {i,i+j}, myhistory.getPoss()[i], myhistory.getPoss()[i+j], triplength, myhistory.getTime()[i],
                                   myhistory.getTime()[i+j], state);
        appendNewTripToTour(mytrip);
        i+=j;
        tripID++;
    }
}

void ReProduce_Taxi_tour::appendNewTripToTour(const ReProduce_Taxi_Trip &newtrip) {
    ReProduce_Taxi_tour::trips.push_back(newtrip);
}

/****************** Getter and Setters **********************/

vector<ReProduce_Taxi_Trip> &ReProduce_Taxi_tour::getTrips() {
    return trips;
}





