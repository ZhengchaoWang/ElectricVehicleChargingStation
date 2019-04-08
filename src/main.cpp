/* Demo_04_Main.cpp */
/*
 * TO DO
 *
 * Write the output function of indicators in Taxi_Performance_General class.
 *
 */

#include<string>
#include <vector>
#include <random>
// For time record
#include <ctime>

#include "station_optimization/optimization/local_search/Local_Search.h"
#include "station_optimization/optimization/simulated_annealing/Simulated_Annealing_Budget.h"

#include "station_optimization/simulation/helpers/One_Simulation.h"
#include "station_optimization/simulation/taxi/normal_taxi/Taxi.h"
#include "station_optimization/simulation/taxi/taxi_request/Taxi_Request.h"
#include "station_optimization/simulation/taxi/taxi_request/Request_Set.h"
#include "station_optimization/simulation/helpers/Simulation_General.h"
#include "station_optimization/simulation/helpers/Stations_Piles_Info.h"
#include "station_optimization/simulation/taxi/ReProduce_Taxi_Fleet.h"
#include "station_optimization/simulation/helpers/Initial_Info.h"

using namespace std;
using dist_t = uniform_int_distribution<>;
using param_t = dist_t::param_type;

/*
 * Helper functions
 */

vector<int> get_random_variable(unsigned int seeds, int type, int number){

    // Initialize the random engines
    mt19937 seed_gen(seeds);
    std::uniform_int_distribution<> integer_distribution;
    param_t  p0{1, static_cast<int>(10000000)};

    // create the output vector
    vector<int> random_number_vec;
    switch (type){
        case (0):{
            for(int i = 0; i < number; i++){
                random_number_vec.push_back(integer_distribution(seed_gen,p0));
            }
            break;
        }
        default:
            break;
    }
    return random_number_vec;
}

Stations_Piles_Info get_basic_stations_info(string & mystationfile, bool run_cluster = false){
    // Get the correct information for the type of chargers and its corresponding price.

    string my_Initialstationconfig, chargestationfile, pilefile;

    if (mystationfile == "None"){
        if (run_cluster){
            my_Initialstationconfig = "/home/zhewang/info/stationconfig.txt";
        } else{
            my_Initialstationconfig = "/Volumes/Documents/Research/PhD/First project/results/simulatedannealing/goodexample1/stationconfig.txt";
        }
    } else{
        my_Initialstationconfig = mystationfile;
    }

    if (run_cluster){

         chargestationfile = "/home/zhewang/info/stationlocation6frommap.txt";
         pilefile = "/home/zhewang/info/price0.txt";
    } else{
         pilefile = "/Volumes/Documents/Research/PhD/Data/SFTaxi/pileinfo/price0.txt";
         chargestationfile = "/Volumes/Documents/Research/PhD/Data/SFTaxi/stationlocation/stationlocation6frommap.txt";

    }

    Stations_Piles_Info my_basic_information;
    my_basic_information.setAllFilesPath(chargestationfile,pilefile,my_Initialstationconfig);
    my_basic_information.initiateFromFiles();
    return my_basic_information;
}

void output_data(string& filename, vector<vector<double>>& data){
    ofstream outputfile(filename);
    if(outputfile.is_open()){

        for(int i = 0; i < static_cast<int>(data[0].size()); i++){
            for (auto &j : data) {
                outputfile<< j[i] << "\t";
            }
            outputfile << endl;
        }
    } else {
        printf("file open failed");
    }
    outputfile.close();
}

