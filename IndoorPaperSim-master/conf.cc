#include "conf.h"
#include <string>
#include "vector_math.h"
#include <cstdlib>

extern void setActiveEdge(long antennaId, antenna * p, 
        long nearestNodeID, map<long, node*>& nodes); 





conf* conf::pconf = NULL;

void conf::node_read(const string& filename){
//read in graph_node.csv, construct a map of nodes with id

    ifstream nodesfile(filename.c_str());
if(!nodesfile){
    cout<<"Cannot open nodes.csv!!!"<<endl;
}

string line;
getline(nodesfile, line);//skip the first row
while(getline(nodesfile, line)){

char *linec = new char[line.size()+1];
strcpy(linec, line.c_str());
char *pch = strtok(linec, ",");
//cout<<pch<<endl;
long nodeid = atol(pch);
strtok(NULL, ",");//skip x in units cm
strtok(NULL, ",");//skip y in units cm
pch = strtok(NULL,","); //read in type
string type(pch);
pch = strtok(NULL,",");//read in x in meter
   // cout<<pch<<endl;
double x = atof(pch);
pch = strtok(NULL,",");
   // cout<<pch<<endl;
double y = atof(pch);
node *n = new node(x, y);

if(type.compare("ROOM") == 0){
    n->type = node::ROOM;
}
else if (type.compare("HALLWAY") == 0){
    n->type = node::HALLWAY;
}
else{
    n->type = node::DOOR;
}

nodes.insert(pair<long, node*> (nodeid, n));

}
nodesfile.close();
}

void conf::edge_read(const string& filename){
 //read in the graph_edge.csv, add adjEdge to nodes
ifstream edgefile(filename.c_str());
if(!edgefile){
    cout<<"cannot open file edges.csv!!!\n";
}

string line;
getline(edgefile, line);//skip the first line
while(getline(edgefile, line)){
    char *linec = new char[line.size()+1];
    strcpy(linec, line.c_str());
    char *pch = strtok(linec, ",");
    int edgeID = atoi(pch);
    pch = strtok(NULL, ",");
    long nodeid1 = atol(pch);
    pch = strtok(NULL, ",");
    long nodeid2 = atol(pch);
    

    double cost = nodes[nodeid1]->distance(nodes[nodeid2]);
    edges.insert(pair<int, edge*>(edgeID, new edge(nodeid1,nodeid2, cost)));
#ifdef DEBUG
    cout<<nodeid1<<" to "<<nodeid2<<": "<<cost<<endl;
#endif
    nodes[nodeid1]->addAdjNode(nodeid1, nodeid2, cost);
    nodes[nodeid2]->addAdjNode(nodeid2, nodeid1, cost);
}
edgefile.close();

}

void conf::antenna_read(const string& filename){
//read in the antennas.csv, add nearest node to antenna
ifstream antennafile("antennas.csv");
if(!antennafile){
    cout<<"cannot open file antennas.csv!!!\n";
}

string line;
getline(antennafile, line);
while(getline(antennafile, line)){
    char *linec = new char[line.size()+1];
    strcpy(linec, line.c_str());
    char *pch = strtok(linec, ",");
    long antennaID = atol(pch);
    pch = strtok(NULL, ",");
    double x = atof(pch);
    pch = strtok(NULL, ",");
    double y = atof(pch);
    antenna *atn = new antenna(x,y);
    antennas.insert(pair<long, antenna*>(antennaID, atn));
    
    pair<long, node*> nodeNearest = *min_element(nodes.begin(), nodes.end(),nodelt(atn));
#ifdef DEBUG 
    cout<<"antenna"<<antennaID<<" is nearest to node";
    cout<<(nodeNearest.first)<<endl;
    cout<<" distance is "<<atn->distance(nodeNearest.second)<<endl;
#endif
    set<long> queue, visited;
    queue.clear();
    visited.clear();
    queue.insert(nodeNearest.first);
    
    while(!queue.empty()){
        long src = *queue.begin();//take the first element from queue and span from it
        queue.erase(queue.begin());//remove the first element
        visited.insert(src);//mark this node as visited
    vector<edge>& adjEdges = nodes[src]->adjEdge;
    for (int i = 0; i < adjEdges.size(); i++){
          int dst = adjEdges[i].dst;
         if (visited.count(dst) == 0){
             //for every unvisited node
         node* dstNode = nodes[dst];
         atn->addEdge(adjEdges[i]);
#ifdef DEBUG
         cout<<"Affected Edges of antenna"<<antennaID<<": ";
         cout<<adjEdges[i].src<<" "<<adjEdges[i].dst<<endl;
#endif
         adjEdges[i].setActive(antennaID);
         for(int j = 0; j < dstNode->adjEdge.size(); j++){
             if (dstNode->adjEdge[j].dst == src){
                 dstNode->adjEdge[j].setActive(antennaID);
             }
         }

        if (dstNode->distance(atn) < R/2){
            queue.insert(dst);
            }
         }
    }
    }
}
antennafile.close();

}

