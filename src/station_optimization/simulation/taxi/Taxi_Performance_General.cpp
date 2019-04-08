//
// Created by ZhengchaoWANG on 30.07.18.
//

#include "station_optimization/simulation/helpers/Taxi_Performance_General.h"

template <class T> void Taxi_Performance_General::getTaxiPerformance( vector<T> & mytaxifleet) {
    // According to different simulation method,(point based or trip based)
    switch (mytaxifleet.begin()->getChargesimulationalgorithm()){
        case(1):{
            for (auto &iter : mytaxifleet) {

                auto totaldemand = int(iter.getTd().size());
                int serveddemand = 0;

                double totaldemanddistance = 0, serveddemanddistance = 0, totaldistance = 0;
                int totaldemandtime = 0, serveddemandtime = 0, totaltime = 0;

                if(iter.getTd().size() != iter.getDemandnumber()){ //iter->getDemandID().size()
                    printf("This is a taxi that has different taxi demand. Check \n");
                    cout << iter.getId_() << endl;
                }

                int i = 0;
                for(auto demanditer : iter.getTd()){
                    totaldemanddistance += demanditer.getLength();
                    totaldemandtime += demanditer.getTime()[1] - demanditer.getTime()[0];
                    if(demanditer.isServed()){
                        serveddemanddistance += demanditer.getLength();
                        serveddemandtime += demanditer.getTime()[1] - demanditer.getTime()[0];
                        serveddemand +=1;
                    }
                    Taxi_Demand onedemand = iter.getTd()[i];
                    array<int,2> thisdemand = iter.getDemandID()[i];
                    if(onedemand.getTime() != thisdemand){
                        cout << "There is a td demand not match the demand from data at demand " << i << " for taxi " << iter.getId_() << endl;
                    }
                    i++;
                }
                addValueToVector(iter.getId_(), totaldemand, serveddemand, totaldemanddistance, serveddemanddistance, totaldemandtime, serveddemandtime,
                                 iter.getMytour().getTrips().size(),totaldistance, totaltime);
            }
            break;
        }
        case(2):{
            for (auto &iter : mytaxifleet) {

                /* demand coverage ratio */
                int demandnumber = 0, serveddemandnumber = 0, totalnumber = 0;
                double totaldemanddistance = 0, serveddemanddistance = 0, totaldistance = 0;
                int totaldemandtime = 0, serveddemandtime = 0, totaltime = 0;

                /* charge performance */
                vector<int> thiswaitduration = {0};
                vector<int> thischargeduration = {0};

                for(auto tripiter : iter.getMytour().getTrips()){
                    totaldistance += tripiter.getTriplength();
                    totaltime += tripiter.getEndtime() - tripiter.getBegintime();
                    if(tripiter.getCustag()) {
                        totaldemanddistance += tripiter.getTriplength();
                        totaldemandtime += tripiter.getEndtime() - tripiter.getBegintime();
                        demandnumber += 1;
                        if (tripiter.isServed()) {
                            serveddemanddistance += tripiter.getTriplength();
                            serveddemandtime += tripiter.getEndtime() - tripiter.getBegintime();
                            serveddemandnumber += 1;
                        }
                    }
                }
                addValueToVector(iter.getId_(), demandnumber, serveddemandnumber, totaldemanddistance, serveddemanddistance,
                                 totaldemandtime, serveddemandtime, iter.getMytour().getTrips().size(),totaldistance, totaltime);

                for(auto chargeiter : iter.getTaxicharges()){
                    if ((chargeiter.getBeginchargetime() - chargeiter.getArrivetime()) >= 0){
                        thiswaitduration.push_back(chargeiter.getBeginchargetime() - chargeiter.getArrivetime());
                    } else{
                        cout << "This is a ignorance of a charge" << '\t' << chargeiter.getBeginchargetime() - chargeiter.getArrivetime() << endl;
                    }
                    if (chargeiter.getChargetime() >=0){
                        thischargeduration.push_back(chargeiter.getChargetime());
                    } else{
                        cout << "The charge does not finish" << '\t' << chargeiter.getChargetime() << endl;
                    }
                    //chargestationID.push_back(chargeiter->getChargestationID());
                }
                addTaxiChargeInfo(thiswaitduration, thischargeduration);
            }
            most_charge = mytaxifleet[1].getTaxicharges();
            break;
        }
        default:
            break;
    }
}

