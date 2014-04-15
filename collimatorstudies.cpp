//  collimatorstudies.cpp
//  Tested on OSX 10.9 & SLC 6.5 (09/01/2014)
//
//  Tool for automated Collimator simulations, uses SixTrack
//
//  Created by Daniel Spitzbart 2013/2014
//  daniel.spitzbart@cern.ch
//
//  github.com/danbarto/collimatorsim

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <math.h>
#include <string.h> //necessary for compiling in SLC

using namespace std;

void welcomescreen(){
    cout << endl;
    cout << "##########################################" << endl;
    cout << "## AUTOMATED COLLIMATOR SIMULATION TOOL ##" << endl;
    cout << "##########################################" << endl << endl << endl;
}

int checkfile(char filename[100]){
    cout << "Checking " << filename << endl;
    ifstream check;
    check.open(filename);
    if(check==0){
        check.close();
        cout << "Could not find " << filename << ", stopping..." << endl;
        return 0;
    }
    else{
        check.close();
        cout << "File " << filename << " found" << endl;
        return 1;
    }
}

class allcolls{
private:
    string collname, material;
    int collnumber, type, absorbedParticles; //type 0=no collimator, 1=H, 2=V, 3=H+V
    float s_center, s_front, s_back, length, halfGap;
public:
    allcolls(){
        collname="";material="";collnumber=0;type=0;s_center=0.;length=0.;s_front=s_center-length/2.;s_back=s_center+length/2.;halfGap=0.;absorbedParticles=0;
    }
    allcolls(string name, string mat, int number, int t, float pos, float len, float op){
        collname=name;material=mat;collnumber=number;type=t;s_center=pos;length=len;s_front=pos-len/2.;s_back=pos-len/2.;halfGap=op;absorbedParticles=0;
    }
    allcolls(string name, int num, float pos){
        collname=name;material="";collnumber=num;type=0;s_center=pos;length=0.;s_front=0.;s_back=0.;halfGap=0.;absorbedParticles=0;
    }
    
    int absorbedInColl(double spos){
        if(spos>=s_front-0.01 && spos <=s_back+0.01){
            absorbedParticles++;
            return collnumber;
        }
        else return 0;
    }
    
    int findColl(double givPosLast, double givPosAct){ //check if BOTH positions are in a collimator area (safety check, should be true)
        if(givPosLast>=s_front-0.01 && givPosLast<=s_back+0.01 && givPosAct>=s_front-0.01 && givPosAct<=s_back+0.01) return 1;
        else return 0;
    }
    int isInRightArea(double aperture, double lX, double aX, double lY, double aY){
        float ipApCol, angleR1, angleR2,anglePart;
        ipApCol=sqrt(aperture*aperture-halfGap*halfGap);
        angleR1=atan((ipApCol-lX)/(2.*halfGap));
        angleR2=atan((ipApCol+lX)/(2.*halfGap));
        anglePart=atan((aX-lX)/(aY-lY));
        if(anglePart<angleR1&&anglePart>angleR2){
            return collnumber; //absorbed
        }
        else return 0;
    }
    
    void addAbsorbed(){
        absorbedParticles++;
    }
    int getAbsorbed(){
        return absorbedParticles;
    }
    
    string getName(){
        return collname;
    }
    
    float getSfront(){return s_front;}
    float getSback(){return s_back;}
    float getHalfGap(){return halfGap;}
    int getType(){return type;}
    int getCollNumber(){return collnumber;}
    void ResetAbsorbed(){absorbedParticles=0;}
    float getLength(){return length;}
    string getMaterial(){return material;}
    void updateData(string mat, int nT, float len, float op){
        material=mat;type=nT;length=len;s_front=s_center-len/2.;s_back=s_center+len/2.;halfGap=op;
    }
    
    int isAbsorbedOrLost(double lastX, double actX, double lastY, double actY, double apX, double apY){ //check if particle did hit the collimator and if it was absorbed
        if(type==2){
            if(actY*actY>lastY*lastY && actY*actY>halfGap*halfGap){
                return collnumber; //safely absorbed, should never be the case (std ST case)
            }
            else {
                if(isInRightArea(apX,lastX,actX,lastY,actY)==1){
                    return collnumber; //absorbed
                }
                else return 0; //hit aperture -> lost
            }
        }
        else if(type==1){
            if(actX*actX>lastX*lastX && actX*actX>halfGap*halfGap){
                return collnumber;
            }
            else {
                if(isInRightArea(apY,lastY,actY,lastX,actX)==1){
                    return collnumber;
                }
                else return 0;
            }
        }
        else if(type==3){ // not defined as not possible yet
            if(actX*actX>halfGap*halfGap || actY*actY>halfGap*halfGap){
                return collnumber;
            }
            else return 0;
        }
        else {
            return 0;
        }
    }
    
    ~allcolls(){}
    
};

class partdata{ // Class for particle data
private:
    int pid, turn, type, losstype;
    double spos, xpos, xp, ypos, yp, de, radius;
    
public:
    
    partdata(){
        pid=0; turn=0; spos=0; xpos=0; xp=0; ypos=0; yp=0; de=0; type=0; radius=0; losstype=0;
    }
    partdata(int a1, int a2, double a3, double a4, double a5, double a6, double a7, double a8, int a9, int a10){
        pid=a1; turn=a2; spos=a3; xpos=a4; xp=a5; ypos=a6; yp=a7; de=a8; type=a9; radius=(sqrt((a4*a4)+(a6*a6))); losstype=a10;
    }
    
