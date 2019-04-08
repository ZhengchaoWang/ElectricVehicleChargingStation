//
// Created by ZhengchaoWANG on 30.11.18.
//

#include "station_optimization/optimization/VNS/VNDS.h"

void VNDS::generateNeighbor() {
    /*
     * Generate different type of neighborhood from different structure.
     */
    switch (number_type){
        case (1):{

        }
        case (2):{

        }
        case (3):{

        }
        case (4):{

        }
        default:
            break;
    }
}

void VNDS::generateShakingSpace() {
    /*
     * To be careful to check if we need to empty dont_touch_space, shaking_space.
     */
    for(int i = 0; i < original_solution.size(); i++){
        if (original_solution[i] == first_neighbor[i]){
            dont_touch_space.push_back(original_solution[i]);
        } else {
            shaking_space.push_back(original_solution[i]);
            shaking_space_position.push_back(i);
        }
    }
}

void VNDS::generateShakingSpaceNeighbor(){
    int i = 0;
}

void VNDS::makeupWholeSpace() {
    neighbors_config = original_solution;
    for(int i = 0; i < shaking_space_position.size(); i++){
        //neighbors_config[shaking_space_neighbor[i]] = shaking_space_neighbor[i];
    }
}

void VNDS::run() {
    Heuristics::run();
}

void VNDS::originalLocalSearch() {
    generateNeighbor(); // Generate the first neighborhood to search on
    generateShakingSpace(); // Based on the first neighborhood, generate a shaking space
    bool continuesearch = true;
    while (continuesearch) {
        generateShakingSpaceNeighbor(); //
        makeupWholeSpace(); //
        oneSimulation();  //
        neighbor_performance_vect.push_back(neighbor_performance);
        performance_vector.push_back(current_performance);
        if (compareResult()){
            shaking_space = shaking_space_neighbor;
            current_station_config = neighbors_config;
        }
        bool stop = true;
        if (stop){
            continuesearch = false;
        }
    }
}

bool VNDS::compareResult() {
    return false;
}

void VNDS::oneSimulation() {

}


