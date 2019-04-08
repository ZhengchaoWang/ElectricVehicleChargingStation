//
// Created by ZhengchaoWANG on 03.08.18.
//


#include "station_optimization/optimization/local_search/Local_Search.h"


/************************* Actions methods **********************************/

void Local_Search::generateNeighbor() {

    int time_span = end_time - begin_time;
    vector<array<double,2>> utilization_ratio_vec(all_utilization_time_rate.size());
    vector<array<double,2>> history_utilization_records(all_utilization_time_rate.size());
    for(int  i = 0; i< all_utilization_time_rate.size();i++) {
        if(accumulate(current_station_config[i].begin(), current_station_config[i].end(),0)>0){
            double used_rate_time = 0, total_rate_time = 0.0;
            for (int j = 0; j < all_utilization_time_rate[i].size(); j++) {
                // compute the average utilization.
                if (current_station_config[i][j] > 0) {
                    used_rate_time += all_utilization_time_rate[i][j] * pile_charging_rate[j]*current_station_config[i][j];
                    total_rate_time += pile_charging_rate[j] * current_station_config[i][j];
                }
            }
            utilization_ratio_vec[i] = {used_rate_time / total_rate_time, static_cast<double>(i)};
            history_utilization_records[i] = {used_rate_time,total_rate_time};
        } else {
            utilization_ratio_vec[i] = {1, static_cast<double>(i)};
            history_utilization_records[i] = {1,1};
        }

    }
    sort(utilization_ratio_vec.begin(),utilization_ratio_vec.end(),[](const array<double,2> & a, const array<double,2> & b){
        return a[0]<b[0];
    });

    neighbors_config = current_station_config;
    for(int k = 0; k < utilization_ratio_vec.size();k++){
        auto i = static_cast<int>(utilization_ratio_vec[k].back());
        if (utilization_ratio_vec[k].front() > UR_max_threshold[i]) {
            if (average_wait_frequency[i] < AWF_max_threshold[i]) {
                if (average_wait_time[i] > AWT_max_threshold[i]) {
                    // This station is very utilized. for all the time, increase the config
                    increaseSignificant(i);
                } else {
                    // this station is very utilized, but is quitbalanced config. try to work around
                    increaseSlightly(i);
                }
            } else {
                if (average_wait_time[i] > AWT_max_threshold[i]) {
                    // frequency is quite small, this means that the station is engaged during some time, therefore try
                    // increase the neighbor's config.
                    increaseNeighbor(utilization_ratio_vec, k);
                } /*else {
                    // This case, the utilization is quite a lot, but very balenced, so try to work around and slightly decrease.
                    slightDecrease();
                }*/
            }
        } else {
            if (average_wait_frequency[i] < AWF_max_threshold[i]) {
                if (average_wait_time[i] > AWT_max_threshold[i]) {
                    cout << "This is a bit wired, utilization is low but a lot of waiting. Tune the parameters" << endl;
                } else {
                    cout << "This is a less wired, but still not very understandable" << endl;
                }
            } else {
                if (average_wait_time[i] > AWT_max_threshold[i]) {
                    // There are some cases that the station is engaged with impulse demand, therefore, modify its
                    // neighbors config and decrease own config.
                    decreaseConfig(utilization_ratio_vec, history_utilization_records, k);
                    increaseNeighbor(utilization_ratio_vec, k);
                } else {
                    // All of it are less utilized, decrease the config of the stations
                    decreaseConfig(utilization_ratio_vec, history_utilization_records, k);
                }
            }
        }
    }
}

void Local_Search::increaseSignificant(int stationID) {

    for(int i = 0; i < pile_price_by_type.size(); i++){
        int used_places = accumulate(neighbors_config[stationID].begin(),neighbors_config[stationID].end(),0);
        double used_budget = computeUsedBudget(neighbors_config);
        if(min(maximum_pile_number_in_stations[stationID] - used_places, static_cast<int>((budget-used_budget)/pile_price_by_type[i])) >0){
            neighbors_config[stationID][i] += 1;
        }
    }
}

void Local_Search::increaseSlightly(int stationID) {
    int used_places = accumulate(neighbors_config[stationID].begin(),neighbors_config[stationID].end(),0);
    double used_budget = computeUsedBudget(neighbors_config);
    if(min(maximum_pile_number_in_stations[stationID] - used_places, static_cast<int>((budget-used_budget)/pile_price_by_type.back())) >0){
        neighbors_config[stationID].back() += 1;
    }
}

