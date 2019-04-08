/* ReProduce_Taxi.cpp */
// Created by ZhengchaoWANG on 13.06.18.
//

/*
 * In the threshold method,
 * For many loop with index begin with 1, we want to begin with next timestamp comparing to current one.
 * We cannot use long stop for charging by now.
 */

/* To DO;
 * syros  zante elafonisos
 */

#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi.h"


using namespace std;

void ReProduce_Taxi::generatePlan(Charging_Station_Manager &cfm,  const int & threshold) {

    /* In this function, we try to generate the plan for a vehicle based on its current state and CFs. There might be several
     * methods to do so.
     * 1, Threshold method with tracking points by tracking points iteration. Vehicle will begin to charge as long as his soc is lower than a threshold.
     * 2, Threshold method with iterating over trip.
     */
    switch (chargesimulationalgorithm){
        case(1) : {
            /*
             * Here, we come to the threshold method.
             * In this method, the actions of vehicle are divided into 7 different actions. from 0 to 6.
             * 0: taxi begins to drive without passenger. It tries to finish this trip or try to charge in the middle.
             * 1; taxi begins to drive with passenger.  It tries to finish this trip or try to charge in the middle.
             * 2: Taxi tries to charge. At this state, it tries to find which station to charge and how long it takes to arrive at the charge station.
             * 3: Taxi arrives at charge station. It will decide to charge or to wait depends on the availability of slots.
             * 4: Taxi begins to charge. It will decide to charge for how long.
             * 5: Taxi departure charge station. It will decide which points in the trajectory to merge.
             * 6: This is the end of tracking.
             */
            switch (cus) {
                case(0) : {
                    double drivedistance = 0, leftsoc = soc;
                    for (int i = 1; i < myhistory.getCustags().size()-mycurrentstateinhistory; i++) {

                        drivedistance += myhistory.getDistance()[mycurrentstateinhistory+i];
                        leftsoc -= myhistory.getDistance()[mycurrentstateinhistory+i] * consumrate/capf;

                        // Two criteria to make a new plan. First one, change the customer tag, second one, out of planenergy.
                        // as long as the futuer does not change a passenger tag , we keep
                        // adding future trajectories to the visited one until the planenergy is ran out.

                        if(myhistory.getCustags()[mycurrentstateinhistory+i]) {

                            setThePlan(i, 1, -drivedistance * consumrate);
                            break;
                        }else {
                            if (leftsoc<ra) {
                                setThePlan(i, 2, -drivedistance * consumrate);
                                break;
                            } else {
                                continue;
                            }
                        }
                    }
                    break;
                }
                case(1): {
                    /*
                     * When the simulation ends at this time, it is the same as above. The for loop will break automatically
                     * and all the indexs are 0.
                     */

                    double totaldistance = 0;                                // The total distance of one trip
                    int driveendindex = 0;                                  // The end of a drive with customer.
                    double leftsoc = soc;
                    // variables use to store value if the taxi demand cannot be served.
                    int criticalindex = 0;
                    double criticaldistance = 0;
                    bool notcritical = true;

                    // In this loop, as long as the future is not empty, we can get a driveendindex which is at least 0.

                    for (int i = 1; i < myhistory.getCustags().size() - mycurrentstateinhistory; i++) {

                        totaldistance += myhistory.getDistance()[mycurrentstateinhistory+i];
                        leftsoc -= myhistory.getDistance()[mycurrentstateinhistory+i] * consumrate / capf;
                        if ((leftsoc < ra) & notcritical) {
                            criticalindex = i;
                            criticaldistance = totaldistance;
                            notcritical = false;
                        }

                        // To deduce if the driving state has  changed or not.

                        if (*(myhistory.getCustags().begin()+mycurrentstateinhistory+i) & ((mycurrentstateinhistory+i) != (myhistory.getCustags().size()-1))) {
                            continue;
                        } else {
                            driveendindex = i;
                            break;
                        }
                    }

                    // Get the information to initialize a taxi_demand instance.



                    if (leftsoc >= minSoc) {

                        // if this taxi demand can be covered, we then add the taxi demand to the served one
                        // initialize the instance of a taxi_demand

                        if(this->time < trackend ){
                            generateAddTaxiDemand(driveendindex, totaldistance, true);
                        }
                        if (leftsoc >= ra ){
                            setThePlan(driveendindex, 0, -totaldistance * consumrate);
                        }else{
                            // if the soc after finishing the drive is smaller than the critical soc, the taxi driver will
                            // try to charge his vehicle.
                            setThePlan(driveendindex, 2, -totaldistance * consumrate);
                        }

                    } else {
                        if(this->time < trackend ){
                            generateAddTaxiDemand(driveendindex, totaldistance, false);
                        }

                        setThePlan(criticalindex, 2, -criticaldistance * consumrate);
                    }
                    break;
                }

                case(2):{ // find a charge station and move there. Before put everything into the plan, we need to check
                    // if the simulation is finished.
                    if(this->time < trackend ){
                        Charge_Choice chargechoice = chargeStationChoice(cfm);
                        auto timetostation = int(chargechoice.getDistance()/drivespeed);

                        // to avoid the same time problem.
                        if(timetostation==0){
                            timetostation++;
                        }
                        int index = findIndexInFuture(this->time + timetostation);

                        // Initiate the plan.
                        setThePlan(index, 3, -chargechoice.getDistance() * consumrate, min(trackend, this->time + timetostation),chargechoice.getPos());

                    }
                    break;
                }
                case(3) : {  // arrive at the station, if slot is available, charge; wait otherwise.
                    /*
                    * When the taxi arrive at this stage, it means that this taxi arrive at a charging station and tries to
                    * charge. However, we need to inspect how long the taxi needs to charge and hong long the taxi has to wait.
                    */
                    if(this->time < trackend ){
                        array<double,2> stationpos = plan.getPlanposition()[0];

                        auto chargefacilityiter = findTheChargeStaion(cfm,stationpos);

                        // check if the taxi could charge directly (the occupied charge station is smaller than the ). Otherwise, he can charge to somewhere else.
                        // Here has a problem, it returns true even if its false.
                        auto leftslot = static_cast<int>(chargefacilityiter->second.getCap() - chargefacilityiter->second.getOcupslot() - chargefacilityiter->second.getWaitlist().size());
                        if (1 <= leftslot) {

                            //determine how long to charge
                            int chargeduration = int((chargeTo - soc) * capf / min(chargefacilityiter->second.getRate(), this->getChargerate()));
                            int index = findIndexInFuture( time + chargeduration);
                            setThePlan(index, 5, (chargeTo-soc)*capf, min(trackend, this->time + chargeduration));
                        } else {
                            setThePlan(0, 4, 0, this->time);
                        }
                    }else{
                        setThePlan(0, 6, 0, trackend );
                    }
                    break;
                }
                case(4):{ //begin to charge.
                    if(this->time < trackend ){
                        array<double,2> stationpos = plan.getPlanposition()[0];

                        auto chargefacilityiter = findTheChargeStaion(cfm,stationpos);

                        auto chargeduration = int((chargeTo - soc) * capf / min(chargefacilityiter->second.getRate(), this->getChargerate()));
                        int index = findIndexInFuture(time + chargeduration);
                        setThePlan(index, 5, (chargeTo - soc) * capf, min(trackend, this->time + chargeduration));
                    } else{
                        setThePlan(0, 6, 0, trackend );
                    }
                    break;
                }
                case(5):{ // charge finished. It seems that here cause a bug.
                    // At this point, taxi will leave charging staion and try to find the point where he can cut into.
                    if(this->time < trackend ){
                        array<double,2> stationpos = plan.getPlanposition()[0];

                        // find the first planposition in the tracking trajectory so that the taxi could catch up with.
                        int i;
                        for (i = 0; i < myhistory.getTime().size()-mycurrentstateinhistory; i++){

                            double drivedistance = twoPointsDistance(myhistory.getPoss()[mycurrentstateinhistory+i], pos);

                            if (drivedistance < (myhistory.getTime()[mycurrentstateinhistory+i] - time) *drivespeed){

                                if(myhistory.getCustags()[mycurrentstateinhistory+i] & !myhistory.getCustags()[mycurrentstateinhistory+i-1]){
                                    if(i==0 & myhistory.getTime()[mycurrentstateinhistory+i] ==this->time){
                                        setThePlan(i+1, 1, -drivedistance * consumrate, this->time+1);
                                    } else{
                                        setThePlan(i, 1, -drivedistance * consumrate);
                                    }
                                } else{
                                    if(i==0 & myhistory.getTime()[mycurrentstateinhistory+i] ==this->time){
                                        setThePlan(i+1, 0, -drivedistance * consumrate, this->time+1);
                                    } else{
                                        setThePlan(i, 0, -drivedistance * consumrate);
                                    }
                                }
                                break;
                            }
                        }
                        if(i == myhistory.getTime().size()-mycurrentstateinhistory & i != 0){
                            setThePlan(i-1, 0, 0);
                        }

                    } else{
                        setThePlan(0, 6, 0, trackend );
                    }

                    break;
                }
                default:break;
            }
            break;
        }
        case(2):{
            switch(cus){
                case(0):{
                    keepRunningBeforeCharging();
                    break;
                }
                case(1):{
                    keepRunningBeforeCharging();
                    break;
                }
                case(2):{
                    if(this->time < trackend ){
                        Charge_Choice chargechoice = chargeStationChoice(cfm);
                        auto timetostation = int(chargechoice.getDistance()/drivespeed);

                        // to avoid the same time problem.
                        if(timetostation==0){
                            timetostation++;
                        }
                        this->currentcharge.setWantCharge(chargechoice.getCfID(),this->soc,this->pos,this->time, chargechoice.getPos());

                        int index = findIndexInFuture(this->time + timetostation);
                        setThePlan(index, 3, -chargechoice.getDistance() * consumrate, min(trackend, this->time + timetostation),chargechoice.getPos());
                    }
                    break;
                }
                case(3):{
                    if(this->time < trackend ){
                        this->currentcharge.setArriveCharge(this->soc, this->time, -plan.getPlanenergy()[0]/consumrate);

                        array<double,2> stationpos = plan.getPlanposition()[0];

                        auto chargefacilityiter = findTheChargeStaion(cfm,stationpos);

                        // check if the taxi could charge directly (the occupied charge station is smaller than the ). Otherwise, he can charge to somewhere else.
                        // Here has a problem, it returns true even if its false.
                        int leftslot = chargefacilityiter->second.getCap() - chargefacilityiter->second.getOcupslot() - int(chargefacilityiter->second.getWaitlist().size());
                        if (1 <= leftslot) {
                            //determine how long to charge
                            int chargeduration = int((chargeTo - soc) * capf / min(chargefacilityiter->second.getRate(), this->getChargerate()));
                            int index = findIndexInFuture( time + chargeduration);
                            setThePlan(index, 5, (chargeTo-soc)*capf, min(trackend, this->time + chargeduration));
                            this->currentcharge.setBeginCharge(this->time, min(chargefacilityiter->second.getRate(),  this->getChargerate()),0, 0, chargeduration, chargeTo);
                        } else {
                            setThePlan(0, 4, 0, this->time);
                        }
                    }else{
                        setThePlan(0, 6, 0, trackend );
                    }
                    break;
                }
                case(4):{
                    if(this->time < trackend ){
                        array<double,2> stationpos = plan.getPlanposition()[0];

                        auto chargefacilityiter = findTheChargeStaion(cfm,stationpos);

                        auto chargeduration = int((chargeTo - soc) * capf / min(chargefacilityiter->second.getRate(), this->getChargerate()));
                        int index = findIndexInFuture(time + chargeduration);
                        setThePlan(index, 5, (chargeTo - soc) * capf, min(trackend, this->time + chargeduration));
                        this->currentcharge.setBeginCharge(this->time, min(chargefacilityiter->second.getRate(), this->getChargerate()), 0, 0, chargeduration, chargeTo);
                    } else{
                        setThePlan(0, 6, 0, trackend );
                    }
                    break;
                }
                case(5):{
                    if(this->time < trackend ){
                        array<double,2> stationpos = plan.getPlanposition()[0];

                        // find the trip where the vehicle could catch up with the first point.
                        int i;
                        for(i = tripindex; i < mytour.getTrips().size(); i++){
                            double drivedistance = twoPointsDistance(mytour.getTrips()[i].getBeginposition(), pos);
                            if(drivedistance < (mytour.getTrips()[i].getBegintime() - this->time)*drivespeed){
                                if(mytour.getTrips()[i].getBegintime() == this->time) {
                                    setThePlan(mytour.getTrips()[i].getBeginendindex()[0] - mycurrentstateinhistory,
                                               mytour.getTrips()[i].getCustag(), -drivedistance * consumrate, this->time +1);
                                } else{
                                    setThePlan(mytour.getTrips()[i].getBeginendindex()[0] - mycurrentstateinhistory,
                                               mytour.getTrips()[i].getCustag(), -drivedistance * consumrate);
                                }
                                break;
                            }
                        }
                        if(i == mytour.getTrips().size()){
                            setThePlan(mytour.getTrips()[i-1].getBeginendindex()[1] - mycurrentstateinhistory,
                                       mytour.getTrips()[i].getCustag(), 0.0);
                        }
                        this->currentcharge.setFinishCharge(twoPointsDistance(plan.getPlanposition()[0], this->pos),plan.getPlantime()[0]-this->time, this->soc + plan.getPlanenergy()[0]/this->capf);
                    }else{
                        setThePlan(0, 6, 0, trackend );
                    }
                    break;
                }
                default:break;
            }
            break;
        }
        default:break;
    }
}
/***********************************************************************************************************************
// After getting a plan, the vehicle moves according to his plan and change the states of vehicle and the Charging station according
// to needs.
***********************************************************************************************************************/
void ReProduce_Taxi::move(Charging_Station_Manager & cfm) {

    /*
     * carried out the plan generated in the generatePlan() method.
     */

    auto chargefacilityiter = findTheChargeStaion(cfm, this->pos);

    switch (chargesimulationalgorithm){
        case(1):{
            switch (this->plan.getPlannextstate()[0]){
                case(0):{
                    /*
                     * As the next action is to move without passenger, we check if its former state is charging, if it is, we need
                     * to relise the occupied slots. Otherwise, we do nothing.
                     *
                     * With respect to change the customer tag, we need to do that when we have the cus 5->0/1, 1->2, 2->3, 3->4/5
                     * 4->5.
                     *
                     * When we say that there is a taxi demand missed, we need to add that into the function. This can happen when
                     * the state change from 5->0/1,  2->3, 3->4/5. 4->5.
                     *
                     * 1->2 is complex due to the assumption, therefore, we do change of customer tag and find missed trip inside
                     * generatePlan().
                     */
                    if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time);
                        chargefacilityiter->second.deleteChargeEnd(this->time);
                        getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                    }
                    break;
                }
                case(1):{
                    if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time);
                        chargefacilityiter->second.deleteChargeEnd(this->time);
                        getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                    }
                    break;
                }
                case(3):{
                    getMissedDrive (plan.getPlantrackindex()[0]- mycurrentstateinhistory);
                    break;
                }
                case(4):{
                    chargefacilityiter->second.addWaitList(this->id_, this->time);
                    break;
                }
                case(5):{
                    chargefacilityiter->second.addOccupied(this->time);
                    getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                    chargefacilityiter->second.addChargeEnd(this->plan.getPlantime()[0]);

                    if(cus ==4){
                        chargefacilityiter->second.departWaitList(this->id_, this->time);
                    }
                    break;
                }
                case(6):{
                    if(cus==4){
                        chargefacilityiter->second.departWaitList(this->id_, this->time);
                    }else if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time);
                        chargefacilityiter->second.deleteChargeEnd(this->time);
                    }
                    break;
                }
                default:break;
            }
            // update the states.
            this->time = plan.getPlantime()[0];
            this->pos = plan.getPlanposition()[0];
            this->cus = plan.getPlannextstate()[0];
            this->soc = this->soc + plan.getPlanenergy()[0]/capf;
            this->mycurrentstateinhistory = plan.getPlantrackindex()[0];
            break;
        }
        case (2):{
            switch (this->plan.getPlannextstate()[0]){
                case(0):{
                    /*
                     * As the next action is to move without passenger, we check if its former state is charging, if it is, we need
                     * to relise the occupied slots. Otherwise, we do nothing.
                     *
                     * With respect to change the customer tag, we need to do that when we have the cus 5->0/1, 1->2, 2->3, 3->4/5
                     * 4->5.
                     *
                     * When we say that there is a taxi demand missed, we need to add that into the function. This can happen when
                     * the state change from 5->0/1,  2->3, 3->4/5. 4->5.
                     *
                     * 1->2 is complex due to the assumption, therefore, we do change of customer tag and find missed trip inside
                     * generatePlan().
                     */
                    if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time);
                        chargefacilityiter->second.deleteChargeEnd(this->time);
                        //getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                        taxicharges.push_back(currentcharge);
                    }
                    break;
                }
                case(1):{
                    if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time);
                        chargefacilityiter->second.deleteChargeEnd(this->time);
                        //getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                        taxicharges.push_back(currentcharge);
                    }
                    break;
                }
                case(4):{
                    chargefacilityiter->second.addWaitList(this->id_, this->time);
                    break;
                }
                case(5):{
                    chargefacilityiter->second.addOccupied(this->time);
                    //getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                    chargefacilityiter->second.addChargeEnd(this->plan.getPlantime()[0]);

                    // This is done in the charge facility actions.
                    /*if(cus ==4){
                        chargefacilityiter->second.departWaitList(this->id_, this->time);
                    }*/
                    break;
                }
                case(6):{
                    taxicharges.push_back(currentcharge);
                    if(cus==4){
                        chargefacilityiter->second.departWaitList(this->id_, this->time);
                        //getMissedDrive(plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                    }else if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time);
                        chargefacilityiter->second.deleteChargeEnd(this->time);
                        //getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                    }
                    break;
                }
                default:break;
            }
            this->time = plan.getPlantime()[0];
            this->pos = plan.getPlanposition()[0];
            this->cus = plan.getPlannextstate()[0];
            this->soc = this->soc + plan.getPlanenergy()[0]/capf;
            this->mycurrentstateinhistory = plan.getPlantrackindex()[0];
            this->tripindex = computetripindex(plan.getPlantime()[0]);
            break;
        }
        default:
            break;
    }
}


