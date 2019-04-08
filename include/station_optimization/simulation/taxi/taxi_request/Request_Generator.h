//
// Created by ZhengchaoWANG on 19.07.18.
//

#ifndef CHARING_REQUEST_GENERATOR_H
#define CHARING_REQUEST_GENERATOR_H

#include "Taxi_Request.h"

#include <vector>
#include <array>

using namespace std;

class Request_Generator{
private:
    Taxi_Request        taxirequest;

public:
    Request_Generator() = default;

    void fixedTaxiRequestGenerator();

    void simulateTaxiRequestGenerator();

};





#endif //CHARING_REQUEST_GENERATOR_H
