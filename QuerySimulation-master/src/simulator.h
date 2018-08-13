#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#pragma once

#include <boost/parameter/name.hpp>
#include <boost/parameter/preprocessor.hpp>
#include <boost/parameter/keyword.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <CGAL/Point_set_2.h>

#include "walkinggraph.h"
#include "particle.h"

namespace simulation {

namespace param {

BOOST_PARAMETER_NAME(num_object)
BOOST_PARAMETER_NAME(num_particle)
BOOST_PARAMETER_NAME(radius)
BOOST_PARAMETER_NAME(unit)
BOOST_PARAMETER_NAME(knock_door_prob)
BOOST_PARAMETER_NAME(enter_room_prob)
BOOST_PARAMETER_NAME(threshold)
BOOST_PARAMETER_NAME(success_rate)

}

class Simulator_impl_
{

  friend class RangeQuery;
  friend class NearestNeighbor;

 public:

  void
  run(double duration);

  std::vector<landmark_t>
  positions(double t);

  boost::unordered_map<int, boost::unordered_map<int, double> >
  predict(double t);

  boost::unordered_map<int, boost::unordered_map<int, double> >
  predict1(double t);

  // protected:

  Simulator_impl_() {}

  template <class ArgumentPack>
  Simulator_impl_(const ArgumentPack &args)
      : num_object_(args[param::_num_object])
      , num_particle_(args[param::_num_particle | 64])
      , radius_(args[param::_radius | 120.0])
      , unit_(args[param::_unit | 20])
      , knock_door_prob_(args[param::_knock_door_prob | 0.1])
      , enter_room_prob_(args[param::_enter_room_prob | 0.1])
      , success_rate_(args[param::_success_rate | 0.95])
  {
    initialize();
  }

  void
  initialize();

  landmark_t
  random_inside_reader(int i) const;

  bool
  predict_(boost::unordered_map<
           int, boost::unordered_map<int, double> > &out,
           int obj, double t, int limit = 2);

  bool
  predict1_(boost::unordered_map<
            int, boost::unordered_map<int, double> > &out,
            int obj, double t, int limit = 2);

  // system parameters
  int num_object_;
  int num_particle_;
  double radius_;
  double unit_;
  double knock_door_prob_;
  double enter_room_prob_;
  double success_rate_;

  WalkingGraph g_;

  // objects in the system
  std::vector<Particle> objects_;
};

class Simulator : public Simulator_impl_
{
 public:
  BOOST_PARAMETER_CONSTRUCTOR(
      Simulator, (Simulator_impl_), param::tag,
      (required
       (num_object, *))
      (optional
       (num_particle, *)
       (radius, *)
       (unit, *)
       (knock_door_prob, *)
       (enter_room_prob, *)))
};

}

#endif  // SIMULATOR_H_
