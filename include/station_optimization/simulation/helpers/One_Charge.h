//
// Created by ZhengchaoWANG on 06.07.18.
//

#ifndef CHARING_ONE_CHARGE_H
#define CHARING_ONE_CHARGE_H

#include <array>

using namespace std;

class One_Charge{
private:

    int                     chargestationID;    // which station that the charge is carried out
    double                  startSoC;           // the state of charge when he decides to charge.
    array<double,2>         startposition;      // the point that the vehicle decides to charge.
    int                     starttime = 0;          // the time that the vehicle decides to charge.
    array<double,2>         chosenstationpos;   // position of chosen station

    double                  arriveSoC;          // the state of charge when vehicle arrives at the charging station.
    int                     arrivetime = 0;         // the time that the vehicle arrives at the station.
    double                  distancetostation;  // how far that the vehicle drives to arrive at the charging station.

    int                     beginchargetime = 0;    // the time that the vehicle begins to charge.
    double                  chargerate;         // how fast the vehicle is charging during this charge.
    int                     chargetype;
    double                  chargecost = 0;         // how much the vehicle has to pay for this charge.
    int                     chargetime = 0;         // how long that the vehicle uses to charge (charge end time - begin charge time).
    double                  endSoc;             // the state of charge at the end of charging.

    double                  distancetotracking=0; // how far that the vehicle has to drive before merge the tracking trajectories.
    int                     timetotracking=0;     // how long does it take for the vehicle to merge the tracking trajectory from the charging station.
    double                  finalSoC;           // the state of charge at the end of merging to the tracking trajectories.

    bool                    finished = false;
public:
    One_Charge() = default;

    void setWantCharge(int chargestationID, double startSoC, array<double,2> startposition, int starttime, array<double,2> stationpos);
    void setArriveCharge(double arriveSoC, int arrivetime, double distancetostation);
    void setBeginCharge(int beginchargetime, double chargerate, int chargetype, double chargecost, int chargetime, double endSoC);
    void setFinishCharge(double distancetotracking, int timetotracking, double finalSoC, bool finished = true); // Here, there is a little problem, if we goes from 5 to 6, then, the charge is finished

    void stopBeforeFinishCharge(int endtime);
    /********** Setters and getters ********************/
    int getChargestationID() const;

    void setChargestationID(int chargestationID);

    double getStartSoC() const;

    void setStartSoC(double startSoC);

    const array<double, 2> &getStartposition() const;

    void setStartposition(const array<double, 2> &startposition);

    int getStarttime() const;

    void setStarttime(int starttime);

    double getArriveSoC() const;

    void setArriveSoC(double arriveSoC);

    int getArrivetime() const;

    void setArrivetime(int arrivetime);

    double getDistancetostation() const;

    void setDistancetostation(double distancetostation);

    double getChargerate() const;

    void setChargerate(double chargerate);

    double getChargecost() const;

    void setChargecost(double chargecost);

    int getChargetime() const;

    void setChargetime(int chargetime);

    double getEndSoc() const;

    void setEndSoc(double endSoc);

    double getDistancetotracking() const;

    void setDistancetotracking(double distancetotracking);

    int getTimetotracking() const;

    void setTimetotracking(int timetotracking);

    double getFinalSoC() const;

    void setFinalSoC(double finalSoC);

    int getBeginchargetime() const;

    void setBeginchargetime(int beginchargetime);

    int getChargeType() const;

    const array<double, 2> &getChosenstationpos() const;

};

#endif //CHARING_ONE_CHARGE_H