/*
 * The taxi demand that is revealed and served, add them into the list one by one.
 */
void ReProduce_Taxi::tdAdd(Taxi_Demand & newDemand) {

    if(!td.empty()){
        if(td.back().getTime()!= newDemand.getTime()){
            td.push_back(newDemand);
        }
    } else{
        td.push_back(newDemand);
    }
}

Charge_Choice ReProduce_Taxi::chargeStationChoice(Charging_Station_Manager &cfm) {
    // find the closest charging staion.
    double minimaldistance = numeric_limits<double>::infinity();;
    int   chargingstationID = 0;
    array<double,2>  chargingstationpos ={0.0,0.0};
    int   chargingduration = 0;
    double thischargerate = 0;

    for(auto const & it : cfm.getCfset()){
        // This is to choose the closest charging station from current planposition to charge.
        double distance = twoPointsDistance(it.second.getPos(), pos);
        if (distance < minimaldistance){
            chargingstationID = it.second.getId_();
            chargingstationpos = it.second.getPos();
            minimaldistance = distance;
            thischargerate = it.second.getRate();
        }
    }
    chargingduration = computeChargeDuration(minimaldistance,thischargerate);

    return Charge_Choice(chargingstationpos, chargingstationID,chargingduration,minimaldistance);
}

/*
 * This method is similar to above, but tries to implement more algorithm. For example, use long gap duration to charge.
 */
