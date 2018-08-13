
#include "queryWindow.h"
#include "conf.h"

#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_real.hpp>
#include <iostream>
static boost::mt19937 rng;
#define totalArea (45.9*30)
#define width 5

rect<double> queryWindow::genQueryWindow(double ratio){
    map<int, hallway*> hallways = conf::get()->getHallways();

    boost::uniform_int<> nd(27, 30);

    boost::variate_generator<boost::mt19937&, 
                                     boost::uniform_int<> > var_nor(rng, nd);
      int randomID = var_nor();
      double queryArea = ratio*totalArea;
      double length = queryArea/width;

      if(hallways[randomID]->direction == hallway::horizontal){
          if(length>(hallways[randomID]->a_max[0]-hallways[randomID]->a_min[0])){
              cout<<"query size is too big!!!\n";
              exit(0);
          }
          double min = hallways[randomID]->a_min[0]+length/2;
          double max = hallways[randomID]->a_max[0]-length/2;
         
      boost::uniform_real<> nd2(min, max);

     boost::variate_generator<boost::mt19937&,  boost::uniform_real<> > var_nor2(rng, nd2);
     double center_x = var_nor2();
     double center_y = (hallways[randomID]->a_min[1]+hallways[randomID]->a_max[1])/2;
     return rect<double>(center_x-length/2, center_y-width/2, center_x+length/2, center_y+width/2);
      }
      else{

         if(length>(hallways[randomID]->a_max[1]-hallways[randomID]->a_min[1])){
              cout<<"query size is too big!!!\n";
              exit(0);
          }
          double min = hallways[randomID]->a_min[1]+length/2;
          double max = hallways[randomID]->a_max[1]-length/2;


     boost::uniform_real<> nd3(min, max);

     boost::variate_generator<boost::mt19937&,  boost::uniform_real<> > var_nor3(rng, nd3);
     double center_y = var_nor3();
     double center_x = (hallways[randomID]->a_min[0]+hallways[randomID]->a_max[0])/2;

     return rect<double>(center_x-width/2,center_y-length/2, center_x+width/2, center_y+length/2);
      }

}



  


