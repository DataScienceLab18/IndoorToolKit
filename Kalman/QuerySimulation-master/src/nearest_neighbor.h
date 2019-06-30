#ifndef NEAREST_NEIGHBOR_H_
#define NEAREST_NEIGHBOR_H_

#pragma once

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#include "simulator.h"

namespace simulation {

class NearestNeighbor
{
 public:
  NearestNeighbor(Simulator &sim);

  void
  prepare(double t);

  boost::unordered_map<int, double>
  query(int k);

  boost::unordered_map<int, double>
  predict(int k);

  void
  random_object();

 private:
  Simulator &sim_;
};

}

#endif  // NEAREST_NEIGHBOR_H_