template<class T> void Taxi_Performance_General::computeTaxiRevenueProfit(vector<T> & mytaxifleet) {
    /* The revenue of taxis is just a simple calculation of the incoem of the served passengers.
     * it is equal to basic charge of each trip + the income of distances.
     */

    originalrevenue.reserve(mytaxifleet.size());
    taxirevenue.reserve(mytaxifleet.size());
    taxiprofits.reserve(mytaxifleet.size());

    for (auto &taxiiter : mytaxifleet) {
        double fixedcharge = taxiiter.getFixedtripcharge();
        double begindistance = taxiiter.getMinimumdistance();
        double chargeperdistance = taxiiter.getChargeperdistance();

        double totalincome = 0, originalincome = 0;
        for(auto tripiter = taxiiter.getMytour().getTrips().begin(); tripiter != taxiiter.getMytour().getTrips().end(); tripiter++){
            if(tripiter->getCustag()){
                double this_trip_revenue = fixedcharge + max(0.0,tripiter->getTriplength() - begindistance)*chargeperdistance;
                originalincome += this_trip_revenue;
                if(tripiter->isServed()){
                    totalincome += this_trip_revenue;
                }
            }
        }
        originalrevenue.push_back(originalincome);
        taxirevenue.push_back(totalincome);
        if (originalincome != 0){
            revenue_ratio = totalincome/originalincome;
        }

        double totalcost = 0;
        for(auto &chargeiter : taxiiter.getTaxicharges()){
            totalcost += chargeiter.getChargecost();
        }
        taxiprofits.push_back(totalincome - totalcost);
    }
}

//template<class T>
void Taxi_Performance_General::computePerformance(vector<ReProduce_Taxi_General>& mytaxifleet) { //
    computeTaxiRevenueProfit(mytaxifleet);
    getTaxiPerformance(mytaxifleet);

    int all_wait_duration = 0, all_charge_duration = 0, served_time = 0, total_time = 0, agg_served_demand = 0, agg_total_demand = 0;
    double dis_served_demand_ratio = 0.0;
    for(int i = 0; i < demandnumber.size(); i++){
        if (demandnumber[i] != 0){
            dis_served_demand_ratio += ((double)serveddemandnumber[i])/demandnumber[i];
        }
        agg_served_demand +=serveddemandnumber[i]; agg_total_demand += demandnumber[i];
        served_time += servedtime[i]; total_time += totaldemandtime[i];
        if (!taxiswaitduration[i].empty()){
            all_wait_duration += accumulate(taxiswaitduration[i].begin(),taxiswaitduration[i].end(),0);
        }
        if (!taxischargeduration[i].empty()){
            all_charge_duration += accumulate(taxischargeduration[i].begin(), taxischargeduration[i].end(),0);
        }
    }
    //performance = (alpha*served_time - alpha1*(total_time-served_time) - beta * all_wait_duration - gamma * all_charge_duration)/1000000;
    switch (objectivefunctionchoice) {
        case (0): {
            performance = (served_time - alpha*(all_wait_duration + all_charge_duration))/3600.0/demandnumber.size();
            break;
        }
        case (1): {
            performance = (served_time - alpha*(total_time-served_time))/3600.0/demandnumber.size();
            break;
        }
        case (2):{
            performance = (dis_served_demand_ratio + alpha*revenue_ratio)/double(demandnumber.size());
            break;
        }
        case (3): {
            performance = (served_time - alpha *(total_time-served_time) - beta * all_wait_duration - gamma * all_charge_duration)/3600.0/demandnumber.size();
            break;
        }
        case (4): {
            performance = -(all_wait_duration + all_charge_duration)/3600.0/demandnumber.size();
            break;
        }
        default:
            break;
    }
}

