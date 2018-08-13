#include "rawDataSimulator.h"
#include <fstream>
#include "conf.h"
#include "vector_math.h"
#include <iostream>
#include <stdlib.h>
#include "object.h"
#include <boost/algorithm/string.hpp>
#define nodes conf::get()->getNodes() 
#define antennas conf::get()->getAntennas() 
#define range conf::get()->getRange()
//#define DEBUG
int rawDataSimulator::timeInSec = 0;
int rawDataSimulator::numObj=0;
//not used
void rawDataSimulator::fromUser(){
   // cout<<"How many objects would you like to simulate?: ";
    //cin>>numOfObjects;
    cout<<"How long in seconds would you like to simulate?: ";
    cin>>timeInSec;
}

void rawDataSimulator::fromUser(int time){
    timeInSec = time;

}
/*this method reads in the init.txt, which
 * records the initial position of a bunch of
 * objects, and reads in the first (@param) numObjSm lines
 * to simulate people walking randomly 
 * the output is agg2.txt which is the sensor reading results
 * and groundtruth.txt which is people's true locaiton trace
 */
void rawDataSimulator::rawDataGene(int numObjSm){

ifstream init("init2.txt");
if (!init){
    cout<<"Cannot open file inti.txt!!!\n ";
}
string line;
ofstream raw("agg2.txt");
ofstream truth("groundtruth.txt");
numObj = 0;
while(getline(init, line) && numObj<numObjSm){
    numObj++;
#ifdef DEBUG
    cout<<"This is simulated object "<<numObj<<endl;
#endif
    vector<string> tokens;
    boost::split(tokens, line, boost::is_any_of(" "));
    int objectID = atoi(tokens[0].c_str());
    int srcNode = atoi(tokens[1].c_str());
    int dstNode = atoi(tokens[2].c_str());
    double dis = atof(tokens[3].c_str());
    object o(srcNode, dstNode, dis);
   
    for (int time = 0; time < timeInSec; time++){
        truth<<"Timestamp: "<<time<<" tagID: "<<objectID<<" "<< o.l.src <<" "<< o.l.dst<< " "<<o.l.disTodst<<endl;
        bool detected = false;

        node* n1 = nodes[o.l.src];
        node* n2 = nodes[o.l.dst];
        vector<edge> edges = n1->adjEdge;
        for(int i = 0; i < edges.size(); i++){
           if (edges[i].dst == o.l.dst){
              if( edges[i].isActive() ){
               vector<long> antennaIDs = edges[i].activeAntenna;
               vector_math stod_vector(n2->x - n1->x, n2->y - n1->y);
               vector_math unitVector = stod_vector.getUnit();
               node *location = new node(n2->x-o.l.disTodst*unitVector.x, n2->y - o.l.disTodst*unitVector.y);
               for(int j = 0; j < antennaIDs.size(); j++){
                  if (location->distance(antennas[antennaIDs[j]]) < range/2){
                      raw<<"Timestamp: "<<time<<" antennaID: "<<antennaIDs[j]<<" tagID: "<<objectID<<endl;
                      detected = true;
                  }
                 }
              delete location;

              }
           }
          
        }

       if (!detected){
                      //if the tag is not detectable by any atenna
         raw<<"Timestamp: "<<time<<" antennaID: 0"<<" tagID: "<<objectID<<endl;
        }
       o.move();
    }
}

init.close();
raw.close();
truth.close();



}
//not used
void rawDataSimulator::rawDataGene(const string& tracefilename){
//read in the trace file, and generate synthetic data

ifstream trace("trace");
if(!trace){
    cout<<"cannot open file trace\n ";
}
ofstream raw("raw.txt");
string line;
getline(trace, line);
long nodeID1 = atol(line.c_str());
double s_time = 0; //indicate the starting timestamp for a given edge
while(getline(trace, line)){
    long nodeID2 = atol(line.c_str());
    node* n1 = nodes[nodeID1];
    node* n2 = nodes[nodeID2];
    vector<edge> edges = n1->adjEdge;
    for(int i = 0; i < edges.size(); i++){
        if (edges[i].dst == nodeID2){
            int sampleTime = edges[i].cost/0.1;//suppose speed is 1m/s, and sample rate is 10hz.
; 
           if( edges[i].isActive() ){
               vector<long> antennaIDs = edges[i].activeAntenna;
               vector_math stod_vector(n2->x - n1->x, n2->y - n1->y);
               vector_math unitVector = stod_vector.getUnit();
           
              for(int timestamp = 0; timestamp < sampleTime; timestamp++){
                  
                  double movedDis = 0.1*timestamp;
                  vector_math movedVector = unitVector*movedDis;
                  node *location = new node(n1->x+movedVector.x, n1->y + movedVector.y);
                  bool detected = false;
                  for(int j = 0; j < antennaIDs.size(); j++){
                  if (location->distance(antennas[antennaIDs[j]]) < range/2){
                      raw<<"Timestamp: "<<s_time+ 0.1*timestamp <<" antennaID: "<<antennaIDs[j]<<endl;
                      detected = true;
                  }
                 }
                  if (!detected){
                      //if the tag is not detectable by any atenna
                      raw<<"Timestamp: "<<s_time+ 0.1*timestamp <<" antennaID: 0"<<endl;
                }
            }
           }
        s_time += sampleTime/10.0;

        }
        }
nodeID1 = nodeID2;
}
                 
trace.close();
raw.close();

}
