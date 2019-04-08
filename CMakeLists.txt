cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
project(charing)

set(CMAKE_CXX_STANDARD 14)

######################## Cplex dependency Here is the right version to uncomment
#find_package(CPLEX REQUIRED)
#if (CPLEX_FOUND)
#    include_directories(${CPLEX_INCLUDE_DIR})
#    link_directories(${CPLEX_LIBRARY})
#endif ()

#if (CPLEX_CONCERT_FOUND)
#    include_directories(${CPLEX_CONCERT_INCLUDE_DIR})
#    link_directories(${CPLEX_CONCERT_LIBRARY})
#endif ()

#if (CPLEX_ILOCPLEX_FOUND)
#    include_directories(${CPLEX_ILOCPLEX_INCLUDE_DIR})
#    link_directories(${CPLEX_ILOCPLEX_LIBRARY})
#endif ()

######## Complier message
#set(CMAKE_BUILD_TYPE Release)
message(STATUS "System: ${CMAKE_SYSTEM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")

######## Set output directories.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../build/lib)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
endif ()

######## find all the source files.
#file(GLOB SOURCE_FILES *.cpp)

######## Set executable file name, and add the source files for it.
#add_executable(CplexC++Demo ${SOURCE_FILES})

######## Add Dependency Library
#target_link_libraries(CplexC++Demo cplex-library cplex-concert ilocplex)


include_directories(include)
include_directories(include/station_optimization/simulation/charging_facility)
include_directories(include/station_optimization/simulation/private)
include_directories(include/station_optimization/simulation/taxi)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(charing
        include/station_optimization/simulation/charging_facility/Charging_Station.h
        include/station_optimization/simulation/charging_facility/Charging_Station_Manager.h
        include/station_optimization/simulation/private/Private_Vehicle.h
        include/station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi.h
        include/station_optimization/simulation/taxi/Taxi_Demand.h
        include/station_optimization/simulation/taxi/Taxi_History.h
        include/station_optimization/simulation/helpers/Charge_Choice.h
        include/station_optimization/simulation/helpers/Plan.h
        include/station_optimization/simulation/taxi/Vehicle.h
        src/station_optimization/simulation/charging_facility/Charging_Station.cpp
        src/station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi.cpp
        src/main.cpp
        src/station_optimization/simulation/helpers/Plan.cpp
        src/station_optimization/simulation/taxi/Vehicle.cpp include/station_optimization/simulation/helpers/Taxi_Performance.h
        src/station_optimization/simulation/charging_facility/Charging_Station_Manager.cpp
        include/station_optimization/simulation/taxi/Optimal_Charge_Choice.h
        src/station_optimization/simulation/taxi/Optimal_Charge_Choice.cpp
        include/station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_Trip.h
        src/station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_Trip.cpp
        include/station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_Tour.h
        src/station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_Tour.cpp
        include/station_optimization/simulation/helpers/One_Charge.h
        src/station_optimization/simulation/helpers/One_Charge.cpp
        include/station_optimization/simulation/taxi/taxi_request/Taxi_Request.h
        src/station_optimization/simulation/taxi/taxi_request/Taxi_Request.cpp
        include/station_optimization/simulation/taxi/taxi_request/Request_Generator.h
        src/station_optimization/simulation/taxi/taxi_request/Request_Generator.cpp
        src/station_optimization/simulation/taxi/Taxi_Performance.cpp
        include/station_optimization/optimization/simulated_annealing/Simulated_Annealing_Budget.h
        src/station_optimization/optimization/simulated_annealing/Simulated_Annealing_Budget.cpp
        include/station_optimization/optimization/simulated_annealing/Simulated_Annealing_Tradeoff.h
        src/station_optimization/optimization/simulated_annealing/Simulated_Annealing_Tradeoff.cpp
        include/station_optimization/simulation/charging_facility/Charging_Station_Performance.h
        src/station_optimization/simulation/charging_facility/Charging_Station_Performance.cpp
        include/station_optimization/simulation/helpers/One_Simulation.h
        src/station_optimization/simulation/helpers/One_Simulation.cpp
        include/station_optimization/simulation/taxi/ReProduce_Taxi_Fleet.h
        include/station_optimization/simulation/charging_facility/Charging_Station_General.h
        src/station_optimization/simulation/charging_facility/Charging_Station_General.cpp
        include/station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.h
        src/station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.cpp
        include/station_optimization/simulation/charging_facility/Charging_Station_General_Manager.h
        src/station_optimization/simulation/charging_facility/Charging_Station_General_Manager.cpp
        include/station_optimization/simulation/helpers/Taxi_Performance_General.h
        src/station_optimization/simulation/taxi/Taxi_Performance_General.cpp
        include/station_optimization/simulation/helpers/Stations_Piles_Info.h
        src/station_optimization/simulation/helpers/Station_Piles_Info.cpp
        include/location_problem/Pmedian/P_Median.h src/location_problem/Pmedian/P_Median.cpp
        include/station_optimization/optimization/local_search/Local_Search.h
        src/station_optimization/optimization/local_search/Local_Search.cpp
        include/station_optimization/optimization/heuristic/Heuristics.h
        src/station_optimization/optimization/heuristic/Heuristics.cpp
        include/station_optimization/optimization/local_search/Local_Search_Simulation.h
        include/station_optimization/simulation/helpers/Simulation.h
        src/station_optimization/simulation/helpers/test.cpp
        src/station_optimization/optimization/local_search/Local_Search_Simulation.cpp
        include/station_optimization/simulation/taxi/Taxi_Fleet.h
        include/station_optimization/simulation/taxi/normal_taxi/Taxi.h
        src/station_optimization/simulation/taxi/normal_taxi/Taxi.cpp
        include/station_optimization/simulation/taxi/taxi_request/Request_Assignment.h
        include/station_optimization/simulation/taxi/taxi_request/Request_Set.h
        src/station_optimization/simulation/taxi/taxi_request/Request_Set.cpp
        include/station_optimization/simulation/taxi/Vehicle_Fleet.h
        include/station_optimization/simulation/helpers/Simulation_General.h
        include/station_optimization/simulation/helpers/Initial_Info.h
        src/station_optimization/simulation/helpers/Initial_Info.cpp
        include/station_optimization/simulation/helpers/Mobility_Demand_Container.h
        include/station_optimization/optimization/VNS/VNDS.h
        include/station_optimization/optimization/VNS/AVNDS.h
        include/station_optimization/optimization/VNS/VNSwASHT.h
        src/station_optimization/optimization/VNS/VNDS.cpp
        src/station_optimization/optimization/VNS/AVNDS.cpp
        src/station_optimization/optimization/VNS/VNSwASHT.cpp
        include/parameters.h)

target_link_libraries(charing ${Boost_LIBRARIES})
#cplex-library cplex-concert ilocplex