void Local_Search::increaseNeighbor(vector<array<double,2>> & utilization_ratio_vec, int k) {
    auto i = static_cast<int>(utilization_ratio_vec[k].back());
    int j = findNeighborStation(i);

    int used_places = accumulate(neighbors_config[j].begin(),neighbors_config[j].end(),0);
    double used_budget = computeUsedBudget(neighbors_config);

    vector<int> increasable_num (pile_price_by_type.size());
    for(int l = 0; l < pile_price_by_type.size(); l++){
        increasable_num[l] = min(maximum_pile_number_in_stations[l] - used_places, static_cast<int>((budget-used_budget)/pile_price_by_type[l]));
    }
    if(single_type_in_stations[j]){
        vector<int> type = findInstalledType(j);
        if(type.size()>1){
            cout <<" There is a problem with single type in stations" << endl;
        }
        bool changed = false;
        for(int m = 0 ; m < increasable_num.size();m++){
            if((increasable_num[m]!=0) & (type[0] == m)){
                neighbors_config[j][m] +=1;
                changed = true;
                break;
            }
        }
        if(!changed){
            cout << "This time, there is a failure in increasing slot number" << endl;
        }
    } else {
        for(int m = 0 ; m < increasable_num.size();m++){
            if(increasable_num[m]!=0){
                neighbors_config[j][m] +=1;
                break;
            }
        }
    }
}

void Local_Search::decreaseConfig(vector<array<double,2>> & utilization_ratio_vec, vector<array<double,2>> &history_utilization_records, int k ) {
    auto i = static_cast<int>(utilization_ratio_vec[k].back());

    double excess_rate_time = history_utilization_records[i].back() - history_utilization_records[i].front()/desired_utilize_ratio[i];

    vector<int> desired_decrease_num, feasible_decrease_num, position;
    for(int j = 0; j< neighbors_config[i].size(); j++){
        if(neighbors_config[i][j]>0){
            desired_decrease_num.push_back(
                    static_cast<int>(excess_rate_time/pile_charging_rate[j]));
            feasible_decrease_num.push_back(neighbors_config[i][j]);
            position.push_back(j);
        }
    }
    // decrease equally the number of all the installed piles.
    for(int j = 0; j < desired_decrease_num.size(); j++){
        neighbors_config[i][position[j]] -= min(feasible_decrease_num[j], static_cast<int>(desired_decrease_num[j]/desired_decrease_num.size()));
    }
}

int Local_Search::findNeighborStation(int i) {
    int min_index = -999;
    double minimum_distance = LONG_MAX;
    for(int j = 0; j < neighbors_config.size(); j++){
        if((j != i) & (twoPointsDistance(station_locations[i], station_locations[j]) < minimum_distance)){
            minimum_distance = twoPointsDistance(station_locations[i], station_locations[j]);
            min_index = j;
        }
    }
    return min_index;
}

double Local_Search::computeUsedBudget(vector<vector<int>> &config) {

    double used_budget = 0.0;
    for(auto fig : config){
        for(int i = 0; i < fig.size(); i++){
            used_budget += fig[i]*pile_price_by_type[i];
        }
    }
    return used_budget;
}



void Local_Search::run() {

    array<int,2> time_span = findtimelimit(my_fleet_pointer->getMy_fleet());
    begin_time = time_span.front(); end_time = time_span.back();
    neighbors_config = current_station_config;

    bool keepsearch = true;
    int running_iteration = 1;

    while(keepsearch){
        time_t start,timeend;
        time(&start);
        defineTheThresholds();

        Charging_Station_General_Manager my_CSGM =  getChargingFacilityManager(begin_time);

        Local_Search_Simulation this_simulation(my_fleet_pointer->getMy_fleet(), my_CSGM, begin_time, end_time);
        this_simulation.setObjectivechoice(objectivechoice);
        this_simulation.run();

        getThePerformances(this_simulation);

        if(this_simulation.getSimulationPerformance()>best_performance){
            best_stations_config = neighbors_config;
            best_performance = this_simulation.getSimulationPerformance();
            best_coverage_ratio = this_simulation.getCoverage_ratio();
        }
        this->performance_vector.push_back(this_simulation.getSimulationPerformance());
        this->neighbor_performance_vect.push_back(this_simulation.getSimulationPerformance());
        this->best_performance_vect.push_back(best_performance);


        generateNeighbor();

        current_station_config = neighbors_config;

        if(running_iteration > 40){
            keepsearch = false;
        }
        running_iteration ++;
        time(&timeend);
        double dif = difftime(timeend,start);
        printf("Elasped time of one local search is %.2lf seconds. \n", dif);
    }
}

