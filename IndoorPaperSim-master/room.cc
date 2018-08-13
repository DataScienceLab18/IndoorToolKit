#include "room.h"
#include "conf.h"
#include <algorithm>
#include <math.h>
#define nodes (conf::get()->getNodes())

//constructor
room::room(int id1, double d1, double d2, double d3, double d4, int n1, int n2): id(id1), rect(d1, d2, d3, d4){
   nodeIn[0] = n1;
   nodeIn[1] = n2;
    
}

vector<pair<double, double> > room::getAnchors(){
   double x1 = nodes[nodeIn[0]]->x;
   double y1 = nodes[nodeIn[0]]->y;
   double x2 = nodes[nodeIn[1]]->x;
   double y2 = nodes[nodeIn[1]]->y;
   vector<pair<double, double> > result; 
//horizontal case
   if (y1 == y2){
    //horizontal case
    double y_cor = y1;
    double x_min = min(x1, x2);
    double x_max = max(x1, x2);
   
    for(int i = floor(x_min+0.5); i <= floor(x_max+0.5); i++){
        result.push_back(make_pair(i, y_cor));
    }
}
else{

    //vertical case
double x_cor = x1;
double y_min= min(y1, y2);
double y_max= max(y1, y2);

    for(int i = floor(y_min+0.5); i <= floor(y_max+0.5); i++){
        result.push_back(make_pair(x_cor, i));
    }

}
   
   
   return result;
   
   }


