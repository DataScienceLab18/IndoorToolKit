#include "kNN_Hua.h"
#include "conf.h"
#include "huaRtree.h"
#include <algorithm>
#include <limits>
#define width 3.0 //assume all rects are 3 by 3
#define rectHT huaRtree::rectHT //rect to objects
#define objToRects huaRtree::objToRects
#define range (conf::get()->getRange())
kNN_Hua* kNN_Hua::p=NULL;
typedef struct mystruct{
    int obj;
    double s, l;
    mystruct(int o, double sIn, double lIn){
        obj=o;
        s=sIn;
        l=lIn;
    }
}entry;

static bool myComp(entry e1, entry e2){
    return e1.l<e2.l;
}

class myComp2{
    public:
        int obj;
        myComp2(int objID){
        obj=objID;
        }
        bool operator()(entry e){
            return obj==e.obj;
        }
};

void kNN_Hua::init_rectConn(){
    rectConn[27].push_back(28);
    for(int i=28; i<46; i++){
        rectConn[i].push_back(i-1);
        rectConn[i].push_back(i+1);
     }
    rectConn[46].push_back(45);
    rectConn[30].push_back(55);
    rectConn[31].push_back(55);



     for(int i=48; i<54; i++){
        rectConn[i].push_back(i-1);
        rectConn[i].push_back(i+1);
     }
 
    rectConn[47].push_back(48);
    rectConn[47].push_back(45);
    rectConn[47].push_back(44);

    rectConn[54].push_back(53);
    rectConn[51].push_back(63);
    rectConn[50].push_back(63);

    for(int i=56; i<63;i++){
       rectConn[i].push_back(i-1);
       rectConn[i].push_back(i+1);
     }
    rectConn[55].push_back(56);
    rectConn[55].push_back(31);
    rectConn[55].push_back(30);

    rectConn[63].push_back(62);
    rectConn[63].push_back(50);
    rectConn[63].push_back(51);
}



double kNN_Hua::networkDis(int rectID1, int rectID2)
{
  vector<int> frontier;
  vector<int> visited;
  double dis=width/2;

  if(rectID1 == rectID2){return 0;}

  frontier.insert(frontier.end(),rectConn[rectID1].begin(), rectConn[rectID1].end());
  visited.push_back(rectID1);//so won't search backwards from adjacent rects of rectID1

  while(find(frontier.begin(), frontier.end(), rectID2) == frontier.end()){
      //while haven't searched to dst ID
      visited.insert(visited.end(),frontier.begin(), frontier.end());
      dis+=width;
      int size=frontier.size();
      for(int i=0; i<size;i++){
          
          for(int j=0; j<rectConn[frontier[i]].size(); j++){
              int adjNode=rectConn[frontier[i]][j];
              if(find(visited.begin(),visited.end(),adjNode)==visited.end()){
                  //if adjNode is not visited yet
                  frontier.push_back(adjNode);
              }
          }
      }
      //remove the first "size" elements from frontier
      frontier.erase(frontier.begin(), frontier.begin()+size);
  }

  return dis;
}

kNN_Hua::kNN_Hua(){
    init_rectConn();
}

void kNN_Hua::initialize(){
    if(p==NULL){
        p=new kNN_Hua();
    }
}

kNN_Hua* kNN_Hua::get(){
    if(p!=NULL){
    return p;
    }
}
//this method finds the smallest and largest distance of an object's possible rects to query point's rect
void kNN_Hua::find_s_l(vector<int> rects, int rectQ, double& sIn, double& lIn){
   double s=numeric_limits<double>::max();
   double l=numeric_limits<double>::min();
   for(int i=0; i< rects.size(); i++){
      if(s>networkDis(rectQ, rects[i])){
          s=networkDis(rectQ, rects[i]);
      }
      if(l<networkDis(rectQ, rects[i])){
          l=networkDis(rectQ, rects[i]);
      }
   }
   sIn=s;
   //if the only element in rects happen to be rectQ
   if(l==0){
       lIn=l+range/2;
   }
   else{
       lIn=l+range;
   }
}





vector<int> kNN_Hua::query(int rect, int k){
 
    vector<entry> candidates;
    vector<int> visited;
    vector<int> frontier;
    double f=numeric_limits<double>::max();
//add all objects in the rect to candidates
    for(int i=0;i<rectHT[rect].size();i++){
        int objID=rectHT[rect][i].tagID;
        vector<int> rects=objToRects[objID];
        double s, l;
        find_s_l(rects, rect, s, l);
        entry e(objID, s, l);
        candidates.push_back(e);

    }
    visited.push_back(rect);
    frontier.insert(frontier.end(),rectConn[rect].begin(), rectConn[rect].end());

    if(candidates.size()>=k){
        nth_element(candidates.begin(), candidates.begin()+k-1, candidates.end(), myComp);
        f=candidates[k].l;
    }
//all elements in frontier have the same distance to q
//in simpliefied model.
    while(!frontier.empty() && networkDis(rect, frontier[0])<f){ 
        int currentSize = frontier.size();
        //todo: remove from 0 to currentSize-1 from frontier
        visited.insert(visited.end(),frontier.begin(), frontier.end());


        for(int j=0; j<currentSize; j++){
           int rectID=frontier[j];
           for(int i=0;i<rectHT[rectID].size();i++){
              int objID=rectHT[rectID][i].tagID;

              //if meet a new object which is not in candidates set, then push the new object and its s, l to candidates

              if(find_if(candidates.begin(), candidates.end(), myComp2(objID))==candidates.end()){
                 vector<int> rects=objToRects[objID];
                 double s, l;
                 find_s_l(rects, rect, s, l);
                 entry e(objID, s, l);
                 candidates.push_back(e);
              }
           }
                      //the for loop adds all unvisited adjacent rects of rectID to frontier
           for(int m=0; m<rectConn[rectID].size(); m++){
               if(find(visited.begin(), visited.end(), rectConn[rectID][m])==visited.end()){
                   //if the rect is not visited yet
                   frontier.push_back(rectConn[rectID][m]);
               }
           }

        }

        frontier.erase(frontier.begin(), frontier.begin()+currentSize);
//update f

        if(candidates.size()>=k){
        nth_element(candidates.begin(), candidates.begin()+k-1, candidates.end(), myComp);
        f=candidates[k].l;
    }

    }
//sort the candidates on l, return the first k objects as result
    sort(candidates.begin(), candidates.end(), myComp);
    vector<int> result;
    for(int i=0; i<k; i++){
      result.push_back(candidates[i].obj);
    }

    return result;
}






  


     



