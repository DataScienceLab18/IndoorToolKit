#ifndef _Particle_h
#define _Particle_h
#include "node.h"
#include <map>
using namespace std;
class Particle{
    //nested class, which represents the location of a particle. the particle is on edge src to dst(nodeId), its distance to dst node is distodst.
public:  class location{
 public:  long src, dst;
          double disTodst;
          location(){src = 0; dst = 0; disTodst = 0;}
          location(long s, long d, double dis):src(s), dst(d), disTodst(dis){}          };
//fields
location l;
double weight;
double speed;
//constructor
Particle(long s, long d, double disTodst, double w);
//
void move();
//private methods to help move()
protected:

// return the current edge's length
double edgeLength();
// choose the nextedge randomly, and set l;
virtual void nextEdge();

//if a particle is in a room, it will leave the room with probability 0.1, stay in the room with p 0.9.
bool prob_leaveRoom();
};

#endif
