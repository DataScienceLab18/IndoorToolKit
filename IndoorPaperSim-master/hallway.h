#ifndef _hallway_h
#define _hallway_h
#include "rect.h"

class hallway: public rect<double> {

    public:
        static const int horizontal = 1;
        static const int vertical = 2;

        int id;
        int direction; //horizontal or vertical
        double edge_cor;//center graph edge's coordinate 
        hallway(int id1, double d1, double d2, double d3, double d4, int dr, double edge): id(id1), rect(d1, d2, d3, d4), direction(dr), edge_cor(edge){
        
        
        
        }
};

#endif