    int showpid(){return pid;}
    int showturn(){return turn;}
    double showspos(){return spos;}
    double showxpos(){return xpos;}
    double showxp(){return xp;}
    double showypos(){return ypos;}
    double showyp(){return yp;}
    double showde(){return de;}
    int showtype(){return type;}
    double showradius(){return radius;}
    int showlosstype(){return losstype;}
    ~partdata(){}
    
    const double getspos(){return spos;
    }
    
    void outputpart(){
        cout << pid << " " << turn << " " << spos << " " << xpos << " " << xp << " " << ypos << " " << yp << " " << de << " " << type << " " << radius << " " << losstype <<endl;
    }
};

class aperdata{ //Coass for aperture data
private:
    string apername;
    int shape;
    double s0, s1, s2, stili5_b, apx, apy, apr;
    
public:
    aperdata(){
        apername="", s0=0, s1=0, s2=0, stili5_b=0, apx=0, apy=0, apr=0;
    }
    aperdata(string b1, double b2, double b3, double b4, double b5, double b6, double b7){
        apername=b1; s0=b2; s1=b3; s2=b4; stili5_b=b5; apx=(b6*0.001); apy=(b7*0.001); apr=(sqrt((b6*0.001)*(b6*0.001)+(b7*0.001)*(b7*0.001))); shape=0;
        if(apx!=apy || b1=="MB" || b1=="SX1" || b1=="SY2")shape=1; // decision is made which parts of the aperture are considered as hyperelliptic (shape=1) or circular (shape=0)
    }
    
    string showap(){return apername;}
    double shows0(){return s0;}
    double shows1(){return s1;}
    double shows2(){return s2;}
    double showstili5_b(){return stili5_b;}
    double showapx(){return apx;}
    double showapy(){return apy;}
    double showapr(){return apr;}
    int showshape(){return shape;}
    ~aperdata(){}
    
    void outputaper(){
        cout << apername << " " << s0 << " " << s1 << " " << s2 << " " << stili5_b << " " << apx << " " << apy << " " << apr <<endl;
    }
    
};

class particles : public partdata{
private:
    ifstream tracks;
    string line;
    int d1, d2, d9, pid, key, key2, n_hyper, eofstat, firstturn;
    double d3, d4, d5, d6, d7, d8, kx, ky, dx, dy, intersec;
    char d4s[15], d5s[15], d6s[15], d7s[15];
    list<partdata> parts;
    list<partdata>::iterator itref, itlast;
    list<string> errormessages;
public:
    particles(){
        cout << "Init" << endl;
        key=1;
        d1=d2=d9=pid=n_hyper=eofstat=firstturn=0;
    }
    
    void fileopen(char trackfile[50]){
        if(checkfile(trackfile)==1){
            tracks.open(trackfile);
        }
        eofstat=0;
    }
    
    int checkopen(){
        if(eofstat==0)return 1;
        else return 0;
    }
    
    void fileclose(char trackfile[50]){tracks.close();}
    
    void setkey(int k){key=k;} //eventuell weghauen
    
    int readparticle(int run){
        key2=1;
        eofstat=firstturn=0;
        pid=d1;
        while(d1==pid){
            if(key2==1){
                firstturn=(d2-1);
                key2=0;
            }
            if(d1!=0)parts.push_back(partdata(d1,d2,d3,d4*0.001,d5,d6*0.001,d7,d8,d9,0));
            getline(tracks, line);
            if(line.length()>120){
                stringstream error;
                string sentence="Trackfile-error,PID:";
                error << sentence << d1 << ",turn:" << d2 << ",setup:" << run << endl;
                error >> line;
                errormessages.push_back(line);
                cout << line << endl;
            }
            else{
                stringstream sstr;
                sstr << line;
                sstr >> d1 >> d2 >> d3 >> d4s >> d5s >> d6s >> d7s >> d8 >> d9;
            }
            d4=atof(d4s);
            d5=atof(d5s);
            d6=atof(d6s);
            d7=atof(d7s);
            
            if(key==1){
                firstturn=(d2-1);
                pid=d1;
                key=0;
            }
            if(d1!=pid) {
                return 100;
            }
            if(tracks.eof()){
                eofstat=1;
                return 99;
            }
        }
    return 0;
    }
    
    int listend(){
        if(itref==parts.end())return 1;
        else return 0;
    }
    
    void clearpart(){parts.clear();}
    int showpid(){return pid;}
    
    void init(){
        itref=parts.begin();
        itlast=parts.begin();
    }
    
    int size(){
        return parts.size();
    }
    
    void show(){
        itref=parts.begin();
        while(itref!=parts.end()){
            cout << itref->showpid() << " " << itref->showturn() << " " << itref->showspos() << " " << itref->showxpos() << " " << itref->showxp() << " " << itref->showypos() << " " << itref->showyp() << " " << itref->showde() << " " << itref->showtype() << " " << itref->showlosstype() << endl;
            itref++;
        }
    }
    
    void inc(){
        itlast=itref;
        itref++;
    }
    
    void dec(){
        itref=itlast;
        itlast--;
    }
    
    int taketurnlast(){return itlast->showturn();}
    
