using namespace std;


class partdata{ // Class for particle data
private:
    int pid, turn, type, losstype;
    double spos, xpos, xp, ypos, yp, de, radius;

public:

    partdata(){
        pid=0; turn=0; spos=0; xpos=0; xp=0; ypos=0; yp=0; de=0; type=0; radius=0; losstype=0;
    }

    partdata(int p, int t, double s){
      pid=p;turn=t;spos=s;

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

    // bool compSpos(const partdata & a, const partdata & b) {
    //   //int t1=a.turn, t2=b.turn;
    //   if(a.turn==b.turn){
    //     return a.spos < b.spos;
    //   }
    //   else{
    //     return a.turn < b.turn;
    //   }
    // }

    bool operator<(const partdata& other) const {
      int t1=turn, t2=other.turn;
      if(t1==t2){
        return spos < other.spos;
      }
      return turn < other.turn;

    }

    const double getspos(){return spos;
    }

    void outputpart(){
        cout << pid << " " << turn << " " << spos << " " << xpos << " " << xp << " " << ypos << " " << yp << " " << de << " " << type << " " << radius << " " << losstype <<endl;
    }
};
