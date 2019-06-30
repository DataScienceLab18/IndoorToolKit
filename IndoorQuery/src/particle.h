#ifndef SRC_PARTICLE_H_
#define SRC_PARTICLE_H_

#pragma once

#include <iostream>
#include <vector>
#include <utility>  // std::pair

#include "walkinggraph.h"

namespace simulation {

class Particle
{
 public:
  Particle(const WalkingGraph &g, int id = -1,
           const landmark_t &pos = {0, 0, -1});
  Particle(const Particle &other);

  landmark_t
  advance(double duration = -1.0);

  landmark_t
  pos(double t = -1) const;

  void
  print(std::ostream &os) const;

  const int
  id() const { return id_; }

  double
  v() const { return velocity_; }

 private:

  const WalkingGraph &g_;

  const int id_;
  landmark_t pos_;
  double velocity_;

  // <timestamp, nodeid>
  std::vector<std::pair<double, int> > history_;
};

}

#endif  // SRC_PARTICLE_H_