    double shows(){return itref->showspos();}
    double showslast(){return itlast->showspos();}
    double takex(){return itref->showxpos();}
    double takexlast(){return itlast->showxpos();}
    double takexp(){return itref->showxp();}
    double takexplast(){return itlast->showxp();}
    double takey(){return itref->showypos();}
    double takeylast(){return itlast->showypos();}
    double takeyp(){return itref->showyp();}
    double takeyplast(){return itlast->showyp();}
    double takede(){return itref->showde();}
    double takedelast(){return itlast->showde();}
    double taketype(){return itref->showtype();}
    double taketypelast(){return itlast->showtype();}
    
    unsigned errorsize(){return errormessages.size();}
    
    void showerrors(){
        list<string>::iterator iterror=errormessages.begin();
        cout << "Encountered errors:" << endl;
        while(iterror != errormessages.end()){
            cout << *iterror << endl;
            iterror++;
        }
    }
    
    int calcturns(){return itlast->showturn()-firstturn;}
    
    int sameturn(){
        if(itlast->showturn()==itref->showturn())return 1;
        else return 0;
    }
    
    int endofsim(double ring){
        if(itlast->showturn()==100 && itlast->showspos()>(ring-1))return 1;
        else return 0;
    }
    
    int sixtrackradius(){
        if(radius(itlast->showxpos(),itlast->showypos())>0.99)return 1;
        else return 0;
    }
    
    int sixtrackangle(){
        if(itlast->showxp()>10 || itlast->showxp()<-10 || itlast->showyp()>10 || itlast->showyp()<-10) return 1;
        else return 0;
    }
    
    void straightx(){
        kx=(itlast->showxpos()-itref->showxpos())/(itlast->showspos()-itref->showspos());
        dx=itlast->showxpos()-kx*itlast->showspos();
    }
    void straighty(){
        ky=(itlast->showypos()-itref->showypos())/(itlast->showspos()-itref->showspos());
        dy=itlast->showypos()-ky*itlast->showspos();
    }
    
    double xv(double svalue){return kx*svalue+dx;}
    double yv(double svalue){return ky*svalue+dy;}
    
    double xp(){return itref->showxp();}
    double yp(){return itref->showyp();}
    double x(){return itref->showxpos();}
    double y(){return itref->showypos();}

    double radius(double x, double y){
        return sqrt((x*x)+(y*y));
    }
    
    int insideradius(double v1, double v2, double r2){
        double r1=sqrt((v1*v1)+(v2*v2));
        if(r1 < r2) return 1;
        else return 0;
    }
    
    int insidehyper(double v1, double v2, double r1, double r2){
        n_hyper=4;
        if(pow((v1/r1),n_hyper)+pow((v2/r2),n_hyper)<1)return 1;
        else return 0;
    }
    
    int calcintersecc(double apx){
        intersec=itlast->showspos();
        while(insideradius(xv(intersec),yv(intersec),apx)==1){
            intersec+=0.001;
            if(intersec>=itref->showspos())return 0;
        }
        return 1;
    }
    
    int calcintersech(double apx, double apy){
        intersec=itlast->showspos();
        while(insidehyper(xv(intersec),yv(intersec),apx,apy)==1){
            intersec+=0.001;
            if(intersec>=itref->showspos())return 0;
        }
        return 1;
    }
    
    double showintersec(){return intersec;}
};

class aperturelist : public aperdata{
private:
    int shaperef, shapelast;
    list<aperdata> aplist;
    list<aperdata>::iterator itref, itlast;
    string c1;
    double c2, c3, c4, c5, c6, c7, length;
public:
    aperturelist(){
        char aperturefile[50];
        sprintf(aperturefile, "%s", "aperture2.dat");
        if(checkfile(aperturefile)==1){
            ifstream inaper;
            inaper.open("aperture2.dat");
            while (!inaper.eof()){
                inaper >> c1 >> c2 >> c3 >> c4 >> c5 >> c6 >> c7;
                if( inaper.eof() ) break;
                aplist.push_back(aperdata(c1,c2,c3,c4,c5,c6,c7));
            }
            inaper.close();
        }
    }
    
    void showaperture(){
        itref=aplist.begin();
        while(itref!=aplist.end()){
            cout << itref->showap() << endl;
            itref++;
        }
    }
    
    void makelength(){
        itref=aplist.end();
        itref--;
        length=itref->shows1();
    }
    
    void init(){
        itref=aplist.begin();
        itlast=aplist.begin();
    }
    
    double ringlength(){return length;}
    
    int apinc(){
        itlast=itref;
        itref++;
        if(itref->showshape()==itlast->showshape() && itref->showapx()==itlast->showapx() && itref->showapy()==itlast->showapy())return 0;
        else return 1;
    }
    
    double showsnow(){return itref->shows2();}
    double showslast(){return itlast->shows2();}
    double x(){return itref->showapx();}
    double y(){return itref->showapy();}
    double xlast(){return itlast->showapx();}
    double ylast(){return itlast->showapy();}
    int getshaperef(){return itref->showshape();}
    int getshapelast(){return itlast->showshape();}
};

class run{
private:
    vector<int> abspid;
    vector<float> beamprops;
    list<partdata> lost, tracklist;
    list<allcolls> collList;
    aperturelist aperture;
    int case1, case2, start, stop, switcher, maketordered, dontusesix, collimatornumber, mode, firstrun, temp, totalparticles, breaker;
    string completeline,namepuffer,listLength;
    char outfilenameeff[100], temp3[10], inputcollimator[50], collimator[50], inputfort[20], fort[20], orderedtrack[50], allabsorptions[50], allabsorptionsnew[50], firstimpacts[100], firstimpactsnew[100], collgaps[50], collgapsnew[50],flukafile[100], flukafilenew[100], outfilenameloss[100], outfileaperture[100], outfilecollimator[100], outfileexceed[100], outfilestrangeloss[100], statisticsfile[100], bugfile[100], particletrack[100], particlefile[100], aperturefile[50];
    float puffer, temp4, temp5, temp6, temp7;
public:
    run(){
    }
    
