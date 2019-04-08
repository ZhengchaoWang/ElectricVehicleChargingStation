//
// Created by ZhengchaoWANG on 07.09.18.
//

#ifndef CHARING_REQUEST_SET_H
#define CHARING_REQUEST_SET_H


#include<vector>
#include <memory>
#include "Taxi_Request.h"

using namespace std;

class Request_Set{
private:
    /*
     * Unique_ptr can not use "=" operator, it has to use move() to transfer ownership.
     */
    vector<shared_ptr<Taxi_Request>>                                                          request_set{}; // The set of request that needs to be assigned in future.

public:

    /********************************************* Constructors ***********************************************/

    Request_Set(vector<shared_ptr<Taxi_Request>> & this_request_set): request_set(this_request_set){};
    Request_Set() = default;
    /********************************************* Actions ***********************************************/

    void addNewRequest(shared_ptr<Taxi_Request> newrequest);

    void addNewRequestVec(vector<shared_ptr<Taxi_Request>> & newrequests);

    void eraseARequest(vector<shared_ptr<Taxi_Request>>::iterator eraseiter);

    bool isRequestLeft();

    /********************************************* Getter and setter ***********************************************/
     vector<shared_ptr<Taxi_Request>> &getLeft_request_set() {
        return request_set;
    }

    void setRequest_set( vector<shared_ptr<Taxi_Request>> &request_set) {
        Request_Set::request_set = request_set;
    }
};

#endif //CHARING_REQUEST_SET_H
