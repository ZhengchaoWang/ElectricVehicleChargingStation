//
// Created by ZhengchaoWANG on 03.08.18.
//

#include "station_optimization/optimization/heuristic/Heuristics.h"

/* Protected */
template<class bidiiter>
bidiiter Heuristics::randomUnique(bidiiter begin, bidiiter end, int numrandom) {

    auto left = std::distance(begin, end);
    while (numrandom--) {
        bidiiter r = begin;
        param_t  p0{0, static_cast<int>(left - 1)};
        std::advance(r,integer_distribution(initial_engine, p0));
        std::swap(*begin, *r);
        ++begin;
        --left;
    }
    return begin;
}

double Heuristics::twoPointsDistance(array<double, 2> pos1, array<double, 2> pos2){
    double dlat = (pos2[0]-pos1[0]) * M_PI/180;
    double dlon = (pos2[1]-pos1[1]) * M_PI/180;
    double a = pow(sin(dlat/2),2) + cos(pos1[0]*M_PI/180)*cos(pos2[0]*M_PI/180)*pow(sin(dlon/2),2);
    double distance = 2*atan2(sqrt(a),sqrt(1.0-a)) * 6378.135;

    return distance;
}

void Heuristics::generateSequece(vector<int> & target_vector, int begin, int end) {

    target_vector.reserve(static_cast<unsigned long>(abs(end - begin) + 1));
    if(begin < end){
        for(int i = 0; i<= end-begin; i++){
            target_vector.push_back(begin +i);
        }
    } else {
        for(int i = 0; i<=begin-end; i++){
            target_vector.push_back(begin-i);
        }
    }
}

void Heuristics::generateSequenceWithErase(vector<int> &target_vector, int begin, int end,
                                                           vector<int> eraser) {
    if(eraser.size()>1){
        sort(eraser.begin(),eraser.end());
    }

    generateSequece(target_vector,begin,end);
    for(int i : eraser){
        target_vector.erase(find(target_vector.begin(), target_vector.end(),i));
    }
}

void Heuristics::initializeStationsUsingSeeds() {

    /* We generate a random number of piles in the station
     * It can be a combination of piles in the station
     * If there is a budget, we should using this budget to confine the number of slots.
     *
     * Here, we first choose the stations that going to install charging piles.
     *
     * Then we choose the kinds of charging piles to install in the chosen stations.
     *
     * Finally, if the lcoation can add more charging piles, we choose the number of each charging piles.
     *
     * If the budget has been used out, we stop the initialization no matter that if we initialized all the chosen stations.
     */

    initiateCurrentStationConfig();

    auto location_size = int(station_locations.size());
    auto  pile_types = static_cast<int>(pile_price_by_type.size());

    param_t  p0{0, location_size-1};
    int initial_chosen_station_num = integer_distribution(initial_engine,p0);

    vector<int> initial_chosen_stations_ID; generateSequece(initial_chosen_stations_ID, 0,location_size-1);
    randomUnique(initial_chosen_stations_ID.begin(), initial_chosen_stations_ID.end(), initial_chosen_station_num);

    double left_budget = budget;
    for(int i = 0; i < initial_chosen_station_num; i++){
        param_t p{1, pile_types};

        int chosen_pile_type_quantity = integer_distribution(initial_engine,p); // This value can be regarded as the type of charging pile or number of types of charging piles;

        if(single_type_in_stations[initial_chosen_stations_ID[i]]){
            int space = min(maximum_pile_number_in_stations[initial_chosen_stations_ID[i]], (int)floor(left_budget/pile_price_by_type[chosen_pile_type_quantity-1]));
            if(space>=1) {
                param_t p1{1, space};
                int pile_num = integer_distribution(initial_engine, p1);
                left_budget -= pile_num * pile_price_by_type[chosen_pile_type_quantity - 1];
                if (left_budget < minimal_price) {
                    break;
                } else {
                    current_station_config[initial_chosen_stations_ID[i]][chosen_pile_type_quantity - 1] = pile_num;
                }
            }
        }else{
            vector<int> my_types; generateSequece(my_types, 0, pile_types-1);
            randomUnique(my_types.begin(), my_types.end(), chosen_pile_type_quantity);

            int left_pile_num = maximum_pile_number_in_stations[initial_chosen_stations_ID[i]];
            for(int k = 0; k < chosen_pile_type_quantity; k++){
                int avaliable_space = min(left_pile_num, (int)floor(left_budget/pile_price_by_type[my_types[k]]));
                if(avaliable_space > 0){
                    param_t p1{1,avaliable_space};
                    int pile_num = integer_distribution(initial_engine,p1);

                    left_budget -= pile_num*pile_price_by_type[my_types[k]];
                    if(left_budget < minimal_price){
                        break;
                    }else{
                        current_station_config[initial_chosen_stations_ID[i]][my_types[k]] = pile_num;
                        left_pile_num -= pile_num;
                    }
                }
            }
        }
    }
}

