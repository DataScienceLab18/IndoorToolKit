#include "queryProc.h"
#include "conf.h"
#include <iostream>
#include "index_operator.inl"
extern bool MySearchCallback(int id, void* arg);
#define tree (conf::get()->getRtree())
#define nodes (conf::get()->getNodes())
#define Anchors (ParticleFilter::global)

vector<ParticleFilter::index> queryProc::query(const rect<double>& r ) {
    
    map<int, hallway*> hallways = conf::get()->getHallways();
    map<int, room*> rooms = conf::get()->getRooms();

     vector<int>* hit = new vector<int>;
     tree->Search(r.a_min, r.a_max, MySearchCallback, hit);
      TYPE result; // vector<index> to hold query result
#ifdef DEBUG
     for(int i = 0; i < hit->size(); i++){
      cout<<"hit vector contains: "<<(*hit)[i]<<endl;
  }
#endif
     for(int i = 0; i < hit->size(); i++){

         int id = (*hit)[i];
         if (id < 27){
             //rooms with id 1-26
             
vector<pair<double, double> > anchors = rooms[id]->getAnchors();
TYPE result1;
for (int i = 0; i < anchors.size(); i++){

result1 += Anchors[anchors[i]];
}

//need to multiply the ratio of query in room part area with room area
double queryRoomArea = r.intersect((*rooms[id])).getArea();
double roomArea = rooms[id]->getArea();

result1 *= (queryRoomArea/roomArea);
result += result1;
}//close if room

else{
// if hallway
vector<pair<double, double> > anchors;
double ratio = 0.0;
if (hallways[id]->direction == hallway::horizontal){
    double leftedge = r.a_min[0];
    double rightedge = r.a_max[0];
    int left, right;
    if (leftedge > (int) leftedge){
        left = (int) leftedge + 1;
    }
    else{
        left = (int) leftedge;
    }

    right = (int) rightedge;
for(int i = left; i <= right; i++){
    anchors.push_back(make_pair(i, hallways[id]->edge_cor));
}

 rect<double> temp = r.intersect((*hallways[id]));
 ratio = (temp.a_max[1]-temp.a_min[1])/(hallways[id]->a_max[1]-hallways[id]->a_min[1]);
}
else{
    double bottomedge = r.a_min[1];
    double topedge = r.a_max[1];
    int bottom, top;
    if (bottomedge > (int) bottomedge){
         bottom = (int) bottomedge + 1;
    }
    else{
        bottom = (int) bottomedge;
    }

    top = (int) topedge;

    for(int i = bottom; i<=top; i++){
        anchors.push_back(make_pair(hallways[id]->edge_cor, i));
    }

 rect<double> temp = r.intersect((*hallways[id]));

   ratio = (temp.a_max[0]-temp.a_min[0])/(hallways[id]->a_max[0]-hallways[id]->a_min[0]);
}
TYPE result1;
    for(int i = 0; i < anchors.size(); i++){
        result1 += Anchors[anchors[i]];
    }
     result1 *= ratio;
     result += result1;


}//close else for hallway case

}//close for loop 

//because doing approximate calculation here, may occur cases where prob of an object is greater than 1, which will hurt the KL calculation. Clean up
for(int i=0; i<result.size(); i++){
    if(result[i].p>=1){
        result[i].p = 0.9999;
    }
}

return result; 
}