void Taxi_Performance_General::computeChargingStationIndex(Charging_Station_General_Manager &mystations, int simulation_span) {
    computeStationUtilizationRatio(mystations, simulation_span);
    computeStationWaitIndex(mystations);
    computeStationRevenue(mystations);
}

template<class T>
void Taxi_Performance_General::computeStationUtilizationRatio(T &my_stations, int simulation_span) {
    all_utilization_time_rate.reserve(my_stations.getCfset().size());
    for(auto station:my_stations.getCfset()){
        vector<double> utlization_time_rate(station.second.getRate().size());
        for(int i=0; i<station.second.getCap().size();i++){
            if(station.second.getCap()[i] > 0){
                double used_time_rate = 0.0;
                for(auto myiter = station.second.getSlotutili()[i].begin(); myiter != station.second.getSlotutili()[i].end()-1;myiter++){
                    used_time_rate += ((myiter+1)->front() -  myiter->front()) * myiter->back();
                }
                utlization_time_rate[i] = used_time_rate/simulation_span/station.second.getCap()[i];
            } else {
                utlization_time_rate[i] = -999;
            }
        }
        all_utilization_time_rate.push_back(utlization_time_rate);
    }
}

template<class T>
void Taxi_Performance_General::computeStationWaitIndex(T &my_stations) {

    for(auto item:my_stations.getCfset()){
        int total_wait_duration = 0,  queue_length = 0;
        int this_wait_number = 0;
        if(accumulate(item.second.getCap().begin(),item.second.getCap().end(),0) >0){
            for(auto my_array : item.second.getWaitduration()){
                total_wait_duration += my_array.back();
            }
            this_wait_number = item.second.getWaitduration().size();
            if(!item.second.getWaitnumber().empty()){
                queue_length = findMaximumQueueLength(item.second.getWaitnumber());
            }
        }
        total_wait_number.push_back(this_wait_number);
        total_wait_time.push_back(total_wait_duration);
        max_queue_length.push_back(queue_length);
        if(this_wait_number==0){
            average_wait_time.push_back(-999);
        } else {
            average_wait_time.push_back(double(total_wait_duration)/this_wait_number);
        }
    }
}



/********************************************************************************************************************
 *  Private functions
 ********************************************************************************************************************/

template<typename T1, size_t T2>
bool Taxi_Performance_General::arraiesComare(array<T1, T2> & my_array1, array<T1, T2> & my_array2) {
    return my_array1[1] < my_array2[1] ;
}

int Taxi_Performance_General::findMaximumQueueLength(const vector<array<int, 2>> &my_queue_series) {
    int max_queue_length = 0;
    for(auto item : my_queue_series){
        max_queue_length = max(max_queue_length, item.back());
    }
    return max_queue_length;
}

void Taxi_Performance_General::addValueToVector(const int & id, const int & demandnumber, const int & serveddemandnumber, const double & demanddistance,
                                                const double & serveddistance, const int & demandtime, const int & servedtime,
                                                const int& totaltripnumber, const double& totaldistance, const int& totaltime) {
    this->taxiID.push_back(id);
    this->demandnumber.push_back(demandnumber);
    this->serveddemandnumber.push_back(serveddemandnumber);

    this->serveddistance.push_back(serveddistance);
    this->totaldemanddistance.push_back(demanddistance);

    this->totaldemandtime.push_back(demandtime);
    this->servedtime.push_back(servedtime);

    this->totalnumber.push_back(totaltripnumber);
    this->totaldistance.push_back(totaldistance);
    this->totaltime.push_back(totaltime);

}

