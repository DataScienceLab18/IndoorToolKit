#include "conf.h"
#include "Particle.h"
#include "ParticleFilter.h"
#include "aggregateRaw.h"
#include "rawDataSimulator.h"
#include "rect.h"
#include "rawDataProc.h"
#include "queryProc.h"
#include "huaRtree.h"
#include "gtQueryProc.h"
#include "queryWindow.h"
#include "math.h"
#include "ShortestPath.h"
#include "kNN_s.h"
#include "kNN_truth.h"
#include "kNN_Hua.h"
#include "kNN_qp_reader.h"
bool MySearchCallback(int id, void* arg) 
{
  //printf("Hit data rect %d\n", id);
  vector<int>* p = (vector<int>* ) arg;
  p->push_back(id);
  return true; // keep going
}

class tagIDEqual{
    public:
        int tagID;
        tagIDEqual(int id): tagID(id){};
        bool operator()(ParticleFilter::index temp){
            return tagID == temp.tagID;
        }
};

class kNNEqual:public tagIDEqual{
    public:
        kNNEqual(int id):tagIDEqual(id){};
};
    //#define DEBUG
#define rooms conf::get()->getRooms()
//#define DEBUG1
int main(int arg, char *argv[]){
    //command line argument
    int numP = atoi(argv[1]);//first argument: number of Particles
    int timeshot = atoi(argv[2]);//second: the timeshot of queries
    int numR = atoi(argv[3]);//third: number of atns whose readings we want to keep
    int numObjSm = atoi(argv[4]);//fourth: number of objects moving around
    double queryWindowSize = atof(argv[5]);//fifth: the size of query window, represented by ratio of query area to total area.
    int kNN_k = atof(argv[6]);
    double range = atof(argv[7]);
    
    conf::initialize("nodes.csv", "edges.csv", "antennas.csv", "rooms.csv", range);
    conf* pconf = conf::get();

    /*generate 1000 moving objects init location

map<int, edge*> edges=pconf->getEdges();
int edgeCount = edges.size();
ofstream init("init2.txt");
srand((unsigned) time(NULL));
int i=1;
while(i<=1000){
//randomly generate an edgeID
int edgeID = rand()%edgeCount;
if(edgeID==0){edgeID=1;}
init<<i<<" "<<(edges[edgeID])->src<<" "<<edges[edgeID]->dst<<" ";
double dis=(edges[edgeID])->cost*rand()/(double) RAND_MAX;
init<<dis<<endl;
i++;
}
init.close();
*/
    
    //map<long, node*> nodes =pconf->getNodes();

    //map<long, antenna*> antennas = pconf->getAntennas();
    
    rawDataSimulator rs;
    rs.fromUser(timeshot);
    rs.rawDataGene(numObjSm);
   
   rawDataProc proc(rs.numObj, rs.timeInSec);
   #ifdef DEBUG

   for(int i = 0; i < p->size(); i++){
       cout<<(*p)[i].time<<" "<<(*p)[i].tagID<<" "<<(*p)[i].atnID<<endl;
   }

   map<int, room*>::iterator it = rooms.begin();
   for(; it != rooms.end(); it++){
       cout<<it->first<<endl;
   }
#endif
   
   map<int, vector<rawDataProc::reading> >* m = proc.sepObj("agg2.txt", numR);//the second parameter is how many atns readings to keep
//cout<<"364 object "<<(*m)[364].size()<<endl;
   
   for(int i = 0; i < rs.numObj; i++){
   ParticleFilter pf;
   vector<rawDataProc::reading> readings = (*m)[i+1];
#ifdef DEBUG11
    cout<<"This is object "<<i+1<<endl;
    cout<<"Init around atn "<<readings[0].atnID<<endl;
#endif
      pf.initialize(numP, readings[0].atnID);
        #ifdef DEBUG1
        cout<<"After Initialization\n";
        pf.print();
        #endif
        
    
    for(int j = 1; j < readings.size(); j++){
           pf.move();
           #ifdef DEBUG1
           cout<<"After move() is called\n";
           pf.print();
           #endif
           pf.reWeight(readings[j].atnID);
           #ifdef DEBUG1
           cout<<"After reWeight() is called\n";
           pf.print();
           #endif
           pf.reSample();
           #ifdef DEBUG1
           cout<<"After reSample() is called\n";
           pf.print();
           #endif
     }
         //  }
    
    /*else{
        pf.move();
cout<<"After move() is called\n";
           pf.print();
    }*/
    pf.toAnchors(i+1);

    }
#ifdef DEBUG 
    ParticleFilter::showAnchors();
#endif
    huaRtree::buildRtree("rects.csv");
    huaRtree::buildAtnToRect("conn.csv");
    huaRtree::addTorectHT(m);

    gtQueryProc::initialize();
    gtQueryProc* p_gt = gtQueryProc::get();
    vector<ParticleFilter::index> result3;
    rect<double> qWindow(0, 0, 0, 0); 
    vector<double> KL_hua, KL_pf;
// simulate concurrent queries, 50 queries in total
    for (int queryNum = 0; queryNum < 50; queryNum++){
    do{
    qWindow = queryWindow::genQueryWindow(queryWindowSize);
    result3 = p_gt->query(qWindow);
    }
    while(result3.size()==0);
#ifdef DEBUG
    cout<<"query "<<queryNum<<": "<<qWindow.a_min[0]<<" "<<qWindow.a_min[1];
    cout<<" "<<qWindow.a_max[0]<<" "<<qWindow.a_max[1]<<endl;
#endif
    vector<ParticleFilter::index> result = queryProc::query(qWindow);
#ifdef DEBUG
    cout<<"Particle Filter method: \n";
    for (int i = 0; i<result.size(); i++){
        cout<<result[i].tagID<<" "<<result[i].p<<endl;
    }

#endif

// LU hua's query method

    vector<ParticleFilter::index> result2 = huaRtree::query(qWindow);
#ifdef DEBUG
    cout<<"Hua Lu's symbolic model method: \n";
    for (int i = 0; i<result2.size(); i++){
        cout<<result2[i].tagID<<" "<<result2[i].p<<endl;
    }

//groundtruth query

    cout<<"query on ground truth data: \n";
    for (int i = 0; i<result3.size(); i++){
        cout<<result3[i].tagID<<" "<<result3[i].p<<endl;
    }
#endif    

//KL computation for one query: average of fact*log(fact/predict) for result set
    double sum_hua = 0;
    double sum_pf = 0;
    int count_hua = 0;
    int count_pf = 0;
    for (int i = 0; i < result3.size(); i++){
       if(result3[i].p < 0.00001){
           continue;
       }
       vector<ParticleFilter::index>::iterator it_hua = find_if(result2.begin(), result2.end(), tagIDEqual(result3[i].tagID));
       if(it_hua!= result2.end())
         { 
           if(result3[i].p <1&&it_hua->p<1 ){
           sum_hua += result3[i].p*log(result3[i].p/it_hua->p) + (1-result3[i].p)*log((1-result3[i].p)/(1-it_hua->p));}
           else{
               sum_hua += result3[i].p*log(result3[i].p/it_hua->p);
           }
           count_hua++;
         }
              
       vector<ParticleFilter::index>::iterator it_pf = find_if(result.begin(), result.end(), tagIDEqual(result3[i].tagID));
       if(it_pf!= result.end())
      { 
          if(result3[i].p <1 ){
            sum_pf += result3[i].p*log(result3[i].p/it_pf->p) + (1-result3[i].p)*log((1-result3[i].p)/(1-it_pf->p));}
          else{
            sum_pf += result3[i].p*log(result3[i].p/it_pf->p);
           }

           count_pf++;
      }
      /* 
       else{
           //if the true result is not in the PF result, give a penalty
           sum_pf += 1;
           count_pf++;
       }
       */
    }
    //push KL to a vector to store KL of different queries
    if(count_hua!=0){
    KL_hua.push_back(sum_hua/count_hua);//for every query, compute the average formula of KL
    // KL_hua.push_back(sum_hua);
    }
    if(count_pf!=0){
     KL_pf.push_back(sum_pf/count_pf);
    // KL_pf.push_back(sum_pf);
    }
    }

    ofstream KL("KL.txt", ios::app);
    ofstream top1("top1.txt", ios::app);
    double average = 0;
    for (int i = 0; i < KL_hua.size(); i++){
        average += KL_hua[i];
    }
#ifdef DEBUG
    cout<<"HuaKL: "<<average/KL_hua.size()<<endl;//compute the average of KL for many queries
#endif
    KL<<"HuaKL: "<<average/KL_hua.size()<<" ";

    average = 0;
    for (int i = 0; i < KL_pf.size(); i++){
        average += KL_pf[i];
    }
#ifdef DEBUG
    cout<<"PFKL: "<<average/KL_pf.size()<<endl;
#endif
    KL<<"PFKL: "<<average/KL_pf.size()<<endl;

    //top1 success rate: the percentage of objects whose max probability location is within 2m of its true location
    //it is irrelavant with queries, only examines filtered result
    pair<double, double> rate = ParticleFilter::top1();
    top1<<"Top1: "<<rate.first<<" Top2: "<<rate.second<<endl;
#ifdef DEBUG
    ParticleFilter::showObjectHash();
#endif
    //get query point
    string file_kNN="kNN_queryPoint.csv";
    kNN_qp_reader::initialize(file_kNN);
    map<int, location> qps=kNN_qp_reader::get()->getQueryPoint();
    map<int, location>::iterator it=qps.begin();
    kNN_truth::initialize();
    ofstream knnresult("kNNresult.txt", ios::app);
    
    for(; it!= qps.end(); it++){
#ifdef DEBUG
    cout<<"current query rect is "<<it->first<<endl;
#endif
    //kNN query
    vector<ParticleFilter::index> result_knn=kNN_s::query(it->second.src, it->second.dst, it->second.dis, kNN_k);
#ifdef DEBUG
    cout<<"PF kNN result: tagID p\n";
    for(int i=0; i<result_knn.size(); i++){
        cout<<result_knn[i].tagID<<" "<<result_knn[i].p<<endl;
    }
    cout<<endl;
#endif
   //kNN groundtruth query
    vector<pair<int, double> > result_kNN_truth = kNN_truth::get()->query(it->second.src, it->second.dst, it->second.dis, kNN_k);
#ifdef DEBUG
    cout<<"kNN groundtruth result: objectID dis2q\n";
    for(int i=0; i<result_kNN_truth.size(); i++){
        cout<<result_kNN_truth[i].first<<" "<<result_kNN_truth[i].second<<endl;
    }
    cout<<endl;
#endif
    //kNN Hua method query
    kNN_Hua::initialize();
    vector<int> result_kNN_Hua = kNN_Hua::get()->query(it->first, kNN_k);
#ifdef DEBUG
    cout<<"kNN Hua result Max Prob Set: objectID\n";
    for(int i=0; i<result_kNN_Hua.size(); i++){
        cout<<result_kNN_Hua[i]<<endl;
    }
    cout<<endl;
#endif
    //collect data
     int PFkNNhit=0;
     int HuakNNhit=0;
     for(int i=0; i<result_kNN_truth.size(); i++){
       if(find_if(result_knn.begin(), result_knn.end(),kNNEqual(result_kNN_truth[i].first))!=result_knn.end()){
           PFkNNhit++;
       }
       if(find(result_kNN_Hua.begin(), result_kNN_Hua.end(),result_kNN_truth[i].first)!=result_kNN_Hua.end() ){
           HuakNNhit++;

     }
     }
     knnresult<<double (PFkNNhit)/kNN_k<<" "<<double (HuakNNhit)/kNN_k<<endl;
    }
    knnresult.close();

     return 0;
    }
