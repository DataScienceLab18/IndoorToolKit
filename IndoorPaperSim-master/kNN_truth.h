#ifndef _kNN_truth_h
#define _kNN_truth_h

#include <map>
#include <vector>
using namespace std;

class kNN_truth{
    private:
        map<pair<int, int>, vector<int> > edgeToObj;//edge(node1, node2)->objects on this edge
        static kNN_truth *p;//singleton pointer
        kNN_truth();

    public:
        static void initialize();
        static kNN_truth* get();
        vector<pair<int, double> > query(int src, int dst, double dis, int k);
};

#endif
 
