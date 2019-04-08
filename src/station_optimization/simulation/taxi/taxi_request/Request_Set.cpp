//
// Created by ZhengchaoWANG on 07.09.18.
//

#include "station_optimization/simulation/taxi/taxi_request/Request_Set.h"

void Request_Set::addNewRequest(shared_ptr<Taxi_Request> newrequest) {
    this->request_set.push_back(newrequest);
}

void Request_Set::eraseARequest(vector<shared_ptr<Taxi_Request>>::iterator eraseiter) {
    this->request_set.erase(eraseiter);
}

bool Request_Set::isRequestLeft() {
    return request_set.empty();
}

void Request_Set::addNewRequestVec(vector<shared_ptr<Taxi_Request>> &newrequests) {
    this->request_set.reserve(request_set.size()+newrequests.size());
    this->request_set.insert(request_set.end(),newrequests.begin(),newrequests.end());
}
