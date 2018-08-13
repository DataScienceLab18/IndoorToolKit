#ifndef _rawDataSimulator_h
#define _rawDataSimulator_h
#include <string>
using std::string;
class rawDataSimulator{
    
           public:
// int numOfObjects;
        static int timeInSec;
        static int numObj;
        static void fromUser();
        static void fromUser(int time);
        static void rawDataGene(const string& tracefilename);
        void rawDataGene(int numObjSm);
};

#endif


