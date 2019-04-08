/* Plan.h */
// Created by ZhengchaoWANG on 13.06.18.
//

#ifndef CHARING_PLAN_H
#define CHARING_PLAN_H


#include <deque>
#include <array>

using namespace std;

class Plan {
private:
    deque<int>                                      plan_time;
    deque<int>                                plan_next_state; // 'empty move = 0. with customer move = 1, choose charge station = 2, arrive at charge station= 3, begin to charge = 4, leave charge station = 5'
    deque<array<double,2>>                      plan_position;
    deque<double>                                 plan_energy; // Here, when the planenergy is +, then we mean that it is charging. When the planenergy is '-', the vehicle is consuming planenergy for next step.
    deque<int>                               plan_track_index; // at the plan point, which track point I corresponding to.
    deque<double>                         plan_charge_rate = {0.0};
    deque<int>                            plan_charge_type = {0};
public:
    // Constructors

    Plan(deque<int> & time, deque<int> & action, deque<array<double,2>> & position, deque<double> & energy, deque<int> & plantrackindex)
            :plan_time(time),plan_next_state(action), plan_position(position),plan_energy(energy), plan_track_index(plantrackindex){};

    Plan(deque<int> & time, deque<int> & action, deque<array<double,2>> & position, deque<double> & energy, deque<int> & plantrackindex, deque<double> & planchargerate, deque<int> & planchargetype)
            :plan_time(time),plan_next_state(action), plan_position(position),plan_energy(energy), plan_track_index(plantrackindex),plan_charge_rate(planchargerate), plan_charge_type(planchargetype){}


    Plan(){}

    // distructor
   // ~Plan(){}

    // update the Plan with the latest Plan.

    void updatePlan(deque<int> & newtime, deque<int> & newaction, deque<array<double,2>> & newposition, deque<double> & newenergy, deque<int> &newindex);

    double compute_distance( const array<double,2> & pos1, const array<double,2> & pos2);

    void deleteFirstElement();

    /* Setter and Getter */
    const deque<int> &getPlantime() const {
        return plan_time;
    }

    const deque<int> &getPlannextstate() const {
        return plan_next_state;
    }

    const deque<array<double, 2>> &getPlanposition() const {
        return plan_position;
    }

    const deque<double> &getPlanenergy() const {
        return plan_energy;
    }

    const deque<int> &getPlantrackindex() const {
        return plan_track_index;
    }

    const deque<double> &getPlanchargerate() const {
        return plan_charge_rate;
    }

    const deque<int> &getPlanChargeType() const {
        return plan_charge_type;
    }
};

#endif //CHARING_PLAN_H