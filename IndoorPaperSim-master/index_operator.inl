#include "ParticleFilter.h"

#define TYPE vector<ParticleFilter::index>

inline TYPE operator + (const TYPE& lo,const TYPE& ro){
    TYPE result(lo);
    for(int i = 0; i < ro.size(); i++){
        bool flag = false;
        for(int j = 0; j < result.size(); j++){
            if (result[j].tagID == ro[i].tagID){
                flag = true;
                result[j].p += ro[i].p;
            }
        }

        if (!flag){
            result.push_back(ro[i]);
        }

    }

    return result;
}

inline void operator += (TYPE& lo, const TYPE& ro){
    for(int i = 0; i < ro.size(); i++){
        bool flag = false;
        for(int j = 0; j < lo.size(); j++){
            if (lo[j].tagID == ro[i].tagID){
                flag = true;
                lo[j].p += ro[i].p;
            }
        }

        if (!flag){
            lo.push_back(ro[i]);
        }

    }
}

inline void operator *= (TYPE& lo, double d){
    for (int i = 0; i<lo.size(); i++){
        lo[i].p *= d;
    }
}

inline TYPE operator * (const TYPE& lo, double d){
    TYPE result(lo);
    for(int i = 0; i<lo.size(); i++){
       result[i].p *= d;
    }
    return result;
}
