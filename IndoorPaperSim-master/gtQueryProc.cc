#include "gtQueryProc.h"
#include "RTree.h"
#include "node.h"
#include <fstream>
#include "rawDataSimulator.h"
#include "conf.h"
#include <boost/algorithm/string.hpp>

#define rtree (conf::get()->getRtree())
#define nodes (conf::get()->getNodes())

extern bool MySearchCallback(int id, void* arg);


gtQueryProc* gtQueryProc::p = NULL;

gtQueryProc::gtQueryProc(){
}

void gtQueryProc::initialize(){
    if (p == NULL){
    p = new gtQueryProc;
    
ifstream file("groundtruth.txt");
if(!file){
    cout<<"Cannot open groundtruth.txt"<<endl;
}
map<int, hallway*> hallways = conf::get()->getHallways();
map<int, room*> rooms = conf::get()->getRooms();

string line;
for (int i = 1; i <= rawDataSimulator::numObj; i++){
    for (int j = 0; j < rawDataSimulator::timeInSec; j++){
        getline(file, line);
    }
    
    vector<string> strs;
    boost::split(strs, line, boost::is_any_of(" "));

    int srcNode = atoi(strs[4].c_str());
    int dstNode = atoi(strs[5].c_str());

    double disTodst = atof(strs[6].c_str());
    
    p->groundtruth[i]=Particle::location(srcNode,dstNode, disTodst);
    if (nodes[srcNode]->type == node::ROOM || nodes[dstNode]->type == node::ROOM){
     //the object is in a room
     int roomID;
    map<int, room*>::iterator it = rooms.begin();
    for(; it != rooms.end(); it++){
        if(it->second->nodeIn[0]==srcNode || it->second->nodeIn[1] == srcNode){
         roomID = it->first;
         break;
        }
    }

    p->roomHT[roomID].push_back(i);
    }
    else if (nodes[srcNode]->type == node::DOOR || nodes[dstNode]->type == node::DOOR){
        int doorNode, hallwayNode;
        if (nodes[srcNode]->type == node::DOOR){
            doorNode = srcNode;
            hallwayNode = dstNode;
        }
        else{
            doorNode = dstNode;
            hallwayNode = srcNode;
        }

        map<int, hallway*>::iterator it = hallways.begin();
        for(; it!=hallways.end(); it++){
#ifdef DEBUG
cout<<it->first<<endl;
#endif

            if(it->second->contain(nodes[hallwayNode]->x, nodes[hallwayNode]->y)){
                if (it->second->direction == hallway::horizontal){
            p->hallwayHT[it->first].push_back(make_pair(i,nodes[hallwayNode]->x));
                }
                else{
            p->hallwayHT[it->first].push_back(make_pair(i,nodes[hallwayNode]->y));
                }
               // break;
            }
        }
    }

    else{
        //in hallway case
         map<int, hallway*>::iterator it = hallways.begin();
        for(; it!=hallways.end(); it++){
#ifdef DEBUG
cout<<it->first<<endl;
#endif
        if (it->second->contain(nodes[srcNode]->x, nodes[srcNode]->y)&& it->second->contain(nodes[dstNode]->x, nodes[dstNode]->y)){

            if (it->second->direction == hallway::horizontal){

            double t = nodes[dstNode]->x + ((nodes[srcNode]->x > nodes[dstNode]->x)?1:-1)*disTodst;
            p->hallwayHT[it->first].push_back(make_pair(i, t));
            }
            else{
            double t = nodes[dstNode]->y + ((nodes[srcNode]->y > nodes[dstNode]->y)?1:-1)*disTodst;
            p->hallwayHT[it->first].push_back(make_pair(i, t));
            }
            break;
        }
        }
    }
}
}
}

gtQueryProc* gtQueryProc::get(){
    if (p != NULL){
    return p;
    }
}

TYPE gtQueryProc::query(const rect<double>& r){
map<int, hallway*> hallways = conf::get()->getHallways();
map<int, room*> rooms = conf::get()->getRooms();

vector<int>* hit = new vector<int>;
rtree->Search(r.a_min, r.a_max, MySearchCallback, hit);
TYPE result;
for (int i = 0; i < hit->size(); i++){
    int id = (*hit)[i];
    if( id<27){
        //room
        double ratio = rooms[id]->intersect(r).getArea()/rooms[id]->getArea();
        vector<int> objectsIn = roomHT[id];
        for (int j = 0; j < objectsIn.size(); j++){
        ParticleFilter::index tempObj(objectsIn[j], ratio);
        result.push_back(tempObj);
        }
    }
    else{
        //hallway
        vector<pair<int, double> > index = hallwayHT[id];

        if (hallways[id]->direction == hallway::horizontal){
         for (int j = 0; j < index.size(); j++){
                if (index[j].second > r.a_min[0] && index[j].second < r.a_max[0]){
                    rect<double> temp = hallways[id]->intersect(r);
                    double ratio = (temp.a_max[1]-temp.a_min[1])/(hallways[id]->a_max[1]-hallways[id]->a_min[1]);
                     ParticleFilter::index tempObj(index[j].first, ratio);

                    result.push_back(tempObj);
                }
            }
        }
        else{
         for(int j=0; j<index.size(); j++){
             if(index[j].second>r.a_min[1] && index[j].second<r.a_max[1]){
                 rect<double> temp = hallways[id]->intersect(r);

                 double ratio = (temp.a_max[0]-temp.a_min[0])/(hallways[id]->a_max[0]-hallways[id]->a_min[0]);
                    ParticleFilter::index tempObj(index[j].first, ratio);

                    result.push_back(tempObj);

                 }
         }
        }
    }
}
return result;
}

const map<int, Particle::location>& gtQueryProc::getGT() const{
    return groundtruth;
}
