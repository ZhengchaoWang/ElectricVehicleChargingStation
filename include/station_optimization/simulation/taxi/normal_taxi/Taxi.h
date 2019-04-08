//
// Created by ZhengchaoWANG on 06.09.18.
//

#ifndef CHARING_TAXI_H
#define CHARING_TAXI_H


#include "station_optimization/simulation/taxi/Vehicle.h"
#include "station_optimization/simulation/taxi/taxi_request/Request_Set.h"
#include "station_optimization/simulation/taxi/taxi_request/Taxi_Request.h"

using namespace std;

class Taxi : public Vehicle{
private:
    Request_Set                                                         my_served_request; // served request of one taxi during simulation.
    shared_ptr<Taxi_Request>                                            my_future_request;
    Request_Set                                                         available_request;

    double                                                                    revenue = 0;
    bool                                                                 customer = false;
    // Related to the cost
    double                                                             initial_cost = 3.5;
    double                                                      unit_distance_cost = 1.71;
    double                                                          minimal_distance =0.0; // This distance is with in the initial cost.

    /*********************************************** Helper function ************************************************/
    void addTaxiRequest(shared_ptr<Taxi_Request> newrequest);
    void computeServiceFee (double travel_distance);

public:

    /*********************************************** Constructors ************************************************/
    using Vehicle::Vehicle;

    /********************************************* Decisions to make *********************************************/
    void searchForPassenger();

    void emptyTripDecision();

    void acceptTaxiRequest(shared_ptr<Taxi_Request> & newrequest);

    void charge();

    /************************************************* Actions ***************************************************/
    void drive(shared_ptr<Taxi_Request> & newrequest);

    void emptyDrive();

    void run();

    /******************************************** getters and setters ********************************************/
    bool isCustomer() const;

    void setCustomer(bool customer);

    void setMy_future_request(shared_ptr<Taxi_Request> &my_future_request);

    void setAvailable_request( vector<shared_ptr<Taxi_Request>> &available_request);
};

#endif //CHARING_TAXI_H
