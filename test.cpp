#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <algorithm>
#include <sstream>
#include <math.h>

#include "partdata.h" //class for particle properties
#include "particles.h" //class for particle lists

int main(){
  list<partdata> testlist;
  list<partdata>::iterator testit=testlist.begin();
  testlist.push_back(partdata(1,1,10.));
  testlist.push_back(partdata(1,2,5.));

  testit=testlist.begin();
  cout << testit->showspos() << endl;
  testit++;
  cout << testit->showspos() << endl;

  testlist.sort();
  testit=testlist.begin();
  cout << testit->showspos() << endl;
  testit++;
  cout << testit->showspos() << endl;

  return 0;


}
