//
// Created by ZhengchaoWANG on 20.07.18.
//
#include <station_optimization/optimization/simulated_annealing/Simulated_Annealing_Budget.h>

void Simulated_Annealing_Budget::run() {

    /* This function is the general structure of the simulated-annealing algorithm.
     * It first compute the performance of the initial configurations and compare with the given performance. We have to adopt it.
     *
     * Then, we get the a neighbor of the current solution. and evaluate its performance, if the performance is better, accept,
     * otherwise using the probability to decide accept or not. Repeat this for n times which can be determined.
     *
     * Then decrease the temperature.
     *
     * Repeat the above steps until the temperature is lower than a threshold.
     */

    simulation_span = findtimelimit(my_fleet_pointer->getMy_fleet());
    best_stations_config = current_station_config;
    neighbors_config = best_stations_config;
    int temperature_decrease_times = 0;
    time_t start,timeend;
    time(&start);
    while(temperature >= stop_temperature){
        for(int i = 0; i <one_loop_searching_times; i++ ){

            getPerformance(my_fleet_pointer->getMy_fleet()); // get the performance of the neighbor configuration.
            if(compareThePerformance(current_performance, neighbor_performance)){
                current_station_config = neighbors_config;
                current_performance = neighbor_performance;
                computeUsedBudget();
                if(compareThePerformance(best_performance, neighbor_performance)){
                    best_stations_config = neighbors_config;
                    best_performance = neighbor_performance;
                }
            }else{
                if(isadopted()){
                    current_station_config = neighbors_config;
                    current_performance = neighbor_performance;
                    computeUsedBudget();
                }
            }
            generate_neighbor_times = 0;
            generateNeighbor();

            performance_vector.push_back(current_performance);
            neighbor_performance_vect.push_back(neighbor_performance);
            best_performance_vect.push_back(best_performance);
        }
        temperature *= decrease_ratio;
        temperature_decrease_times ++;
        if (temperature_decrease_times%50 == 0){
            time(&timeend);
            double dif = difftime(timeend,start);
            printf("Elasped time of %d iterations for temperature decrease is %.2lf seconds\n", temperature_decrease_times,dif);
            time(&start);
        }
    }
}

void Simulated_Annealing_Budget::getPerformance(vector<ReProduce_Taxi_General> this_fleet){
    /*
     * IF WE ARE AN GOVERNMENTAL OFFICE< WE MIGHT HAVE A TRADE OFF BETWEEN THE BUDGET, STATION PERFORMNACE AND TAXI PERFORMANCE.
     * The performance of the charging station depends on: maxqueuelength; totalqueuelength; demandserveratio; averagewaitingtime, average charging time;
     *
     * other factors can be added to this performance measure.
     *
     * How the queue might influence the perception of the taxi drivers. If it is likely to have queue in the station(the time out of whole period is congested),
     * and how long the drivers have to wait on average (average queue length if all piles are the same).
     * Additionally, we can also add the longest (second longest) queue length into the objective function.
     *
     * From the taxi side, we might be also interesting in the average waiting time in the station, and average charging time
     * These might be superposition with the queue in the charging station, but might have different effect. So, we can investigate differently.
     *
     * Third, from the taxi request point view, we need also to consider the service quality of the taxi (service ratio).
     *
     * IF WE ARE MANAGER OF CHARGING STATION, WE MIGHT BE INTERESTED IN MAXIMIZE THE BENEFITS OF STATION.
     *
     * IF WE ARE THE COMMITTE OF TAXI DRIVERS, WE MIGHT BE INTERESTED IN MAXOMOZE THE TOTAL REVENUE.
     */


    Charging_Station_General_Manager this_CSGM = this->getChargingFacilityManager(simulation_span[0]);

    One_Simulation this_simulation(this_fleet, this_CSGM, simulation_span[0], simulation_span[1], output_path);
    this_simulation.setObjective_alpha(objective_alpha);
    this_simulation.setObjectivefunctionchoice(objectivefunctionchioce);
    this_simulation.run();
    neighbor_performance = this_simulation.getSimulationPerformance();
}