void Heuristics::initiateCurrentStationConfig() {
    for(int i = 0; i < station_locations.size();i++){
        vector<int> this_vec(pile_price_by_type.size(),0);
        current_station_config.push_back(this_vec);
    }
}

void Heuristics::nonEmptyStations(vector<int> & non_empty_stations) {

    for(int i = 0; i < station_locations.size(); i++){
        if(accumulate(current_station_config[i].begin(),current_station_config[i].end(),0) > 0){
            non_empty_stations.push_back(i);
        }
    }
}

Charging_Station_General_Manager Heuristics::getChargingFacilityManager(int starttime, bool want_neighbor) {
    Charging_Station_General_Manager mychargestations = Charging_Station_General_Manager(0);
    map<int, Charging_Station_General> cfset;
    int used_station = 0, index = 0;

    vector<vector<int>>* station_config;
    if(want_neighbor){
        station_config = & neighbors_config;
    } else {
        station_config = & best_stations_config;
    }
    for (auto &i : *station_config) {
        if(accumulate(i.begin(), i.end(), 0) > 0){
            vector<double> this_rate = findUsedPileRate(i);
            vector<int> this_types = findInstalledType(i);
            Charging_Station_General mychargestation(used_station, station_locations[index], this_rate, this_types);
            mychargestation.initiateDefaultElements(starttime);
            cfset.insert(std::pair<int, Charging_Station_General>(used_station, mychargestation));
            used_station++;
        }
        index++;
    }
    mychargestations.setCfset(cfset);
    return mychargestations;
}

vector<int> Heuristics::findInstalledType(int stationnode) {
    vector<int> installed_type;
    for(int i = 0; i <current_station_config[stationnode].size();i++){
        if(current_station_config[stationnode][i]>0){
            installed_type.push_back(i);
        }
    }
    return installed_type;
}

vector<int> Heuristics::findInstalledType(vector<int> &this_config) {
    vector<int> this_vector;
    for (int i : this_config) {
        if(i >0){
            this_vector.push_back(i);
        }
    }
    return this_vector;
}

vector<double> Heuristics::findUsedPileRate(vector<int> &this_config) {
    vector<double> this_vector;

    for(int i =0 ; i<this_config.size(); i++){
        if(this_config[i]>0){
            this_vector.push_back(pile_charging_rate[i]);
        }
    }
    return this_vector;
}

void Heuristics::computeUsedBudget() {
    used_budget = 0;
    for(auto station_config : current_station_config) {
        for (int i = 0; i < station_config.size(); i++) {
            used_budget += pile_price_by_type[i] * station_config[i];
        }
    }
}

void Heuristics::setupBudget() {

    my_fleet_pointer->computeTotalDistance();

    double chargetimes = my_fleet_pointer->getTotal_distance()/120;

    budget = budget_ratio*chargetimes/pile_charging_rate.front()/3600 * pile_price_by_type.front();

    cout << budget <<endl;
}

