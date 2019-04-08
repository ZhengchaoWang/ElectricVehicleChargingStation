//
// Created by ZhengchaoWANG on 07.08.18.
//

#ifndef CHARING_LOCAL_SEARCH_SIMULATION_H
#define CHARING_LOCAL_SEARCH_SIMULATION_H

#include <station_optimization/simulation/helpers/One_Simulation.h>

class Local_Search_Simulation : public One_Simulation{
private:
    vector<int>                                              max_queue_length;
    vector<int>                                               total_wait_time;
    vector<double>                                          average_wait_time;
    vector<double>                                     average_wait_frequency;     // How many seconds between two waiting vehicles.

    vector<vector<double>>                          all_utilization_time_rate;     // The area under the used number and

    vector<double>                                             coverage_ratio;     // this is in the order of number ratio, distance ratio, time ratio.

    int                                                         algorithm = 1;
    int                                                   objectivechoice = 0;

    /* private methods for inner utilizing */
    void setStationPerformance(const vector<int> &queuelength, const vector<int> &totalwaittime,
                               const vector<vector<double>> &allratetimeratio,
                               const vector<int> & totalwaitnumber, const vector<double> &averagewaittime, double integratedperformance);

    void setTaxiCoverageRatio(Taxi_Performance_General & thisperformance);

    void pushbackCoverageRatio(double num_ratio, double dist_ratio, double time_ratio, unsigned long total_num = 0);

public:

    using One_Simulation::One_Simulation;

    void run() override;

    /* Necessary Getters */

    const vector<int> &getQueue_length() const;

    const vector<int> &getTotal_wait_time() const;

    const vector<vector<double>> &getAll_utilization_time_rate() const;

    const vector<double> &getCoverage_ratio() const;

    const vector<double> &getAverage_wait_time() const;

    const vector<double> &getAverage_wait_frequency() const;

    void setObjectivechoice(int objectivechoice);

    void setAlgorithm(int algorithm);
};

#endif //CHARING_LOCAL_SEARCH_SIMULATION_H