void Simulated_Annealing_Budget::generateNeighbor() {
    /* The strategy used to generate the neighbors is to
     *
     * add new pile on top of the existing configurations
     * decrease the pile numbers.
     *
     * 1) while stations to add or decrease, 2) how many kind of piles to increase of decrease, 3) increase by how many piles
     * 4) Budget constraints.
     *
     * Swap the number of piles in stations
     *      between which stations to swap
     *      swap 1, 2, 3 kinds of piles?
     *      swap what kind of piles?  slow, median, fast, or even slow with fast.
     *   In addition to these procedures, we might also swap the used charging station types in the same station.
     *   This is analytical to using different charging rate in the station.
     *
     * the ratio between adding/deleting piles and swaping is 1:2 as we think same configuration in different place can
     * have significant impact on the performance.
     */
    neighbor_changed = false; generate_neighbor_times ++;
    int totalnumber = accumulate(neighbor_add_swap_ratio.begin(), neighbor_add_swap_ratio.end(),0);

    param_t  p0{1, totalnumber};
    int random_number = integer_distribution(neighbor_engine,p0);
    if(random_number <= neighbor_add_swap_ratio[0]){
        int totalratio = accumulate(neighbor_add_delete_ratio.begin(), neighbor_add_delete_ratio.end(),0);
        param_t p1{1, totalratio};
        random_number = integer_distribution(neighbor_engine,p1);
        if((random_number<=neighbor_add_delete_ratio[0]) & (used_budget< budget - minimal_price)){
            addpilenumber();
        } else {
            decreasepilenumber();
        }
    }else{
        int totalratio = accumulate(neighbor_inner_mutual_swap_ratio.begin(), neighbor_inner_mutual_swap_ratio.end(),0);
        param_t p1{1, totalratio};
        random_number = integer_distribution(neighbor_engine,p1);
        if(random_number<=neighbor_add_delete_ratio[0]) {
            innerSwapPileNumber();
        }else{
            mutualSwapPileNumber();
        }
    }
    if(!neighbor_changed & (generate_neighbor_times<=10)){
        generateNeighbor();
    }
}

void Simulated_Annealing_Budget::addpilenumber() {
    /*
     * First find the stations that allow us to add more charging piles.
     * Then find how many kinds of charging piles are allowable by the space and by the budget.
     * Followed by find the kind of charging piles to change. determined by a random variable.
     * add to the chosen kind of charging piles a random generated number.
     */
    // Determine the station to add piles.
    neighbors_config = current_station_config; // Here, we need deep copy.
    vector<int> addable_station_IDs;
    if(maximum_pile_number_in_stations[0]!=INT_MAX){
        for(int i = 0; i<station_locations.size(); i++){
            if(accumulate(current_station_config[i].begin(),current_station_config[i].end(),0) < maximum_pile_number_in_stations[i]){
                addable_station_IDs.push_back(i);
            }
        }
    } else {
        generateSequece(addable_station_IDs, 0, static_cast<int>(station_locations.size() - 1));
    }

    if(!addable_station_IDs.empty()){
        param_t p0{0, static_cast<int>(addable_station_IDs.size() - 1)};
        int stationID = addable_station_IDs[integer_distribution(neighbor_engine,p0)];

        int total_addable_number_by_space = maximum_pile_number_in_stations[stationID] - accumulate(current_station_config[stationID].begin(),current_station_config[stationID].end(),0);

        // In the simulated annealing, we also restrain the step size by stations_maximal_increase_decrease_number.
        vector<int> each_type_addable_number_by_heuristic = stations_maximal_increase_decrease_number[stationID];

        // Find the changable number of piles by the budget.
        vector<int> affordable_type;
        if(budget!=LONG_MAX){
            for(int i = 0; i< pile_price_by_type.size(); i++){
                int addable_number_by_constraints = min(total_addable_number_by_space, static_cast<int>(floor((budget - used_budget) / pile_price_by_type[i])));
                each_type_addable_number_by_heuristic[i] = min(each_type_addable_number_by_heuristic[i],addable_number_by_constraints);
                if(floor((budget - used_budget)/pile_price_by_type[i])>0){
                    affordable_type.push_back(i);
                }
            }
        }else{
            generateSequece(affordable_type, 0, static_cast<int>(pile_price_by_type.size() - 1));
        }

        if(!affordable_type.empty()){
            if(single_type_in_stations[stationID]){
                bool addagain = false;
                for (int i : affordable_type) {
                    if(current_station_config[stationID][i]!=0){
                        if(!addagain){
                            cout<< "This add again to the single type station. It is wrong. Check!!!!" <<endl;
                        }
                        param_t p{1, static_cast<int>(each_type_addable_number_by_heuristic[i])};
                        neighbors_config[stationID][i] +=integer_distribution(neighbor_engine,p);
                        addagain = true;
                        neighbor_changed = true;
                    }
                }
            } else {
                param_t p{0, static_cast<int>(affordable_type.size()-1)};
                int change_type = affordable_type[integer_distribution(neighbor_engine,p)];
//                int addable_pile_number = each_type_addable_number_by_heuristic[change_type];
                param_t p1{1, each_type_addable_number_by_heuristic[change_type]};

                neighbors_config[stationID][change_type] +=integer_distribution(neighbor_engine,p1);
                neighbor_changed = true;
            }
        }
    }
}

