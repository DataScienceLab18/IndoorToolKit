#ifndef _kNNs_h
#define _kNNs_h
#include "ParticleFilter.h"
#include "rect.h"
#include <vector>
using namespace std;
typedef ParticleFilter::index myOPpair;//object prob pair
class kNN_s{
        public:
        static vector<myOPpair> query(int src, int dst, double dis, int k);
};

#endif
