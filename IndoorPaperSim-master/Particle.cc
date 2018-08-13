#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include "Particle.h"
#include "conf.h"
#include "edge.h"

#define nodes (conf::get()->getNodes())
static boost::mt19937 rng;

Particle::Particle(long s, long d, double disTodst, double w){
    l.src = s;
    l.dst = d;
    l.disTodst = disTodst;
    weight = w;
 //below is to generate random speed with 
 //normal distribution(1, 0.2)
 
   
    boost::normal_distribution<> nd(1.0, 0.2);

      boost::variate_generator<boost::mt19937&, 
                                     boost::normal_distribution<> > var_nor(rng, nd);
      speed = var_nor();
     
}

 
//model particle's move, once per second
void Particle::move(){
if(l.disTodst >= speed){
    //continue move on the current edge
    l.disTodst -= speed;
}
else if(l.disTodst > 0){
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
        if(prob_leaveRoom()){
           nextEdge();
        }
    }
    else{
        nextEdge();
    }
}

}

bool Particle::prob_leaveRoom(){
      boost::uniform_real<> nd(0.0, 1.0);

      boost::variate_generator<boost::mt19937&,  boost::uniform_real<> > var_nor(rng, nd);
      return var_nor() < 0.2;

}


double Particle::edgeLength(){
vector<edge> adjEdges = nodes[l.src]->adjEdge;
double edgeLength;
for(int i = 0; i < adjEdges.size(); i++){
    if (adjEdges[i].dst == l.dst){
        edgeLength = adjEdges[i].cost;
        break;
    }
}

return edgeLength;
}

void Particle::nextEdge(){
      vector<edge> adjEdges = nodes[l.dst]->adjEdge;
       int choice = adjEdges.size();
      
       boost::uniform_int<> nd(1, choice);

      boost::variate_generator<boost::mt19937&, 
                                     boost::uniform_int<> > var_nor(rng, nd);
      int random = var_nor();

   if((nodes[l.dst]->type) != node::ROOM&& l.dst != 3000 && l.dst != 3059 && l.dst != 3043){
       //when the node is initially in a room node, it can go 
       //to any edge connected from this node
                
      //for non room node, the particle shouldn't go back to src node
       //
       
       while(adjEdges[random - 1].dst == l.src){
           random = var_nor();
       }
   }

      l.src = l.dst;
      l.dst = adjEdges[random - 1].dst;
      l.disTodst = edgeLength() - (speed-l.disTodst);
      if (l.disTodst < 0){
          Particle::nextEdge();
      }
}
