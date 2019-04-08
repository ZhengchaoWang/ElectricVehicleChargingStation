//
// Created by ZhengchaoWANG on 06.09.18.
//

#include "station_optimization/simulation/taxi/normal_taxi/Taxi.h"

void Taxi::addTaxiRequest(shared_ptr<Taxi_Request> newrequest){
    this->my_served_request.addNewRequest(newrequest);
}

void Taxi::acceptTaxiRequest(shared_ptr<Taxi_Request> &newrequest) {
    /*
     * Should we decide acceptance in this class or in assignment class?
     * In this class, then it needs a communication between taxi and assignment.
     */
    double distance_to_request = twoPointsDistance(pos, newrequest->getOrigin());
    double travel_distance = distance_to_request +
            twoPointsDistance(newrequest->getOrigin(), newrequest->getDestination());
    bool accpeted = true;
    if(customer){
        cout << "There is a problem with the assignment algorithm or the customer tag change" << endl;
    }
    if(((soc-travel_distance*consumrate/capf) < minSoc) & (int(distance_to_request/drivespeed)<newrequest->getWaitwillingness())){
        accpeted = false;
    }
    if(accpeted){
        this->customer = true;
        drive(newrequest);
        addTaxiRequest(newrequest);

    }
}

void Taxi::computeServiceFee(double travel_distance) {
    this->revenue += initial_cost + unit_distance_cost*(travel_distance-minimal_distance);
}

void Taxi::drive(shared_ptr<Taxi_Request> & newrequest) {
    /*
     * This function achieve that drive the customer to its destination and change its state accordingly.
     * In addition, the request also change its states and add corresponding information to itself.
     */
    double distance_to_request = twoPointsDistance(pos,newrequest->getOrigin());
    newrequest->setStarttime(int(distance_to_request/drivespeed)+time);
    double request_length = twoPointsDistance(newrequest->getOrigin(), newrequest->getDestination());
    newrequest->setArrivetime(int(request_length/drivespeed)+newrequest->getStarttime());
    this->pos = newrequest->getDestination();
    this->time = newrequest->getArrivetime();
    this->soc -= (distance_to_request + request_length)*consumrate/capf;
    this->soc = false;
}

void Taxi::emptyDrive() {

}

void Taxi::run() {

}
/****************************************** Getter and setters ********************************************/
bool Taxi::isCustomer() const {
    return customer;
}

void Taxi::setCustomer(bool customer) {
    Taxi::customer = customer;
}

void Taxi::setMy_future_request(shared_ptr<Taxi_Request> &my_future_request) {
    Taxi::my_future_request = my_future_request;
}

void Taxi::searchForPassenger() {

}

void Taxi::emptyTripDecision() {

}

void Taxi::setAvailable_request( vector<shared_ptr<Taxi_Request>> &available_request) {
    Taxi::available_request.setRequest_set(available_request);
}

void Taxi::charge() {


}