Charge_Choice  ReProduce_Taxi::chargeStationChoice(Charging_Station_Manager &cfm, const int &method) {

    // Initiate a charge_choice instance.
    double minimaldistance = numeric_limits<double>::infinity();;
    int   chargingstationID = 0;
    array<double,2>  chargingstationpos ={0.0,0.0};
    int   chargingduration = 0;

    double thischargerate = 0;

    switch(CFchoicealgorithm){
        case(1):{
            // This is to choose the closest charging station from current planposition to charge.
            for(auto const & it : cfm.getCfset()){
                double distance = twoPointsDistance(it.second.getPos(), pos);
                if (distance < minimaldistance){
                    chargingstationID = it.second.getId_();
                    chargingstationpos = it.second.getPos();
                    minimaldistance = distance;
                    thischargerate = it.second.getRate();
                }
            }
            chargingduration = computeChargeDuration(minimaldistance,thischargerate);
            break;
        }
        case(2):{
            // This is to choose the charge station that allows me to charge to the desired soc as fast as I can.
            int predictedchargeend = numeric_limits<int>::infinity();
            int thischargeend;
            for(auto  & it : cfm.getCfset()){
                double distance = twoPointsDistance(it.second.getPos(), pos);

                thischargerate = it.second.getRate();
                if(thischargerate > this->getChargerate()){
                    thischargerate = this->getChargerate();
                }

                thischargeend= int(distance/drivespeed) + max(estimateWaitDuration(it.second), int(distance/drivespeed)) +
                                    int((distance*consumrate+ (chargeTo-soc)*capf)/thischargerate);

                if(predictedchargeend > thischargeend){
                    predictedchargeend = thischargeend;

                    chargingstationID = it.second.getId_();
                    chargingstationpos = it.second.getPos();
                    minimaldistance = distance;
                }
            }
            break;
        }
        case(3):{
            // Given a limit time window and current position, I want to choose a charging station so as to charge as much as possible.
            // It also needs to guarantee that I can go back to the points as well.

            break;
        }
        default:break;
    }
    return Charge_Choice(chargingstationpos, chargingstationID,chargingduration,minimaldistance);
}

