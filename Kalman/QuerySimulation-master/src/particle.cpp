#include <algorithm>
#include <fstream>
#include <boost/random.hpp>
#include <boost/math/distributions.hpp>
#include <boost/random/normal_distribution.hpp>

#include <boost/tuple/tuple.hpp>

#include "particle.h"
#include "global.h"

namespace simulation {

using std::cout;
using std::endl;

Particle::Particle(const WalkingGraph &g, int id,
                   const landmark_t &pos)
    : g_(g)
    , id_(id)
{
  boost::random::normal_distribution<> norm(1, 0.1);

  velocity_ = norm(gen);
  //cout<<"pos:"<<pos.get<2>()<<endl;
  pos_ = pos.get<2>() < 0 ? g.random_pos() : pos;
  //cout<<"initial"<<id<<","<<pos_.get<0>()<<","<<pos_.get<1>()<<","<<pos_.get<2>()<<endl;
  history_.push_back(std::make_pair(
      -pos_.get<2>() * g_.weight(pos_.get<0>(),
                                 pos_.get<1>()) / velocity_,
      pos_.get<0>()));
  //cout<<"history:"<<history_.size()<<endl;
}



Particle::Particle(const Particle &other)
    : g_(other.g_)
    , id_(other.id_)
    , pos_(other.pos_)
    , history_(other.history_)
{
  boost::random::normal_distribution<> norm(other.velocity_, 0.1);
  velocity_ = norm(gen);
}

landmark_t
Particle::advance(double duration)
{
  int &source = pos_.get<0>();
  int &target = pos_.get<1>();
  double &p = pos_.get<2>();
  double w = g_.weight(source, target);

//  cout<<"path:"<<source<<"to "<<target<<endl;
//  cout<<"w:"<<w<<endl;
//  cout<<"duration:"<<duration<<endl;
//    cout<<"velocity_:"<<velocity_<<endl;
//  cout<<"p:"<<p<<endl;
  double elapsed = history_.back().first + w * p / velocity_,
      left = w * (1 - p),
      dist = duration <= 0 ? velocity_ - left
      : duration * velocity_ - left;

  boost::random::uniform_real_distribution<> unifd(0, 1);
//cout<<"left:"<<left<<endl;
//  cout<<"dist___________________:"<<dist<<endl;
  while (true) {
    elapsed += left / velocity_;
    history_.push_back(std::make_pair(elapsed, target));

    if (dist < 0) break;

    int pre = source;
    source = target;
    target = g_.random_next(source, pre);
    left = g_.weight(source, target);

    dist -= left;
//    cout<<"source:"<<source<<endl;
//    cout<<"target:"<<target<<endl;
//    cout<<"dist:"<<dist<<endl;
//    cout<<"left:"<<left<<endl;
  }

    //cout<<"______"<<endl;
  //cout<<source<<endl;
  //cout<<target<<endl;
  p = 1 + dist / g_.weight(source, target);

  //cout<<p<<endl;
  return pos();
}


landmark_t
Particle::advance1(double duration)
{
  int &source = pos_.get<0>();
  int &target = pos_.get<1>();
  double &p = pos_.get<2>();
  double w = g_.weight(source, target);

//  cout<<"path:"<<source<<"to "<<target<<endl;
//  cout<<"w:"<<w<<endl;
//  cout<<"duration:"<<duration<<endl;
//    cout<<"velocity_:"<<velocity_<<endl;
//  cout<<"p:"<<p<<endl;
  double elapsed = history_.back().first + w * p / velocity_,
      left = w * (1 - p),
      dist = duration <= 0 ? velocity_ - left
      : duration * velocity_ - left;

  boost::random::uniform_real_distribution<> unifd(0, 1);
//cout<<"left:"<<left<<endl;
//  cout<<"dist___________________:"<<dist<<endl;
  while (true) {
    elapsed += left / velocity_;
    history_.push_back(std::make_pair(elapsed, target));

    if (dist < 0) break;

    int pre = source;
    source = target;
    target = g_.random_next(source, pre);
    left = g_.weight(source, target);

    dist -= left;
//    cout<<"elapsed:"<<elapsed<<endl;
//    cout<<"source:"<<source<<endl;
//    cout<<"target:"<<target<<endl;
//    cout<<"dist:"<<dist<<endl;
//    cout<<"left:"<<left<<endl;
  }

    //cout<<"______"<<endl;
  //cout<<source<<endl;
  //cout<<target<<endl;
  p = 1 + dist / g_.weight(source, target);

  //cout<<p<<endl;
  return pos();
}

landmark_t
Particle::advanceGround(double duration,int index)
{
  int &source = pos_.get<0>();
  int &target = pos_.get<1>();
  double &p = pos_.get<2>();
  double w = g_.weight(source, target);

//  cout<<"path:"<<source<<"to "<<target<<endl;
//  cout<<"w:"<<w<<endl;
//  cout<<"duration:"<<duration<<endl;
//  cout<<"velocity_:"<<velocity_<<endl;
//  cout<<"p:"<<p<<endl;
//  cout<<"time at start:"<<history_.back().first<<endl;
  //history_.pop_back();
  history_.clear();
//  cout<<"history size:"<<history_.size()<<endl;
//  cout<<"object："<<index<<endl;
  const std::string DataRoot("/home/bo/indoor/IndoorToolKit/QuerySimulation-master/data/my/");

  std::ifstream fin(DataRoot + "record"+std::to_string(index%19)+".txt");
    std::string line;
    while (std::getline(fin, line)) {
    //cout<<"xx"<<endl;
      //boost::algorithm::trim_left(line);
      //if (line.compare(0, Commenter.size(), Commenter) == 0) continue;
      std::stringstream ss(line);
      double x0, x1, x2;
      ss >> x0 >> x1 >> x2;
      //cout<<(double)x0/100<<"-"<<x1<<endl;
      history_.push_back(std::make_pair((double)x0/100, x1));
      //p = 1 + dist / g_.weight(source, target);
    }
    fin.close();
    //history_.pop_back();
    /*
  double elapsed = history_.back().first + w * p / velocity_,
      left = w * (1 - p),
      dist = duration <= 0 ? velocity_ - left
      : duration * velocity_ - left;

  boost::random::uniform_real_distribution<> unifd(0, 1);
  cout<<"left:"<<left<<endl;
   cout<<"dist___________________:"<<dist<<endl;
  while (true) {
    elapsed += left / velocity_;
    history_.push_back(std::make_pair(elapsed, target));

    if (dist < 0) break;

    int pre = source;
    source = target;
    target = g_.random_next(source, pre);
    left = g_.weight(source, target);

    dist -= left;
//    cout<<"current-----------"<<endl;
//    cout<<"source:"<<source<<endl;
//    cout<<"target:"<<target<<endl;
//    cout<<"dist:"<<dist<<endl;
//    cout<<"elapsed:"<<elapsed<<endl;
  }

    //cout<<"______"<<endl;
  //cout<<source<<endl;
  //cout<<target<<endl;
  p = 1 + dist / g_.weight(source, target);
  */
//  cout<<"history size:"<<history_.size()<<endl;
  p=0;
  return pos();
}

landmark_t
Particle::pos(double t) const
{
  int source = pos_.get<0>();
  int target = pos_.get<1>();
  double p = pos_.get<2>();
//  cout<<"t:"<<t<<endl;
  if (t >= 0) {
    auto cur = std::upper_bound(
        history_.begin(), history_.end(), t,
        [](const double t, const std::pair<double, int> &p)
        { return t < p.first; });

    if (history_.cend() == cur) {
    //cout<<"0"<<endl;
      double left = (t - history_.back().first) * velocity_,
          w = g_.weight(source, target);

      if (left > p * w) {
        source = target = history_.back().second;
        p = 0;
      } else p = left / w;

    } else {
      auto pre = std::prev(cur);
      //cout<<"stop1:"<<cur->first<<endl;
      source = pre->second;
      target = cur->second;
      p = (t - pre->first) * velocity_ / g_.weight(source, target);
    }
  }
//  cout<<"t:"<<t<<endl;
//  cout<<"source:"<<source<<endl;
//  cout<<"target:"<<target<<endl;
//  cout<<"p:"<<p<<endl;
  return boost::make_tuple(source, target, p);
}

landmark_t
Particle::pos1(double t) const
{
  int source = pos_.get<0>();
  int target = pos_.get<1>();
  double p = pos_.get<2>();
//  cout<<"t:"<<t<<endl;
  if (t >= 0) {
    auto cur = std::upper_bound(
        history_.begin(), history_.end(), t,
        [](const double t, const std::pair<double, int> &p)
        { return t < p.first; });

    if (history_.cend() == cur) {
    //cout<<"0"<<endl;
      double left = (t - history_.back().first) * velocity_,
          w = g_.weight(source, target);

      if (left > p * w) {
        source = target = history_.back().second;
        p = 0;
      } else p = left / w;

    } else {
      auto pre = std::prev(cur);
      //cout<<"stop1:"<<cur->first<<endl;
      source = pre->second;
      target = cur->second;
      p = (t - pre->first) * velocity_ / g_.weight(source, target);
    }
  }
//  cout<<"t:"<<t<<endl;
//  cout<<"source:"<<source<<endl;
//  cout<<"target:"<<target<<endl;
//  cout<<"p:"<<p<<endl;
  return boost::make_tuple(source, target, p);
}

void
Particle::print(std::ostream &ostream) const
{
  for (auto it = history_.cbegin(); it != history_.cend(); ++it)
    ostream << it->first << ' ' << it->second << std::endl;
}

}


