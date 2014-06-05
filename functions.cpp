#include <fstream>

using namespace std;

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