void Simulated_Annealing_Budget::decreasepilenumber() {
    /*
     * In this step, we first need to determine which station to decrease the number of charging piles.
     * Then, inside the station, we need to determine how many types of charging piles to change.
     * Finally, we need to decide the decreasing number of the chosen charging piles.
     *
     */

    neighbors_config = current_station_config;
    vector<int> decreasable_station_IDs; nonEmptyStations(decreasable_station_IDs);
    if(!decreasable_station_IDs.empty()){
        param_t p0{0, static_cast<int>(decreasable_station_IDs.size() - 1)};
        int stationID = decreasable_station_IDs[integer_distribution(neighbor_engine,p0)];

        vector<int> decreasable_pile_types = findInstalledType(stationID);
        vector<int> maximal_changable_pile_size = stations_maximal_increase_decrease_number[stationID];
        for(int i = 0; i < current_station_config[stationID].size(); i++){
            maximal_changable_pile_size[i] = min(maximal_changable_pile_size[i], current_station_config[stationID][i]);
        }
        if(single_type_in_stations[stationID]){
            if(decreasable_pile_types.size()!=1){
                cout << "There is a problem in decreasing the charging pile number, it sould be 1 but not" <<endl;
            } else {
                param_t  p1{1,maximal_changable_pile_size[decreasable_pile_types[0]]};
                neighbors_config[stationID][decreasable_pile_types[0]] -= integer_distribution(neighbor_engine,p1);
            }
        } else{
            // determine the number of charging types to change.
            param_t p{1, static_cast<int>(decreasable_pile_types.size())};
            int to_change_pile_type_number = integer_distribution(neighbor_engine,p);

            randomUnique(decreasable_pile_types.begin(),decreasable_pile_types.end(),to_change_pile_type_number);

            for(int i=0; i<to_change_pile_type_number;i++){
                param_t p1{1, static_cast<int>(maximal_changable_pile_size[decreasable_pile_types[i]])};
                neighbors_config[stationID][decreasable_pile_types[i]] -= integer_distribution(neighbor_engine,p1);
            }
        }
        neighbor_changed = true;
    }
}

void Simulated_Annealing_Budget::mutualSwapPileNumber() {
    /*
     * In this swapping, we need to find a station with charging piles installed. then we find another station to swap with this station.
     *
     * Then, we need to find how many types of charging piles can be swapped,
     *
     * Finally, we can swap the charging piles
     *
     * Here, we do not need to worry about the budget since it swap to the corresponding type.
     */
    neighbors_config = current_station_config;

    vector<int> installed_station_IDs; nonEmptyStations(installed_station_IDs);
    if(!installed_station_IDs.empty()){
        param_t p0{0, static_cast<int>(installed_station_IDs.size()-1)};
        int first_station_ID = installed_station_IDs[integer_distribution(neighbor_engine,p0)];

        vector<int> left_station_IDs;
        generateSequenceWithErase(left_station_IDs, 0 , static_cast<int>(station_locations.size()-1),{first_station_ID});

        param_t p{0, static_cast<int>(left_station_IDs.size()-1)};
        int second_station_ID =left_station_IDs[integer_distribution(neighbor_engine,p)];

        if(single_type_in_stations[first_station_ID] or single_type_in_stations[second_station_ID]) {
            vector<int> swappable_type;
            if (single_type_in_stations[first_station_ID]) {
                swappable_type = findInstalledType(first_station_ID);
            } else {
                swappable_type = findInstalledType(second_station_ID);
            }
            if (swappable_type.size() != 1) {
                cout << "There is a problem in swapping the single type stations" << endl;
            } else {
                neighbors_config[first_station_ID][swappable_type[0]] = current_station_config[second_station_ID][swappable_type[0]];
                neighbors_config[second_station_ID][swappable_type[0]] = current_station_config[first_station_ID][swappable_type[0]];
                neighbor_changed = true;
            }

        } else{
            vector<int> swappable_type;
            for(int i = 0; i < current_station_config[first_station_ID].size(); i++){
                if(current_station_config[first_station_ID][i] != current_station_config[second_station_ID][i]){
                    swappable_type.push_back(i);
                }
            }
            if(!swappable_type.empty()){
                param_t p1{1, static_cast<int>(swappable_type.size())};
                int swap_type_number = integer_distribution(neighbor_engine,p1);
                randomUnique(swappable_type.begin(),swappable_type.end(), swap_type_number);

                for(int i = 0; i<swap_type_number; i++){
                    neighbors_config[first_station_ID][swappable_type[i]] = current_station_config[second_station_ID][swappable_type[i]];
                    neighbors_config[second_station_ID][swappable_type[i]] = current_station_config[first_station_ID][swappable_type[i]];
                }
                neighbor_changed = true;
            }
        }
    }
}