    // This is the basic routine, see functions for better understanding
    int execute(){
        firstrun=1;
        if(readinparameters()==0)return 199;
        if(checkfileexistance()==0)return 199;
        while(start<=stop){
            totalparticles=0;
            makefilenames();
            filerename();
            if (mode==1) readcoll();
            else if (mode==2) readfortfile();
            if(firstrun==1){
                readcoll();
                makeeff();
                firstrun=0;
            }
            if(dontusesix==0)if(sixtrack()==201)return 199;
            if(maketordered==1)makeorderedfile();
            readabsorbed();
            if(dontusesix==0 && maketordered==0){
                for(int i=1; i<=2; i++){
                    updateCollimators();
                    sprintf(particlefile, "%s%u%s", "particle",i,".dat");
                    trackrun(particlefile);
                }
            }
            else {
                updateCollimators();
                trackrun(orderedtrack);
            }
            makeoutput();
            writeGeom();
            filerenameback();
            start++;
        }
        return 1;
    }
    
    
    //Routine for proceeding through the track
    void trackrun(char file[50]){
        particles party;
        party.fileopen(file);
        party.setkey(1);
        while(party.checkopen()==1){
            party.readparticle(start);
            party.init();
            cout << "Processing particle: " << party.showpid() << endl;
            aperture.makelength();
            aperture.init();
            totalparticles++;
            breaker=0;
            while(aperture.showsnow() <= party.shows()){ //get the particle into the right ring section
                if(aperture.apinc()==1){ // check for size/shape changes of the aperture
                    if(party.sameturn()==1){
                        party.straightx(); // calculate straight lines for loss checks
                        party.straighty();
                        if(aperture.getshaperef()==0)case1=party.insideradius(party.xv(aperture.showslast()),party.yv(aperture.showslast()),aperture.x());
                        else case1=party.insidehyper(party.xv(aperture.showslast()),party.yv(aperture.showslast()),aperture.x(), aperture.y());
                        if(aperture.getshapelast()==0)case2=party.insideradius(party.xv(aperture.showslast()),party.yv(aperture.showslast()),aperture.xlast());
                        else case2=party.insidehyper(party.xv(aperture.showslast()),party.yv(aperture.showslast()),aperture.xlast(), aperture.ylast());
                        if(case1==1 && case2==0){ // two loss cases
                            lost.push_back(partdata(party.showpid(),party.calcturns(),aperture.showslast(),party.xv(aperture.showslast()),party.takexp(),party.yv(aperture.showslast()),party.takeyp(),party.takede(),party.taketype(),5));
                            breaker=1;
                            break;
                        }
                        else if(case1==0 && case2==1){
                            if(aperture.getshapelast()==0){
                                party.calcintersecc(aperture.xlast());
                                lost.push_back(partdata(party.showpid(),party.calcturns(),party.showintersec(),party.xv(party.showintersec()),party.takexp(),party.yv(party.showintersec()),party.takeyp(),party.takede(),party.taketype(),4));
                                breaker=1;
                                break;
                            }
                            else {
                                party.calcintersech(aperture.xlast(),aperture.ylast());
                                lost.push_back(partdata(party.showpid(),party.calcturns(),party.showintersec(),party.xv(party.showintersec()),party.takexp(),party.yv(party.showintersec()),party.takeyp(),party.takede(),party.taketype(),4));
                                breaker=1;
                                break;
                            }
                        }
                    }
                }
            while(party.shows()>aperture.ringlength()){ // if particle at end of ring -> increase one step, get aperture back to beginning
                    party.inc();
                    aperture.init();
                }
            while(aperture.showsnow() > party.shows()){ // if particle is in the right ring section, proceed through the track-list and check for losses
                if(party.listend()==1){ // if the track-list end is reached, check if particle is absorbed or sth else happens
                    int listLength;
                    listLength=party.size();
                    if(listLength>1){
                        double sB,xB,yB;
                        party.dec();
                        sB=party.showslast();
                        xB=party.takexlast();
                        yB=party.takeylast();
                        party.inc();
                        temp=checkabsorbed(party.showpid(),sB,party.showslast(),xB,party.takexlast(),yB,party.takeylast(),aperture.x(),aperture.y(),2);
                    }
                    else temp=checkabsorbed(party.showpid(),party.showslast(),party.showslast(),0,0,0,0,0,0,1);
                    if(temp!=0) lost.push_back(partdata(party.showpid(),party.calcturns(),party.showslast(),party.takexlast(),party.takexplast(),party.takeylast(),party.takeyplast(),party.takedelast(),party.taketypelast(),temp));
                    else if(party.endofsim(aperture.ringlength())==1) lost.push_back(partdata(party.showpid(),party.calcturns(),party.showslast(),party.takexlast(),party.takexplast(),party.takeylast(),party.takeyplast(),party.takedelast(),party.taketypelast(),2));
                    else if(party.sixtrackradius()==1) lost.push_back(partdata(party.showpid(),party.calcturns(),party.showslast(),party.takexlast(),party.takexplast(),party.takeylast(),party.takeyplast(),party.takedelast(),party.taketypelast(),6));
                    else if(party.sixtrackangle()==1) lost.push_back(partdata(party.showpid(),party.calcturns(),party.showslast(),party.takexlast(),party.takexplast(),party.takeylast(),party.takeyplast(),party.takedelast(),party.taketypelast(),7));
                    else lost.push_back(partdata(party.showpid(),party.calcturns(),party.showslast(),party.takexlast(),party.takexplast(),party.takeylast(),party.takeyplast(),party.takedelast(),party.taketypelast(),8));
                    breaker=1;
                    break;
                }
                else{
                    if(aperture.getshaperef()==0){ // check losses in the two different shapes (hyperellipse, circular)
                        if(party.insideradius(party.x(),party.y(),aperture.x())==0){
                            party.straightx();
                            party.straighty();
                            party.calcintersecc(aperture.xlast());
                            lost.push_back(partdata(party.showpid(),party.calcturns(),party.showintersec(),party.xv(party.showintersec()),party.takexp(),party.yv(party.showintersec()),party.takeyp(),party.takede(),party.taketype(),3));
                            breaker=1;
                            break;
                        }
                        else party.inc();
                    }
                    else{
                        if(party.insidehyper(party.x(),party.y(),aperture.x(),aperture.y())==0){
                            party.straightx();
                            party.straighty();
                            party.calcintersech(aperture.xlast(),aperture.ylast());
                            lost.push_back(partdata(party.showpid(),party.calcturns(),party.showintersec(),party.xv(party.showintersec()),party.takexp(),party.yv(party.showintersec()),party.takeyp(),party.takede(),party.taketype(),1));
                            breaker=1;
                            break;
                        }
                        else party.inc();
                    }
                }
            }
            if(breaker==1)break;
            }
            //Routine for trackoutput
            if(switcher==start){
                int lastturn=party.taketurnlast();
                for(int w=1; w<450; w++)
                {
                    if(party.listend()==1)party.dec();
                    tracklist.push_back(partdata(party.showpid(),party.calcturns(),party.shows(),party.takex(),party.takexp(),party.takey(),party.takeyp(),party.takede(),party.taketype(),0));
                    party.dec();
                    if(party.taketurnlast()!=lastturn)break;
                }
                list<partdata>::iterator ittrack=tracklist.end();
                sprintf( particletrack, "%s%u%s",   "trackpart", party.showpid(), ".dat"  );
                ofstream tracker;
                tracker.open(particletrack);
                while(ittrack!=tracklist.begin()){
                    ittrack--;
                    tracker << ittrack->showpid() << '\t' << ittrack->showturn()  << '\t' << ittrack->showspos()  << '\t' << ittrack->showxpos()  << '\t' << ittrack->showxp()  << '\t' << ittrack->showypos()  << '\t' << ittrack->showyp()  << '\t' << ittrack->showde()  << '\t' <<ittrack->showtype()  << '\t' << ittrack->showradius() << endl;
                }
                tracklist.clear();
                tracker.close();
            }
            party.clearpart(); //Clear list
            
        }
        if(party.errorsize()==0)cout << "No errors occured" << endl;
        else party.showerrors();
        party.fileclose(file);
        
    }
    
