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
#include <algorithm>
#include <sstream>
#include <math.h>

#include "STabs.h" //class for absorption list of SixTrack
#include "allcolls.h" //class for the collimator properties
#include "partdata.h" //class for particle properties
#include "particles.h" //class for particle lists
#include "aperdata.h" //class for aperturedata
#include "aperturelist.h" //class for aperture lists
#include "run.h" //The algorithm

using namespace std;

void welcomescreen(){
    cout << endl;
    cout << "##########################################" << endl;
    cout << "## AUTOMATED COLLIMATOR SIMULATION TOOL ##" << endl;
    cout << "##########################################" << endl << endl << endl;
}

bool operator== ( const STabs &S1, const STabs &S2)
{
  return S1.PID == S2.PID;
}


int main()
{
    welcomescreen();
    run simulation;
    if(simulation.execute()==1)cout << "Finished" << endl;
    return 0;
}