void Taxi_Performance_General::addTaxiChargeInfo(const vector<int> &thiswaitduration, const vector<int> &thischargeduration) {
    this->taxiswaitduration.push_back(thiswaitduration);
    this->taxischargeduration.push_back(thischargeduration);
    if (thiswaitduration.empty()) {
        this->total_wait_time.push_back(0);
    } else {
        this->total_wait_time.push_back(accumulate(thiswaitduration.begin(), thiswaitduration.end(), 0));
    }
    if (thischargeduration.empty()) {
        this->total_wait_time.push_back(0);
    } else {
        this->total_charge_time.push_back(accumulate(thischargeduration.begin(), thischargeduration.end(), 0));
    }
}
void Taxi_Performance_General::writeTaxiDemandIndicatorToTxt(const string &filename) {
    ofstream outputfile(filename);
    if(outputfile.is_open()){
        outputfile <<"taxiID" << "\t" << "total trip" << "\t" << "demand trip" <<  "\t" <<"served demand" << "\t" << "total distance"
                   <<"\t" <<"demand distance" << "\t" << "served distance" << "\t" <<"total time(h)" << "\t" <<"demand time(h)" << "\t"
                   <<"served time(h)"  <<"\t"<< "taxi revenue" << "\t" << "taxi profit" << "\t" << "original revenue" <<
                   '\t' << "wait time " << "\t" << "charge time" << endl;
        for(int i = 0; i < this->demandnumber.size(); i++){

            outputfile<< taxiID[i] << "\t" << totalnumber[i] << "\t" << demandnumber[i] << "\t" <<serveddemandnumber[i] << "\t"
                      <<totaldistance[i] << "\t" <<totaldemanddistance[i] << "\t" <<serveddistance[i] << "\t" <<totaltime[i]
                      << "\t" << totaldemandtime[i] << "\t" <<servedtime[i] <<"\t"<< taxirevenue[i] << "\t" << taxiprofits[i]
                      << "\t" << originalrevenue[i] <<"\t" << total_wait_time[i] <<"\t" << total_charge_time[i] << endl;
        }
    } else {
        printf("file open failed");
    }
    outputfile.close();
}

void Taxi_Performance_General::writeStationSummaryIndicatorToTxt(const string &filename) {
    ofstream outputfile(filename);
    if(outputfile.is_open()){
        outputfile <<"charging station ID" << "\t" << "Maximum queue length" << "\t" << "averge wait time" << "\t" <<"total wait number" << "\t" <<"utilization ratio" << endl;
        for(int i = 0; i < this->max_queue_length.size(); i++){
            outputfile<< i << "\t" << max_queue_length[i] << "\t" << average_wait_time[i] << "\t" <<total_wait_number[i];
            for(auto j : all_utilization_time_rate[i]) {
                outputfile << "\t" << j;
            }
            outputfile <<endl;
        }
    } else{
        printf("file open failed");
    }
    outputfile.close();
}

template<class T>
void Taxi_Performance_General::computeStationRevenue(T &my_stations) {
    /*
     * wait to finish.
     */

}

void Taxi_Performance_General::writeTaxiChargingDemandsToTxt(const string &filename) {
    ofstream outputfile(filename);
    if(outputfile.is_open()){
        outputfile <<"charge ID" << "\t" << "begin charge time" << "\t" << "lat" << "\t" <<"lon" << "\t" <<"arrive time"
                   <<"\t" << "station lat" <<"\t" << "station lon" <<"\t" << "station ID"<< endl;
        int i = 1;
        for(auto one_charge:most_charge){
            outputfile<< i << "\t" << one_charge.getBeginchargetime() << "\t" << one_charge.getStartposition().front()
                      << "\t" << one_charge.getStartposition().back() << "\t" << one_charge.getArrivetime() << "\t"
                      << one_charge.getChosenstationpos().front() <<"\t" << one_charge.getChosenstationpos().back() << "\t" << one_charge.getChargestationID() <<endl;
            i++;
        }
    } else{
        printf("file open failed");
    }
    outputfile.close();

}




