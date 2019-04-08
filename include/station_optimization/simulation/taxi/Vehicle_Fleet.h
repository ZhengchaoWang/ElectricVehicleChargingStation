//
// Created by ZhengchaoWANG on 09.09.18.
//

#ifndef CHARING_VEHICLE_FLEET_H
#define CHARING_VEHICLE_FLEET_H

#include<vector>
#include <station_optimization/simulation/taxi/normal_taxi/Taxi.h>
#include <station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.h>

using namespace std;

//template <class T>
class Vehicle_Fleet{
private:
    int                                                                                               id{};
    vector<shared_ptr<Taxi>>                                                                       my_fleet{};

public:
    /********************************************* Constructors ***********************************************/

//    Vehicle_Fleet(vector<Taxi> this_fleet):my_fleet(this_fleet) {};

    Vehicle_Fleet(vector<shared_ptr<Taxi>> this_fleet): my_fleet(this_fleet) {};

    Vehicle_Fleet(int id, vector<shared_ptr<Taxi>> &my_fleet) : id(id), my_fleet(my_fleet) {};

    /********************************************* Actions ***********************************************/
    void erase_member(vector<shared_ptr<Taxi>>::iterator taxi_iter){
        this->my_fleet.erase(taxi_iter);
    };

    void add_Member(shared_ptr<Taxi> & member){
        this->my_fleet.push_back(member);
    };
    /********************************************* Setter and getter ***********************************************/
    vector<shared_ptr<Taxi>> &getMy_fleet() {
        return my_fleet;
    }
};
/*
template <class T>
void Vehicle_Fleet<T>::erase_member(vector<shared_ptr<T>>::iterator taxi_iter) {
    this->my_fleet.erase(taxi_iter);
};

template<class T>
void Vehicle_Fleet<T>::add_Member(shared_ptr<T> &member) {
    this->my_fleet.push_back(member);
};*/
#endif //CHARING_VEHICLE_FLEET_H
