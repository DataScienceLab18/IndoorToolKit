#include <fstream>
#include <string>
using namespace std;
int main(int args, char* argv[]){

    double dis = atof(argv[1]);
    string filename = string("atn") + to_string((int) dis) + string(".csv");
    ofstream outfile(filename);
    int atn = 1;
    double x = 8.5;
    double y = 0;

    while(y <= 25){
      outfile<<atn<<" "<<x<<" "<<y<<endl;
      atn++;
      y += 3;
      y += dis;
    }

    x = 8.5+3;
    y = 23.8;
    while(x<=45){
      outfile<<atn<<" "<<x<<" "<<y<<endl;
      atn++;
      x=x+3+dis;
    }

    x=36.55;
    y=23.8-3;
    while(y>=0){
        outfile<<atn<<" "<<x<<" "<<y<<endl;
        atn++;
        y=y-3-dis;
    }
    x=8.5+3;
    y=10.2;
    while(y<36.55-3){
        outfile<<atn<<" "<<x<<" "<<y<<endl;
        atn++;
        y=y+3+dis;
    }
}

