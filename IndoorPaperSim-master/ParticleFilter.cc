#include "ParticleFilter.h"
#include "Particle.h"
#include "node.h"
#include "edge.h"
#include "rawDataSimulator.h"
#include <vector>
#include <iostream>
#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_real.hpp>
#include "converter.h"
#include "conf.h"
#include "Particle.h"
#include <math.h>
#include "antenna.h"
#include "gtQueryProc.h"
#include <algorithm>
using namespace std;


#define antennas (conf::get()->getAntennas())
#define nodes (conf::get()->getNodes())
#define range (conf::get()->getRange())

typedef antenna atn;
static boost::mt19937 rng;


bool myMax(ParticleFilter::index2 a, ParticleFilter::index2 b){
    return a.p < b.p;
}
bool myComp(ParticleFilter::index2 a, ParticleFilter::index2 b){
    return a.p>b.p;
}
map<pair<double, double>, vector<ParticleFilter::index> > ParticleFilter::global;
map<pair<double, double>, vector<ParticleFilter::index> > ParticleFilter::global2;

map<int, vector<ParticleFilter::index2> > ParticleFilter::objectHash;
void ParticleFilter::initialize(int numOfParticles, long antennaID){
    particles.clear();
    numOfP = numOfParticles;
    vector<edge> affectedEdges = antennas[antennaID]->getCoveredEdges();
     do{
     boost::uniform_int<> nd(1, affectedEdges.size());

      boost::variate_generator<boost::mt19937&, 
                                     boost::uniform_int<> > var_nor(rng, nd);
      int random = var_nor();
      edge e = affectedEdges[random - 1];
     //the above code decides the edge
     boost::uniform_real<> nd2(0.0, e.cost);

     boost::variate_generator<boost::mt19937&,  boost::uniform_real<> > var_nor2(rng, nd2);
     double dis = var_nor2();
     //convert an on edge point to a general point
     node p = converter::convert(e.src, e.dst, dis);
     if (p.distance(antennas[antennaID]) < range/2){
         particles.push_back(new Particle(e.src, e.dst, dis, 1.0/numOfParticles));
     }
     //change the particle's direction
     p = converter::convert(e.dst, e.src, dis);
      if (p.distance(antennas[antennaID]) < range/2){
         particles.push_back(new Particle(e.dst, e.src, dis, 1.0/numOfParticles));
     }
     }while(particles.size() < numOfParticles);
}


void ParticleFilter::move(){
    for (int i = 0; i < particles.size(); i++){
        particles[i]->move();
    }
}


void ParticleFilter::reWeight(long atnID){
    double sumOfWt = 0;
    for (int i = 0; i < particles.size(); i++){
        node p = converter::convert(particles[i]->l.src, particles[i]->l.dst, particles[i]->l.disTodst);
        if (atnID!=0){
        double dis = p.distance(antennas[atnID]); 
        if(dis < range/2){
            particles[i]->weight = 1;
         }
        else{
            particles[i]->weight = exp(-30*(dis-range/2));
            //particles[i]->weight = 0.1;
        }
            sumOfWt += particles[i]->weight;
        }
        
        else{
        pair<long, atn*> atnNearest = *min_element(antennas.begin(), antennas.end(),conf::nodelt(&p));
        double dis = atnNearest.second->distance(&p); 

        if (dis < range/2){
            //need a high order, since results show order of 2 is 
            //sharp enough
            //particles[i]->weight = pow(dis/(range/2), 8);
            particles[i]->weight = 0; 
        }
        else{
            particles[i]->weight = 1;
        }
            sumOfWt += particles[i]->weight;

        }
        
    }
   for (int i = 0; i < particles.size(); i++){
       particles[i]->weight /= sumOfWt;
   }
}


void ParticleFilter::reSample(){
double w[particles.size()];
vector<Particle*> temp = particles;
particles.clear();
double sum = 0;
for (int i = 0; i < temp.size(); i++){
    sum += temp[i]->weight;
    w[i] = sum;
}

int j = 0;
for (int i = 0; i < temp.size(); i++){
       while(w[j] < (2*i + 1.0)/( temp.size() * 2) )
        j++;
    particles.push_back(new Particle(temp[j]->l.src, temp[j]->l.dst, temp[j]->l.disTodst, 1.0/temp.size()));
}

}

void ParticleFilter::print(){
    for (int i = 0; i < particles.size(); i++ ){
        cout<<"Particle #"<<i<<": ";
        cout<<particles[i]->l.src<<" "<<particles[i]->l.dst<<" "<<particles[i]->l.disTodst<<"  weight: "<<particles[i]->weight<<"  speed: "<<particles[i]->speed<<endl;
    }
}

