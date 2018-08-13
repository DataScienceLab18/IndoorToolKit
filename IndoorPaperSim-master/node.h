#ifndef _node_h
#define _node_h
#include "math.h"
#include <vector>
#include "edge.h"

using std::vector;

class node{
//fields
    public:
static const int ROOM = 1, DOOR = 2, HALLWAY = 3;
int type;
double x, y;
std::vector<edge> adjEdge;

//constructor
node( double x, double y);
//add adjacent nodes and edges
 void addAdjNode(long id1, long id2, double cost);
// get the adjacent edges list
vector<edge> getAdjEdges();

//calculate distance between two coordinates
double distance(node *node2);
    };
#endif