int ReProduce_Taxi::computeChargeDuration(double distance, double charge_rate) {
    // the charging duration is the plantime that is used to fill the gap and the consumption to the charging station from
    // current planposition.
    return int(((chargeTo-soc)*capf + distance*consumrate)/charge_rate);
}

int ReProduce_Taxi::computeChargeduration(double chargerate) {

    return int((chargeTo - soc) * capf / chargerate);
}

// By now the following two functions assume that the charging duration etc are fixed.
int ReProduce_Taxi::exactWaitDuration(Charging_Station_Manager &cfm) {
    int waittime = 0;
    for (auto &iter : cfm.getCfset()) {
        if(iter.second.getPos() == this->pos){
            const vector<int> & otherschargeend = iter.second.getChargeend();
            auto queuelength = static_cast<int>(iter.second.getWaitlist().size());
            waittime = otherschargeend[queuelength] - this->time;
            break;
        }
    }
    return waittime;
}

int ReProduce_Taxi::estimateWaitDuration( Charging_Station & cf) {

    return min(cf.getChargeend()[cf.getWaitlist().size()]-this->time, 0);
}

/*
 * This is tries to find the corresponding driving trajectory of taxi while he has to go to charge. The returned
 * index is the one whose time is more than needed.

 * However, problem might happen if there is a long stop between index-1 and index. solve it.
 */