void ParticleFilter::toAnchors(int tagID){
    map<pair<double, double>, int> anchors,anchors2;

    int srcID, dstID;
    double x_cor, y_cor;
    for (int i = 0; i < particles.size(); i++){
       srcID = particles[i]->l.src;
       dstID = particles[i]->l.dst;
//for the purpose of range query, the edge segment from hallway to door is projected to one point only
       if(nodes[srcID]->type==node::DOOR && nodes[dstID]->type == node::HALLWAY){
               x_cor=nodes[dstID]->x;
               y_cor=nodes[dstID]->y;
       
           if (nodes[srcID]->x == nodes[dstID]->x){
             anchors[make_pair(ceil(x_cor-0.5), y_cor)]++;
           }
           else{
             anchors[make_pair(x_cor, ceil(y_cor-0.5))]++;
           }
       }
       else if (nodes[dstID]->type==node::DOOR && nodes[srcID]->type == node::HALLWAY){
             x_cor=nodes[srcID]->x;
             y_cor=nodes[srcID]->y;
       
           if (nodes[srcID]->x == nodes[dstID]->x){
             anchors[make_pair(ceil(x_cor-0.5), y_cor)]++;
           }
           else{
             anchors[make_pair(x_cor, ceil(y_cor-0.5))]++;
           }

    }
       else{
           //not on the edge DOOR to HALLWAY
       //check whether the particle is on a 
       //horizontal or vertical graph edge
       if (nodes[srcID]->x == nodes[dstID]->x){
           //vertical case
           x_cor = nodes[srcID]->x;
           y_cor = ((nodes[srcID]->y > nodes[dstID]->y)? 1: -1)*particles[i]->l.disTodst + nodes[dstID]->y;
           pair<double, double> cor = make_pair(x_cor, ceil(y_cor-0.5));
           anchors[cor] ++;//anchors[cor] would initially be 0 if 
                          //cor is a new key 
       
       }     

       else{
           //horizontal case
           y_cor = nodes[srcID]->y;
           x_cor = ((nodes[srcID]->x > nodes[dstID]->x)? 1: -1)*particles[i]->l.disTodst + nodes[dstID]->x;
           pair<double, double> cor = make_pair(ceil(x_cor-0.5), y_cor);
           anchors[cor] ++;
         
        }
       }


       }
        
     for (int i = 0; i < particles.size(); i++){
       srcID = particles[i]->l.src;
       dstID = particles[i]->l.dst;
       //check whether the particle is on a 
       //horizontal or vertical graph edge
       if (nodes[srcID]->x == nodes[dstID]->x){
           //vertical case
           x_cor = nodes[srcID]->x;
           y_cor = ((nodes[srcID]->y > nodes[dstID]->y)? 1: -1)*particles[i]->l.disTodst + nodes[dstID]->y;
           pair<double, double> cor = make_pair(x_cor, ceil(y_cor-0.5));
           anchors2[cor] ++;//anchors[cor] would initially be 0 if 
                          //cor is a new key 
         }            

       else{
           //horizontal case
           y_cor = nodes[srcID]->y;
           x_cor = ((nodes[srcID]->x > nodes[dstID]->x)? 1: -1)*particles[i]->l.disTodst + nodes[dstID]->x;
           pair<double, double> cor = make_pair(ceil(x_cor-0.5), y_cor);
           anchors2[cor] ++;
          

    }
       }

    
    
    //display part to check result           
    map<pair<double, double>, int>::iterator it = anchors.begin();
    for(; it != anchors.end(); it++){
#ifdef DEBUG

        cout<<(*it).first.first<<" "<<(*it).first.second;
        cout<<": "<<(*it).second<<endl;
#endif
        struct index2 temp2(it->first.first, it->first.second, it->second/(double) numOfP);
        objectHash[tagID].push_back(temp2);
        struct index temp(tagID, it->second/ (double) numOfP);
        global[it->first].push_back(temp);

    }

 map<pair<double, double>, int>::iterator it2 = anchors2.begin();
    for(; it2 != anchors2.end(); it2++){
        struct index temp_kNN(tagID, it2->second/ (double) numOfP);
        global2[it2->first].push_back(temp_kNN);
      }
}

void ParticleFilter::showAnchors(){
    map<pair<double, double>, vector<index> >::iterator i = global.begin();
    for (; i != global.end(); i++){
    cout<<i->first.first<<" "<<i->first.second<<" ";
        for (int j = 0; j< i->second.size(); j++){
            cout<<(i->second)[j].tagID<<" "<<(i->second)[j].p<<" ";
        }
        cout<<endl;
    }
}

void ParticleFilter::showObjectHash(){
    map<int, vector<struct index2> >::iterator it = objectHash.begin();

    for (; it!=objectHash.end();it++){
        cout<<"tagID: "<<it->first<<endl;
        vector<struct index2>::iterator itt=it->second.begin();
        for(; itt!=it->second.end();itt++){
            cout<<itt->anchor.first<<" "<<itt->anchor.second;
            cout<<" "<<itt->p<<endl;
        }
    }


}

pair<double, double> ParticleFilter::top1(){
    int numObj = rawDataSimulator::numObj;
    map<int, Particle::location> gtLocation = gtQueryProc::get()->getGT();
    //iterate through objects
    double top1_hit = 0;
    double top2_hit = 0;
    for(int i = 1; i <= numObj; i++){
    vector<index2>::iterator it_max=max_element(objectHash[i].begin(), objectHash[i].end(), myMax);
    nth_element(objectHash[i].begin(), objectHash[i].begin()+1,objectHash[i].end(),myComp);
    //the iterator to second large prob element
    vector<index2>::iterator it_second=objectHash[i].begin()+1;
    const Particle::location gt = gtLocation[i];
    node n= converter::convert(gt.src, gt.dst, gt.disTodst);
    double dis = sqrt(pow(it_max->anchor.first-n.x, 2) + pow(it_max->anchor.second-n.y, 2));
    double dis2 = sqrt(pow(it_second->anchor.first-n.x, 2) + pow(it_second->anchor.second-n.y, 2));

#ifdef DEBUG
    cout<<"tagID: "<<i<<" gt Location: "<<n.x<<" "<<n.y<<endl;
#endif
    if (dis <= 2){
        top1_hit++;
    }
    if (dis <= 2||dis2<=2){
        top2_hit++;
    }
}
    return make_pair(top1_hit/numObj, min((top2_hit)/numObj,1.0));
}

ParticleFilter::~ParticleFilter(){
    for(int i=0; i<particles.size(); i++){
        delete particles[i];
    }

}
