//
// Created by ZhengchaoWANG on 06.09.18.
//

#ifndef CHARING_TAXI_FLEET_H
#define CHARING_TAXI_FLEET_H

#include <vector>
#include <string>
#include <array>


#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.h"
#include "station_optimization/simulation/taxi/normal_taxi/Taxi.h"
#include "Taxi_History.h"

using  namespace std;

class Taxi_Fleet {
private:
    int                                                                                   id{};
    vector<Taxi>                                                                    my_fleet{};

public:

    /********************************************* Constructors ***********************************************/

    Taxi_Fleet() = default;

    /********************************************* Actions ***********************************************/
    void erase_Taxi(vector<Taxi>::iterator taxi_iter);

    /********************************************* Setter and getter ***********************************************/
    vector<Taxi> &getMy_fleet() {
        return my_fleet;
    }
};

void Taxi_Fleet::erase_Taxi(vector<Taxi>::iterator taxi_iter) {
    this->my_fleet.erase(taxi_iter);
}

#endif //CHARING_TAXI_FLEET_H
