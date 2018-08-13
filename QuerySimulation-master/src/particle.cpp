#include <algorithm>

#include <boost/random.hpp>
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
  boost::random::normal_distribution<> norm(120, 10);
  velocity_ = norm(gen);

  pos_ = pos.get<2>() < 0 ? g.random_pos() : pos;

  history_.push_back(std::make_pair(
      -pos_.get<2>() * g_.weight(pos_.get<0>(),
                                 pos_.get<1>()) / velocity_,
      pos_.get<0>()));
}

Particle::Particle(const Particle &other)
    : g_(other.g_)
    , id_(other.id_)
    , pos_(other.pos_)
    , history_(other.history_)
{
  boost::random::normal_distribution<> norm(other.velocity_, 10);
  velocity_ = norm(gen);
}

landmark_t
Particle::advance(double duration)
{
  int &source = pos_.get<0>();
  int &target = pos_.get<1>();
  double &p = pos_.get<2>();

  double w = g_.weight(source, target);
  double elapsed = history_.back().first + w * p / velocity_,
      left = w * (1 - p),
      dist = duration <= 0 ? velocity_ - left
      : duration * velocity_ - left;

  boost::random::uniform_real_distribution<> unifd(0, 1);

  while (true) {
    elapsed += left / velocity_;
    history_.push_back(std::make_pair(elapsed, target));

    if (dist < 0) break;

    int pre = source;
    source = target;
    target = g_.random_next(source, pre);
    left = g_.weight(source, target);

    dist -= left;
  }

  p = 1 + dist / g_.weight(source, target);

  return pos();
}

landmark_t
Particle::pos(double t) const
{
  int source = pos_.get<0>();
  int target = pos_.get<1>();
  double p = pos_.get<2>();

  if (t >= 0) {
    auto cur = std::upper_bound(
        history_.begin(), history_.end(), t,
        [](const double t, const std::pair<double, int> &p)
        { return t < p.first; });

    if (history_.cend() == cur) {
      double left = (t - history_.back().first) * velocity_,
          w = g_.weight(source, target);

      if (left > p * w) {
        source = target = history_.back().second;
        p = 0;
      } else p = left / w;

    } else {
      auto pre = std::prev(cur);
      source = pre->second;
      target = cur->second;
      p = (t - pre->first) * velocity_ / g_.weight(source, target);
    }
  }

  return boost::make_tuple(source, target, p);
}

void
Particle::print(std::ostream &ostream) const
{
  for (auto it = history_.cbegin(); it != history_.cend(); ++it)
    ostream << it->first << ' ' << it->second << std::endl;
}

}
