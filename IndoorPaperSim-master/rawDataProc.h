#ifndef _rawDataProc_h
#define _rawDataProc_h

#include <vector>
#include <string>
#include <map>
using namespace std;
class rawDataProc{
    public:
        class reading;
    private:
    int numOfobjects;
    int sampleTime;
    vector<reading> *p;
    map<int, vector<reading> > *m;
    public:
    class reading{
        public:
        int time;
        int tagID;
        int atnID;
        reading(int a1, int a2, int a3):time(a1), tagID(a2), atnID(a3){};
    };
    rawDataProc(int numO, int t);
    private:
    void proc(const string& file, int numatn);
    public:
    map<int, vector<reading> >* sepObj(const string& file, int numatn);
};


#endif