void configuration_optimization_different_budget(vector<int> & case_number, bool run_on_cluster){
    /**** Set the budget ratio for the analysis. ****/
    vector<double> budget_ratio = {6.0,7.0}; //3.0,4.0,5.0,
    /**** Get the basic charging station information ****/
    string my_taxifile_root, outputpathSA_root;
    if (run_on_cluster){
        my_taxifile_root = "/home/zhewang/data/day";
        outputpathSA_root = "/home/zhewang/result/SANO/day";
    } else{
        my_taxifile_root =  "/Users/zhengchaowang/Code/SFTaxi/Python/dailysplittrajectorydata/day";
        outputpathSA_root = "/Volumes/Documents/Research/PhD/First project/results/simulatedannealing/goodexample91/day";
    }
    string mystationfile = "None";
    Stations_Piles_Info my_basic_information = get_basic_stations_info(mystationfile, run_on_cluster);
    /**** A vector to store the best performance of charging stations ****/
    vector<double> performance_summary_vect = {};

    /**** Get random variables to seed SA alogirhtm****/
    vector<int> random_seeds = get_random_variable(564158413, 0, 3);
    int intial_seed = random_seeds[0], neighbor_seed = random_seeds[1], adoption_seed = random_seeds[2];
    /*************************************************************************************************************
        This is to get the configuration under different buget and requests.
    **************************************************************************************************************/
    for(int i:case_number){

        cout << "This is the data " << i << endl;

        /**** get the right data file for the trajectory of taxis.****/
        string taxifile = my_taxifile_root + to_string(i)+"/";

        for (double j :budget_ratio){
            cout << "This is the budget ratio " << j << endl;

            /**** Create a series of ReProduce_Taxi_Fleet from the data file. ****/
            auto my_fleet = ReProduce_Taxi_Fleet(0,taxifile);
            if (!my_fleet.isInitialized()){
                my_fleet.readMyFleetFromFiles();
                my_fleet.setInitialized(true);
            }
            /**** Set the directory of the output path ****/
            //string outputpathSA = "/Volumes/Documents/Research/PhD/First project/results/simulatortest/initial/";
            string outputpathSA = outputpathSA_root + to_string(i) + "/bf" + to_string(int(j)) + "/";

            Simulated_Annealing_Budget SA(my_basic_information.getStation_locations(), my_basic_information.getPile_rate(), my_basic_information.getPile_price());
            /**** Initialize the seeds for SA. ****/
            SA.setSeeds(static_cast<unsigned int>(intial_seed), static_cast<unsigned int>(neighbor_seed),
                        static_cast<unsigned int>(adoption_seed));
            /**** Initialize the budget factor to adjust the budget ****/
            SA.setBudget_ratio(j/5.0);
            SA.setMy_fleet_pointer(&my_fleet);
            /*
             * // The order of the control sequence is default initialization current_station_config, minimal price,
             * max pile number installable, initializeStationsMaximalIncreaseDecreaseValue, singletype;
             */
            SA.defaultClassInitialization(true, true, true, true, {false});
            /****Set the output directory for the results. ****/
            SA.setOutput_path(outputpathSA);
            /**** Run the optimization procedure (SA) algorithm ****/
            SA.run();
            SA.outputResult();
            /**** Record the performance ****/
            performance_summary_vect.push_back(SA.getBest_performance_vect().back());
        }
    }
}

void configuration_optimization_different_objective_ratio(vector<int> & day_vec, bool run_on_cluster,
                                                          int objectivefunctionchoice, vector<double> & objective_alpha){
/*
 * This is used to test the sensitivity to alpha in the objective functions on different objective function.
 */
    /**** Set the alpha value, this value will be divided by 5. ****/
    vector<double> performance_summary_vect = {};
    string my_taxifile_root, outputpathSA_root;
    if (run_on_cluster){
        my_taxifile_root = "/home/zhewang/data/day";
        outputpathSA_root = "/home/zhewang/result/SANO/day";
    } else {
        my_taxifile_root =  "/Users/zhengchaowang/Code/SFTaxi/Python/dailysplittrajectorydata/day";
        outputpathSA_root = "/Volumes/Documents/Research/PhD/First project/results/simulatedannealing/goodexample91/day";
    }

    //string outputpathSA = outputpathSA_root+to_string(1)+"/sen" + to_string(int(objective_alpha[1])) + "/";
    /**** Get the correct information for the type of chargers and its corresponding price. ****/
    string mystationfile = "None";
    Stations_Piles_Info my_basic_information = get_basic_stations_info(mystationfile, run_on_cluster);
    /**** Get initial random number to seeds the algorithm ****/
    vector<int> random_seeds = get_random_variable(564158413, 0, 3);
    int intial_seed = random_seeds[0], neighbor_seed = random_seeds[1], adoption_seed = random_seeds[2];
    for (int i : day_vec){
        cout << "This is case number " << i << endl;
        /**** get the right data file for the trajectory of taxis ****/
        //string taxifile = "/Users/zhengchaowang/Code/SFTaxi/Python/dailysplittrajectorydata/day5/";
        string taxifile = my_taxifile_root + to_string(i) + "/";
        auto my_fleet = ReProduce_Taxi_Fleet(0,taxifile);
        if (!my_fleet.isInitialized()){
            my_fleet.readMyFleetFromFiles();
            my_fleet.setInitialized(true);
        }
        for(double performance_alpha:objective_alpha){
            cout << "This objective alpha " << performance_alpha << endl;
            string outputpathSA = outputpathSA_root+to_string(i)+"/sen" + to_string(int(performance_alpha)) + "/";

            Simulated_Annealing_Budget SA(my_basic_information.getStation_locations(), my_basic_information.getPile_rate(), my_basic_information.getPile_price());

            SA.setSeeds(static_cast<unsigned int>(intial_seed), static_cast<unsigned int>(neighbor_seed),
                        static_cast<unsigned int>(adoption_seed));
            SA.setObjectivefunctionchioce(objectivefunctionchoice);
            SA.setObjective_alpha(performance_alpha/5.0);
            SA.setMy_fleet_pointer(&my_fleet);
            SA.defaultClassInitialization(true, true, true, true, {false});
            SA.setOutput_path(outputpathSA);

            SA.run();
            cout << SA.getBest_performance() << endl;
            SA.outputResult();
            performance_summary_vect.push_back(SA.getBest_performance_vect().back());
        }
    }
}

