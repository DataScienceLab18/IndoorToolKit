#ifndef _gtQueryProc_h
#define _gtQueryProc_h
#include "ParticleFilter.h"
#include "rect.h"
#include "Particle.h"
#include <string>
typedef vector<ParticleFilter::index> TYPE;
using namespace std;

class gtQueryProc{
    private:
    map<int, vector<int> > roomHT;// room hash table recording objects inside it
    map<int, vector<pair<int, double> > > hallwayHT; //hallway hash table recording objects in it and their location along the length of hallway
    map<int, Particle::location> groundtruth;
    gtQueryProc();
    static gtQueryProc *p;

    public:
    static void initialize();
    static gtQueryProc* get();
    TYPE query(const rect<double>& r);
    const map<int, Particle::location>& getGT() const;


};

#endif
