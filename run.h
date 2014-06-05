using namespace std;

class run{
private:
    vector<STabs> abspid;
    vector<double> beamprops;
    list<partdata> lost, tracklist;
    list<allcolls> collList;
    list<allcolls>::iterator collListIt;
    aperturelist aperture;
    int case1, case2, start, stop, switcher, maketordered, dontusesix, collimatornumber, mode, firstrun, temp, temp2, totalparticles, breaker;
    string completeline,namepuffer,listLength;
    char outfilenameeff[100], temp3[10], inputcollimator[50], collimator[50], inputfort[20], fort[20], orderedtrack[50], allabsorptions[50], allabsorptionsnew[50], firstimpacts[100], firstimpactsnew[100], collgaps[50], collgapsnew[50],flukafile[100], flukafilenew[100], outfilenameloss[100], outfileaperture[100], outfilecollimator[100], outfileexceed[100], outfilestrangeloss[100], statisticsfile[100], bugfile[100], particletrack[100], particlefile[100], aperturefile[50];
    double puffer, temp4, temp5, temp6, temp7,positionshiftdouble;
    double positionshift,collhalflength;
public:
    run(){
    }

    // This is the basic routine, see functions for better understanding
    int execute(){
        firstrun=1;
        if(readinparameters()==0)return 199;
        //aperture.create(positionshift);
        //aperturelist aperture(positionshift);
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
            collListIt=collList.begin();
            collhalflength=(double)(collListIt->getLength()/2.);
            if(positionshift>0.){
              aperture.create(positionshift+collhalflength); //Ansehen ob das nicht überholt ist!!
            }
            else aperture.create(0.);
            if(dontusesix==0)if(sixtrack()==201)return 199;
            if(maketordered==1)makeorderedfile();
            //aperture.showaperture();
            readabsorbed();
            if(dontusesix==0 && maketordered==0){
                updateCollimators();
                for(int i=1; i<=2; i++){
                    //updateCollimators();
                    //collListIt=collList.begin();
                    //cout << collListIt->getSfront() << " " << collListIt->getSback() << endl;
                    sprintf(particlefile, "%s%u%s", "particle",i,".dat");
                    trackrun(particlefile);
                }
            }
            else {
                updateCollimators();
                //collListIt=collList.begin();
                //cout << collListIt->getSfront() << " " << collListIt->getSback() << endl;
                trackrun(orderedtrack);
            }
            makeoutput();
            writeGeom();
            filerenameback();
            aperture.empty();
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
            party.tracksort();
            party.init();
            cout << "Processing particle: " << party.showpid() << endl;
            aperture.makelength(); //kill
            aperture.init();
            totalparticles++;
            breaker=0;
            //cout << aperture.showsnow() << " " << party.shows() << " " << aperture.ringlength() <<  endl;
            int firstenter=1;
            while(/*party.shows() >= aperture.showsnow() || party.shows() < aperture.showsnowfront() || */firstenter==1){ //loop for one particle
                //firstenter=0;
                //cout << aperture.showsnow() << endl;
                while(aperture.showsnow()<=party.shows())aperture.apinc(); //get the particle into the right ring section
                while(party.shows()>aperture.ringlength()){ // if particle at end of ring -> increase one step, get aperture back to beginning
                    party.inc();
                    aperture.init();
                }
                while(party.shows()>=aperture.showsnowfront() && party.shows()<aperture.showsnow()){ // if particle is in the right ring section, proceed through the track-list and check for losses
                    //cout << "Ordinary " << party.x() << " " << party.y() << " " << party.shows() << " " << aperture.showsnow() << endl;
                    if(party.listend()==1){ // if the track-list end is reached, check if particle is absorbed or sth else happens
                        //cout << "The end is neigh!" << endl;
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
                            else {
                                party.inc();
                                //cout << party.showturn() << " " << party.shows()<< endl;
                                if(/*party.sameturn()==0|| */party.shows() > aperture.ringlength()){
                                    aperture.init();
                                    break;
                                }
                            }
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
                            else {
                                party.inc();
                                if(party.shows() > aperture.ringlength()){
                                    aperture.init();
                                    break;
                                }
                            }
                        }
                    }

                }
                while(party.shows()>aperture.ringlength()){ // if particle at end of ring -> increase one step, get aperture back to beginning
                    party.inc();
                    aperture.init();

                }
                if(breaker==1)break;
                if(party.listend()==1){ // if the track-list end is reached, check if particle is absorbed or sth else happens
                    //cout << "The end is neigh!" << endl;
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
                //aperture.apinc();
                //cout << "next" << party.shows() << " " << aperture.showsnow() << endl;
                while(party.shows()<=aperture.showsnowfront() && party.shows()>=aperture.showsnow()){ //if the particle leaves the right ring section, proceed through the aperture list and check if the particle crosses hits it

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
        para >> positionshift;
        para >> completeline;
        para >> collimatornumber;
        positionshiftdouble=(double)(positionshift);
        for(int p=0; p<collimatornumber; p++){
            para >> namepuffer;
            para >> puffer;
            //cout << puffer - positionshiftdouble << endl;
            collList.push_back(allcolls(namepuffer,p+101,puffer-positionshiftdouble));
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
                if(checkfile(collgapsnew)==0)return 0;
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
            stringstream sstr; // change the string of the length to double
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
        double newHalfGap,newLength,fPuff,PhalfLength;
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
            collit=collList.begin();
            PhalfLength=collit->getLength()/2.;
            while(collit != collList.end()){
                collit->updateCenter(PhalfLength);
                //cout << collit->getSfront() << " " << collit->getSback() << endl;
                collit++;
            }
            collList.sort();
        //}
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
            sixabsorptions >> temp >> temp2 >> temp4;
            abspid.push_back(STabs(temp, temp2, temp4));
            getline(sixabsorptions, completeline); //what does this line exactly? -> check if works properly
            if(sixabsorptions.eof())break;
        }
        sixabsorptions.close();
        cout << "Total particles absorbed according to ST: " << abspid.size()-1 << endl;
    }
    int checkabsorbed(int checker, double slast, double sact, double lx, double ax, double ly, double ay, double apx, double apy, int sw){
        vector<STabs>::iterator abspit=abspid.begin();
        list<allcolls>::iterator collit=collList.begin();
        abspit=abspid.begin();
        //double temp2;
        //while(abspit!=abspid.end()){
        //    temp2=*abspit;
        //    cout << temp2 << endl;
        //    abspit++;
        //}
        abspit=find(abspid.begin(),abspid.end(),STabs(checker));
        temp=abspit->getPID();
        //cout << abspit->getPID();
        int ret=0;
        if(temp==checker){
            //check which collimator and delete element

            collit=collList.begin();
            while(collit!=collList.end() && ret==0){
                ret=collit->absorbedInColl(abspit->getPosition());
                if(ret!=0) break;
                collit++;
            }
            cout << "Absorbed in " << ret << endl;
            abspid.erase(abspit);
            return ret;
        }
        else {
            return 0;
            if(sw==2){
                while(collit!=collList.end() && ret==0){
                    if(collit->findColl(slast,sact)==1){
                        ret=collit->isAbsorbedOrLost(lx,ax,ly,ay,apx,apy);
                    }
                    if(ret!=0) break;
                    collit++;
                }
                cout << "Absorbed in " << ret << endl;
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

        vector<STabs>::iterator abspit=abspid.begin();

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
        double sum, eff, ineff, speed;
        sum=hitaperture+hitcollimator+exceed+strange;
        eff=hitcollimator/sum;
        ineff=(1.*(hitaperture+strange))/hitcollimator;
        speed=(1.*lessthanone)/hitcollimator;

        char materialH[10], materialV[10];
        double lengthH, lengthV;

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
            statistics << "Collimator: " << collit->getCollNumber() << "-" << collit->getName() << " "  << collit->getAbsorbed() << endl; //<< " " << collit->getSfront() << " " << collit->getSback() << endl;
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
        if(mode==1){
          rename(inputcollimator, collimator);
        }
        if(mode==2)rename(inputfort, fort);
        if(dontusesix==1){
            rename(allabsorptionsnew, allabsorptions);
            rename(collgapsnew, collgaps);
        }
    }
    void filerenameback(){
        if(mode==1){
          rename(collimator,inputcollimator);
        }
        if(mode==2)rename(fort,inputfort);
        rename(firstimpacts,firstimpactsnew); //rename all the files that were used
        rename(allabsorptions,allabsorptionsnew);
        rename(flukafile,flukafilenew);
        rename(collgaps, collgapsnew);
    }

};
