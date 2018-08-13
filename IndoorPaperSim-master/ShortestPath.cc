#include "ShortestPath.h"
#include "conf.h"
#include <vector>
#include <algorithm>
#define nodes conf::get()->getNodes()
typedef struct myStruct{
    int first;//node ID
    double second;//dis to Src node
    int parent;//parent node ID
    myStruct(int id, double dis, int pid){
        first=id;
        second=dis;
        parent=pid;
    }
}entry;
//this method is used in priority queue
bool disCompGreater(entry e1, entry e2){
    return e1.second>e2.second;
}
class IDEqual{
    private:
        int ID;
    public:
        IDEqual(int IDtofind){
            ID=IDtofind;
        }
       bool operator()(entry e){
           return e.first==ID;
       }
};

vector<int> ShortestPath::getPath(int node1, int node2){
// a min heap to store frontier nodes
vector<entry> frontier;
vector<entry> visited;

vector<int> path;
entry e = myStruct(node1, 0, node1);
frontier.push_back(e);
make_heap(frontier.begin(), frontier.end(), disCompGreater);

while(!frontier.empty()){
    entry temp = frontier.front();
    pop_heap(frontier.begin(), frontier.end(),disCompGreater);
    frontier.pop_back();
    //the above pops the least distance node 
    int nid = temp.first;
    double dis = temp.second;
    visited.push_back(temp);
    if(nid==node2){break;}
    vector<edge> edges = nodes[nid]->getAdjEdges();
    for(int i = 0; i < edges.size(); i++){
        int link_node = edges[i].dst;
        // if the linked node is not expanded yet
        if (find_if(visited.begin(), visited.end(), IDEqual(link_node))==visited.end()){
           //calculate the dis(node1, temp.nid) + dis(temp.nid, link_node)
            double dis_linkNode = dis + edges[i].cost;
            vector<entry>::iterator it = find_if(frontier.begin(), frontier.end(), IDEqual(link_node));
            //if link_node is already in frontier, update its dis if necessary
            if(it!=frontier.end()&&it->second > dis_linkNode){
                it->second = dis_linkNode;
                it->parent = nid;
            }
            //if link_node is not in frontier, then add it 
            if(it==frontier.end()){
                entry e = myStruct(link_node, dis_linkNode, nid);
                frontier.push_back(e);
                push_heap(frontier.begin(), frontier.end(),disCompGreater);
            }
        }
    }
}
//if found node2
//if(!frontier.empty()){
int current = node2;
while(current!=node1){
vector<entry>::iterator it = find_if(visited.begin(), visited.end(), IDEqual(current));
path.push_back(it->parent);
current = it->parent;
}
//}
reverse(path.begin(), path.end());
return path;


}


