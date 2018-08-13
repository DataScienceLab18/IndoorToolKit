#ifndef _ParticleFilter_h
#define _ParticleFilter_h

#include "Particle.h"
#include "rawDataProc.h"
#include <utility>
/*This class has two main responsibilities.
 *1. drive the method in particle class
 *2. assign particles to anchor points and store
 *   the statistics to map anchors
 */
class ParticleFilter{
    private:
        vector<Particle*> particles;
        int numOfP;
    public:
        struct index{
            int tagID;
            double p;
            index(int ID, double p1): tagID(ID), p(p1){
            }
        };
        struct index2{
            pair<double, double> anchor;
            double p;
            index2(double x, double y, double prob){
                anchor.first = x;
                anchor.second = y;
                p = prob;
            }
        };
        static map<int, vector<index2> > objectHash;
        //this hashtable is used for range query
        static map<pair<double, double>, vector<index> > global; 
        //this hashtable is used for kNN query
        static map<pair<double, double>, vector<index> > global2;  
        //this method will distribute particles around an antenna
        //@param numOfparticles the number of Particles used
        //@param antennaID the antenna which will concentrate particles
        void initialize(int numOfParticles, long antennaID);
        //Destructor is very important here,otherwise too many moving objects' PFs are going to fill out the memory, throw "bad_alloc"error. Since this class holds many "new" particles
        ~ParticleFilter();
        void move();
        //if there is no reading, then don't do reweight;
        //but in case there is a reading at this timestamp, need to do reweighting
        void reWeight(long atnID);
        void reSample();
        void print();
        void toAnchors(int tagID);
        /*this method prints out data in the "global" Map
         */
        static void showAnchors();
        static void showObjectHash();
        /*this method calculates the top1 success rate of filtered result within 2m of the groundtruth data
         */
        static pair<double, double> top1();
};

#endif

