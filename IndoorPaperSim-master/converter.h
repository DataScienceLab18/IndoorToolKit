
#ifndef _converter_h
#define _converter_h
#include "node.h"
/*
 *this class will convert a position on edge
 *to a common point as node, then we can calculate
 *its distance to antenna
 *@param dis is the distance to dst node
 */
class converter{
    public:
        static node convert(long src, long dst, double dis);

};

#endif
