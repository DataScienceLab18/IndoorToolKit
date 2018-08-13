#include <fstream>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <stdlib.h>

using namespace std;
int main(int arg, char* argv[]){
    
    ifstream file("top1.txt");
    string line;
    double total1=0;
    double total2=0;
    //double huatotal=0;
    int linecount=0;
    while(getline(file, line)){
        vector<string> tokens;
        boost::split(tokens, line, boost::is_any_of(" "));
        double top1=atof(tokens[1].c_str());
        double top2=atof(tokens[3].c_str());
        total1+=top1;
        total2+=top2;
       // pftotal+=pfKL;
        linecount++;
    }

    ofstream fileout("top1Final.txt",ios::app);
    fileout<<total1/linecount<<" "<<total2/linecount<<endl;
    fileout.close();
}


