//
// Created by ZhengchaoWANG on 20.07.18.
//

#include "station_optimization/simulation/helpers/Taxi_Performance.h"

void Taxi_Performance::getTaxiPerformance(vector<ReProduce_Taxi> &mytaxifleet) {
    // According to different simulation method,(point based or trip based)
    switch (mytaxifleet.begin()->getChargesimulationalgorithm()){
        case(1):{
            for(auto iter = mytaxifleet.begin(); iter!=mytaxifleet.end(); iter ++){

                int totaldemand = int(iter->getTd().size());
                int serveddemand = 0;

                double totaldistance = 0;
                double serveddistance = 0;
                int totaltime = 0;
                int servedtime = 0;

                if(iter->getTd().size() != iter->getDemandnumber()){ //iter->getDemandID().size()
                    printf("This is a taxi that has different taxi demand. Check \n");
                    cout << iter->getId_() << endl;
                }

                int i = 0;
                for(auto demanditer = iter->getTd().begin(); demanditer!= iter->getTd().end(); demanditer++){
                    totaldistance += demanditer->getLength();
                    totaltime += demanditer-> getTime()[1] - demanditer->getTime()[0];
                    if(demanditer->isServed()){
                        serveddistance += demanditer->getLength();
                        servedtime += demanditer->getTime()[1] - demanditer->getTime()[0];
                        serveddemand +=1;
                    }

                    Taxi_Demand onedemand = iter->getTd()[i];
                    array<int,2> thisdemand = iter->getDemandID()[i];
                    if(onedemand.getTime() != thisdemand){
                        cout << "There is a td demand not match the demand from data at demand " << i << " for taxi " << iter->getId_() << endl;
                    }
                    i++;
                }
                addValueToVector(iter->getId_(), totaldemand, serveddemand, totaldistance, serveddistance, totaltime, servedtime, iter->getDemandnumber());
            }
            break;
        }
        case(2):{
            for(auto iter = mytaxifleet.begin(); iter!=mytaxifleet.end(); iter ++) {

                /* demand coverage ratio */
                int totaldemand = 0;
                int serveddemand = 0;
                double totaldistance = 0;
                double serveddistance = 0;
                int totaltime = 0;
                int servedtime = 0;

                /* charge performance */
                vector<int> thiswaitduration;
                vector<int> thischargeduration;

                for(auto tripiter = iter->getMytour().getTrips().begin(); tripiter != iter->getMytour().getTrips().end(); tripiter ++){
                    if(tripiter->getCustag()) {
                        totaldistance += tripiter->getTriplength();
                        totaltime += tripiter->getEndtime() - tripiter->getBegintime();
                        totaldemand += 1;
                        if (tripiter->isServed()) {
                            serveddistance += tripiter->getTriplength();
                            servedtime += tripiter->getEndtime() - tripiter->getBegintime();
                            serveddemand += 1;
                        }
                    }
                }
                addValueToVector(iter->getId_(), totaldemand, serveddemand, totaldistance, serveddistance, totaltime, servedtime, iter->getDemandnumber());

                for(auto chargeiter = iter->getTaxicharges().begin(); chargeiter != iter->getTaxicharges().end(); chargeiter++ ){
                    thiswaitduration.push_back(chargeiter->getBeginchargetime() - chargeiter->getArrivetime());
                    thischargeduration.push_back(chargeiter->getChargetime());
                    //chargestationID.push_back(chargeiter->getChargestationID());
                }
                addTaxiChargeInfo(thiswaitduration, thischargeduration);
            }
            break;
        }
        default:
            break;
    }
}

void Taxi_Performance::addValueToVector(const int &id, const int &totaldemand, const int &serveddemand,
                                   const double &totaldistance, const double &serveddistance, const int &totaltime,
                                   const int &servedtime, const int &thistaxidemand) {

    this->taxiID.push_back(id);
    this->totaldemand.push_back(totaldemand);
    this->serveddemand.push_back(serveddemand);
    this->serveddistance.push_back(serveddistance);

    this->totaldistance.push_back(totaldistance);
    this->totaltime.push_back(totaltime);
    this->servedtime.push_back(servedtime);

    this->atotaldemand.push_back(thistaxidemand);

}

void Taxi_Performance::addTaxiChargeInfo(const vector<int> &thiswaitduration, const vector<int> &thischargeduration) {

    this->taxiswaitduration.push_back(thiswaitduration);
    this->taxischargeduration.push_back(thischargeduration);

}

void Taxi_Performance::writeTaxiDemandIndicatorToTxt(const string &filename) {
    ofstream outputfile(filename);
    if(outputfile.is_open()){
        outputfile <<"taxiID" << "\t" << "true total demand" << "\t" << "total demand" << "\t" <<"difference" << "\t" <<"served demand" << "\t" << "total distance"
                   << "\t" << "served distance" << "\t" <<"total time" << "\t" <<"served time"  <<endl;
        for(int i = 0; i < this->totaldemand.size(); i++){

            outputfile<< taxiID[i] << "\t" << atotaldemand[i] << "\t" << totaldemand[i] << "\t" <<atotaldemand[i] - totaldemand[i] << "\t" <<serveddemand[i] << "\t" <<totaldistance[i]
                      << "\t" <<serveddistance[i] << "\t" <<totaltime[i]  << "\t" <<servedtime[i] <<endl;
        }
    } else{
        printf("file open failed");
    }
    outputfile.close();
}


void Taxi_Performance::computeTaxiRevenueProfit(vector<ReProduce_Taxi> & mytaxifleet) {
    /* The revenue of taxis is just a simple calculation of the incoem of the served passengers.
     * it is equal to basic charge of each trip + the income of distances.
     */

    taxirevenue.reserve(mytaxifleet.size());
    taxiprofits.reserve(mytaxifleet.size());

    int i = 0;
    for(auto taxiiter = mytaxifleet.begin(); taxiiter!= mytaxifleet.end(); taxiiter++ ){
        double fixedcharge = taxiiter->getFixedtripcharge();
        double begindistance = taxiiter->getMinimumdistance();
        double chargeperdistance = taxiiter->getChargeperdistance();

        double totalincome = 0;
        for(auto tripiter = taxiiter -> getMytour().getTrips().begin(); tripiter != taxiiter -> getMytour().getTrips().end(); tripiter++){
            if(tripiter->getCustag()){
                if(tripiter->isServed()){
                    totalincome += fixedcharge + max(0.0,tripiter->getTriplength() - begindistance)*chargeperdistance;
                }
            }
        }
        taxirevenue[i] = totalincome;

        double totalcost = 0;
        for(auto chargeiter = taxiiter->getTaxicharges().begin(); chargeiter != taxiiter->getTaxicharges().end(); chargeiter++ ){
            totalcost += chargeiter->getChargecost();
        }
        taxiprofits[i] = totalcost;
        i++;
    }



}


