//
// Created by ZhengchaoWANG on 08.08.18.
//

#include "station_optimization/optimization/local_search/Local_Search_Simulation.h"

void Local_Search_Simulation::run() {
    time_t start,timeend;
    time(&start);
    for(int timeStamp = begin_time; timeStamp <= end_time;timeStamp++){
        One_Simulation::stationAction(timeStamp);
        One_Simulation::taxiAction(timeStamp);
    }

    Taxi_Performance_General thisperformance;
    thisperformance.setObjectivefunctionchoice(objectivechoice);
    thisperformance.computePerformance(this->my_fleet);//thisperformance.
    thisperformance.computeChargingStationIndex(this->my_stations,end_time-begin_time);

    setStationPerformance(thisperformance.getQueue_length(), thisperformance.getTotal_wait_time(),
                          thisperformance.getAll_utilization_time_rate(), thisperformance.getTotal_wait_number(),
                          thisperformance.getAverage_wait_time(), thisperformance.getPerformance());

    setTaxiCoverageRatio(thisperformance);

    time(&timeend);
    double dif = difftime(timeend,start);
    printf("Elasped time of one simulation is %.2lf seconds", dif);
}

void Local_Search_Simulation::setTaxiCoverageRatio(Taxi_Performance_General &thisperformance) {
    switch (algorithm){
        case (0):{
            double number_ratio = 0.0, distance_ratio = 0.0, time_ratio = 0.0;
            for(int i = 0; i < thisperformance.getTotaldemand().size();i++){
                number_ratio += double(thisperformance.getServeddemand()[i])/thisperformance.getTotaldemand()[i];
                distance_ratio += thisperformance.getServeddistance()[i]/thisperformance.getTotaldistance()[i];
                time_ratio += double(thisperformance.getServedtime()[i])/thisperformance.getTotaltime()[i];
            }
            pushbackCoverageRatio(number_ratio,distance_ratio,time_ratio,thisperformance.getTotaldemand().size());
            break;
        }
        case (1):{
            int total_served_demand = 0,  total_served_time = 0, total_demand = 0, total_time = 0;
            double total_served_distance = 0.0, total_distance = 0.0;
            for(int i = 0; i<thisperformance.getTotaldemand().size();i++){
                total_served_demand += thisperformance.getServeddemand()[i];
                total_demand += thisperformance.getTotaldemand()[i];
                total_served_time +=thisperformance.getServedtime()[i];
                total_time += thisperformance.getTotaltime()[i];
                total_served_distance += thisperformance.getServeddistance()[i];
                total_distance += thisperformance.getTotaldistance()[i];
            }
            pushbackCoverageRatio(double(total_served_demand)/total_demand, total_served_distance/total_distance, double(total_served_time)/total_time);
            break;
        }
        default:
            break;
    }
}

void Local_Search_Simulation::setStationPerformance(const vector<int> &queuelength, const vector<int> &totalwaittime,
                                                    const vector<vector<double>> &allratetimeratio,
                                                    const vector<int> & totalwaitnumber, const vector<double> &averagewaittime,
                                                    const double integratedperformance) {

    this->max_queue_length = queuelength;
    this->total_wait_time = totalwaittime;
    this->all_utilization_time_rate = allratetimeratio;
    this->average_wait_time = averagewaittime;
    for(int v : totalwaitnumber){
        if(v!=0){
            this->average_wait_frequency.push_back(double(end_time-begin_time)/v);
        } else {
            this->average_wait_frequency.push_back(LONG_MAX);
        }

    }
    this->simulation_performance = integratedperformance;
}

void
Local_Search_Simulation::pushbackCoverageRatio(double num_ratio, double dist_ratio, double time_ratio, unsigned long total_num) {
    if(total_num!=0){
        coverage_ratio.push_back(num_ratio/total_num);
        coverage_ratio.push_back(dist_ratio/total_num);
        coverage_ratio.push_back(time_ratio/total_num);
    } else {
        coverage_ratio.push_back(num_ratio);
        coverage_ratio.push_back(dist_ratio);
        coverage_ratio.push_back(time_ratio);
    }
}

const vector<int> &Local_Search_Simulation::getQueue_length() const {
    return max_queue_length;
}

const vector<int> &Local_Search_Simulation::getTotal_wait_time() const {
    return total_wait_time;
}

const vector<vector<double>> &Local_Search_Simulation::getAll_utilization_time_rate() const {
    return all_utilization_time_rate;
}

const vector<double> &Local_Search_Simulation::getCoverage_ratio() const {
    return coverage_ratio;
}

void Local_Search_Simulation::setAlgorithm(int algorithm) {
    Local_Search_Simulation::algorithm = algorithm;
}

const vector<double> &Local_Search_Simulation::getAverage_wait_time() const {
    return average_wait_time;
}

const vector<double> &Local_Search_Simulation::getAverage_wait_frequency() const {
    return average_wait_frequency;
}

void Local_Search_Simulation::setObjectivechoice(int objectivechoice) {
    Local_Search_Simulation::objectivechoice = objectivechoice;
}
