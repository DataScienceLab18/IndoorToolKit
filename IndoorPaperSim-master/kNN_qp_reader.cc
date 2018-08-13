#include "kNN_qp_reader.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>

kNN_qp_reader* kNN_qp_reader::p=NULL;

kNN_qp_reader::kNN_qp_reader(string filename){
    ifstream file(filename.c_str());
    if(!file){
      cout<<"Cannot open file "<<filename<<endl;
    }
    string line;
    while(getline(file, line)){
    vector<string> tokens;
    boost::split(tokens, line, boost::is_any_of(","));
    int rectID=atoi(tokens[0].c_str());
    int src=atoi(tokens[1].c_str());
    int dst=atoi(tokens[2].c_str());
    double dis=atof(tokens[3].c_str());
    qps.insert(make_pair(rectID,location(src, dst, dis)));
    }
}

map<int, location> kNN_qp_reader::getQueryPoint(){
    return qps;
}

void kNN_qp_reader::initialize(string filename){
    if(p==NULL){
        p=new kNN_qp_reader(filename);
    }
}

kNN_qp_reader* kNN_qp_reader::get(){
    if(p!=NULL){
    return p;
    }
}