int ReProduce_Taxi::findIndexInFuture ( int needfoundtime){

    int index;
    for(index = 0; index< myhistory.getTime().size() - mycurrentstateinhistory; index++) {
        if(myhistory.getTime()[index + mycurrentstateinhistory] > needfoundtime){
            break;
        }
    }
    if(index==myhistory.getTime().size()- mycurrentstateinhistory-1 ){
        std::cout <<"This is the end of the tracking, finished on the way to charge" << std::endl;
    }
    return index;
}

/*
 * When the vehicle deviate from its original route, he might miss some drives that are with passenger. This is an important
 * indicator to the performance.
 *
 * Besides, this function also helps to change the customer tag into 0.
 */
void ReProduce_Taxi::getMissedDrive ( int index){

    switch (chargesimulationalgorithm){
        case(1):{
            vector<bool> custag = myhistory.getCustags();
            int further = 0;
            if(custag[mycurrentstateinhistory+index-1] == 1){  // Why -1? this is because that we can catch the point at mycurrentstateinhistory + index.
                while(((mycurrentstateinhistory+index +further) < custag.size()) & custag[mycurrentstateinhistory+index +further]){
                    further++;
                }
            }
            for(int i = 0 ; i<=index+further-1;i++){

                if(custag[mycurrentstateinhistory+i] == 1){
                    myhistory.getCustags()[mycurrentstateinhistory+i] = false;
                    int j = 1;
                    while(((i+j+mycurrentstateinhistory) < custag.size()) & custag[mycurrentstateinhistory+i+j]){
                        myhistory.getCustags()[mycurrentstateinhistory+i+j] = false;
                        j++;
                    }

                    int beginindex = mycurrentstateinhistory +i;
                    int endindex = mycurrentstateinhistory +i+j;
                    if(endindex >=custag.size()){
                        endindex = int(custag.size()-1);
                    }
                    if(beginindex != endindex){
                        array<int,2> time = {myhistory.getTime()[beginindex], myhistory.getTime()[endindex]};  // The beginning and end plantime of the taxi ride.
                        double length = std::accumulate(myhistory.getDistance().begin() +beginindex,
                                                        myhistory.getDistance().begin() +endindex , 0.0); // The length of the ride demand
                        array<double,2> sPosition = myhistory.getPoss()[beginindex]; // start planposition of the taxi ride
                        array<double,2> ePosition = myhistory.getPoss()[endindex];
                        Taxi_Demand ataxidemand =Taxi_Demand(time, length, sPosition, ePosition, false);
                        tdAdd (ataxidemand);
                    }
                    i += j;
                }
            }
        }
        default:
            break;
    }
}

