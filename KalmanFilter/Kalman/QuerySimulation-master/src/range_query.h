#ifndef RANGE_QUERY_H_
#define RANGE_QUERY_H_

#pragma once

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

#include "simulator.h"

namespace simulation {

class RangeQuery
{
 public:
  RangeQuery(Simulator &sim)
      :  sim_(sim) { }

  void
  prepare(double t);

  boost::unordered_map<int, double>
  query();

  boost::unordered_map<int, double>
  predict();

  bool
  random_window(double ratio);

 private:
  Simulator &sim_;
};

}

#endif  // RANGE_QUERY_H_