void Simulated_Annealing_Budget::innerSwapPileNumber() {
    // Check again.
    neighbors_config = current_station_config;
    vector<int> swappable_stations; nonEmptyStations(swappable_stations);
    if(!swappable_stations.empty()){
        param_t  p{0, static_cast<int>(swappable_stations.size()-1)}; // here is a mistake, we
        int chosen_station = swappable_stations[integer_distribution(neighbor_engine,p)];
        vector<int> installed_type = findInstalledType(chosen_station);

        int chosen_installed_type;
        if(single_type_in_stations[chosen_station]){
            chosen_installed_type = installed_type[0];
            if(installed_type.size() !=1){
                cout << "There is a problem as the station is single type but has more than one type" << endl;
            }
        } else {
            param_t p1{0, static_cast<int>(installed_type.size())-1};
            chosen_installed_type = installed_type[integer_distribution(neighbor_engine,p1)];
        }
        vector<int> left_pile_type;
        generateSequenceWithErase(left_pile_type, 0, static_cast<int>(current_station_config[chosen_station].size()-1), {chosen_installed_type});

        param_t p2{0, static_cast<int>(left_pile_type.size()-1)};
        int exchange_type =left_pile_type[integer_distribution(neighbor_engine,p2)];
        double new_budget = used_budget-pile_price_by_type[chosen_installed_type]*current_station_config[chosen_station][chosen_installed_type] -
                            pile_price_by_type[exchange_type]*current_station_config[chosen_station][exchange_type] +
                            pile_price_by_type[chosen_installed_type]*current_station_config[chosen_station][exchange_type] +
                            pile_price_by_type[exchange_type]*current_station_config[chosen_station][chosen_installed_type];

        if(budget >= new_budget){
            neighbors_config[chosen_station][exchange_type] = current_station_config[chosen_station][chosen_installed_type];
            neighbors_config[chosen_station][chosen_installed_type] = current_station_config[chosen_station][exchange_type];
            neighbor_changed = true;
        }
    } else {
        cout<<"There is no station that installs any charging piles" << endl;
    }


/*    int max_chosen_num = 0, max_exchange_num = 0;
    if(chosen_installed_type < exchange_type){
        if(single_type_in_stations[chosen_station]){
            max_exchange_num = current_station_config[chosen_station][chosen_installed_type];
        } else {
            if(current_station_config[chosen_station][chosen_installed_type] >= current_station_config[chosen_station][exchange_type]){
                max_chosen_num = current_station_config[chosen_station][exchange_type];
                max_exchange_num = current_station_config[chosen_station][chosen_installed_type];
            } else {
                max_chosen_num = static_cast<int>(floor(current_station_config[chosen_station][chosen_installed_type] +
                                                        pile_price_by_type[exchange_type]*(current_station_config[chosen_station][exchange_type] -
                                                         current_station_config[chosen_station][chosen_installed_type])/pile_price_by_type[chosen_installed_type]));
                max_exchange_num = current_station_config[chosen_station][chosen_installed_type];
            }
        }
    } else {
        if(single_type_in_stations[chosen_station]){
            max_exchange_num =static_cast<int>(floor(pile_price_by_type[chosen_installed_type]*current_station_config[chosen_station][chosen_installed_type]/pile_price_by_type[exchange_type]));
        } else {
            if(current_station_config[chosen_station][chosen_installed_type] <= current_station_config[chosen_station][exchange_type]){
                max_chosen_num = current_station_config[chosen_station][exchange_type];
                max_exchange_num = current_station_config[chosen_station][chosen_installed_type];
            } else {
                max_exchange_num = static_cast<int>(floor(current_station_config[chosen_station][exchange_type] +
                                                        pile_price_by_type[chosen_installed_type]*(current_station_config[chosen_station][chosen_installed_type] -
                                                                                           current_station_config[chosen_station][exchange_type])/pile_price_by_type[exchange_type]));
                max_chosen_num = current_station_config[chosen_station][exchange_type];
            }
        }
    }

    if(max_chosen_num!=0 or max_exchange_num != 0){
        neighbors_config[chosen_station][exchange_type] = max_exchange_num;
        neighbors_config[chosen_station][chosen_installed_type] = max_chosen_num;
        neighbor_changed = true;
    }*/

}


