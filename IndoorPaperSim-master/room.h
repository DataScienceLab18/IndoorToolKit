#ifndef _rooms_h
#define _rooms_h
#include <vector>
#include <utility>
#include "rect.h"
using namespace std;
class room: public rect<double> {
    public:
        int id;
        int nodeIn[2]; //nodes inside this room, used in determine anchor points inside the room
        room(int id, double d1, double d2, double d3, double d4, int n1, int n2);
        vector<pair<double, double> > getAnchors();//get anchor points inside this room
};


#endif