void Local_Search::getThePerformances(Local_Search_Simulation & this_simulation) {
    emptyThePerformances();
    matchThePerformanceFromSimulation(this_simulation);
}

void Local_Search::emptyThePerformances() {
    this->all_utilization_time_rate = {};
    this->queue_length = {};
    this->total_wait_time= {};
    this->average_wait_time= {};
    this->average_wait_frequency= {};
}

void Local_Search::matchThePerformanceFromSimulation(Local_Search_Simulation & this_simulation) {

    int j = 0;
    for (auto &one_station_config : neighbors_config) {
        vector<double> pile_utilization_ratio(one_station_config.size(),-999);
        if(accumulate(one_station_config.begin(), one_station_config.end(),0) > 0){
            int m = 0;
            for(int k = 0; k<one_station_config.size();k++){
                if(one_station_config[k]>0){
                    pile_utilization_ratio[k] = this_simulation.getAll_utilization_time_rate()[j][m];
                    m++;
                }
            }
            pushBackPerformances(pile_utilization_ratio, this_simulation.getQueue_length()[j], this_simulation.getTotal_wait_time()[j],
                                 this_simulation.getAverage_wait_time()[j],this_simulation.getAverage_wait_frequency()[j]);
            j++;
        } else {
            pushBackPerformances(pile_utilization_ratio,-999,-999,-999,-999); // Which means there is no pile installed.
        }
    }
}

void Local_Search::pushBackPerformances(vector<double> & pile_utilization_ratio,int qlength, int totwaittime, double averagewaittime, double averagewaitfrequency){
    this->all_utilization_time_rate.push_back(pile_utilization_ratio);
    this->queue_length.push_back(qlength);
    this->total_wait_time.push_back(totwaittime);
    this->average_wait_time.push_back(averagewaittime);
    this->average_wait_frequency.push_back(averagewaitfrequency);
}



void Local_Search::initializeDesiredUtilizationRatio(double desiredratio) {
    for(int i = 0; i < station_locations.size();i++){
        desired_utilize_ratio.push_back(desiredratio);
    }
}

void Local_Search::defineTheThresholds() {
    emptyPerformanceThresholds();
    if(envolve){
        for(auto station_config : current_station_config){
            if(accumulate(station_config.begin(), station_config.end(),0)>0){
                pushBackThresholds(computeURT(station_config), computeAWFT(station_config), computeAWTT(station_config));
            } else {
                pushBackThresholds(0,-999,LONG_MAX);
            }
        }
    } else {
        for(const auto &station_config:current_station_config){
            pushBackThresholds(default_threshold[0], default_threshold[1], default_threshold[2]);
        }
    }

}

void Local_Search::pushBackThresholds(double UR_threshold, double AWF_threshold, double AWT_threshold) {
    this->UR_max_threshold.push_back(UR_threshold);
    this->AWF_max_threshold.push_back(AWF_threshold);
    this->AWT_max_threshold.push_back(AWT_threshold);
}

double Local_Search::computeAverageChargeRate(vector<int> & station_config){
    double total_rate = 0.0, total_pile_num = 0.0;
    for(int i = 0; i < station_config.size();i++){
        total_rate += station_config[i]*pile_charging_rate[i];
        total_pile_num += station_config[i];
    }
    return (total_rate/total_pile_num);
}

double Local_Search::computeURT(vector<int> &station_config) {

    return min(0.8, 60.0*(computeAverageChargeRate(station_config)));
}

double Local_Search::computeAWFT(vector<int> &station_config) {
    return (60.0*accumulate(station_config.begin(), station_config.end(),0));
}

double Local_Search::computeAWTT(vector<int> &station_config) {
    return (1.5*20.0/computeAverageChargeRate(station_config));
}

void Local_Search::emptyPerformanceThresholds() {
    this->UR_max_threshold ={};
    this->AWF_max_threshold={};
    this->AWT_max_threshold={};
}

void Local_Search::setObjectivechoice(int objectivechoice) {
    Local_Search::objectivechoice = objectivechoice;
}




