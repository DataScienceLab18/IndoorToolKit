#ifndef _huaRtree_h
#define _huaRtree_h
#include <string>
#include <map>
#include "rect.h"
#include "RTree.h"
#include "ParticleFilter.h"
#include <vector>
#include "rawDataProc.h"
using namespace std;
class huaRtree{

    public:
        static map<int, rect<double>*> huaRects;
        static RTree<int, double, 2> rtree;
        static map<int, vector<ParticleFilter::index> > rectHT;
        static map<int, vector<int> > objToRects;
        static map<int, vector<int> > atnTorect;
        static void buildRtree(const std::string& file);
        static void buildAtnToRect(const string& file);
        static void addTorectHT(map<int, vector<rawDataProc::reading> >* m);
        static vector<ParticleFilter::index> query(const rect<double>& q); 
};


#endif
