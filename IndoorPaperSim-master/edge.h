#ifndef _edge_h
#define _edge_h
#include <vector>
using namespace std;

class edge{
    //fields
    public:
    long src, dst;
double cost;
vector<long> activeAntenna;
//constructor
edge(long srcid, long dstid, double cost) {
    src = srcid;
    dst = dstid;
    this->cost = cost;
}
//methods for generating raw readings
void setActive(long antennaID){
    activeAntenna.push_back(antennaID);
}

bool isActive(){
    return activeAntenna.size()>0;
}

//method for kNN_s,find anchor points on an edge
//@param ap the previous anchor point on this edge
static bool findAPfrom(int src, int dst, pair<double, double>& ap);

};


#endif
