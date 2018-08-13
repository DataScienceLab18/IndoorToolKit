#ifndef _rect_h
#define _rect_h
#include <algorithm>
template<class T>
class rect{
    public:
        T a_min[2];
        T a_max[2];
        rect(T e1, T e2, T e3, T e4){
            a_min[0] = e1;
            a_min[1] = e2;
            a_max[0] = e3;
            a_max[1] = e4;
        }

        T getArea() const{
            return (a_max[1] - a_min[1])*(a_max[0]-a_min[0]);
        }
        
        rect<T> intersect(const rect<T>& r) const{
            T temp1 = max(a_min[0], r.a_min[0]);
            T temp2 = max(a_min[1], r.a_min[1]);
            T temp3 = min(a_max[0], r.a_max[0]);
            T temp4 = min(a_max[1], r.a_max[1]);
            return rect(temp1, temp2, temp3, temp4);
        }

        bool contain(T x, T y){
            return x>=a_min[0]&&x<=a_max[0]&&y>=a_min[1]&&y<=a_max[1];
        }
};

#endif