template<typename T>
vector<size_t> Heuristics::sort_indexes(const vector<T> &v) {
    // initialize original index locations
    vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);

    // sort indexes based on comparing values in v
    sort(idx.begin(), idx.end(),
         [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

    return idx;
}

void Heuristics::defaultClassInitialization(bool pilenumber , bool MP, bool MPNS, bool SMIDV, vector<bool> singletype, bool budgetcompute ) {
    initializeRandomEngineDistributors();

    if (budgetcompute){
        setupBudget();
    } else{
        budget  = budget * budget_ratio;
    }

    if(singletype.size()==1){
        initializeSingleTypeInStations(singletype[0]);
    } else{
        initializeSingleTypeInStations(singletype);
    }

    if(MP){
        initializeMinimalPrice();
    }

    if(MPNS){
        initializeMaximumPileNumberInStations();
    }

    if(SMIDV){
        initializeStationsMaximalIncreaseDecreaseValue();
    }

    if(pilenumber){
        initializeStationsUsingSeeds();
    }
}

void Heuristics::initializeRandomEngineDistributors() {
    mt19937 initial_gen(initialize_seeds);
    this->initial_engine = initial_gen;

    this->integer_distribution = uniform_int_distribution<> (1,6);
}

void Heuristics::initializeSingleTypeInStations(bool singletype) {
    for(int i = 0; i<station_locations.size(); i++){
        single_type_in_stations.push_back(singletype);
    }
}

void Heuristics::initializeSingleTypeInStations(vector<bool> & singletypes) {
    if(singletypes.size() == station_locations.size()){
        single_type_in_stations = singletypes;
    }else if(singletypes.size() <station_locations.size()){
        single_type_in_stations = singletypes;
        for(auto i = static_cast<int>(singletypes.size()); i < station_locations.size(); i++){
            single_type_in_stations.push_back(false);
        }
    }else{
        single_type_in_stations.assign(singletypes.begin(),singletypes.begin()+station_locations.size());
    }
}

void Heuristics::initializeMaximumPileNumberInStations() {
    maximum_pile_number_in_stations.reserve(station_locations.size());
    for(int i = 0; i < station_locations.size(); i++){
        maximum_pile_number_in_stations.push_back(budget != LONG_MAX ? (int)floor(budget/ pile_price_by_type[1]/station_locations.size()*1.5) : INT_MAX);
    }
}

void Heuristics::initializeMinimalPrice() {
    for(double i:pile_price_by_type){
        if(i < minimal_price){
            minimal_price = i;
        }
    }
}

void Heuristics::initializeStationsMaximalIncreaseDecreaseValue() {
    vector<int> initial_vector(pile_price_by_type.size());

    for(int i = 0; i<pile_price_by_type.size(); i++){
        initial_vector[i] = i+1;
    }

    stations_maximal_increase_decrease_number.reserve(station_locations.size());
    for(int i = 0; i < station_locations.size(); i++){
        stations_maximal_increase_decrease_number.push_back(initial_vector);
    }
}

void Heuristics::setMy_fleet_pointer(ReProduce_Taxi_Fleet *my_fleet_pointer) {
    Heuristics::my_fleet_pointer = my_fleet_pointer;
}

void Heuristics::outputResult() {
    string filename = output_path + "stationconfig.txt";
    outputStationConfig(filename);
    filename = output_path + "performance_history.txt";
    outputPerformanceHistory(filename);
}

void Heuristics::outputStationConfig(string & filename) {

    ofstream outputfile(filename);
    if(outputfile.is_open()){
        outputfile <<"stationID" << "\t" << "lati" << "\t" << "long" <<"\t";
        for(int i = 0; i < best_stations_config[0].size(); i++){
            string my_type = "type" + to_string(i+1);
            outputfile << my_type <<"\t";
        }
        outputfile <<endl;

        outputfile.precision(8);
        outputfile.setf(ios::fixed);
        outputfile.setf(ios::showpoint);

        for(int i = 0; i < this->best_stations_config.size(); i++){

            outputfile<< i << "\t" << setprecision(5) << station_locations[i][0] << "\t" << setprecision(5)<< station_locations[i][1] <<"\t";
            for(auto j : best_stations_config[i]){
                outputfile << j <<"\t";
            }
            outputfile << endl;
        }
    } else{
        printf("file open failed when output station config");
    }
    outputfile.close();
}

void Heuristics::outputPerformanceHistory(string & filename) {


    ofstream outputfile(filename);
    if(outputfile.is_open()) {
        for (int i = 0; i < performance_vector.size(); i++) {
            outputfile << performance_vector[i] << "\t" <<neighbor_performance_vect[i] << "\t" << best_performance_vect[i] << endl;
        }
//        outputfile << computeTheoreticalValue() << endl;
        outputfile.close();
    } else{
        cout <<"Open file failed when output performance history" << endl;
    }
}

double Heuristics::computeTheoreticalValue() {
    double total_time = 0;
    for(auto & my_taxi:my_fleet_pointer->getMy_fleet()){
        for(auto & trip : my_taxi.getMytour().getTrips()){
            if(trip.getCustag()){
                total_time+= (trip.getEndtime()-trip.getBegintime());
            }
        }
    }
    return ((total_time -  my_fleet_pointer->getTotal_distance() * my_fleet_pointer->getMy_fleet().front().getConsumrate()/pile_charging_rate.front()/8)/1000000 );
}

void Heuristics::setBudget_ratio(double budget_ratio) {
    Heuristics::budget_ratio = budget_ratio;
}

const vector<double> &Heuristics::getBest_performance_vect() const {
    return best_performance_vect;
}

double Heuristics::getBest_performance() const {
    return best_performance;
}