    int readinparameters(){
        char parafile[100]="inparameters.dat";
        if(checkfile(parafile)==0)return 0;
        ifstream para;
        para.open(parafile);
        para >> outfilenameeff;
        para >> dontusesix;
        para >> maketordered;
        para >> mode; // changing fort.3 or list_coll.data
        para >> switcher; //define for which particle a trackanalysis is made, 0 for nothing
        para >> start;
        para >> stop;
        para >> completeline;
        para >> collimatornumber;
        for(int p=0; p<collimatornumber; p++){
            para >> namepuffer;
            para >> puffer;
            collList.push_back(allcolls(namepuffer,p+101,puffer));
        }
        para.close();
        return 1;
    }
    
    int checkfileexistance(){
        sprintf(aperturefile,"%s","aperture2.dat");
        if(checkfile(aperturefile)==0)return 0;
        for(int i=start;i<=stop;i++){
            if(mode==1){
                sprintf( inputcollimator, "%s%u%s",   "list_coll_", i, ".data" );
                if(checkfile(inputcollimator)==0)return 0;
            }
            else if(mode==2){
                sprintf( inputfort, "%s%u%s",   "fort_", i, ".3" );
                if(checkfile(inputfort)==0)return 0;
            }
            if(dontusesix==1){
                sprintf(orderedtrack, "%s%u%s",   "tordered_", i, ".dat" );
                sprintf(allabsorptionsnew, "%s%u%s",   "all_absorptions_", i, ".dat" );
                sprintf(collgapsnew, "%s%u%s", "collgaps_", i, ".dat");
                if(checkfile(orderedtrack)==0)return 0;
                if(checkfile(allabsorptionsnew)==0)return 0;
            }
        }
        return 1;
    }
    
    void readcoll(){
        ifstream coll;
        coll.open(collimator);
        string cMaterial;
        for(int i=1; i<=2; i++)getline(coll, completeline); //read first two lines
        
        while(!coll.eof()){
            int r=0;
            while(r<4){
                getline(coll, completeline);
                if(coll.eof())break;
                r++;
            }
            if(coll.eof())break;
            getline(coll, completeline);
            cMaterial=completeline;
            getline(coll, completeline);
            stringstream sstr; // change the string of the length to float
            sstr << completeline;
            sstr >> temp4;
            while(r<8){
                getline(coll, completeline);
                r++;
            }
        }
        coll.close();
        
    }
    