/***********************************************************************************************************************
 * Helper function
 ***********************************************************************************************************************/
/*
 * This function find the charging station in the charging station set by the position of the target charging station.
 * And return the iterator of that charging station in the map.
 */
map<int, Charging_Station>::iterator
ReProduce_Taxi::findTheChargeStaion(Charging_Station_Manager &cfm, array<double, 2> pos) {

    auto chargefacilityiter = cfm.getCfset().begin();

    for(auto iter = cfm.getCfset().begin();iter!=cfm.getCfset().end(); iter++){
        if (iter->second.getPos() == pos){
            chargefacilityiter = iter;
            break;
        }
    }
    return chargefacilityiter;
}

/*
 * In order to avoid to writing the initialization of Plan, put them in a function.
 */
void ReProduce_Taxi::setThePlan(const int & index, const int & nextstate, const double & myenergy, const int & mytime,
        const array<double,2> & position){
    // When mytime ==-999, we know that the taxi is in charging station
    // we dont need mytime and just use index we can find the corresponding value.

    if(mytime == -999){

        deque<int> plantime = {myhistory.getTime()[mycurrentstateinhistory + index]};
        deque<int> plannextstate = {nextstate};
        deque<array<double, 2>> planendposition = {myhistory.getPoss()[mycurrentstateinhistory+index]};
        deque<double> plangainenergy = {myenergy};
        deque<int> plantrackindex = {mycurrentstateinhistory+index};

        Plan newplan(plantime, plannextstate, planendposition, plangainenergy,plantrackindex);

        this->setPlan(newplan);

    }else{

        deque<int> plantime = {min(mytime, this->trackend )};
        deque<int> plannextstate = {nextstate}; // 4 means begin to charge
        deque<array<double, 2>> planendposition = {this->pos}; // the planposition of charge.
        deque<double> plangainenergy = {myenergy};
        deque<int> plantrackindex = {mycurrentstateinhistory + index};

        array<double,2> myarray= {0,0};
        if(position!= myarray){
            planendposition = {position};
        }
        Plan newplan(plantime, plannextstate, planendposition, plangainenergy,plantrackindex);
        this->setPlan(newplan);
    }
}

