#ifndef _antenna_h
#define _antenna_h
#include "node.h"
/* may consider remove this class in the future
 **/
class antenna: public node {
   private:
   vector<edge> coveredEdges; 
   
    public:
        antenna(double x, double y);
        void addEdge(edge e){
            coveredEdges.push_back(e);
        }
        vector<edge> getCoveredEdges(){
            return coveredEdges;
        }
};

#endif

