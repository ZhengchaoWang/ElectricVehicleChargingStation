/* Plan.cpp */
// Created by ZhengchaoWANG on 13.06.18.


// Consider to finish the updatePlan with a Plan.

#include <cmath>
#include "station_optimization/simulation/helpers/Plan.h"

using namespace std;

double Plan::compute_distance(const array<double, 2> & pos1, const array<double, 2> & pos2) {

    double dlat = (pos2[0]-pos1[0])/M_PI;
    double dlon = (pos2[1]-pos1[0])/M_PI;
    double a = pow(sin(dlat/2),2) + cos(pos1[0]/M_PI)*cos(pos2[0]*M_PI/180)*pow(sin(dlon/2),2);
    double distance = 2*atan2(sqrt(a),sqrt(1.0-a)) * 6378.135;

    return distance  ;
}

void Plan::updatePlan(deque<int> &newtime, deque<int> &newaction, deque<array<double, 2>> &newposition,
                      deque<double> & newenergy, deque<int> & newindex) {
    this->plan_time = newtime;
    this->plan_next_state = newaction;
    this->plan_position = newposition;
    this->plan_energy = newenergy;
    this->plan_track_index = newindex;
}

void Plan::deleteFirstElement() {
    this->plan_time.pop_front();
    this->plan_next_state.pop_front();
    this->plan_position.pop_front();
    this->plan_energy.pop_front();
    this->plan_track_index.pop_front();
}

/*void Plan::updatePlan(Plan & newplan) {
    this = newplan;
}*/


