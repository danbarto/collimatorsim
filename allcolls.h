using namespace std;

class allcolls{
private:
    string collname, material;
    int collnumber, type, absorbedParticles; //type 0=no collimator, 1=H, 2=V, 3=H+V
    double s_center, s_front, s_back, length, halfGap;
public:
    allcolls(){
        collname="";material="";collnumber=0;type=0;s_center=0.;length=0.;s_front=s_center-length/2.;s_back=s_center+length/2.;halfGap=0.;absorbedParticles=0;
    }
    allcolls(string name, string mat, int number, int t, double pos, double len, double op){
        collname=name;material=mat;collnumber=number;type=t;s_center=pos;length=len;s_front=pos-len/2.;s_back=pos-len/2.;halfGap=op;absorbedParticles=0;
    }
    allcolls(string name, int num, double pos){
        collname=name;material="";collnumber=num;type=0;s_center=pos;length=0.;s_front=0.;s_back=0.;halfGap=0.;absorbedParticles=0;
    }

    int absorbedInColl(double spos){//nur front! funktioniert das?
        if(spos>=s_center-0.01 && spos <s_center+0.01){//changed <= to <
            absorbedParticles++;
            //cout << spos << endl;
            return collnumber;
        }
        else return 0;
    }

    int findColl(double givPosLast, double givPosAct){ //check if BOTH positions are in a collimator area (safety check, should be true)
        if(givPosLast>=s_front && givPosLast<s_back && givPosAct>=s_front && givPosAct<s_back) return 1;//changed <= to <
        else return 0;
    }
    int isInRightArea(double aperture, double lX, double aX, double lY, double aY){
        double ipApCol, angleR1, angleR2,anglePart;
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

    double getSfront(){return s_front;}
    double getSback(){return s_back;}
    double getHalfGap(){return halfGap;}
    int getType(){return type;}
    int getCollNumber(){return collnumber;}
    void ResetAbsorbed(){absorbedParticles=0;}
    double getLength(){return length;}
    string getMaterial(){return material;}
    void updateCenter(double shift){
        s_center=s_center+shift;s_front=s_front+shift;s_back=s_back+shift;
    }
    void updateData(string mat, int nT, double len, double op){
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

    bool operator<(const allcolls& other) const {
      return s_front < other.s_front;
    }

    ~allcolls(){}

};
