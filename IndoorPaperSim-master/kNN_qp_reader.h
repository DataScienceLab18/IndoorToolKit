#ifndef _kNN_qp_reader_h
#define _kNN_qp_reader_h
#include "Particle.h"
#include <map>
#include <string>
using namespace std;

typedef struct mystruct{
    int src, dst;
    double dis;
    mystruct(int s, int d, double disIn){
        src=s;
        dst=d;
        dis=disIn;
    }
}location;


class kNN_qp_reader{
  private:
      map<int, location> qps;//rectID->center point location
      kNN_qp_reader(string filename);
      static kNN_qp_reader* p;
  public:
      map<int, location> getQueryPoint();
      static void initialize(string filename);
      static kNN_qp_reader* get();
};


#endif