void serveral_results_performance_sensitivity_analysis(vector<int> & check_day_vec, bool run_on_cluster, string & analysistype, vector<int> & file_index){

    /**** Set the station configuration files ****/
    string stationinitialfile = "/Volumes/Documents/Research/PhD/First project/results/simulatedannealing/clusterresultbudgetfromday";
    /**** Set the taxi trajectory data file ****/
    string taxifile = "/Users/zhengchaowang/Code/SFTaxi/Python/dailysplittrajectorydata/day";
    /**** Set the detail simulation results file in a day ****/
    string outputrootSensitivity = "/Volumes/Documents/Research/PhD/First project/results/simulatedannealing/clusterresultbudgetfromday";
/*    *//**** Set all the sensitivity results summery file in a day ****//*
    string outputsummerySensitivity = "/Volumes/Documents/Research/PhD/First project/results/simulatedannealing/clusterresultbudgetfromday";*/
    /**** Set all the sensitivity results in percentage summery file in a day ****/
    //string outputsummeryRatioSensitivity = "/Volumes/Documents HD/Research/PhD/First project/results/simulatedannealing/clusterresultbudgetfromday1/sensitivityresult/ratiosummery.txt";
    /**** Set the value of configfile indexes and trajectory data index ****/
    if (file_index.empty()){
        vector<int> file_index = {2,3,5,6};
    }
    int trajectory_maximal_index = 24;

    /**** In-itialize the containers to store the results ****/
    vector<vector<double>> performance_results = {};

    /**** Main body to run simulations ****/
    for(int day:check_day_vec){
        string this_outputsummerySensitivity = outputrootSensitivity + to_string(day) + "/sensitivityresultnew/summery.txt";
        for (int i : file_index) {
            cout << i << endl;
            string this_stationinitialfile = stationinitialfile + to_string(day) + "/configresultnew/" + analysistype + to_string(i)+ string("/stationconfig.txt");
            vector<double> one_config_sensitivity = {};
            for(int j = 0; j < trajectory_maximal_index; j++){
                cout << j << endl;
                /**** read the fleet trajectory information ****/
                string this_taxifile = taxifile + to_string(j) + string("/");
                auto my_fleet = ReProduce_Taxi_Fleet(0,this_taxifile);
                if (!my_fleet.isInitialized()){
                    my_fleet.readMyFleetFromFiles();
                    my_fleet.setInitialized(true);
                }
                /**** Read the station configuration data ****/
                Stations_Piles_Info my_basic_information = get_basic_stations_info(this_stationinitialfile, run_on_cluster);
                Charging_Station_General_Manager my_station_manager = my_basic_information.getChargingFacilityManager(0);
                /**** Output path ****/
                string outputpathSensitivity = outputrootSensitivity + to_string(day) + "/sensitivityresultnew/day" + to_string(j) + "/" + analysistype + to_string(i) + string("/");

                /**** Create the running class initity and run ****/
                One_Simulation OS(my_fleet.getMy_fleet(), my_station_manager, 0, 86400, true, outputpathSensitivity);
                OS.run();
                one_config_sensitivity.push_back(OS.getSimulationPerformance());
            }
            performance_results.push_back(one_config_sensitivity);
        }
        output_data(this_outputsummerySensitivity, performance_results);
    }

    /**** Output the performance results for all the config and trajectory results  ****/

}

