#include "object.h"
#include "conf.h"
#include "edge.h"
#include "ShortestPath.h"
#include <queue>
#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>

#define nodes (conf::get()->getNodes())
#define maxNode 3074
#define minNode 3000
//#define DEBUG
static boost::mt19937 rng;

object::object(int s, int d, double dis): Particle(s,d, dis, 0),timer(3)
{
}

/*
void object::move(){
if(l.disTodst >= speed){
    //continue move on the current edge
    l.disTodst -= speed;
    //reset timer
    timer = 3;
}
else if(l.disTodst > 0){
    timer = 3;
    //the particle is going to finish the current edge, and have to choose a new edge starting from l.dst
   if((nodes[l.dst]->type) == node::ROOM) {
       //if the dst is room, then let the particle stay at the room
       l.disTodst = 0;
   }
   else{
       nextEdge();  
   }
}
else{
    //the particle falls on a node, either by accident on a non-room type node, or purposely being set to a room node.
    if((nodes[l.dst]->type) == node::ROOM){
        if(timer == 0){
           nextEdge();
        }
        else{timer--;}
    }
    else{
        nextEdge();
    }
}

}
*/

void object::nextEdge(){
    //we assume the object will
    //go straight down on hallway 
    //with p = 0.8
  vector<edge> adjEdges = nodes[l.dst]->adjEdge;
       int choice = adjEdges.size();
      
       boost::uniform_int<> nd(1, choice);

      boost::variate_generator<boost::mt19937&, 
                                     boost::uniform_int<> > var_nor(rng, nd);
      int random = var_nor();

   if((nodes[l.dst]->type) != node::ROOM&& l.dst != 3000 && l.dst != 3059 && l.dst != 3043){
       //when the node is initially in a room node, it can go 
       //to any edge connected from this node
        do{        
      //for non room node, the particle shouldn't go back to src node
       //
      random = var_nor(); 
       while(adjEdges[random - 1].dst == l.src){
           random = var_nor();
       }
       //reduce the p of entering a room
        }while(nodes[adjEdges[random-1].dst]->type == node::DOOR && !prob_enterRoom(0.3));
   }
   
   if (l.dst == 3043){
       do{
           random = var_nor();
       }
       while(nodes[adjEdges[random-1].dst]->type == node::DOOR && !prob_enterRoom(0.3));
   }

      l.src = l.dst;
      l.dst = adjEdges[random - 1].dst;
      l.disTodst = edgeLength() - (speed-l.disTodst);
      if (l.disTodst < 0){
          Particle::nextEdge();
      }
}



bool object::prob_enterRoom(double p){

     boost::uniform_real<> nd(0.0, 1.0);

      boost::variate_generator<boost::mt19937&,  boost::uniform_real<> > var_nor(rng, nd);
      return var_nor() < p;

}


void object::move(){
      boost::uniform_int<> nd(minNode, maxNode);

      boost::variate_generator<boost::mt19937&, 
                                     boost::uniform_int<> > var_nor(rng, nd);
       if(myqueue.empty()){
       //if queue is empty, then regenerate a target
       int targetRoom = var_nor();
       while(nodes[targetRoom]->type != node::ROOM || targetRoom==nextNode){
          targetRoom = var_nor();
      }
#ifdef DEBUG
 cout<<"src node "<<l.dst<<" dst node "<<targetRoom<<endl;
#endif
      //random generate a target room as destination
      vector<int> path = ShortestPath::getPath(l.dst, targetRoom);

      for(int i = 1; i < path.size(); i++){
        myqueue.push(path[i]);
      }
          myqueue.push(targetRoom);
      }

if(l.disTodst >= speed){
    //continue move on the current edge
    l.disTodst -= speed;
    //reset timer
    timer = 3;
}
else if(l.disTodst > 0){
    timer = 3;
    //the particle is going to finish the current edge, and have to choose a new edge starting from l.dst
   if((nodes[l.dst]->type) == node::ROOM) {
       //if the dst is room, then let the particle stay at the room
       l.disTodst = 0;
   }
   else{
       nextNode = myqueue.front();
       myqueue.pop();
       double dis = 0;
       vector<edge> adjEdges = nodes[l.dst]->getAdjEdges();
       for(int i=0; i<adjEdges.size();i++){
           if(adjEdges[i].dst == nextNode){
               dis=adjEdges[i].cost;
               break;
           }
       }
       l.src = l.dst;
       l.dst = nextNode;
       l.disTodst=dis-(speed-l.disTodst);
       
   }
}
else{
    //the particle falls on a node, either by accident on a non-room type node, or purposely being set to a room node.
    if((nodes[l.dst]->type) == node::ROOM){
        if(timer == 0){
           nextNode = myqueue.front();
           myqueue.pop();
           double dis = 0;
           vector<edge> adjEdges = nodes[l.dst]->getAdjEdges();
           for(int i=0; i<adjEdges.size();i++){
           if(adjEdges[i].dst == nextNode){
               dis=adjEdges[i].cost;
               break;
           }
       }
       l.src = l.dst;
       l.dst = nextNode;
       l.disTodst=dis-(speed-l.disTodst);

        }
        else{timer--;}
    }
    else{
        nextNode = myqueue.front();
           myqueue.pop();
           double dis = 0;
           vector<edge> adjEdges = nodes[l.dst]->getAdjEdges();
           for(int i=0; i<adjEdges.size();i++){
           if(adjEdges[i].dst == nextNode){
               dis=adjEdges[i].cost;
               break;
           }
       }
       l.src = l.dst;
       l.dst = nextNode;
       l.disTodst=dis-(speed-l.disTodst);

    }
}

    
}
