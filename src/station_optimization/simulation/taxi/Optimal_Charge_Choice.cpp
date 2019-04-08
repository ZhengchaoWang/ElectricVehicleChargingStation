/*
//
// Created by ZhengchaoWANG on 04.07.18.
//

#include "station_optimization/simulation/taxi/Optimal_Charge_Choice.h"


void Optimal_Charge_Choice::getOptimalChargeChoice() {

    int tripsize = int(tripdistances.size());  // denote as n.
    int stationsize = int(cftocfdistance.size()); // denote as m.


    */
/*
     * tp, tn, ec, sh (1,...,n)
     * ap, an, df, dt [(1,1), (1,2),..., (1,m), (2,1), (2,2),..., (2,m),.......,(n,1),(n,2),...,(n,m)]
     * bp,bn {[(1,1,1), (1,1,2),..., (1,1,m), (1,2,1), (1,2,2),..., (1,2,m),.......,(1,m,1),(1,m,2),...,(1,m,m)],.....,
     *               [(n,1,1), (n,1,2),..., (n,1,m), (n,2,1), (n,2,2),..., (n,2,m),.......,(n,m,1),(n,m,2),...,(n,m,m)]}
     * s(0,...,n), s(0) is given
     * x(0,...,n+1), x(0), x(n+1) = 1;
     * y([(0,1), (0,2),..., (0,m),(1,1), (1,2),..., (1,m), (2,1), (2,2),..., (2,m),.......,(n+1,1),(n+1,2),...,(n+1,m)):  y(0,1), y(0,2), ... , y(0,m), y(n+1,1) ,..., y(n+1,m) = 0
     *//*



    // cplex environment, declare only once time, release it at the end of program.
    IloEnv env;
    // cplex model
    IloModel model(env);
    // decision variables
    IloNumVarArray x(env), y(env), ap(env), an(env), bp(env), bn(env), tp(env), tn(env), ec(env), s(env), sh(env),
            df(env),dt(env); // p represents positive, while n represent negative.

    // Set the variables.
    for (int i = 0; i < tripsize; i++) {
        // tripsize elements variables

        stringstream name;
        // tp/tn/ec chargable energy, each element corresponding to one trip
        name << "tp(" << i+1 << ")";
        tp.add(IloFloatVar(env, 0, batterycapacity, name.str().c_str()));
        stringstream name1;
        name1 << "tn(" << i+1 << ")";
        tn.add(IloFloatVar(env, 0, batterycapacity , name1.str().c_str()));
        stringstream name2;
        name2 << "ec(" << i+1 << ")";
        ec.add(IloFloatVar(env, 0, batterycapacity, name2.str().c_str()));
        // sh: the energy left when arrive at the charging station.
        stringstream name3;
        name3 << "sh(" << i+1<< ")";
        sh.add(IloFloatVar(env, 0, batterycapacity, name3.str().c_str()));

        for(int j = 0; j < stationsize; j++){
            // ap/an auxilliary variables = y_{ij}x_{i+1}/y_{ij}x_{i-1}
            stringstream namej;
            namej << "ap(" << i+1 << j+1 << ")";
            ap.add(IloBoolVar(env,0,1, namej.str().c_str()));
            stringstream namej1;
            namej1 << "an(" << i+1 << j+1 << ")";
            an.add(IloBoolVar(env,0,1, namej1.str().c_str()));
            // df/dt, distances of point to charging staiton and stations to stations. df start from -1, but use index 0 to replace
            stringstream namej2;
            namej2 << "df(" << i+1 << j+1 << ")";
            df.add(IloFloatVar(env,0.0,1000.0, namej2.str().c_str()));
            stringstream namej3;
            namej3 << "dt(" << i+1 << j+1 << ")";
            dt.add(IloFloatVar(env,0.0,1000.0, namej3.str().c_str()));
            // bp/bn auxilliary variables = y_{ij}y_{i+1,k}/y_{ij}y_{i-1,k}
            for(int k = 0; k < stationsize; k++){
                stringstream namek;
                namek << "bp(" << i+1 << j+1 << k+1 << ")";
                bp.add(IloBoolVar(env, 0, 1, namek.str().c_str()));
                stringstream namek1;
                namek1 << "bn(" << i+1 << j+1 << k+1 << ")";
                bn.add(IloBoolVar(env, 0, 1, namek1.str().c_str()));
            }
        }
    }

    for(int i = 0; i< tripsize+1; i++){
        // tripsize +1 elements.
        stringstream name;
        name << "s(" << i << ")";
        s.add(IloFloatVar(env, 0, batterycapacity, name.str().c_str()));
    }

    for(int i = 0; i < tripsize+2; i++){
        stringstream name;
        // variables x0 x1,..., xn.
        name << "x(" << i << ")";
        x.add(IloBoolVar(env, 0, 1, name.str().c_str()));
        for(int j = 0; j <stationsize; j++){
            stringstream namej;
            namej << "y(" << i << j+1 << ")";
            y.add(IloBoolVar(env, 0, 1, namej.str().c_str()));
        }
    }

    */
