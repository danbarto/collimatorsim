using namespace std;

class aperturelist : public aperdata{
private:
    int shaperef, shapelast;
    list<aperdata> aplist;
    list<aperdata>::iterator itref, itlast;
    string c1;
    double c2, c3, c4, c5, c6, c7, length;//,ringlength;
public:
    aperturelist(){
    }

    void create(double shift){
        char aperturefile[50];
        sprintf(aperturefile, "%s", "aperture2.dat");
        if(checkfile(aperturefile)==1){
            ifstream inaper;
            ofstream outaper;
            inaper.open("aperture2.dat");
            outaper.open("aperture_shifted.dat");
            while (!inaper.eof()){
                inaper >> c1 >> c2 >> c3 >> c4 >> c5 >> c6 >> c7;
                if( inaper.eof() ) break;
                aplist.push_back(aperdata(c1,c2-shift,c3-shift,c4-shift,c5,c6,c7));
            }
            length=c4;
            itref=aplist.begin();
            while(itref!=aplist.end()){
                if(itref->shows0()<0){
                    if(itref->shows2()>0){
                        aplist.push_back(aperdata(itref->showap(),0,(itref->shows2())/2.,itref->shows2(),itref->shows2(),itref->showapx()*1000., itref->showapy()*1000.));
                    }
                    itref->shift(length);
                }
                itref++;
            }
            inaper.close();
            aplist.sort();
            itref=aplist.begin();
            while(itref!=aplist.end()){
                //cout << itref->showap() << "\t" << itref->shows0() << " " << itref->shows2() << " " << itref->showapx() << " " << itref->showapy() << endl;
                outaper << itref->showap() << "\t" << itref->shows0() << "\t" << itref->shows1() << "\t" << itref->shows2() << "\t" << itref->showstili5_b() << "\t" << itref->showapx() << "\t" << itref->showapy() << endl;
                itref++;
            }
            outaper.close();

        }
    }

    void empty(){
      aplist.clear();
      }

    void showaperture(){
        itref=aplist.begin();
        while(itref!=aplist.end()){
            cout << itref->shows0() << " " << itref->showap() << " " << itref->shows2() << endl;
            itref++;
        }
        itref=aplist.begin();
        cout << itref->shows0() << endl;
    }

    void makelength(){//kill
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
    double showsnowfront(){return itref->shows0();}
    double showslast(){return itlast->shows2();}
    double showslastfront(){return itlast->shows0();}
    double x(){return itref->showapx();}
    double y(){return itref->showapy();}
    double xlast(){return itlast->showapx();}
    double ylast(){return itlast->showapy();}
    int getshaperef(){return itref->showshape();}
    int getshapelast(){return itlast->showshape();}
};