void LS_search(){
    string this_stationinitialfile = "None";
    string taxifile = "/Users/zhengchaowang/Code/SFTaxi/Python/dailysplittrajectorydata/day1/";
    string outputpathLS = "/Volumes/Documents/Research/PhD/First project/results/localsearch/oneexample1/";
    auto my_fleet = ReProduce_Taxi_Fleet(0,taxifile);
    if (!my_fleet.isInitialized()){
        my_fleet.readMyFleetFromFiles();
        my_fleet.setInitialized(true);
    }
    Stations_Piles_Info my_basic_information = get_basic_stations_info(this_stationinitialfile);

    Local_Search LS(my_basic_information.getStation_locations(),5675335, my_basic_information.getPile_price(), my_basic_information.getPile_rate()); //567654785, 5675335
    LS.setMy_fleet_pointer(&my_fleet);
    LS.initializeDesiredUtilizationRatio(0.4);//0.3,0.2 0.35
    LS.setOutput_path(outputpathLS);
    LS.setObjectivechoice(1);
    LS.defaultClassInitialization(true, true, true, true, {false},false);
    LS.run();
    LS.outputResult();

    Charging_Station_General_Manager stations = LS.getChargingFacilityManager(0,false);
    One_Simulation OS(my_fleet.getMy_fleet(),stations, 0, 86400, true, outputpathLS);
    OS.run();
}

void oneSimulation(vector<int> & case_vec, double alphanum, int objectivefunctionchoice){
    /*string outputpathST = "/Volumes/Documents/Research/PhD/First project/results/simulatortest/initial/";
    string taxifile = "/Users/zhengchaowang/Code/SFTaxi/Python/dailysplittrajectorydata/day13/";
    string intialconfig = "/Volumes/Documents/Research/PhD/First project/results/simulatortest/initial/stationconfig.txt";*/

    for (int i : case_vec){
        string outputpathST = "/Volumes/Documents/Research/PhD/First project/results/simulatortest/debug/";
        string taxifile = "/Users/zhengchaowang/Code/SFTaxi/Python/dailysplittrajectorydata/day13/";
        string intialconfig = "/Volumes/Documents/Research/PhD/First project/results/simulatedannealing/clusterresultbudgetfromday13/configresultnew/sen" + to_string(i) + "/stationconfig.txt";
        auto my_fleet = ReProduce_Taxi_Fleet(0,taxifile);
        if (!my_fleet.isInitialized()){
            my_fleet.readMyFleetFromFiles();
            my_fleet.setInitialized(true);
        }
        Stations_Piles_Info my_basic_information = get_basic_stations_info(intialconfig);
        Charging_Station_General_Manager my_station_manager = my_basic_information.getChargingFacilityManager(0);
        One_Simulation OS(my_fleet.getMy_fleet(),my_station_manager, 0, 86400, true, outputpathST);
        OS.setObjective_alpha(alphanum);
        OS.setObjectivefunctionchoice(objectivefunctionchoice);
        OS.run();
        cout<<OS.getSimulationPerformance() << endl;
    }
    cout << "finish alpha number check" << alphanum <<endl;
}

int main(int argc, char** argv){

    bool run_on_cluster = false;
    vector<int> day_vec = {13};

    for (int i : day_vec){
        vector<int> day= {i};
        vector<double> objective_alpha = {0.0, 5.0, 15.0, 50.0, 500.0, 5000.0, 50000.0};
        int objectivefunctionchoice = 0;
        configuration_optimization_different_objective_ratio(day, run_on_cluster, objectivefunctionchoice, objective_alpha);
        objectivefunctionchoice = 4;
        objective_alpha = {10};
        configuration_optimization_different_objective_ratio(day, run_on_cluster, objectivefunctionchoice, objective_alpha);
    }

    /**********************************************
     * Sensitivity analysis
     **********************************************/
    string  analysis_type = "sen";
    vector<int> alpha_vec = {10}; //{0, 2, 3, 5, 6, 8, 50, 500, 5000, 50000,10}; 0, 5, 15, 50, 500, 5000, 50000
    serveral_results_performance_sensitivity_analysis(day_vec, run_on_cluster, analysis_type,alpha_vec);

    /**********************************************
    * One simulation
    **********************************************/
    /*for (double i :alpha_vec){
        if (i == 10.0){
            oneSimulation(alpha_vec,i/5.0,4);
        } else {
            oneSimulation(alpha_vec,i/5.0,0);
        }
    }*/

    //LS_search();

    return 0;
};

