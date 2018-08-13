#include "node.h"


node::node(double x, double y){
    this->x = x;
    this->y = y;
   }

/*
 * double node::distance(node *node2){
    double dx = torad(x - node2->x);
    double dlong = torad(y - node2->y);
    double x1 = torad(x);
    double x2 = torad(node2->x);
    double R = 6371; //km, earth's radius

    double a = sin(dx/2)*sin(dx/2) + 
               sin(dlong/2) * sin(dlong/2)*cos(x1)*cos(x2);
    double c = 2*atan2(sqrt(a), sqrt(1-a));
    double d = R*c*1000; //m
    return d;
}
*/

double node::distance(node* n){
    double dis = sqrt((this->x - n->x)*(this->x - n->x) + (this->y - n->y)* (this->y - n->y));
    return dis;
}



void node::addAdjNode(long id1, long id2, double cost) {
    edge e(id1, id2, cost);
    adjEdge.push_back(e);
}

vector<edge> node::getAdjEdges() {
    return adjEdge;
}
