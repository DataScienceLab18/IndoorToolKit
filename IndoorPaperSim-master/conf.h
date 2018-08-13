#ifndef _conf_h
#define _conf_h
#include "node.h"
#include "antenna.h"
#include "RTree.h"
#include <fstream>
#include <iostream>
#include <string>
#include "string.h"
#include "stdlib.h"
#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include "room.h"
#include "hallway.h"
using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::vector;
using std::set;
using std::pair;

class conf{
//fields
    private:
map<long, node*> nodes;
map<long, antenna*> antennas;
map<int, edge*> edges;
map<int, room*> rooms;
map<int, hallway*> hallways;
RTree<int, double, 2> *rtree;
double R; //range of readers
//private method, are going to be called in constructor
    private:
void node_read(const string& filename);
void edge_read(const string& filename);
void antenna_read(const string& filename);
void room_read(const string& filename);
//disable its default constructor
conf();
static conf* pconf;
//public
    public:
static void initialize(const string& file1, const string& file2, const string& file3, const string& file4, double range);
static conf* get();
map<long, node*>& getNodes() ;
map<long, antenna*>& getAntennas();
map<int, room*>& getRooms();
map<int, hallway*>& getHallways();
map<int, edge*>& getEdges();
RTree<int, double, 2>* getRtree();
double getRange();

class nodelt{
    public:
        node *n;
        nodelt(node* n){
         this->n = n;
        }
      bool  operator () (pair<long,node*> n1, pair<long, node*> n2){
        return (n->distance(n1.second) < n->distance(n2.second));}
};

};



#endif