    void readfortfile(){
        beamprops.clear();
        int a;
        ifstream beam;
        beam.open(fort);
        while(!beam.eof()){
            getline(beam, completeline);
            a=completeline.find("COLLIMATION");
            if(a>=0){
                for(int i=1; i<=3; i++)getline(beam, completeline);
                stringstream beamstr;
                beamstr << completeline;
                beamstr >> puffer;
                for(int i=1; i<=4; i++){
                    beamstr >> puffer;
                    beamprops.push_back(puffer);
                }
                break;
            }
        }
        beam.close();
    }
    
    void updateCollimators(){
        int iPuff,a,b,newType;
        float newHalfGap,newLength,fPuff;
        string name, mat;
        ifstream colls;
        colls.open(collgaps);
        getline(colls,completeline);
        list<allcolls>::iterator collit=collList.begin();
        
        while(!colls.eof()){
            stringstream collStream;
            getline(colls,completeline);
            collStream << completeline;
            collStream >> iPuff >> name >> fPuff >> fPuff >> fPuff >> newHalfGap >> mat >> newLength;
            collit=collList.begin();
            while(collit != collList.end()){
                a=name.find("H");
                b=name.find("V");
                if(a==4) newType=1;
                else if (b==4) newType=2;
                if(collit->getName()==name)collit->updateData(mat,newType,newLength,newHalfGap);
                collit++;
            }
        }
    }
    
    void writeGeom(){
        list<allcolls>::iterator collit=collList.begin();
        ofstream hcol, vcol;
        hcol.open("hcoldim.dat");
        vcol.open("vcoldim.dat");
        cout << "Writing Collimator Geometry" << endl;
        while(collit != collList.end()){
            if(collit->getType()==1){
                hcol << collit->getSfront() << " 150" << endl;
                hcol << collit->getSfront() << " " << collit->getHalfGap()*1000 << endl;
                hcol << collit->getSback() << " " << collit->getHalfGap()*1000 << endl;
                hcol << collit->getSback() << " 150" << endl;
            }
            else if(collit->getType()==2){
                vcol << collit->getSfront() << " 150" << endl;
                vcol << collit->getSfront() << " " << collit->getHalfGap()*1000 << endl;
                vcol << collit->getSback() << " " << collit->getHalfGap()*1000 << endl;
                vcol << collit->getSback() << " 150" << endl;
            }
            collit++;
        }
        hcol.close();
        vcol.close();
        cout << "Done" << endl;
    }
    
    void makeeff(){
        ofstream efficiency;
        efficiency.open(outfilenameeff);
        if (mode==1)efficiency << "Filestructure: run/materialH/lengthH/materialVlengthV/";
        if (mode==2)efficiency << "Filestructure: run/Hhalo/Hsmear/Vhalo/Vsmear/";
        efficiency << "total/absorbed+lost+exceed+hardoverkick/absorbed/lost/exceed/hardoverkick/cleaningspeed/efficiency/inefficiency/status" << endl;
        cout << "Starting simulation..." << endl;
        efficiency.close();
    }
    
    void readabsorbed(){
        abspid.clear();
        ifstream sixabsorptions;
        sixabsorptions.open(allabsorptions);
        getline(sixabsorptions, completeline); //Read the all_absorptions.dat file to compare my absorbed particles with the ones directly from SixTrack -> distinguish between really absorbed and just look-a-likes
        while(!sixabsorptions.eof()){
            sixabsorptions >> temp; //maybe at some point add turn and s-position check, as it is not 100% sure that if the track ends it always ends at the absorption (but it should)
            abspid.push_back(temp);
            getline(sixabsorptions, completeline);
            if(sixabsorptions.eof())break;
        }
        sixabsorptions.close();
        cout << "Total particles absorbed according to ST: " << abspid.size()-1 << endl;
    }
    int checkabsorbed(int checker, double slast, double sact, double lx, double ax, double ly, double ay, double apx, double apy, int sw){
        vector<int>::iterator abspit=abspid.begin();
        list<allcolls>::iterator collit=collList.begin();
        abspit=find(abspid.begin(),abspid.end(),checker);
        temp=*abspit;
        int ret=0;
        if(temp==checker){
            //check which collimator and delete element
            abspid.erase(abspit);
            collit=collList.begin();
            while(collit!=collList.end() && ret==0){
                ret=collit->absorbedInColl(sact);
                if(ret!=0) break;
                collit++;
            }
            return ret;
        }
        else {
            if(sw==2){
                while(collit!=collList.end() && ret==0){
                    if(collit->findColl(slast,sact)==1){
                        ret=collit->isAbsorbedOrLost(lx,ax,ly,ay,apx,apy);
                    }
                    if(ret!=0) break;
                    collit++;
                }
                return ret;
            }
            else return 0;
        }
    }
    
    int sixtrack(){
        cout << "SixTrack is running, patience please..." << endl;
        system("./SixTrack > out.s");
        cout << "SixTrack simulation finished" << endl;
        int a,b;
        ifstream trackfile;
        trackfile.open("tracks2.dat");
        if(trackfile==0){
            cout << "It seems that SixTrack did not run (trackfile missing!), exiting!" << endl;
            return 201;
        }
        ofstream particle1, particle2;
        particle1.open("particle1.dat");
        particle2.open("particle2.dat");
        cout << "Sorting..." << endl;
            
        getline(trackfile, completeline);
        while(!trackfile.eof()){
            getline(trackfile, completeline);
            a=completeline.find("01 ",2);
            b=completeline.find("02 ",2);
            if(completeline[a+1]=='1' && a<20)particle1 << completeline << endl;
            else if(completeline[b+1]=='2' && b<20)particle2 << completeline << endl;
        }
        trackfile.close();
        particle2.close();
        particle1.close();
        return 1;
    }
    
