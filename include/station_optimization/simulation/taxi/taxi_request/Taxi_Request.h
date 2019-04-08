//
// Created by ZhengchaoWANG on 19.07.18.
//

#ifndef CHARING_TAXI_REQUEST_H
#define CHARING_TAXI_REQUEST_H

#include <cmath>
#include <vector>
#include <array>

using namespace std;

class Taxi_Request{
private:
    int                                         requestID = 0;

    // Property of one taxi request. From the request simulator (generator).
    array<double,2>                             origin;
    array<double,2>                             destination;
    double                                      length = 0;
    int                                         desirestarttime=0;
    int                                         desirearrivetime=0;
    int                                         waitwillingness = 600; // in a unit of seconds.
    int                                         passengernumber=1;

    bool                                        served = false;
    int                                         starttime=0;
    int                                         arrivetime=0;
    int                                         serviceprovidetaxiID = -1;
    double                                      requestcost = 0;

public:

    /* Constructor */
    Taxi_Request() = default;

    Taxi_Request(int requestID, array<double,2> orig, array<double,2> dest, int desirestarttime, int desirearrivetime):
            requestID(requestID), origin(orig),destination(dest), desirestarttime(desirestarttime), desirearrivetime(desirearrivetime){};

    double computeDistance(array<double,2> pos1, array<double,2> pos2);

    void computeDirectlength();

    void requestServedTaxiID(int taxiID);

    /* Setters and getters */

    int getRequestID() const {
        return requestID;
    }

    void setRequestID(int requestID) {
        Taxi_Request::requestID = requestID;
    }

    const array<double, 2> &getOrigin() const {
        return origin;
    }

    void setOrigin(const array<double, 2> &origin) {
        Taxi_Request::origin = origin;
    }

    const array<double, 2> &getDestination() const {
        return destination;
    }

    void setDestination(const array<double, 2> &destination) {
        Taxi_Request::destination = destination;
    }

    double getLength() const {
        return length;
    }

    void setLength(double length) {
        Taxi_Request::length = length;
    }

    int getDesirestarttime() const {
        return desirestarttime;
    }

    void setDesirestarttime(int desirestarttime) {
        Taxi_Request::desirestarttime = desirestarttime;
    }

    int getDesirearrivetime() const {
        return desirearrivetime;
    }

    void setDesirearrivetime(int desirearrivetime) {
        Taxi_Request::desirearrivetime = desirearrivetime;
    }

    int getStarttime() const {
        return starttime;
    }

    void setStarttime(int starttime) {
        Taxi_Request::starttime = starttime;
    }

    int getArrivetime() const {
        return arrivetime;
    }

    void setArrivetime(int arrivetime) {
        Taxi_Request::arrivetime = arrivetime;
    }

    int getPassengernumber() const {
        return passengernumber;
    }

    void setPassengernumber(int passengernumber) {
        Taxi_Request::passengernumber = passengernumber;
    }

    bool isServed() const {
        return served;
    }

    void setServed(bool served) {
        Taxi_Request::served = served;
    }

    int getServiceprovidetaxiID() const {
        return serviceprovidetaxiID;
    }

    void setServiceprovidetaxiID(int serviceprovidetaxiID) {
        Taxi_Request::serviceprovidetaxiID = serviceprovidetaxiID;
    }

    double getRequestcost() const {
        return requestcost;
    }

    void setRequestcost(double requestcost) {
        Taxi_Request::requestcost = requestcost;
    }

    int getWaitwillingness() const {
        return waitwillingness;
    }

};




#endif //CHARING_TAXI_REQUEST_H
