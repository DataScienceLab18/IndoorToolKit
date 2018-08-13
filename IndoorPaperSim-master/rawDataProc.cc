#include "rawDataProc.h"
#include <fstream>
#include "string.h"
#include <iostream>
#include "stdlib.h"
#include <boost/algorithm/string.hpp>
#define DEBUG11
rawDataProc::rawDataProc(int numO, int t): numOfobjects(numO), sampleTime(t){
 p = new vector<reading>;
 m = new map<int, vector<reading> >; 
}
/*this method reads the agg.txt into a 
 * vector, and process it by removing 
 * previous readings beyond two atns for
 * every tag
 */
void rawDataProc::proc(const string& file, int numatn){

    ifstream agg(file.c_str());
     if (!agg){
      cout<<"Cannot open "<<file <<endl;
      }

    string line;
    //read in the whole file to vector<reading> p
    while(getline(agg, line)){
    vector<string> tokens;
    boost::split(tokens,line, boost::is_any_of(" "));
    int currentTimeInS = atoi(tokens[1].c_str());
   
    int atnID = atoi(tokens[3].c_str());

    int tagID = atoi(tokens[5].c_str());
    p->push_back(reading(currentTimeInS, tagID, atnID));
   
    }
        for(int o = numOfobjects; o >= 1; o--){
        int start = o*sampleTime-1;
        for(int i = 0; (i<numatn)&&(start>o*sampleTime-1-sampleTime);i++){
        while((*p)[start].atnID == 0 &&(start>o*sampleTime-1-sampleTime)){
            start--;
        }
        int lastAtn = (*p)[start].atnID;
        while((*p)[start].atnID == lastAtn &&(start>o*sampleTime-1-sampleTime)){
            start--;
        }
        }
        p->erase(p->begin()+(o-1)*sampleTime, p->begin()+start+1);
    }

   agg.close(); 
}

map<int, vector<rawDataProc::reading> >* rawDataProc::sepObj(const string& file, int numatn){
    proc(file, numatn);
    for (int i = 0; i < (*p).size(); i++){
        (*m)[(*p)[i].tagID].push_back((*p)[i]);
    }
    //remove the starting 0s readings, because pf cannot handle initial 0
    map<int, vector<reading> >::iterator it = m->begin();
    for(; it != m->end(); it++){
       vector<reading> temp = it->second;
       int j = 0;
       while(temp[j].atnID ==0){j++;}
       it->second.erase(it->second.begin(), it->second.begin()+j);
    }
    delete p;
    //display result
    #ifdef DEBUG11
   it=m->begin();
   for(; it!=m->end(); it++){
       vector<reading> temp = it->second;
       for(int i=0; i<temp.size(); i++){
       cout<<temp[i].time<<" "<<temp[i].tagID<<" "<<temp[i].atnID<<endl;
   }
   }
#endif


    return m;
}