    void makeorderedfile(){
        ifstream part1in, part2in;
        ofstream tordered;
        part1in.open("particle1.dat");
        sprintf(orderedtrack, "%s%u%s",   "tordered_", start, ".dat" );
        tordered.open(orderedtrack);
        while(!part1in.eof()){
            getline(part1in,completeline);
            if(part1in.eof())break;
            tordered << completeline << endl;
        }
        part1in.close();
        part2in.open("particle2.dat");
        while(!part2in.eof()){
            getline(part2in,completeline);
            if(part2in.eof())break;
            tordered << completeline << endl;
        }
        part2in.close();
        tordered.close();
        cout << "Finished sorting..." << endl;
    }
    
    void makefilenames(){
        sprintf( allabsorptions, "%s",   "all_absorptions.dat" );
        sprintf( allabsorptionsnew, "%s%u%s",   "all_absorptions_", start, ".dat" );
        sprintf( orderedtrack, "%s%u%s",   "tordered_", start, ".dat" );
        sprintf( inputfort, "%s%u%s",   "fort_", start, ".3" );
        sprintf( fort, "%s", "fort.3" );
        sprintf( inputcollimator, "%s%u%s",   "list_coll_", start, ".data" );
        sprintf( collimator, "%s", "list_coll.data" );
        sprintf( outfilenameloss, "%s%u%s",   "finallocation_", start, ".dat" ); // (Re)naming all files
        sprintf( outfileaperture, "%s%u%s",   "apertureloss_", start, ".dat" );
        sprintf( outfilecollimator, "%s%u%s",   "collimatorabs_", start, ".dat" );
        sprintf( outfilestrangeloss, "%s%u%s",   "sixtrackstop_", start, ".dat" );
        sprintf( outfileexceed, "%s%u%s",   "exceedparticles_", start, ".dat" );
        sprintf( statisticsfile, "%s%u%s",   "collimatorhits_", start, ".dat" );
        sprintf( firstimpactsnew, "%s%u%s",   "FirstImpacts_", start, ".dat"  );
        sprintf( firstimpacts, "%s",   "FirstImpacts.dat"  );
        sprintf( flukafilenew, "%s%u%s",   "FLUKA_impacts_", start, ".dat"  );
        sprintf( flukafile, "%s",   "FLUKA_impacts.dat"  );
        sprintf( bugfile, "%s%u%s",   "bugfile_", start, ".dat"  );
        sprintf( collgaps, "%s", "collgaps.dat");
        sprintf( collgapsnew, "%s%u%s", "collgaps_", start, ".dat");
    }
    
