//
// Created by ZhengchaoWANG on 04.07.18.
//

/*
 * What is the advantage to seperate long stop with empty taxi?
 */

#ifndef CHARING_REPRODUCE_TAXI_TRIP_H
#define CHARING_REPRODUCE_TAXI_TRIP_H

#include <array>
#include <vector>

using namespace std;

class ReProduce_Taxi_Trip{

private:
    int                     tripID;

    array<int,2>            beginendindex;
    array<double,2>         beginposition;
    array<double,2>         endposition;
    double                  triplength;
    int                     begintime;
    int                     endtime;
    bool                    custag;
    double                  consumedenergy=0;

    bool                    served = false; // if this trip is served or not.

public:

    ReProduce_Taxi_Trip() = default;

    ReProduce_Taxi_Trip(int ID,array<int,2>beginendindex, array<double,2> beginposition, array<double,2> endposition, double triplength, int begintime
        ,int endtime, bool state):tripID(ID), beginendindex(beginendindex), beginposition(beginposition),endposition(endposition),triplength(triplength),
                                 begintime(begintime),endtime(endtime), custag(state){}

    /* Getters and Setters*/

    const array<int, 2> &getBeginendindex() const {
        return beginendindex;
    }

    const array<double, 2> &getBeginposition() const;

    const array<double, 2> &getEndposition() const;

    double getTriplength() const;

    int getBegintime() const;

    int getEndtime() const;

    bool getCustag() const;

    void setCustag(bool custag);

    double getConsumedenergy() const;

    bool isServed() const;

    void setServed(bool served);

};


#endif //CHARING_REPRODUCE_TAXI_TRIP_H
