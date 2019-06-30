#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>

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
  duration_ = duration;
  for (auto it = objects_.begin(); it != objects_.end(); ++it)
    it->advance(duration);

  // Generate readings for all the objects at all timestamps.
  readings_.clear();
  boost::random::uniform_real_distribution<> unifd(0, 1);
  for (size_t i = 0; i < objects_.size(); ++i) {
    std::vector<int> tmp;
    for (int j = 0; j < duration_; ++j) {
      if (unifd(gen) > success_rate_) tmp.push_back(-1);
      else tmp.push_back(g_.detected_by(objects_[i].pos(j), radius_));
    }
    readings_.push_back(tmp);
  }
}

bool
Simulator_impl_::predict_(
    boost::unordered_map<int, boost::unordered_map<int, double> > &out,
    int obj, double t, int limit)
{
  const std::vector<int> &reading = readings_[obj];

  // The number of valid readings, i.e. reading >= 0, in [start, end]
  // is *limit*.
  int end = t;
  int start = end;
  {
    int last = -1;
    int count = 0;
    for (/* empty */; count < limit && start >= 0; --start) {
      if (reading[start] >= 0 && reading[start] != last) {
        ++count;
        last = reading[start];
      }
    }

    // Not enough observation
    if (count < limit) return false;

    // The last decrement is uncalled for.
    ++start;
  }

  // Initialize subparticles
  std::list<Particle> subparticles;
  for (int i = 0; i < num_particle_; ++i)
    subparticles.push_back(
        Particle(g_, obj, random_inside_reader(reading[start])));

  // This is the filter process where we eliminate those that missed
  // the reader.  This is NOT particle filter, just an extention of
  // symbolic model IMO.
  for (int i = start + 1; i <= end; ++i) {
    for (auto it = subparticles.begin(); it != subparticles.end();
         /* empty */) {
      landmark_t p = it->advance();
      if (reading[i] >= 0 && g_.detected_by(p, radius_) != reading[i])
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

  // Predicting.  During the *remain*, the object's position is
  // unknown, which is exactly what we'd like to predict.
  double remain = t - end;

  double prob = 1.0 / num_particle_;
  for (auto it = subparticles.begin(); it != subparticles.end();
       ++it) {
    landmark_t p = it->advance(remain);
    out[g_.align(p)][it->id()] += prob;
  }
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