void ReProduce_Taxi::generateAddTaxiDemand(int index, double totaldistance, bool success) {
    int endindex = mycurrentstateinhistory+index;
    if(endindex >= this->getMyhistory().getCustags().size()){
        endindex = static_cast<int>(this->getMyhistory().getCustags().size() - 1);
    }
    if(mycurrentstateinhistory != endindex){
        array<int, 2> time_arr = {myhistory.getTime()[mycurrentstateinhistory], myhistory.getTime()[endindex]}; // add the start and end plantime of the drive
        array<double, 2> sPosition = this->pos;
        array<double, 2> ePosition = myhistory.getPoss()[endindex];
        Taxi_Demand oneTD = Taxi_Demand(time_arr, totaldistance, sPosition, ePosition, success);
        tdAdd(oneTD);
    }
    if(!success){
        replace(myhistory.getCustags().begin()+mycurrentstateinhistory, myhistory.getCustags().begin()+endindex,
                true, false);
    }
}

void ReProduce_Taxi::findDemandNumber() {
    int demand = 0;
    for(int i = 0; i < this->getMyhistory().getCustags().size(); i++){

        if(this->getMyhistory().getCustags()[i]){
            int j = 1;
            while(this->getMyhistory().getCustags()[i+j]){
                j++;
            }
            int endindex =i+j;
            if(endindex >= this->getMyhistory().getCustags().size()){
                endindex = static_cast<int>(this->getMyhistory().getCustags().size() - 1);
            }
            if(i!=endindex | i!= (this->getMyhistory().getCustags().size()-1)){
                demand++;
            }
            i+=j;
        }
    }
    this->demandnumber  = demand;
}


void ReProduce_Taxi::findDemandsBeginEnd() {

    for(int i = 0; i < this->getMyhistory().getCustags().size(); i++){

        if(this->getMyhistory().getCustags()[i]){
            int j = 1;
            while(this->getMyhistory().getCustags()[i+j]){
                j++;
            }
            int endindex =i+j;
            if(endindex >= this->getMyhistory().getCustags().size()){
                endindex = static_cast<int>(this->getMyhistory().getCustags().size() - 1);
            }
            if(i!=endindex | i!= (this->getMyhistory().getCustags().size()-1)){
                array<int,2> onebeginend = {this->getMyhistory().getTime()[i],this->getMyhistory().getTime()[endindex]};
                this->demandID.push_back(onebeginend);
            }
            i+=j;
        }
    }
}
/***********************************************************************************************************************
% This allows the vehicle to have a certain inforamtion of future
 * Here it is the future trajectory.
 ***********************************************************************************************************************/


void ReProduce_Taxi::addWaitDuration(int cfid, int duration) {
    if(duration > 0){
        array<int,2> wait = {cfid, duration};
        waitduration.push_back(wait);
    } else{
        printf("Seems unreasonable as the waitduration should be large than 0 \n");
    }
}

/***********************************************************************************************************************
% Tour based charging simulation related methods.
 ***********************************************************************************************************************/

void ReProduce_Taxi::generateTaxiTour() {

    ReProduce_Taxi_tour mytour;
    mytour.generateTourFromTrackingData(this->myhistory);
    this->mytour = mytour;
}


