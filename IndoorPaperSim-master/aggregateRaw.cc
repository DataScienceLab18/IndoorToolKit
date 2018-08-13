#include "aggregateRaw.h"
#include<fstream>
#include<iostream>
#include "string.h"
#include "stdlib.h"
#include <map>
using namespace std;

void aggregateRaw::aggregate(char* inFile, char* outFile){
 ifstream raw(inFile);
  if (!raw){
      cout<<"Cannot open "<<inFile<<endl;
  }
  
  ofstream out(outFile);
  string line;
  map<int, int> time_atn;
  int lineNum = 0;
  int start_time, end_time;
  while(getline(raw, line)){
      char *linec = new char[line.size()+1];

    strcpy(linec, line.c_str());
    char *pch = strtok(linec, " ");
    pch = strtok(NULL, " ");
    int time = (int) atof(pch);

    if(lineNum == 0){
     start_time = time;
    lineNum++;
    }
    end_time = time;
    pch = strtok(NULL, " ");
    pch = strtok(NULL, " ");
    int atnID = atoi(pch);
    time_atn.insert(pair<int, int>(time, atnID));
  }
    
    
    raw.close();
   
    for (int i = start_time; i <= end_time; i++){
        if (time_atn.count(i) > 0){
            out<<"timestamp "<<i<<" atn "<<time_atn[i]<<endl;
        }
        else{
  out<<"timestamp "<<i<<" atn 0"<<endl;
        }
    }

    out.close();
}


  
