#include "converter.h"
#include "vector_math.h"
#include "conf.h"
#include "node.h"

#define nodes (conf::get()->getNodes())
node converter::convert(long src, long dst, double dis){
    node* n1 = nodes[src];
    node* n2 = nodes[dst];
    vector_math stod_vector(n2->x - n1->x, n2->y - n1->y);
    vector_math unitVector = stod_vector.getUnit();
    double movedDis = stod_vector.norm() - dis;
    vector_math movedVector = unitVector*movedDis;
    return node(n1->x+movedVector.x, n1->y + movedVector.y);
}
