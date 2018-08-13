#ifndef _object_h
#define _object_h
#include "Particle.h"
#include <queue>
using namespace std;
/*The object class represents moving objects
 *The key is Moving model
 *Previously we use random moving model
 *Modified to Random waypoint model 9/11/2012
 */

/*
class object: public Particle{
    private:
       int timer;//stay 3s in a room, this variable is to record remaining time
       void nextEdge();
       bool prob_enterRoom(double p);
    public:
       object(int s, int d, double dis);
       void move();
};
*/

class object: public Particle{
    private:
       int timer;//stay 3s in a room, this variable is to record remaining time
       void nextEdge();
       bool prob_enterRoom(double p);
    public:
       object(int s, int d, double dis);
       queue<int> myqueue;
       int nextNode;
       void move();
};


#endif
