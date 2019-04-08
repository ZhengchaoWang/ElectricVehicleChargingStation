//
// Created by ZhengchaoWANG on 06.07.18.
//

#include "station_optimization/simulation/helpers/One_Charge.h"

void One_Charge::setWantCharge(int chargestationID, double startSoC, array<double, 2> startposition, int starttime, array<double,2> stationpos) {
    this->chargestationID = chargestationID;
    this->startSoC = startSoC;
    this->startposition = startposition;
    this->starttime = starttime;
    this->chosenstationpos = stationpos;
}

void One_Charge::setArriveCharge(double arriveSoC, int arrivetime, double distancetostation) {
    this->arriveSoC = arriveSoC;
    this->arrivetime = arrivetime;
    this->distancetostation = distancetostation;
}

void One_Charge::setBeginCharge(int begintimetocharge, double chargerate,int chargetype, double chargecost, int chargetime, double endSoC) {
    this->beginchargetime = begintimetocharge;
    this->chargerate = chargerate;
    this->chargetype = chargetype;
    this->chargecost = chargecost;
    this->chargetime = chargetime;
    this->endSoc = endSoC;
}

void One_Charge::setFinishCharge(double distancetotracking, int timetotracking, double finalSoC, bool finished) {
    this->distancetotracking = distancetotracking;
    this->timetotracking= timetotracking;
    this->finalSoC = finalSoC;
    this->finished = finished;
}

void One_Charge::stopBeforeFinishCharge(int endtime) {
    if (this->arrivetime ==0){
        this->arrivetime = endtime;
    }
    if (this->beginchargetime ==0){
        this->beginchargetime = endtime;
    }
}

/********************* Setters and Getters ****************************************/

int One_Charge::getChargestationID() const {
    return chargestationID;
}

void One_Charge::setChargestationID(int chargestationID) {
    One_Charge::chargestationID = chargestationID;
}

double One_Charge::getStartSoC() const {
    return startSoC;
}

void One_Charge::setStartSoC(double startSoC) {
    One_Charge::startSoC = startSoC;
}

const array<double, 2> &One_Charge::getStartposition() const {
    return startposition;
}

void One_Charge::setStartposition(const array<double, 2> &startposition) {
    One_Charge::startposition = startposition;
}

int One_Charge::getStarttime() const {
    return starttime;
}

void One_Charge::setStarttime(int starttime) {
    One_Charge::starttime = starttime;
}

double One_Charge::getArriveSoC() const {
    return arriveSoC;
}

void One_Charge::setArriveSoC(double arriveSoC) {
    One_Charge::arriveSoC = arriveSoC;
}

int One_Charge::getArrivetime() const {
    return arrivetime;
}

void One_Charge::setArrivetime(int arrivetime) {
    One_Charge::arrivetime = arrivetime;
}

double One_Charge::getDistancetostation() const {
    return distancetostation;
}

void One_Charge::setDistancetostation(double distancetostation) {
    One_Charge::distancetostation = distancetostation;
}

double One_Charge::getChargerate() const {
    return chargerate;
}

void One_Charge::setChargerate(double chargerate) {
    One_Charge::chargerate = chargerate;
}

double One_Charge::getChargecost() const {
    return chargecost;
}

void One_Charge::setChargecost(double chargecost) {
    One_Charge::chargecost = chargecost;
}

int One_Charge::getChargetime() const {
    return chargetime;
}

void One_Charge::setChargetime(int chargetime) {
    One_Charge::chargetime = chargetime;
}

double One_Charge::getEndSoc() const {
    return endSoc;
}

void One_Charge::setEndSoc(double endSoc) {
    One_Charge::endSoc = endSoc;
}

double One_Charge::getDistancetotracking() const {
    return distancetotracking;
}

void One_Charge::setDistancetotracking(double distancetotracking) {
    One_Charge::distancetotracking = distancetotracking;
}

int One_Charge::getTimetotracking() const {
    return timetotracking;
}

void One_Charge::setTimetotracking(int timetotracking) {
    One_Charge::timetotracking = timetotracking;
}

double One_Charge::getFinalSoC() const {
    return finalSoC;
}

void One_Charge::setFinalSoC(double finalSoC) {
    One_Charge::finalSoC = finalSoC;
}

int One_Charge::getBeginchargetime() const {
    return beginchargetime;
}

int One_Charge::getChargeType() const {
    return chargetype;
}

const array<double, 2> &One_Charge::getChosenstationpos() const {
    return chosenstationpos;
}

void One_Charge::setBeginchargetime(int beginchargetime) {
    One_Charge::beginchargetime = beginchargetime;
}