/***************************************************************************************************************
     * constraints
     * *************************************************************************************************************//*

    for (int i =0; i < tripsize;i++){
        // s_{i+1} >= 0
        IloExpr constraint1(env);
        constraint1 += (s[i] + ec[i] - consumerate*tripdistances[i]*x[i+1]);  // x[i+1] is because x[0] is imagenary.
        for (int j = 0; j < stationsize ; j++){
            constraint1 -= consumerate*(df[i*stationsize+j] + dt[i*stationsize +j]);  // be careful about the correcting index.
        }
        model.add(constraint1 >= 0);

        //dynamic of state of charge constraints
        //s_{i+1} = s_{i}+E_{i} - consume_{i}  ;
        IloExpr constraint2(env);
        constraint2 += (-s[i+1]+ s[i] + ec[i] - consumerate*tripdistances[i]*x[i+1]); // same as above.
        for (int j = 0; j < stationsize ; j++){
            constraint2 -= consumerate*(df[i*stationsize+j] + dt[i*stationsize +j]);
        }
        model.add(constraint2 == 0);

        // s_{i+1}h >=0;
        IloExpr constraint3(env);
        constraint3 += s[i];
        for (int j = 0; j < stationsize ; j++){
            constraint3 -= consumerate*(df[i*stationsize+j]);
        }
        model.add(constraint3 >= 0);

        for(int j = 0; j < stationsize; j++){
            // tdf_{i-1,j} = sum_{k} lcc_{kj}y_{i-1,k}y_{i,j} + lpc_{i-1,j} x_{i-1} y_{ij} Here, we have a bug.
            IloExpr constraint(env);
            constraint += df[i*stationsize + j];
            for (int k = 0; k < stationsize ; k++){
                constraint -= cftocfdistance[k][j] * bn[ i*pow(stationsize, 2) + j*stationsize + k];
            }
            constraint -= pointtocfdistance[i][j] * an[i*stationsize+j];
            model.add(constraint == 0);

            // tdt_{j,i} = sum_{k} lcc_{kj}y_{i,j}y_{i+1,k} + lpc_{i,j} x_{i+1} y_{ij}
            IloExpr constraintn(env);
            constraintn += dt[i*stationsize + j];
            for (int k = 0; k < stationsize ; k++){
                constraintn -= cftocfdistance[j][k] * bp[i*pow(stationsize, 2) + j*stationsize + k]; // double check if these bn, an, bp, ap are wrong.
            }
            constraintn -= pointtocfdistance[i+1][j] * ap[i*stationsize+j];
            model.add(constraintn == 0);
        }

        //E_{i} <= t_{i}^{+} + t_{i}^{-}};
        IloExpr constraint4(env);
        constraint4 += (-ec[i] + tn[i] + tp[i]);
        model.add(constraint4 >= 0);

        //E_{i} <= t_{i}^{+} + t_{i}^{-}} + ... ;
        IloExpr constraint5(env);
        constraint5 +=(tp[i] + tn[i] - ec[i]);
        for (int j = 0; j < stationsize; j++){
            constraint5 += usedchargerate[j]*(tripduration[i]*y[(i+1) * stationsize+j] - pointtocfdistance[i+1][j]*ap[i*stationsize+j]/drivespeed -pointtocfdistance[i][j]*an[i*stationsize+j]/drivespeed - waittimeatfacilities[i][j]*y[(i+1)*stationsize+j]);
            for(int k = 0; k<stationsize; k++){
                constraint5 -= usedchargerate[j] * cftocfdistance[j][k]*(bn[i*pow(stationsize, 2)+j*stationsize+k]+ bp[i*pow(stationsize, 2)+j*stationsize+k])/drivespeed;
            }
        }
        model.add(constraint5 >= 0);

        // t_{i}^{+} >= ...
        IloExpr constraint6(env);
        for (int j = 0; j < stationsize; j++){
            constraint6 += usedchargerate[j]*(tripduration[i]*y[(i+1)*stationsize+j] - pointtocfdistance[i+1][j]*ap[i*stationsize+j]/drivespeed -pointtocfdistance[i][j]*an[i*stationsize+j]/drivespeed - waittimeatfacilities[i][j]*y[(i+1)*stationsize+j]);
            for(int k = 0; k<stationsize; k++){
                constraint6 -= usedchargerate[j] *cftocfdistance[j][k]*(bn[i*pow(stationsize, 2)+j*stationsize+k]+ bp[i*pow(stationsize, 2)+j*stationsize+k])/drivespeed;
            }
        }
        constraint6 -= tp[i];
        model.add(constraint6 <= 0);

        // t_{i}^{-} >= ...
        IloExpr constraint7(env);
        for (int j = 0; j < stationsize; j++){
            constraint7 += usedchargerate[j]*(tripduration[i]*y[(i+1)*stationsize+j] - pointtocfdistance[i+1][j]*ap[i*stationsize+j]/drivespeed -pointtocfdistance[i][j]*an[i*stationsize+j]/drivespeed - waittimeatfacilities[i][j]*y[(i+1)*stationsize+j]);
            for(int k = 0; k<stationsize; k++){
                constraint7 -= usedchargerate[j] *cftocfdistance[j][k]*(bn[i*pow(stationsize, 2)+j*stationsize+k]+ bp[i*pow(stationsize, 2)+j*stationsize+k])/drivespeed;
            }
        }
        constraint7 -= tn[i];
        model.add(constraint7 >= 0);

        // Ei <= cap - s_{i+1}h
        IloExpr constraint8(env);
        constraint8 += (sh[i] + ec[i] - batterycapacity);
        model.add(constraint8 <= 0);
        //a
*/
/*        IloExpr constraint9(env);
        constraint9 += sh[i];
        constraint9 += ec[i];
        constraint9 -= batterycapacity;
        model.add(constraint9 <= 0);*//*


        for (int j = 0; j < stationsize; j++){
            // a_{ij}^{-} <= y_{ij};
            IloExpr constraint10(env);
            constraint10 += (an[i*stationsize + j] -y[(i+1)*stationsize+j] );
            model.add(constraint10 <= 0);

            // a_{ij}^{-} <= x_{i-1}
            IloExpr constraint11(env);
            constraint11 += (an[i*stationsize + j] + x[i]);
            model.add(constraint11 <= 0);

            // a_{ij}^{-} >= y_{ij}+x_{i-1} - 1
            IloExpr constraint12(env);
            constraint12 += (ap[i*stationsize + j]-x[i] - y[(i+1)*stationsize+j] + 1);
            model.add(constraint12 >= 0);

            // a_{ij}^{+} <= y_{ij};
            IloExpr constraint13(env);
            constraint13 += (ap[i*stationsize + j]-y[(i+1)*stationsize+j]);
            model.add(constraint13 <= 0);

            // a_{ij}^{+} <= x_{i+1}
            IloExpr constraint14(env);
            constraint14 += (ap[i*stationsize + j] -x[i+2]);
            model.add(constraint14 <= 0);

            // a_{ij}^{+} >= y_{ij}+x_{i+1} - 1
            IloExpr constraint15(env);
            constraint15 += (ap[i*stationsize + j]-x[i+2] - y[(i+1)*stationsize+j] + 1);
            model.add(constraint15 >= 0);

            for(int k = 0; k < stationsize; k++){
                // b_{ijk}^{-} <= y_{i-1,j};
                IloExpr constraint16(env);
                constraint16 += (bn[i*pow(stationsize, 2) + j*stationsize + k]- y[i*stationsize+k]);
                model.add(constraint16 <= 0);

                // b_{ijk}^{-} <= y_{i,k}
                IloExpr constraint17(env);
                constraint17 += bn[i*pow(stationsize, 2) + j*stationsize + k] - y[(i+1)*stationsize + j];
                model.add(constraint17 <= 0);

                // b_{ijk}^{-} >= y_{i-1, j}+y_{i,k} - 1
                IloExpr constraint18(env);
                constraint18 += (1+ bn[i*pow(stationsize, 2) + j*stationsize + k] - y[i*stationsize+k] - y[(i+1)*stationsize + j]) ;
                model.add(constraint18 >= 0);

                // b_{ijk}^{+} <= y_{i,j};
                IloExpr constraint19(env);
                constraint19 += (bp[i*pow(stationsize, 2) + j*stationsize + k] - y[(1+i)*stationsize+j]);
                model.add(constraint19 <= 0);

                // b_{ijk}^{+} <= y_{i+1,k}
                IloExpr constraint20(env);
                constraint20 += (bp[i*pow(stationsize, 2) + j*stationsize + k] - y[(i+2)*stationsize + k]);
                model.add(constraint20 <= 0);

                // b_{ijk}^{+} >= y_{i, j}+y_{i+1,k} - 1
                IloExpr constraint21(env);
                constraint21 += (1+bp[i*pow(stationsize, 2) + j*stationsize + k]-y[(i+2)*stationsize + k] - y[(1+i)*stationsize+j]);
                model.add(constraint21 >= 0);
            }
        }
        // sum_{j} y_{ij} = 1- x_{i}
        IloExpr constraint22(env);
        constraint22 += (1-x[i]);
        for(int j = 0; j< stationsize; j++){
            constraint22 -= y[i*stationsize+j];
        }
        model.add(constraint22 == 0);
    }
    // boundry conditions
    IloExpr constraint23(env);
    constraint23 += x[0];
    model.add(constraint23 == 1);

    IloExpr constraint24(env);
    constraint24 += x[tripsize+1];
    model.add(constraint24 == 1);

    IloExpr constraintinitials0(env);
    constraintinitials0 += s[0];
    model.add(constraintinitials0 == initialenergy);

    for(int j = 0; j<stationsize; j++){
        IloExpr constraint25(env);
        constraint25 += y[j];
        model.add(constraint25 == 0);

        IloExpr constraint26(env);
        constraint26 += y[(tripsize+1)*stationsize + j];
        model.add(constraint26 == 0);
    }
    // function objective
    // max sum_{i} A_{i} + B_{i} x_{i} + beta_{i} x_{i} - alpha (t_{i}^{+} + t_{i}^{-})
    IloExpr obj(env);
    for (int i = 0; i < tripsize; i++) {
        obj += (tripfixreward[i] * x[i+1] + tripdistancereward[i] * x[i+1] +  emptytripreward[i] * x[i+1] - alphapenalty * (tp[i] + tn[i]));
    }
    model.add(IloMaximize(env, obj));

    //define a solver for the constructed model.
    IloCplex solver(model);

    // set the output as stdout
    solver.setOut(std::cout);

    // use one single thread.
    solver.setParam(IloCplex::Threads, 1);
    // solve until 10s.
    // solver.setParam(IloCplex::Param::TimeLimit, 10);

    bool thisisMIP = static_cast<bool>(solver.isMIP());

    // now , solve the model
    bool solved = static_cast<bool>(solver.solve());
    cout << solved << endl;

    solver.getCplexStatus();
    cout << solver.getCplexStatus()<< endl;
    // export the model.
    solver.exportModel("model.lp");

    // export the solution.
    solver.writeSolution("sol.sl");

    // get the function objective value.
    double objective_value = solver.getObjValue();

    //get the decision variables values.
    IloNumArray value_variables(env);
    solver.getValues(value_variables, x);

    //release all the allocated resources
    model.end();
    solver.end();
    obj.end();
}

void Optimal_Charge_Choice::intializeFromData(ReProduce_Taxi &mytaxi, Charging_Station_Manager & chargefacilities) {

    this->drivespeed = mytaxi.getDrivespeed();
    this->consumerate = mytaxi.getConsumrate();
    this->batterycapacity = mytaxi.getCapf();
    this->initialenergy = mytaxi.getSoc() * mytaxi.getCapf();

    vector<ReProduce_Taxi_Trip> mytrips = mytaxi.getMytour().getTrips();

    int starttripindex = 0; // We need to settle this value.
    int endtripindex = starttripindex;

    for(endtripindex = starttripindex; endtripindex < mytrips.size(); endtripindex++){
        if(mytrips[endtripindex].getBegintime() < mytrips[starttripindex].getBegintime()+foresight){
            this->tripdistances.push_back(mytrips[endtripindex].getTriplength());
            this->pointtocfdistance.push_back(
                    pointDistancesToChargingFacilities(mytrips[endtripindex].getBeginposition(), chargefacilities));
            this->tripcustags.push_back(mytrips[endtripindex].getCustag());
            this->tripduration.push_back(mytrips[endtripindex].getEndtime()-mytrips[endtripindex].getBegintime());

            this->tripfixreward.push_back(mytrips[endtripindex].getCustag()*fixreward);
            this->tripdistancereward.push_back(max(0.0, mytrips[endtripindex].getCustag()*(mytrips[endtripindex].getTriplength()-3)*driveprice));
            this->emptytripreward.push_back((1-mytrips[endtripindex].getCustag())*mytrips[endtripindex].getTriplength()*emptyprice);
        } else{
            break;
        }
    }
    this->pointtocfdistance.push_back(
            pointDistancesToChargingFacilities(mytrips[endtripindex - 1].getEndposition(), chargefacilities));
    initializeWithTripSize(endtripindex-starttripindex, chargefacilities.getCfset().size());

    for(auto iter = chargefacilities.getCfset().begin(); iter!= chargefacilities.getCfset().end(); iter++){
        this->usedchargerate.push_back(min(mytaxi.getChargerate(),iter->second.getRate()));
    }


}

void Optimal_Charge_Choice::initializeWithTripSize(const unsigned long triptripsize, const unsigned long chargestationtripsize) {
    this->tripiscarriedout = vector<bool> (triptripsize,true);
    this->chargefacilityID = vector<int> (triptripsize,-1);
    this->chargedenergy = vector<double> (triptripsize,0.0);
}

vector<double> Optimal_Charge_Choice::pointDistancesToChargingFacilities(array<double, 2> point,
                                                                         Charging_Station_Manager &chargefacilities) {

    vector<double> distances;
    for(auto iter = chargefacilities.getCfset().begin(); iter != chargefacilities.getCfset().end(); iter++){
        double distance = twoPointsDistance(point, iter->second.getPos());
        distances.push_back(distance);
    }
    return distances;
}

/////////////////////////////////////////// Helper math function.
double Optimal_Charge_Choice::twoPointsDistance(array<double, 2> pos1, array<double, 2> pos2) {

    double dlat = (pos2[0]-pos1[0]) * M_PI/180;
    double dlon = (pos2[1]-pos1[1]) * M_PI/180;
    double a = pow(sin(dlat/2),2) + cos(pos1[0]*M_PI/180)*cos(pos2[0]*M_PI/180)*pow(sin(dlon/2),2);
    double distance = 2*atan2(sqrt(a),sqrt(1.0-a)) * 6378.135;
    return distance;
}

void Optimal_Charge_Choice::computeReward() {
    for(int i = 0; i < tripdistances.size(); i++){
        this->tripfixreward.push_back(fixreward*tripcustags[i]);
        this->tripdistancereward.push_back(max(0.0,(tripdistances[i]-3)*driveprice*tripcustags[i]));
        this->emptytripreward.push_back((1-tripcustags[i])*tripdistances[i]*emptyprice);
    }
}
/////////////////////////////////////////// Setters and Getters.
const vector<bool> &Optimal_Charge_Choice::getTripiscarriedout() const {
    return tripiscarriedout;
}

void Optimal_Charge_Choice::setTripiscarriedout(const vector<bool> &tripiscarriedout) {
    Optimal_Charge_Choice::tripiscarriedout = tripiscarriedout;
}

const vector<int> &Optimal_Charge_Choice::getChargefacilityID() const {
    return chargefacilityID;
}

void Optimal_Charge_Choice::setChargefacilityID(const vector<int> &chargefacilityID) {
    Optimal_Charge_Choice::chargefacilityID = chargefacilityID;
}

const vector<double> &Optimal_Charge_Choice::getTripdistances() const {
    return tripdistances;
}

void Optimal_Charge_Choice::setTripdistances(const vector<double> &tripdistances) {
    Optimal_Charge_Choice::tripdistances = tripdistances;
}

const vector<vector<double>> &Optimal_Charge_Choice::getPointtocfdistance() const {
    return pointtocfdistance;
}

void Optimal_Charge_Choice::setPointtocfdistance(const vector<vector<double>> &pointtocfdistance) {
    Optimal_Charge_Choice::pointtocfdistance = pointtocfdistance;
}

const vector<vector<double>> &Optimal_Charge_Choice::getCftocfdistance() const {
    return cftocfdistance;
}

void Optimal_Charge_Choice::setCftocfdistance(const vector<vector<double>> &cftocfdistance) {
    Optimal_Charge_Choice::cftocfdistance = cftocfdistance;
}

const vector<vector<int>> &Optimal_Charge_Choice::getWaittimeatfacilities() const {
    return waittimeatfacilities;
}

void Optimal_Charge_Choice::setWaittimeatfacilities(const vector<vector<int>> &waittimeatfacilities) {
    Optimal_Charge_Choice::waittimeatfacilities = waittimeatfacilities;
}

const vector<double> &Optimal_Charge_Choice::getChargedenergy() const {
    return chargedenergy;
}

void Optimal_Charge_Choice::setChargedenergy(const vector<double> &chargedenergy) {
    Optimal_Charge_Choice::chargedenergy = chargedenergy;
}

const vector<int> &Optimal_Charge_Choice::getTripfixreward() const {
    return tripfixreward;
}

void Optimal_Charge_Choice::setTripfixreward(const vector<int> &tripfixreward) {
    Optimal_Charge_Choice::tripfixreward = tripfixreward;
}

const vector<double> &Optimal_Charge_Choice::getTripdistancereward() const {
    return tripdistancereward;
}

void Optimal_Charge_Choice::setTripdistancereward(const vector<double> &tripdistancereward) {
    Optimal_Charge_Choice::tripdistancereward = tripdistancereward;
}

const vector<double> &Optimal_Charge_Choice::getEmptytripreward() const {
    return emptytripreward;
}

void Optimal_Charge_Choice::setEmptytripreward(const vector<double> &emptytripreward) {
    Optimal_Charge_Choice::emptytripreward = emptytripreward;
}

double Optimal_Charge_Choice::getAlphapenalty() const {
    return alphapenalty;
}

void Optimal_Charge_Choice::setAlphapenalty(double alphapenalty) {
    Optimal_Charge_Choice::alphapenalty = alphapenalty;
}

const vector<int> &Optimal_Charge_Choice::getTripduration() const {
    return tripduration;
}

void Optimal_Charge_Choice::setTripduration(const vector<int> &tripduration) {
    Optimal_Charge_Choice::tripduration = tripduration;
}

const vector<bool> &Optimal_Charge_Choice::getTripcustags() const {
    return tripcustags;
}

void Optimal_Charge_Choice::setTripcustags(const vector<bool> &tripcustags) {
    Optimal_Charge_Choice::tripcustags = tripcustags;
}

const vector<double> &Optimal_Charge_Choice::getUsedchargerate() const {
    return usedchargerate;
}

void Optimal_Charge_Choice::setUsedchargerate(const vector<double> &usedchargerate) {
    Optimal_Charge_Choice::usedchargerate = usedchargerate;
}

double Optimal_Charge_Choice::getDrivespeed() const {
    return drivespeed;
}

void Optimal_Charge_Choice::setDrivespeed(double drivespeed) {
    Optimal_Charge_Choice::drivespeed = drivespeed;
}

double Optimal_Charge_Choice::getConsumerate() const {
    return consumerate;
}

void Optimal_Charge_Choice::setConsumerate(double consumerate) {
    Optimal_Charge_Choice::consumerate = consumerate;
}

double Optimal_Charge_Choice::getBatterycapacity() const {
    return batterycapacity;
}

void Optimal_Charge_Choice::setBatterycapacity(double batterycapacity) {
    Optimal_Charge_Choice::batterycapacity = batterycapacity;
}

double Optimal_Charge_Choice::getInitialenergy() const {
    return initialenergy;
}

void Optimal_Charge_Choice::setInitialenergy(double initialenergy) {
    Optimal_Charge_Choice::initialenergy = initialenergy;
}

double Optimal_Charge_Choice::getFixreward() const {
    return fixreward;
}

void Optimal_Charge_Choice::setFixreward(double fixreward) {
    Optimal_Charge_Choice::fixreward = fixreward;
}

double Optimal_Charge_Choice::getDriveprice() const {
    return driveprice;
}

void Optimal_Charge_Choice::setDriveprice(double driveprice) {
    Optimal_Charge_Choice::driveprice = driveprice;
}

double Optimal_Charge_Choice::getEmptyprice() const {
    return emptyprice;
}

void Optimal_Charge_Choice::setEmptyprice(double emptyprice) {
    Optimal_Charge_Choice::emptyprice = emptyprice;
}

int Optimal_Charge_Choice::getForesight() const {
    return foresight;
}

void Optimal_Charge_Choice::setForesight(int foresight) {
    Optimal_Charge_Choice::foresight = foresight;
}



*/