/*********************************** Helper functions **************************************************/


bool Simulated_Annealing_Budget::isadopted() {
    /*
     * Determine if the neighbor is adopted or not under the case that the performance of neighbor is not as good as current solution.
     */
    bool adoption = false;

    if(real_distribution(adoption_engine) < adoptionProbability()){
        adoption = true;
    }
    return  adoption;
}

double Simulated_Annealing_Budget::adoptionProbability() {

    return exp((neighbor_performance - current_performance)/temperature);
}

bool Simulated_Annealing_Budget::compareThePerformance(double performance1, double performance2) {

    bool better = false;

    if(performance1 < performance2){
        better = true;
    }
    return better;
}

/*********************** methods to initialize the parameters ***********************************/

void Simulated_Annealing_Budget::initializeRandomEngineDistributors() {

    mt19937 initial_gen(initialize_seeds);
    this->initial_engine = initial_gen;

    mt19937 neightbor_gen(neighbor_generate_seed);
    this->neighbor_engine = neightbor_gen;

    mt19937 adoption_gen(adoption_seed);
    this->adoption_engine = adoption_gen;

    this->integer_distribution = uniform_int_distribution<> (1,6);
    this->real_distribution = uniform_real_distribution<>(0.0,1.0);
}

/*************************************** Getters and Setters ************************************************/

void Simulated_Annealing_Budget::setTemperatureRalatedParameters(double temperature, double decrease_ratio,
                                                                 double stop_temperature) {
    this->temperature = temperature;
    this->decrease_ratio = decrease_ratio;
    this->stop_temperature = stop_temperature;
}

void Simulated_Annealing_Budget::setGenerateNeighborsParameters(unsigned int neighbor_generate_seed,
                                                                array<int, 2> add_swap_ratio,
                                                                array<int, 2> add_delete_ratio) {
    this ->neighbor_generate_seed = neighbor_generate_seed;
    this->neighbor_add_swap_ratio = add_swap_ratio;
    this->neighbor_add_delete_ratio = add_delete_ratio;

}

void
Simulated_Annealing_Budget::setMaximumPileNumberInStations(const vector<int> &my_maximum_pile_number_in_stations) {

    if(my_maximum_pile_number_in_stations.size() == station_locations.size()){
        Simulated_Annealing_Budget::maximum_pile_number_in_stations = my_maximum_pile_number_in_stations;
    } else {
        cout << "The size of stations and the size of station size constriant vector are different" << endl;

        if(station_locations.size() < my_maximum_pile_number_in_stations.size()){
            Simulated_Annealing_Budget::maximum_pile_number_in_stations.assign(my_maximum_pile_number_in_stations.begin(),my_maximum_pile_number_in_stations.begin()+station_locations.size());
        } else {
            Simulated_Annealing_Budget::maximum_pile_number_in_stations = my_maximum_pile_number_in_stations;
            for(unsigned long i = my_maximum_pile_number_in_stations.size(); i < station_locations.size(); i++){
                maximum_pile_number_in_stations.push_back(10);
            }
        }
    }
}

int Simulated_Annealing_Budget::getOneLoopSearchingTimes() const {
    return one_loop_searching_times;

}

void Simulated_Annealing_Budget::setOneLoopSearchingTimes(int one_loop_searching_times) {
    Simulated_Annealing_Budget::one_loop_searching_times = one_loop_searching_times;
}

void Simulated_Annealing_Budget::defaultClassInitialization(bool pilenumber, bool MP, bool MPNS, bool SMIDV,
                                                            vector<bool> singletype, bool budgetcompute) {
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

void Simulated_Annealing_Budget::setDecreaseRatio(double ratio) {
    this->decrease_ratio = ratio;
}

void Simulated_Annealing_Budget::setSeeds(unsigned int initial_seed,unsigned int neighbor_generate_seed,unsigned int adoption_seed) {
    Simulated_Annealing_Budget::initialize_seeds = initial_seed;
    Simulated_Annealing_Budget::neighbor_generate_seed = neighbor_generate_seed;
    Simulated_Annealing_Budget::adoption_seed = adoption_seed;

}