void conf::room_read(const string& filename){
ifstream roomfile(filename.c_str());
if(!roomfile){
    cout<<"cannot open file rooms.csv!!!\n";
}
ofstream room2("rooms2.csv");
string line;
getline(roomfile, line);//skip the first line
while(getline(roomfile, line)){
    char *linec = new char[line.size()+1];
    strcpy(linec, line.c_str());
    char *pch = strtok(linec, ",");
    int roomID = atoi(pch);
    if (roomID < 27){
    pch = strtok(NULL, ",");
    int nodeid1 = atol(pch);
    pch = strtok(NULL, ",");
    int nodeid2 = atol(pch);
    pch = strtok(NULL, ",");
    pch = strtok(NULL, ",");
    pch = strtok(NULL, ",");
    pch = strtok(NULL, ",");
    pch = strtok(NULL, ",");
    double left1 = atof(pch);
    pch = strtok(NULL, ",");
    double left2 = atof(pch);
    pch = strtok(NULL, ",");
    double right1 = atof(pch);
    pch = strtok(NULL, ",");
    double right2 = atof(pch);
    room2<<roomID<<","<<left1<<","<<left2<<","<<right1<<","<<right2<<endl;
    room *p = new room(roomID, left1, left2, right1, right2, nodeid1, nodeid2);
    rooms[roomID] = p;
    rtree->Insert(p->a_min, p->a_max, roomID);
    }

    else{
     pch = strtok(NULL, ",");
     int direction;
     if (strcmp(pch, "vertical")==0){
       direction = hallway::vertical;
     }
     else{direction = hallway::horizontal;}
     pch = strtok(NULL, ",");
     double edge_cor = atof(pch);
     pch = strtok(NULL, ",");
     pch = strtok(NULL, ",");
     pch = strtok(NULL, ",");
     pch = strtok(NULL, ",");
     pch = strtok(NULL, ",");
     double left1 = atof(pch);
    pch = strtok(NULL, ",");
    double left2 = atof(pch);
    pch = strtok(NULL, ",");
    double right1 = atof(pch);
    pch = strtok(NULL, ",");
    double right2 = atof(pch);
    hallway *h = new hallway(roomID, left1, left2, right1, right2, direction, edge_cor);
    hallways[roomID] = h;
    rtree->Insert(h->a_min, h->a_max, roomID);
    }
}

roomfile.close();
room2.close();
}

conf::conf(){
    }

map<long, node*>& conf::getNodes() {
    return nodes;
}

map<long, antenna*>& conf::getAntennas() {
    return antennas;
}

map<int, room*>& conf::getRooms(){
    return rooms;
}

map<int, hallway*>& conf::getHallways(){
    return hallways;
}

map<int, edge*>& conf::getEdges(){
    return edges;
}

RTree<int, double, 2>* conf::getRtree(){
    return rtree;
}

double conf::getRange(){
    return R;
}

void conf::initialize(const string& file1, const string& file2, const string& file3, const string& file4, double range){
    if (!pconf){
    pconf = new conf;
    pconf-> rtree = new RTree<int, double, 2>;
    pconf-> node_read(file1);
    pconf-> edge_read(file2);
    pconf-> antenna_read(file3);
    pconf-> room_read(file4);
    pconf-> R = range;
    }
}

conf* conf::get(){
    return pconf;
}

