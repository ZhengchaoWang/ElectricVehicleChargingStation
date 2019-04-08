//
// Created by ZhengchaoWANG on 15.06.18.
//

#ifndef CHARING_CHARGE_CHOICE_H
#define CHARING_CHARGE_CHOICE_H

#include <array>

using namespace std;

class Charge_Choice{
private:
    array<double,2>                   pos;                    // plan position of the charge station
    double                       distance;                    // The distances between current planposition and charging station.
    int                              cfID;                    // the id number of the charging station.
    int                          duration = 0;                // how long to charge

public:
    /* Constructor and distructor */

    Charge_Choice() = default;

    Charge_Choice(const array<double, 2> &pos, int cfID, int duration, double distance) : pos(pos), cfID(cfID),
                                                                                             duration(duration), distance(distance) {};

    Charge_Choice(const array<double,2>  & pos,int cfID, double distance):pos(pos),cfID(cfID),distance(distance){};


     //void SetChargeChoice(const array<double, 2> &pos, int cfID, int duration, double distance);
    ~Charge_Choice() = default;

    /* Setter and Getter */

    const array<double, 2> &getPos() const {
        return pos;
    }

    void setPos(const array<double, 2> &pos) {
        Charge_Choice::pos = pos;
    }

    int getCfID() const {
        return cfID;
    }

    void setCfID(int cfID) {
        Charge_Choice::cfID = cfID;
    }

    int getDuration() const {
        return duration;
    }

    void setDuration(int duration) {
        Charge_Choice::duration = duration;
    }

    double getDistance() const {
        return distance;
    }

    void setDistance(double distance) {
        Charge_Choice::distance = distance;
    }

};


#endif //CHARING_CHARGING_CHOICE_H
