#ifndef _vector_math_h
#define _vector_math_h



class vector_math{
    public:
        double x, y;
        vector_math(double x, double y){
            this->x = x;
            this->y = y;
        }
        double operator * (vector_math v){
            return (this->x)*v.x + (this->y)*v.y;
        }
        vector_math operator *(double t){
            return vector_math(this->x * t, this->y * t);
        }
        double norm(){
            return sqrt(x*x + y*y);
        }

        vector_math getUnit(){
            return vector_math(this->x/norm(), this->y/norm());
        }
};

#endif
