#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <boost/math/distributions.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/unordered_map.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/mpl/inserter.hpp>

#include "simulator.h"
#include "global.h"

namespace simulation {

using std::cout;
using std::endl;

// Query window intersects with rooms and hall, generating windows
// with probability accordingly.
static std::vector<std::pair<IsoRect_2, double> > wins_;

// Readings for each objects
static std::vector<std::vector<int> > readings_;

// Simulation duration
static double duration_;

void
Simulator_impl_::initialize()
{
  g_.enter_room(enter_room_prob_);
  g_.knock_door(knock_door_prob_);

  for (int i = 0; i < num_object_; ++i)
    objects_.push_back(Particle(g_, i));
  //cout<<"size="<<objects_.size()<<endl;
}

std::vector<landmark_t>
Simulator_impl_::positions(double t)
{
  std::vector<landmark_t> result;
  for (auto it = objects_.cbegin(); it != objects_.cend(); ++it)
    result.push_back(it->pos(t));
  return result;
}

boost::unordered_map<int, boost::unordered_map<int, double> >
Simulator_impl_::predict(double t)
{

  boost::unordered_map<
    int, boost::unordered_map<int, double> > result;

  for (int i = 0; i < num_object_; ++i)
    predict_(result, i, t);

  return result;
}

boost::unordered_map<int, boost::unordered_map<int, double> >
Simulator_impl_::predict1(double t)
{
  boost::unordered_map<
    int, boost::unordered_map<int, double> > result;

  for (int i = 0; i < num_object_; ++i)
    predict1_(result, i, t);

  return result;
}

landmark_t
Simulator_impl_::random_inside_reader(int i) const
{
  landmark_t pos = g_.reader_pos(i);
  //cout<<"reader"<<i<<","<<pos.get<0>()<<","<<pos.get<1>()<<","<<pos.get<2>()<<endl;
  boost::random::uniform_real_distribution<> unifd(0, 1);

  if (unifd(gen) > 0.5) {
    boost::swap(pos.get<0>(), pos.get<1>());
    pos.get<2>() = 1 - pos.get<2>();
  }

  Particle p(g_, -1, pos);

  return p.advance(radius_ / p.v() * unifd(gen));
}

void
Simulator_impl_::run(double duration)
{
  // Objects are running for *duration*.
//  cout<<"x_____________"<<duration<<endl;
  duration_ = duration;
  int index=0;
  
  for (auto it = objects_.begin(); it != objects_.end(); ++it)
  {

    it->advance1(duration);
    index++;
   }
  // Generate readings for all the objects at all timestamps.
  readings_.clear();
  boost::random::uniform_real_distribution<> unifd(0, 1);
  //cout<<"size:"<<objects_.size()<<endl;
  for (size_t i = 0; i < objects_.size(); ++i) {
    std::vector<int> tmp;
//    cout<<"obj___index__________"<<i<<endl;
    //cout<<"duration_"<<duration_<<endl;
    for (int j = 0; j < duration_; ++j) {
      //cout<<unifd(gen)<<endl;
      //cout<<success_rate_<<endl;
      //cout<<success_rate_<<endl;
      if (unifd(gen) > success_rate_) tmp.push_back(-1);
      else 
	{
//    	   cout<<"object:"<<i<<endl;
//    	   cout<<"duration"<<j<<endl;
//    	   cout<<objects_[i].pos(j).get<1>()<<endl;
	   tmp.push_back(g_.detected_by(objects_[i].pos1(j), radius_));
	   }
    }
//    for(int z=0;z<tmp.size();z++)
//	{
//        cout<<"object:"<<i<<endl;
//	    cout<<z<<"reader:"<<":"<<tmp[z]<<endl;
//	    cout<<"pos:"<<z<<":"<<objects_[i].pos(z).get<0>()<<","<<objects_[i].pos(z).get<1>()<<","<<objects_[i].pos(z).get<2>()<<endl;
//	}
    readings_.push_back(tmp);
  }
}


std::vector<int>
Simulator_impl_::bfsPath(int pre, int from, int to, int depth, std::vector<int> path)
{
  std::vector<int> result;
  Vertex cur = g_.wg_.v(from);

  boost::unordered_set<Vertex> outs;
  auto pit = boost::out_edges(cur, g_.wg_());
  path.push_back(from);
  for (auto it = pit.first; it != pit.second; ++it)
  {
      Vertex v = boost::target(*it, g_.wg_());

//      cout<<"depth:"<<depth<<endl;
//      cout<<from<<endl;
//      cout<<g_.wg_.vid(v)<<endl;
      if (g_.wg_.vid(v)==to)
      {
        for (int j=0;j<path.size();j++)
            cout<< path.at(j)<<endl;
        result=path;
        return result;
      }else{
        if (g_.wg_.vid(v)!=pre&&depth<3&&g_.color(g_.wg_.vid(v)) != DOOR)
        {
            result=bfsPath(from, g_.wg_.vid(v),to,++depth,path);
//            path.pu
            --depth;
            if (result.size()>0)
            {
//            cout<<"zz"<<endl;
                return result;
            }else
            {
//                cout<<"xx"<<endl;
                continue;
            }
        }else{
//        cout<<"yy"<<endl;
            continue;
        }
      }
  }
  return result;

}
bool
Simulator_impl_::predict_(
    boost::unordered_map<int, boost::unordered_map<int, double> > &out,
    int obj, double t, int limit)
{
  const std::vector<int> &reading = readings_[obj];
  // The number of valid readings, i.e. reading >= 0, in [start, end]
  // is *limit*.
//  cout<<"obj:"<<obj<<endl;
//  cout<<"————————"<<endl;
//  cout<<t<<endl;

  int end = t;
  int start = end;
  int first = -1;
    int lastReader = -1;
    int count = 0;
  {

    for (/* empty */; count < limit && start >= 0; --start) {
//      cout<<start<<endl;
//      cout<<obj<<"reading:"<<readings_[obj][start]<<endl;
      if (reading[start] >= 0 && reading[start] != first) {
        ++count;
        first = reading[start];
        if(count==1)
        {
            lastReader=start;
        }
      }
    }

    // Not enough observation
    if (count < limit) return false;

    // The last decrement is uncalled for.
    ++start;
  }
//   cout<<"————————"<<endl;
//  cout<<start<<endl;
//  cout<<end<<endl;
   boost::math::normal_distribution<> myNormal(1.0, 0.1);
  float result = boost::math::cdf(myNormal, 1.0);
//  cout<<"vel:"<<result<<endl;
        std::vector<int> path;

        cout<<"-----"<<endl;
	    cout<<obj<<"reading------start:"<<start<<":"<<readings_[obj][start]<<":"<<g_.align(g_.reader_pos(readings_[obj][start]))<<endl;
	    cout<<obj<<"pos1:"<<start<<":"<<g_.align(g_.reader_pos(readings_[obj][start]))<<endl;
	    std::vector<int> allPath=bfsPath(-1,g_.align(g_.reader_pos(readings_[obj][start])),g_.align(g_.reader_pos(readings_[obj][lastReader])),0,path);
        allPath.push_back(g_.align(g_.reader_pos(readings_[obj][lastReader])));
	    cout<<obj<<"reading------end:"<<lastReader<<":"<<readings_[obj][lastReader]<<":"<<g_.align(g_.reader_pos(readings_[obj][lastReader]))<<endl;
    int roomsNum=0;
    int totalLength=0;
    for(int z=0;z<allPath.size();z++)
	{
      Vertex cur = g_.wg_.v(allPath.at(z));
      auto pit = boost::out_edges(cur, g_.wg_());
      if(z<allPath.size()-1)
      {
           totalLength+=g_.weight(allPath.at(z),allPath.at(z+1));
      }
      for (auto it = pit.first; it != pit.second; ++it)
      {
         Vertex v = boost::target(*it, g_.wg_());
         if(g_.color(g_.wg_.vid(v)) == DOOR)
            roomsNum++;
      }
	}
    cout<<"rooms:"<<roomsNum<<endl;
    cout<<"totalLength:"<<totalLength<<endl;


    double utimeInRoom=9.0;
    for (int i = 0; i <= roomsNum; i++) {
        distance_=
    }
  std::list<Particle> subparticles;
  for (int i = 0; i < num_particle_; ++i)
//    landmark_t random
    subparticles.push_back(
        Particle(g_, obj, random_inside_reader(reading[start])));

//   This is the filter process where we eliminate those that missed
//   the reader.  This is NOT particle filter, just an extention of
//   symbolic model IMO.
  for (int i = start + 1; i <= end; ++i) {

//  out[g_.align(random_inside_reader(reading[start]))][obj] = 1;
    for (auto it = subparticles.begin(); it != subparticles.end();
         /* empty */) {
         //cout<<it->id()<<"initailpos:"<<it->pos(i)<<endl;
      landmark_t p = it->advance();
      if (reading[i] >= 0 && g_.detected_by1(p, radius_) != reading[i])
        it = subparticles.erase(it);
      else
        ++it;
    }

    int sz = subparticles.size();

    if (0 == sz) return false;

    if (sz < num_particle_) {
      boost::random::uniform_int_distribution<> unifi(0, sz - 1);
      int left = num_particle_ - sz;
      for (int i = 0; i < left; ++i)
        subparticles.push_back(
            *boost::next(subparticles.begin(), unifi(gen)));
    }
  }

//   Predicting.  During the *remain*, the object's position is
//   unknown, which is exactly what we'd like to predict.
  double remain = t - end;
  double prob = 1.0 / num_particle_;
  for (auto it = subparticles.begin(); it != subparticles.end();
       ++it) {
    landmark_t p = it->advance(remain);
//    cout<<"sub id:"<<it->id()<<endl;
//    cout<<"g_.align(p):"<<g_.align(p)<<endl;
//    cout<<"pos:"<<p.get<0>()<<endl;
//    cout<<"pos1:"<<p.get<1>()<<endl;
//    cout<<"pos2:"<<p.get<2>()<<endl;
    out[g_.align(p)][it->id()] += prob;
  }
//  out[g_.align(random_inside_reader(reading[start]))][obj] = 1;
//  cout<<"out size:"<<out.size()<<endl;
  return true;
}

bool
Simulator_impl_::predict1_(
    boost::unordered_map<int, boost::unordered_map<int, double> > &out,
    int obj, double t, int limit)
{
  return true;
}

}
