//
// Created by ZhengchaoWANG on 27.07.18.
//

#include "station_optimization/simulation/helpers/One_Simulation.h"

void One_Simulation::stationAction(int current_time) {
    /*
     * First step, delete all the waiting list whose trajectory is already end. add this delete to waiting list to show the irreasonable of the charging choice.
     *
     * Then find the minimum of left charging slots or waiting list so that we can schedule the plan of waiting vehicle.
     * However, the left charging slots is left from last time step. Therefore, there is a little "postpone bug". This is
     * caused by a taxi left the charging station but no action to charge waiting vehicle in the taxiAction(). This postpone
     * is also acceptable as it needs time to charge a following vehicle.
     *
     * Schedule waiting vehicle to charge.
     */
    for (auto &cfiter : this->my_stations.getCfset()){
        int totaldeletenumber= 0;
        for(int i = 0; i< cfiter.second.getWaitlist().size();i++){
            int taxiID = cfiter.second.getWaitlist()[i];

            if(my_fleet[taxiID].getTrackend() == current_time) {
                my_fleet[taxiID].getCurrentcharge().stopBeforeFinishCharge(current_time);
                my_fleet[taxiID].addACharge();
                cfiter.second.departWaitList(taxiID, current_time, i, true);
                totaldeletenumber++;
                i--;
            }
        }
        if(totaldeletenumber > 0){
            cfiter.second.addUnchargedeletenumber(current_time,totaldeletenumber);
        }

        // find the minimum of left charging slots and waitlist;
        vector<double> left_rate;
        vector<int> charge_type_index_in_vector;
        for(int i = 0; i< cfiter.second.getRate().size();i++){
            int freeslotnum = cfiter.second.getCap()[i]- cfiter.second.getOcupslot()[i];
            for(int j = 0; j<freeslotnum;j++){
                left_rate.push_back(cfiter.second.getRate()[i]);
                charge_type_index_in_vector.push_back(i);
            }
        }
        int loopnumber  = (int) min(left_rate.size(), cfiter.second.getWaitlist().size());

        for(int i = 0; i < loopnumber; i++){
            int taxiID = *(cfiter.second.getWaitlist().begin()+i); // Here might be a bug if there is a deletion of two list. Maybe this is the reason.
            // If these car already ended, delete it. otherwise, start to charge it from now and try to get next state.
            if(my_fleet[taxiID].getTrackend()>current_time){
                my_fleet[taxiID].addWaitDuration(cfiter.second.getId_(), current_time - my_fleet[taxiID].getTime());

                int index = my_fleet[taxiID].findIndexInFuture(current_time);
                my_fleet[taxiID].setThePlan(index,4,0,current_time, my_fleet[taxiID].getPos(),left_rate[i], charge_type_index_in_vector[i]);
                my_fleet[taxiID].setTime(current_time);
            } else {
                cout << "A test in stationAction() function. Delete of waiting list is not correctly done. taxi id is " << taxiID << loopnumber << endl;
            }
        }
    }
}

void One_Simulation::taxiAction(int current_time){
    for (auto &my_taxi : this->my_fleet) {
        if(my_taxi.getTime() <= my_taxi.getMyhistory().getTime().back()){
            if (my_taxi.getTime() == current_time){
                my_taxi.generatePlan(this->my_stations);
                my_taxi.move(this->my_stations);
            }
        }
    }
}

void One_Simulation::run() {

    for(int timeStamp = begin_time; timeStamp <= end_time;timeStamp++){
        stationAction(timeStamp);
        taxiAction(timeStamp);
    }

    this_simulation_performance = Taxi_Performance_General();
    this_simulation_performance.setAlpha(objective_alpha);
    this_simulation_performance.setObjectivefunctionchoice(objectivefunctionchoice);
    this_simulation_performance.computePerformance(this->my_fleet);//thisperformance.
    this_simulation_performance.computeChargingStationIndex(this->my_stations,end_time - begin_time);
    this->setSimulation_performance(this_simulation_performance.getPerformance());

    if(output_one_simulation_result){
        outputSimulationResult();
    }
}

void One_Simulation::outputSimulationResult() {
    this_simulation_performance.writeTaxiDemandIndicatorToTxt(output_path + "taxidemandindicators.txt");
    this_simulation_performance.writeStationSummaryIndicatorToTxt(output_path + "StationSummary.txt");
    my_stations.writeStationInfoToTxt(output_path);
}

void One_Simulation::setObjective_alpha(double performance_alpha) {
    One_Simulation::objective_alpha = performance_alpha;
}


