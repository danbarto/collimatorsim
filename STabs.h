using namespace std;

class STabs{
private:
  double position;
  int PID, turn;
public:
  STabs(){
    PID=0;turn=0;position=0.;
  }
  STabs(int p){
    PID=p;turn=0;position=0.;
  }
  STabs(int p, int t, double pos){
    PID=p;turn=t;position=pos;
  }
  int getPID(){
    return PID;
  }
  int getTurn(){
    return turn;
  }
  double getPosition(){
    return position;
  }

  friend bool operator== ( const STabs &S1, const STabs &S2);

  ~STabs(){}

};
