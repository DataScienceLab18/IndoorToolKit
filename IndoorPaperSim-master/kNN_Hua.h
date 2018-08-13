#ifndef _kNN_hua_h
#define _kNN_hua_h
#include <map>
#include <vector>
using namespace std;

class kNN_Hua{

    private:
        //key: rectID, value: all rects on hallway connected to key rectID. Here we ignore rooms.
        map<int, vector<int> > rectConn;
        //this method is according to the floorplan,
        //
        //it builds rectConn.
        void init_rectConn();
        //disable default constructor
        kNN_Hua();
       //return from the center of rectID1 to rectID2 min distance
        double networkDis(int rectID1, int rectID2);
        /*this method find the shortest and longest distance from
         * an object's possible rects to query rect
         */
        void find_s_l(vector<int> rects, int rectQ, double& sIn, double& lIn);
        static kNN_Hua* p;

    public:
        static kNN_Hua* get();
        static void initialize();
        /*this method performs knn query
         * @param rect rectID of query point, supposequery point is at the center of this rect
         * @param k the number k from kNN
         */
        vector<int> query(int rect, int k);
};

#endif