    void makeoutput(){
        int lessthanone, backcheck, hitcollimator, strange, hitaperture, exceed;
        lessthanone=backcheck=hitcollimator=strange=hitaperture=exceed=0;
        
        ofstream apertureloss, collimatorloss, exceedloss, strangeloss, statistics, final, bugs, efffile;
        final.open(outfilenameloss);
        apertureloss.open(outfileaperture);
        collimatorloss.open(outfilecollimator);
        exceedloss.open(outfileexceed);
        strangeloss.open(outfilestrangeloss);
        statistics.open(statisticsfile);
        bugs.open(bugfile);
        
        vector<int>::iterator abspit=abspid.begin();
        
        list<partdata>::iterator itlost=lost.begin();
        while(itlost != lost.end()){//all particle are written into finallocation_i.dat
            final << itlost->showpid() << '\t' << itlost->showturn()  << '\t' << itlost->showspos()  << '\t' << itlost->showxpos()  << '\t' << itlost->showxp()  << '\t' << itlost->showypos()  << '\t' << itlost->showyp()  << '\t' << itlost->showde()  << '\t' << itlost->showtype()  << '\t' << itlost->showradius() << '\t' << itlost ->showlosstype() << endl;
            backcheck++;
            if(itlost ->showlosstype()>100){//absorbed in collimator -> collimatorloss_i.dat
                collimatorloss << itlost->showpid() << '\t' << itlost->showturn()  << '\t' << itlost->showspos()  << '\t' << itlost->showxpos()  << '\t' << itlost->showxp()  << '\t' << itlost->showypos()  << '\t' << itlost->showyp()  << '\t' << itlost->showde()  << '\t' << itlost->showtype()  << '\t' << itlost->showradius() << '\t'<< itlost ->showlosstype() << endl;
                hitcollimator++;
                if(itlost->showturn()==1)lessthanone++;
            }
            else if(itlost ->showlosstype()==7){//simulation stopped by SixTrack
                strangeloss << itlost->showpid() << '\t' << itlost->showturn()  << '\t' << itlost->showspos()  << '\t' << itlost->showxpos()  << '\t' << itlost->showxp()  << '\t' << itlost->showypos()  << '\t' << itlost->showyp()  << '\t' << itlost->showde()  << '\t' << itlost->showtype()  << '\t' << itlost->showradius() << '\t'<< 7 << endl;
                strange++;
            }
            else if(itlost ->showlosstype()==6){//simulation stopped by SixTrack
                strangeloss << itlost->showpid() << '\t' << itlost->showturn()  << '\t' << itlost->showspos()  << '\t' << itlost->showxpos()  << '\t' << itlost->showxp()  << '\t' << itlost->showypos()  << '\t' << itlost->showyp()  << '\t' << itlost->showde()  << '\t' << itlost->showtype()  << '\t' << itlost->showradius() << '\t'<< itlost->showlosstype() << endl;
                strange++;
            }
            else if(itlost ->showlosstype()==1 || itlost ->showlosstype()==3 || itlost ->showlosstype()==4 || itlost ->showlosstype()==5){//lost in aperture -> apertureloss_i.dat
                apertureloss << itlost->showpid() << '\t' << itlost->showturn()  << '\t' << itlost->showspos()  << '\t' << itlost->showxpos()  << '\t' << itlost->showxp()  << '\t' << itlost->showypos()  << '\t' << itlost->showyp()  << '\t' << itlost->showde()  << '\t' << itlost->showtype()  << '\t' << itlost->showradius() << '\t'<< itlost ->showlosstype() << endl;
                hitaperture++;
            }
            else if(itlost ->showlosstype()==2){//exceeded 100 turns -> exceedloss_i.dat
                exceedloss << itlost->showpid() << '\t' << itlost->showturn()  << '\t' << itlost->showspos()  << '\t' << itlost->showxpos()  << '\t' << itlost->showxp()  << '\t' << itlost->showypos()  << '\t' << itlost->showyp()  << '\t' << itlost->showde()  << '\t' << itlost->showtype()  << '\t' << itlost->showradius() << '\t'<< itlost ->showlosstype() << endl;
                exceed++;
            }
            else if(itlost ->showlosstype()==8){//Buglist, all particles that stop for no reason
                bugs << itlost->showpid() << '\t' << itlost->showturn()  << '\t' << itlost->showspos()  << '\t' << itlost->showxpos()  << '\t' << itlost->showxp()  << '\t' << itlost->showypos()  << '\t' << itlost->showyp()  << '\t' << itlost->showde()  << '\t' << itlost->showtype()  << '\t' << itlost->showradius() << '\t'<< itlost ->showlosstype() << endl;
            }
            itlost++;
        }
        
        final.close();
        apertureloss.close();
        collimatorloss.close();
        exceedloss.close();
        strangeloss.close();
        int status=0;
        if(backcheck==totalparticles){
            cout << "Check OK!" << endl;
            status=1;
        }
        float sum, eff, ineff, speed;
        sum=hitaperture+hitcollimator+exceed+strange;
        eff=hitcollimator/sum;
        ineff=(1.*(hitaperture+strange))/hitcollimator;
        speed=(1.*lessthanone)/hitcollimator;
        
        char materialH[10], materialV[10];
        float lengthH, lengthV;

        list<allcolls>::iterator collit=collList.begin();
        while(collit!=collList.end()){
             if(collit->getName()=="TCP.H.1"){
                 size_t strlength=collit->getMaterial().copy(materialH,2,0);
                 materialH[strlength]='\0';
                 lengthH=collit->getLength();
             }
             if(collit->getName()=="TCP.V.1"){
                 size_t strlength=collit->getMaterial().copy(materialV,2,0);
                 materialV[strlength]='\0';
                 lengthV=collit->getLength();
             }
        collit++;
        }

        efffile.open(outfilenameeff, ios::out | ios::app);
        if(mode==1) efffile << start << " " << materialH << " " << lengthH << " " << materialV << " " << lengthV << " ";
        if(mode==2) efffile << start << " " << beamprops[0] << " " << beamprops[1] << " " << beamprops[2] << " " << beamprops[3] << " ";
        efffile << " " << totalparticles << " " << sum << " " << hitcollimator << " " << hitaperture << " " << exceed << " "  << strange << " " << speed << " " << eff << " " << ineff << " " << status << endl;
        efffile.close();
        
        collit=collList.begin();
        while(collit!=collList.end()){
            statistics << "Collimator: " << collit->getCollNumber() << " " << collit->getAbsorbed() << endl;
            collit->ResetAbsorbed();
            collit++;
        }
        
        statistics.close();
        
        cout << "Total: "<< totalparticles  << endl;
        cout << "Absorbed: "<< hitcollimator  << endl;
        cout << "Lost: "<< hitaperture  << endl;
        cout << "Exceeded: "<< exceed  << endl;
        cout << "Hard overkick: "<< strange  << endl;
        cout << "In list: " << strange+exceed+hitaperture+hitcollimator << endl;
        
        lessthanone=backcheck=hitcollimator=strange=hitaperture=exceed=0;
        lost.clear();
    }
    
    void filerename(){
        if(mode==1)rename(inputcollimator, collimator);
        if(mode==2)rename(inputfort, fort);
        if(dontusesix==1){
            rename(allabsorptionsnew, allabsorptions);
            rename(collgapsnew, collgaps);
        }
    }
    void filerenameback(){
        if(mode==1)rename(collimator,inputcollimator);
        if(mode==2)rename(fort,inputfort);
        rename(firstimpacts,firstimpactsnew); //rename all the files that were used
        rename(allabsorptions,allabsorptionsnew);
        rename(flukafile,flukafilenew);
        rename(collgaps, collgapsnew);
    }
    
};


int main()
{
    welcomescreen();
    run simulation;
    if(simulation.execute()==1)cout << "Finished" << endl;
    return 0;
}
