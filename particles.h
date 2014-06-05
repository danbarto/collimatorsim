using namespace std;

#include "functions.cpp"

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
        //parts.sort();
    return 0;
    }

    void tracksort(){
      parts.sort();
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
