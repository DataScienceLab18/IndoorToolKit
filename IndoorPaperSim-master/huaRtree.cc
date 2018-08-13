#include "huaRtree.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <stdlib.h>
#include "rawDataProc.h"
#include "index_operator.inl"
using namespace std;
using namespace boost;

extern bool MySearchCallback(int id, void* arg);
map<int, rect<double>*> huaRtree::huaRects; 
RTree<int, double, 2> huaRtree::rtree;
map<int, vector<ParticleFilter::index> > huaRtree::rectHT;
map<int, vector<int> > huaRtree::atnTorect;
map<int, vector<int> > huaRtree::objToRects;

void huaRtree::buildRtree(const std::string& filename){
ifstream file(filename.c_str());
if (!file){
    cout<<"cannot open "<<filename<<endl;
}

string line;
while(getline(file, line)){
    vector<string> strs;
    boost::split(strs, line, boost::is_any_of(", "));
    int rectID = atoi(strs[0].c_str());
    rect<double> *p = new rect<double>(atof(strs[1].c_str()), atof(strs[2].c_str()), atof(strs[3].c_str()), atof(strs[4].c_str()));
    huaRects[rectID] = p;

    rtree.Insert(p->a_min, p->a_max, rectID);

}
file.close();
}

void huaRtree::buildAtnToRect(const string& filename){
ifstream file(filename.c_str());
if (!file){
    cout<<"cannot open "<<filename<<endl;
}

string line;
while(getline(file, line)){
    vector<string> strs;
    boost::split(strs, line, boost::is_any_of(", "));
    int atnID = atoi(strs[0].c_str());
    for (int i = 1; i < strs.size(); i++){
        atnTorect[atnID].push_back(atoi(strs[i].c_str()));
    }
//the first item in this vector is the atn rect, others arerects accessible from the atn.
}
file.close();
}

void huaRtree::addTorectHT(map<int, vector<rawDataProc::reading> >* m){
map<int, vector<rawDataProc::reading> >::iterator it = m->begin();
#ifdef DEBUG
//display atnTorect
for (int i = 1; i < 20; i++){
    cout<<"atn "<<i<<": ";
    for(int j = 0; j < atnTorect[i].size(); j++){
        cout<<atnTorect[i][j]<<" ";
    }
    cout<<endl;
}
#endif
for(; it!=m->end(); it++){
   vector<rawDataProc::reading> temp = it->second;
   int last = temp.size()-1;
   if(temp[last].atnID != 0){
       //last entry is non zero atnID, still in activation range
       ParticleFilter::index t(it->first, 1);
       int rectID = atnTorect[temp[last].atnID][0];
       rectHT[rectID].push_back(t);
       objToRects[it->first].push_back(rectID);
   }
   else{
       //the last entry is zero atnID
       while(temp[last].atnID == 0){last--;};
       double totalArea = 0;
       int lastAtnID = temp[last].atnID;
       for(int i = 1; i < atnTorect[lastAtnID].size(); i++){
          int rectID = atnTorect[lastAtnID][i];
          totalArea += huaRects[rectID]->getArea();
          if(rectID>=27){
          objToRects[it->first].push_back(rectID);
          }
       }

    for(int i = 1; i < atnTorect[lastAtnID].size(); i++){
       int rectID = atnTorect[lastAtnID][i];
       ParticleFilter::index t(it->first, huaRects[rectID]->getArea()/totalArea);
       rectHT[rectID].push_back(t);
#ifdef DEBUG
       cout<<rectID<<" inserts index "<<t.tagID<<", "<<t.p<<endl;
    
#endif
    }
   }
}
}

TYPE huaRtree::query(const rect<double>& q){

    vector<int>* hit = new vector<int>;
    rtree.Search(q.a_min, q.a_max, MySearchCallback, hit);
    
    TYPE result;
    for(int i = 0; i < hit->size(); i++){
         int rectID = (*hit)[i];
         double ratio = (huaRects[rectID]->intersect(q)).getArea()/huaRects[rectID]->getArea();
         result += rectHT[rectID]*ratio;
    }

    return result;
}
