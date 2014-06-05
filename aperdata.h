using namespace std;

class aperdata{ //Class for aperture data
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

    void shift(double shiftval){
      s0+=shiftval;
      s1+=shiftval;
      s2+=shiftval;
      if(s2>shiftval){
        s2=shiftval;
        s1=(s2-s0)/2.+s0;
        stili5_b=(s2-s0)/2.;
        }
    }

    int showshape(){return shape;}
    ~aperdata(){}

    bool operator<(const aperdata& other) const {
      return s1 < other.s1;
    }

    void outputaper(){
        cout << apername << " " << s0 << " " << s1 << " " << s2 << " " << stili5_b << " " << apx << " " << apy << " " << apr <<endl;
    }

};
