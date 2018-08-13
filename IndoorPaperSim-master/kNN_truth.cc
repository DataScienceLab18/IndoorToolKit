#include "kNN_truth.h"
#include "gtQueryProc.h"
#include <map>
#include <algorithm>
#include "conf.h"
#include <limits>

#define nodes (conf::get()->getNodes())
kNN_truth* kNN_truth::p = NULL;

/*this method returns the distance from an object to node src
 * @param src src node ID
 * @param objsrc src node ID in object location tuple(src, dst, dis)
 */
static double myDistance(int src, int objsrc,int objdst, double dis){
   double edgelength=nodes[objsrc]->distance(nodes[objdst]);
   if(src==objsrc){
       return edgelength-dis;
   }
   else{
       return dis;
   }
}
/*this method is used for getting the kth nearest object in result objects
 */
static bool myComp(const pair<int, double>& obj1, const pair<int, double>& obj2){
    return obj1.second<obj2.second;
}
//the myheapComp is used for min heap
static bool myheapComp(const pair<int, double>& obj1, const pair<int, double>& obj2){
    return obj1.second>obj2.second;
}
/*this method returns the distance from q to the kth neareast
 * object. If result set has less than k elements, then return a very large number.
 */
static double dk(vector<pair<int, double> >& result, int k){
if(result.size()<k){
    return numeric_limits<double>::max();
}
else{
    nth_element(result.begin(), result.begin()+k-1, result.end(), myComp);
    return result[k-1].second;
}
}

kNN_truth::kNN_truth(){
}

void kNN_truth::initialize(){
    if(p==NULL){
        p=new kNN_truth();
        map<int, Particle::location> gt=gtQueryProc::get()->getGT();
        map<int, Particle::location>::iterator it=gt.begin(); 
        for(; it!=gt.end(); it++){
            pair<int, int> edge=make_pair<int, int>(it->second.src, it->second.dst);
            pair<int, int> edge_reverse=make_pair<int, int>(it->second.dst, it->second.src);

            p->edgeToObj[edge].push_back(it->first);
            p->edgeToObj[edge_reverse].push_back(it->first);

        }
    }

}

kNN_truth* kNN_truth::get(){
    if(p!=NULL){
    return p;
    }
}

vector<pair<int,double> > kNN_truth::query(int src, int dst, double dis, int k){
 map<int, Particle::location> gt=gtQueryProc::get()->getGT();

    vector<int> src_dst = edgeToObj[make_pair<int, int>(src, dst)];
    vector<pair<int, double> > result;//(objID, disToq)
    vector<pair<int, double> > frontier;//this should be a queue,(nodeID, disToq)
    vector<int> expanded;
    //add objects on edge src->dst to result set
    for(int i =0; i<src_dst.size(); i++){
        int objID=src_dst[i];
        double disObj2q = fabs(myDistance(src, gt[objID].src, gt[objID].dst, gt[objID].disTodst)-myDistance(src, src, dst, dis));
        result.push_back(make_pair<int, double>(objID, disObj2q));
    }


    double dmax=dk(result, k);
    frontier.push_back(make_pair<int, double>(src, myDistance(src, src, dst, dis)));
    frontier.push_back(make_pair<int, double>(dst,  dis));
    expanded.push_back(src);
    expanded.push_back(dst);
    make_heap(frontier.begin(),frontier.end(),myheapComp);

    pair<int, double> nextNode=frontier.front();//the min distance node to q is poped out and going to be expanded

    pop_heap(frontier.begin(), frontier.end());
    frontier.pop_back();

    while(nextNode.second<dmax){
        int current=nextNode.first;
      for(int i=0; i<nodes[current]->adjEdge.size(); i++){
          int adjNode=nodes[current]->adjEdge[i].dst;
          if(find(expanded.begin(), expanded.end(), adjNode)==expanded.end()){//for every unvisited adjacent node of the current node
        vector<int> current_adj = edgeToObj[make_pair<int, int>(current, adjNode)];
        //add objects on edge current_adj to result
        for(int j =0; j<current_adj.size(); j++){
        int objID=current_adj[j];
        double disObj2q = myDistance(current, gt[objID].src, gt[objID].dst, gt[objID].disTodst)+nextNode.second;
        result.push_back(make_pair<int, double>(objID, disObj2q));
        }
        //add adjacent node to frontier
        double disadj2q=nextNode.second+myDistance(current, current, adjNode, 0);
        frontier.push_back(make_pair<int, double>(adjNode,disadj2q ));
        push_heap(frontier.begin(), frontier.end());
        //add current to expanded
        expanded.push_back(current);
          }
      }
      //update dmax
      dmax=dk(result, k);
      //de-queue the next node n in Q
      nextNode=frontier.front();
      pop_heap(frontier.begin(), frontier.end());
      frontier.pop_back();
    }

    vector<pair<int, double> > result_final(result.begin(), result.begin()+k);
    return result_final;
}









