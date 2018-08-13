#ifndef _queryProc_h
#define _queryProc_h
#include "ParticleFilter.h"
#include "rect.h"
#include <vector>
using namespace std;

class queryProc{
        public:
        static vector<ParticleFilter::index> query(const rect<double>& r);
};

#endif
