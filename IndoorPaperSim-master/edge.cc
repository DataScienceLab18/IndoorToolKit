#include "edge.h"
#include "conf.h"
#define nodes (conf::get()->getNodes())

bool edge::findAPfrom(int src, int dst, pair<double, double>& ap){
    //first check the edge is vertical or horizontal
    if(nodes[src]->x==nodes[dst]->x){
        //vertical
      if(nodes[src]->y>nodes[dst]->y){
         //moving downwards
        if(ap.second-1<nodes[dst]->y){
          return false;
        }
        else{
          ap.second--;
          return true;
        }
      }
      else{
         //moving upward
        if(ap.second+1>nodes[dst]->y){
          return false;
          }
        else{
          ap.second++;
          return true;
        }
      }
    }
    //horizontal
    else{
      if(nodes[src]->x>nodes[dst]->x){
          //moving to left
        if(ap.first-1<nodes[dst]->x){
            return false;
        }
        else{
            ap.first--;
            return true;
        }
      }
      //moving to right
      else{
         if(ap.first+1>nodes[dst]->x){
             return false;
         }
         else{
             ap.first++;
             return true;
         }
      }
    }
}
