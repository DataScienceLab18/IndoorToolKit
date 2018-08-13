#include <fstream>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <stdlib.h>

using namespace std;
int main(int arg, char* argv[]){
    
    ifstream file("KL.txt");
    string line;
    double pftotal=0;
    double huatotal=0;
    int linecount=0;
    while(getline(file, line)){
        vector<string> tokens;
        boost::split(tokens, line, boost::is_any_of(" "));
        double huaKL=atof(tokens[1].c_str());
        double pfKL=atof(tokens[3].c_str());
        huatotal+=huaKL;
        pftotal+=pfKL;
        linecount++;
    }

    ofstream fileout("KLFinal.txt",ios::app);
    fileout<<"pf: "<<pftotal/linecount<<" hua: "<<huatotal/linecount<<endl;
    fileout.close();
}


