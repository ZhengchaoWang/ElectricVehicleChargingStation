//
// Created by ZhengchaoWANG on 08.04.19.
//

#ifndef CHARING_PARAMETERS_H
#define CHARING_PARAMETERS_H


class parameters {

public:
    /*********************************************************************************************
     * These parameters are related to vehicle characteristics
     *********************************************************************************************/

    double                                              chargerate = 1.0/30.0; // charge rate of each station. Here it is kwh/second
    double                                                            capf=24; // capacity of the battery or fuel tank. kwh
    double                                                     consumrate=0.2; // consumption rate of the vehicle per km. 0.2 kwh/km
    double                                                           ra = 0.2; // range anxiety of taxi drivers. Below this value, the taxi will start to charge. 20\%
    double                                                       minSoc = 0.1; // the minimum state of charge that a vehicle wants to have after sending a passenger. 10\%
    double                                                     chargeTo = 1.0; // the level of soc that the vehicle wants to charge to when charge. 100%
    int                                                 CFchoicealgorithm = 1; // 1, cloest, 2, the station allows less wait time. etc
    double                                               drivespeed= 1.0/60.0; // this is a speed chosen by the driver. By now, we set as 60km/h. The value here is km/s.

    /*********************************************************************************************
     * Parameters related to taxis
     *********************************************************************************************/
    int                                         chargesimulationalgorithm = 1; // To control which method is used to generate Plan.
    // Charging duration choice algorithm, more related Threshold based simulation algorithm
    int                                           chargedurationalgorithm = 1; // 1, charge to a certain level. 2, optimize the charge level (might be hard to decide).
    // To compute the revenue of each trips
    double                                              fixedtripcharge = 3.5; // If a trip is taken, then this taxi can receive this amount of money for the first "minimumdistance".
    double                                              minimumdistance =   0; // The first 0 km will not be charged additional cost.
    double                                             chargeperdistance=1.71; // 1.71$ per km after 3.0 km.
    bool                                              longstopseperate = true;
    int                                                       largegap = 3600; // This is a large value than which the neighboring points is larger, they will try to search for charging possibility.
    int                                                      foresight = 3600; // this is a threshold of vehicle foresight to see the future.

    /*********************************************************************************************
     * Parameters related to Heuristic algorithm
     *********************************************************************************************/
    double                                                     budget = 290.0; // This is the monetary budget for the
            // charging station. It is a unitary value with the 20 kw charger has a price of 2, 60 kw charger has a price
                    // of 8, while 120 kw charger has a price of 12.
    double                                                 budget_ratio = 1.0;
    double                                                    used_budget=0.0; // how much have been used?

    /*********************************************************************************************
     * Parameters related to Simulated annealing algorithm
     *********************************************************************************************/
    double                                               temperature = 1000.0;  // A temperature value for simulated annealing.
    double                                             decrease_ratio = 0.995;  // final temperature decrease ratio
    double                                      initial_decrease_ratio = 0.98;  // initial temperature decrease ratio
    double                                          stop_temperature = 0.0001;  // stop temperature
    int                                          one_loop_searching_times = 5;  // How many times to search for neightborhood for one temperature.
    unsigned int                                    adoption_seed = 733631680;  //134683146

};


#endif //CHARING_PARAMETERS_H