void ReProduce_Taxi::keepRunningBeforeCharging() {
    /* In this function, the vehicle keeps running until the trip that it has to charge.
     *
     * When the while the trip has passenger, the taxi will check if it has minSoC after finishing the trip. If has, finish the trip
     * and check if the leftsoc is > than range anxiety, if not, charge. While the taxi has a leftsoc > range anxiety, but has a
     * leftsoc < minSoC after finishing the trip, then it will miss this trip with passenger. In addition, it will tries
     * to charge direactly rather than wait until its leftSoC is smaller than the range anxiety.
     *
     * When the trip does not have passenger, it will check when it has a leftsoc < range anxiety. and charge
     */

    double leftsoc = this->soc;
    int currenttripindex = this->tripindex;
    bool gotchargeplan = false;
    while(currenttripindex < mytour.getTrips().size()){

        leftsoc -= (mytour.getTrips()[currenttripindex].getTriplength()*this->consumrate)/this->capf;

        if(mytour.getTrips()[currenttripindex].getCustag()){
            if(leftsoc >= this->getMinSoc()){
                mytour.getTrips()[currenttripindex].setServed(true);
                updateTaxiState(currenttripindex);
                if(leftsoc < this->getRa()){
                    setTheChargePlan(mytour.getTrips()[currenttripindex].getBeginendindex()[1]);
                    gotchargeplan = true;
                    break;
                }
            }else{
                setTheChargePlan(mytour.getTrips()[currenttripindex].getBeginendindex()[0]);
                gotchargeplan = true;
                break;
            }
        }else{
            if(leftsoc < this->getRa()){
                int chargeindex = findBeginPositionToCharge(currenttripindex);
                setTheChargePlan(chargeindex);
                gotchargeplan = true;
                break;
            }else{
                updateTaxiState(currenttripindex);
            }
        }
        currenttripindex++;
    }
    if(!gotchargeplan & (currenttripindex==mytour.getTrips().size())){
        setTheChargePlan(mytour.getTrips()[currenttripindex-1].getBeginendindex()[1]);
    }else if(!gotchargeplan & (currenttripindex!=mytour.getTrips().size())){
        cout <<"strenge things happen, check!" << endl;
    }
}

// Think if it is necessary to update Taxi State after each trip.
void ReProduce_Taxi::updateTaxiState(int currenttripindex) {
    this->cus = this->mytour.getTrips()[currenttripindex].getCustag();
    this->time = this->mytour.getTrips()[currenttripindex].getEndtime();
    this->mycurrentstateinhistory = this->mytour.getTrips()[currenttripindex].getBeginendindex()[1];
    this->tripindex +=1;
    this->pos = this->mytour.getTrips()[currenttripindex].getEndposition();
    this->soc -= this->mytour.getTrips()[currenttripindex].getTriplength()*this->consumrate/this->capf;
}

int ReProduce_Taxi::findBeginPositionToCharge(int currenttripindex) {
    // This tries to find the position where the soc will be lower than the range anxiety.

    array<int,2> thistripbeginendindex = mytour.getTrips()[currenttripindex].getBeginendindex();
    double leftsoc = this->soc;

    if(thistripbeginendindex[0] != mycurrentstateinhistory){
        cout << "The begin index is not equal to my current state in history. check the updating of mycurrentstateinhistory!!! " << endl;
    }
    int j;
    for(j = thistripbeginendindex[0]+1; j <=thistripbeginendindex[1]; j++){
        leftsoc -= myhistory.getDistance()[j]*this->consumrate/this->capf;
        // In order to avoid the case that the distance between two points are so large that the leftsoc goes from > soc to less than 0.
        if(leftsoc<=this->ra & leftsoc >=this->minSoc){
            break;
        } else if(leftsoc < this->minSoc) {
            j--;
            break;
        }
    }
    return j;
}

void ReProduce_Taxi::setTheChargePlan(int chargepoint) {
    // If this function is called when the taxi trip has passenger, then the drivedistance should be 0.
    int j =0;
    double drivedistance = 0;
    for(j = this->mycurrentstateinhistory+1; j <= chargepoint; j++){
        drivedistance += myhistory.getDistance()[j];
    }
    setThePlan(chargepoint-mycurrentstateinhistory, 2, -drivedistance * this->consumrate);
}

int ReProduce_Taxi::computetripindex(int futuretime) {

    int futuretripindex = tripindex;

    for(int j = this->tripindex; j < this->mytour.getTrips().size(); j++){
        if((mytour.getTrips()[j].getBegintime() <= futuretime) & (mytour.getTrips()[j].getEndtime() > futuretime)){
            futuretripindex = j;
            break;
        }
    }
    return futuretripindex;
}














