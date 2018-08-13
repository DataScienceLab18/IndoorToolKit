#include "kNN_s.h"
#include "conf.h"
#include "index_operator.inl"
#define nodes (conf::get()->getNodes())
#define AP2ObjHT (ParticleFilter::global2)
typedef ParticleFilter::index myOPpair;

typedef struct entry{
    int node, nodePrev;
    pair<double, double> ap;
}myentry;

static double getTotalProb(vector<myOPpair>& result){
    double p=0;
    for(int i=0; i<result.size();i++){
      p+=result[i].p;
    }
    return p;
}


vector<myOPpair> kNN_s::query(int src, int dst, double dis, int k){
    vector<myOPpair> result;
    vector<myentry> frontier; //to be exploreed edges 
    pair<double, double> first_ap;
    if(nodes[src]->x==nodes[dst]->x){
        first_ap.first=nodes[src]->x;

        if(nodes[src]->y<nodes[dst]->y){
          first_ap.second=floor(nodes[dst]->y-dis);
        }
        else{
          first_ap.second=floor(nodes[dst]->y+dis);
        }
    }
    else{
        first_ap.second=nodes[src]->y;

        if(nodes[src]->x<nodes[dst]->x){
            first_ap.first=floor(nodes[dst]->x-dis);
        }
        else{
            first_ap.first=floor(nodes[dst]->x+dis);
        }
    }

    result+=AP2ObjHT[first_ap];
    myentry e1, e2;
    e1.node=src;
    e1.nodePrev=dst;
    e1.ap=first_ap;

    e2.node=dst;
    e2.nodePrev=src;
    e2.ap=first_ap;

    frontier.push_back(e1);
    frontier.push_back(e2);
    bool going=(getTotalProb(result)<k);    
      while(going){
      int i=0;
      while(going&&i<frontier.size()){
         if(edge::findAPfrom(frontier[i].nodePrev,frontier[i].node, frontier[i].ap)){
             //find the next anchor point on frontier[i] edge, return true case
             result+=AP2ObjHT[frontier[i].ap];
             going = (getTotalProb(result)<k);
             i++;
         }
         else{
             //return false case, should expand from frontier[i].node
             int node=frontier[i].node;
             for(int j=0; j<nodes[node]->adjEdge.size();j++){
                 if(nodes[node]->adjEdge[j].dst!=frontier[i].nodePrev){
                     myentry temp;
                     temp.node=nodes[node]->adjEdge[j].dst;
                     temp.nodePrev=node;
                     if(nodes[temp.node]->x==nodes[temp.nodePrev]->x){
                         temp.ap.first=nodes[temp.node]->x;
                         if(nodes[temp.node]->y>nodes[temp.nodePrev]->y){
                           temp.ap.second=ceil(nodes[temp.nodePrev]->y);
                         }
                         else{
                           temp.ap.second=floor(nodes[temp.nodePrev]->y);
                         }
                     }
                     else{
                         temp.ap.second=nodes[temp.node]->y;
                         if(nodes[temp.node]->x>nodes[temp.nodePrev]->x){
                           temp.ap.first=ceil(nodes[temp.nodePrev]->x);
                         }
                         else{
                           temp.ap.first=floor(nodes[temp.nodePrev]->x);
                         }
                     }
                     result+=AP2ObjHT[temp.ap];
                     frontier.push_back(temp);

                 }
             }
           frontier.erase(frontier.begin()+i);
           going = (getTotalProb(result)<k);

         }
      }
      }
      return result;
}
