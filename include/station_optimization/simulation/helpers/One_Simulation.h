//
// Created by ZhengchaoWANG on 27.07.18.
//

#ifndef CHARING_ONE_SIMULATION_H
#define CHARING_ONE_SIMULATION_H

#include <vector>

#include <sys/types.h>
#include <dirent.h>
#include <boost/filesystem.hpp>


#include "Simulation.h"


using namespace std;

namespace fs = boost::filesystem ;

class One_Simulation : public Simulation {
protected:
    double                                                          simulation_performance = -9999999999.0;
    double                                                          objective_alpha = 1;
public:

    One_Simulation() = default;

    using Simulation::Simulation;

    void stationAction(int current_time) override;

    void taxiAction(int current_time) override;

    void run() override;

    void outputSimulationResult() override;

    double getSimulationPerformance() const {
        return simulation_performance;
    }

    void setSimulation_performance(double simulation_performance){
        One_Simulation::simulation_performance = simulation_performance;
    }

    void setObjective_alpha(double performance_alpha);

};



#endif //CHARING_ONE_SIMULATION_H
