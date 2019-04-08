//
// Created by ZhengchaoWANG on 27.07.18.
//

#include "station_optimization/simulation/taxi/reproduce_taxi/ReProduce_Taxi_General.h"

void ReProduce_Taxi_General::generatePlan(Charging_Station_General_Manager & cfm,  const int & threshold) {

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
                        // To deduce if the driving state has changed or not.
                        if (*(myhistory.getCustags().begin()+mycurrentstateinhistory+i) & ((mycurrentstateinhistory+i) != (myhistory.getCustags().size()-1))) {
                            continue;
                        } else {
                            driveendindex = i;
                            break;
                        }
                    }
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
                        Charge_Choice chargechoice = chargeStationChoice(cfm, this->time);
                        auto timetostation = int(chargechoice.getDistance()/drivespeed);

                        // to avoid the same time problem.
                        if(timetostation==0){
                            timetostation++;
                        }
                        int index = findIndexInFuture(this->time + timetostation);

                        // Initiate the plan.
                        setThePlan(index, 3, -chargechoice.getDistance() * consumrate, min(trackend, this->time + timetostation), chargechoice.getPos());
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
                        int current_empty_slots = 0;
                        int find_empty_charging_pile = false;
                        for(int i=0; i<chargefacilityiter->second.getCap().size(); i++){
                            int this_type_left_slots = chargefacilityiter->second.getCap()[i] - chargefacilityiter->second.getOcupslot()[i];
                            if( this_type_left_slots >= 0){
                                current_empty_slots +=this_type_left_slots;
                            } else {
                                cout << "There is a mistake as the occupied is more than capacity" <<endl;
                                cout << "charge facility id is " << chargefacilityiter->second.getId_() <<
                                     "charge facility installed number is" << chargefacilityiter->second.getCap()[i]
                                     << "occupied number is " << chargefacilityiter->second.getOcupslot()[i] << endl;
                            }
                            if(current_empty_slots > chargefacilityiter->second.getWaitlist().size()){
                                int chargeduration = int((chargeTo - soc) * capf/min(chargefacilityiter->second.getRate()[i], this->getChargerate()));
                                int index = findIndexInFuture( time + chargeduration);
                                setThePlan(index, 5, (chargeTo-soc)*capf, min(trackend, this->time + chargeduration));
                                find_empty_charging_pile = true;
                            }
                        }
                        if(!find_empty_charging_pile){
                            setThePlan(0,4,0,this->time);
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

                        auto chargeduration = int((chargeTo - soc) * capf / min(plan.getPlanchargerate()[0], this->getChargerate()));
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
                        Charge_Choice chargechoice = chargeStationChoice(cfm, this->time);
                        auto timetostation = int(chargechoice.getDistance()/drivespeed);
                        // to avoid the same time problem.
                        if(timetostation==0){
                            timetostation++;
                        }
                        this->currentcharge = One_Charge();
                        this->currentcharge.setWantCharge(chargechoice.getCfID(),this->soc, this->pos, this->time, chargechoice.getPos());
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
                        int current_empty_slots = 0;
                        int find_empty_charging_pile = false;
                        for(int i=0; i<chargefacilityiter->second.getCap().size(); i++){
                            int this_type_left_slots = chargefacilityiter->second.getCap()[i] - chargefacilityiter->second.getOcupslot()[i];
                            if( this_type_left_slots >= 0){
                                current_empty_slots +=this_type_left_slots;
                            } else {
                                cout << "There is a mistake as the occupied is more than capacity" <<endl;
                            }
                            if(current_empty_slots > chargefacilityiter->second.getWaitlist().size()){
                                double charge_rate = min(chargefacilityiter->second.getRate()[i], this->getChargerate());
                                auto chargeduration = int((chargeTo - soc) * capf/charge_rate);
                                int index = findIndexInFuture( time + chargeduration);
                                setThePlan(index, 5, (chargeTo-soc)*capf, min(trackend, this->time + chargeduration),  {0,0}, charge_rate, i);
                                this->currentcharge.setBeginCharge(this->time, charge_rate, i, chargefacilityiter->second.getService_price()[i]*chargeduration, chargeduration, chargeTo);
                                find_empty_charging_pile = true;
                                break;
                            }
                        }
                        if(!find_empty_charging_pile){
                            setThePlan(0,4,0,this->time);
                        }
                    }else{
                        setThePlan(0, 6, 0, trackend );
                    }
                    break;
                }
                case(4):{
                    if(this->time < trackend ){
                        auto chargefacilityiter = findTheChargeStaion(cfm,plan.getPlanposition()[0]);
                        double used_charge_rate = min(plan.getPlanchargerate()[0], this->getChargerate());
                        auto chargeduration = int((chargeTo - soc) * capf /used_charge_rate );
                        int index = findIndexInFuture(time + chargeduration);
                        setThePlan(index, 5, (chargeTo - soc) * capf, min(trackend, this->time + chargeduration), {0,0}, used_charge_rate, plan.getPlanChargeType()[0]);
                        this->currentcharge.setBeginCharge(this->time, used_charge_rate, plan.getPlanChargeType()[0],
                                                           chargefacilityiter->second.getService_price()[plan.getPlanChargeType()[0]]*chargeduration , chargeduration, chargeTo);
                    } else{
                        setThePlan(0, 6, 0, trackend);
                    }
                    break;
                }
                case(5):{
                    if(this->time < trackend){
                        // find the trip where the vehicle could catch up with the first point.
                        int i;
                        for(i = tripindex; i < mytour.getTrips().size(); i++){
                            double drivedistance = twoPointsDistance(mytour.getTrips()[i].getBeginposition(), pos);
                            if(drivedistance < (mytour.getTrips()[i].getBegintime() - this->time)*drivespeed){
                                if(mytour.getTrips()[i].getBegintime() == this->time) {
                                    setThePlan(mytour.getTrips()[i].getBeginendindex()[0] - mycurrentstateinhistory,
                                               mytour.getTrips()[i].getCustag(), -drivedistance * consumrate, this->time +1);
                                } else {
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
                        setThePlan(0, 6, 0, trackend);
                    }
                    break;
                }
                default:break;
            }
            break;
        }
        default:
            break;
    }
}
/***********************************************************************************************************************
// After getting a plan, the vehicle moves according to his plan and change the states of vehicle and the Charging station according
// to needs.
***********************************************************************************************************************/
void ReProduce_Taxi_General::move(Charging_Station_General_Manager & cfm) {

    /*
     * Take out the plan generated in the generatePlan() method.
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
                        chargefacilityiter->second.departOccupied(this->time,currentcharge.getChargeType());
                        chargefacilityiter->second.deleteChargeEnd(this->time,currentcharge.getChargeType());
                        getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                    }
                    break;
                }
                case(1):{
                    if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time, currentcharge.getChargeType());
                        chargefacilityiter->second.deleteChargeEnd(this->time,currentcharge.getChargeType());
                        getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                    }
                    break;
                }
                case(3):{
                    getMissedDrive (plan.getPlantrackindex()[0]- mycurrentstateinhistory);
                    break;
                }
                case(4):{
                    chargefacilityiter->second.addChargingArrival(this->time);
                    chargefacilityiter->second.addWaitList(this->id_, this->time);
                    break;
                }
                case(5):{
                    chargefacilityiter->second.addOccupied(this->time, currentcharge.getChargeType());
                    getMissedDrive (plan.getPlantrackindex()[0]-mycurrentstateinhistory);
                    chargefacilityiter->second.addChargeEnd(this->plan.getPlantime()[0],currentcharge.getChargeType());
                    if(cus ==4){
                        chargefacilityiter->second.departWaitList(this->id_, this->time);
                    } else {
                        chargefacilityiter->second.addChargingArrival(this->time);
                    }
                    break;
                }
                case(6):{
                    if(cus==4){
                        chargefacilityiter->second.departWaitList(this->id_, this->time);
                    }else if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time, currentcharge.getChargeType());
                        chargefacilityiter->second.deleteChargeEnd(this->time,currentcharge.getChargeType());
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
                        chargefacilityiter->second.departOccupied(this->time, currentcharge.getChargeType());
                        chargefacilityiter->second.deleteChargeEnd(this->time,currentcharge.getChargeType());
                        taxicharges.push_back(currentcharge);
                    }
                    break;
                }
                case(1):{
                    if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time, currentcharge.getChargeType());
                        chargefacilityiter->second.deleteChargeEnd(this->time,currentcharge.getChargeType());
                        taxicharges.push_back(currentcharge);
                    }
                    break;
                }
                case(4):{
                    chargefacilityiter->second.addChargingArrival(this->time);
                    chargefacilityiter->second.addWaitList(this->id_, this->time);
                    break;
                }
                case(5):{
                    chargefacilityiter->second.addOccupied(this->time, plan.getPlanChargeType()[0]);
                    chargefacilityiter->second.addChargeEnd(this->plan.getPlantime()[0],currentcharge.getChargeType());
                    if(cus ==4){
                        chargefacilityiter->second.departWaitList(this->id_, this->time);
                    } else {
                        chargefacilityiter->second.addChargingArrival(this->time);
                    }
                    break;
                }
                case(6):{
                    currentcharge.stopBeforeFinishCharge(this->time);
                    taxicharges.push_back(currentcharge);
                    if(cus==4){
                        chargefacilityiter->second.departWaitList(this->id_, this->time);
                    }else if(cus==5){
                        chargefacilityiter->second.departOccupied(this->time, currentcharge.getChargeType());
                        chargefacilityiter->second.deleteChargeEnd(this->time,currentcharge.getChargeType());
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
void ReProduce_Taxi_General::tdAdd(Taxi_Demand & newDemand) {

    if(!td.empty()){
        if(td.back().getTime()!= newDemand.getTime()){
            td.push_back(newDemand);
        }
    } else{
        td.push_back(newDemand);
    }
}

/*
 * This is tries to find the corresponding driving trajectory of taxi while he has to go to charge. The returned
 * index is the one whose time is more than needed.

 * However, problem might happen if there is a long stop between index-1 and index. solve it.
 */
int ReProduce_Taxi_General::findIndexInFuture ( int needfoundtime){

    int index;
    for(index = 0; index< myhistory.getTime().size() - mycurrentstateinhistory; index++) {
        if(myhistory.getTime()[index + mycurrentstateinhistory] > needfoundtime){
            break;
        }
    }
    return index;
}

/*
 * When the vehicle deviate from its original route, he might miss some drives that are with passenger. This is an important
 * indicator to the performance.
 *
 * Besides, this function also helps to change the customer tag into 0.
 */
void ReProduce_Taxi_General::getMissedDrive ( int index){

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
 * In order to avoid to writing the initialization of Plan, put them in a function.
 */
void ReProduce_Taxi_General::setThePlan(const int & index, const int & nextstate, const double & myenergy,
                                                     const int & mytime, const array<double,2> & position, const double & charge_rate, const int & chargetype){
    // When mytime ==-999, we know that the taxi is in charging station
    // we dont need mytime and just use index we can find the corresponding value.
    deque<int> plantime, plannextstate, plantrackindex, planchargetype;
    deque<array<double, 2>> planendposition;
    deque<double> plangainenergy, plan_charge_rate;

    plannextstate = {nextstate};
    plangainenergy = {myenergy};
    plantrackindex = {mycurrentstateinhistory + index};

    if(mytime == -999){

        plantime = {myhistory.getTime()[mycurrentstateinhistory + index]};
        planendposition = {myhistory.getPoss()[mycurrentstateinhistory+index]};

    }else{
        plantime = {min(mytime, this->trackend )};
        planendposition = {this->pos}; // the planposition of charge.
        array<double,2> myarray= {0,0};
        if(position!= myarray){
            planendposition = {position};
        }
    }
    if(charge_rate != 0){
        plan_charge_rate = {charge_rate};
        planchargetype = {chargetype};
        Plan newplan(plantime, plannextstate, planendposition, plangainenergy,plantrackindex, plan_charge_rate, planchargetype);
        this->setPlan(newplan);
    }else{
        Plan newplan(plantime, plannextstate, planendposition, plangainenergy,plantrackindex);
        this->setPlan(newplan);
    }
}

void ReProduce_Taxi_General::generateAddTaxiDemand(int index, double totaldistance, bool success) {
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

void ReProduce_Taxi_General::findAllDemandNumber() {
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


void ReProduce_Taxi_General::findAllDemandTrips() {

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
% Tour based charging simulation related methods.
 ***********************************************************************************************************************/

void ReProduce_Taxi_General::generateTaxiTour() {

    ReProduce_Taxi_tour mytour;
    if(longstopseperate){
        mytour.generateStopTourFromTrackingData(this->myhistory);
    } else {
        mytour.generateTourFromTrackingData(this->myhistory);
    }


    this->mytour = mytour;
}


void ReProduce_Taxi_General::keepRunningBeforeCharging() {
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
void ReProduce_Taxi_General::updateTaxiState(int currenttripindex) {
    this->cus = this->mytour.getTrips()[currenttripindex].getCustag();
    this->time = this->mytour.getTrips()[currenttripindex].getEndtime();
    this->mycurrentstateinhistory = this->mytour.getTrips()[currenttripindex].getBeginendindex()[1];
    this->tripindex +=1;
    this->pos = this->mytour.getTrips()[currenttripindex].getEndposition();
    this->soc -= this->mytour.getTrips()[currenttripindex].getTriplength()*this->consumrate/this->capf;
}

int ReProduce_Taxi_General::findBeginPositionToCharge(int currenttripindex) {
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

void ReProduce_Taxi_General::setTheChargePlan(int chargepoint) {
    // If this function is called when the taxi trip has passenger, then the drivedistance should be 0.
    int j =0;
    double drivedistance = 0;
    for(j = this->mycurrentstateinhistory+1; j <= chargepoint; j++){
        drivedistance += myhistory.getDistance()[j];
    }
    setThePlan(chargepoint-mycurrentstateinhistory, 2, -drivedistance * this->consumrate);
}

int ReProduce_Taxi_General::computetripindex(int futuretime) {

    int futuretripindex = tripindex;

    for(int j = this->tripindex; j < this->mytour.getTrips().size(); j++){
        if((mytour.getTrips()[j].getBegintime() <= futuretime) & (mytour.getTrips()[j].getEndtime() > futuretime)){
            futuretripindex = j;
            break;
        }
    }
    return futuretripindex;
}

void ReProduce_Taxi_General::addWaitDuration(int station_id, int wait_duration) {
    this->waitduration.push_back({station_id,wait_duration});
}