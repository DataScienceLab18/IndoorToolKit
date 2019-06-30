#include <iostream>

#include "range_query.h"
#include "global.h"

namespace simulation {

using std::cout;
using std::endl;

static std::vector<std::pair<IsoRect_2, double> > wins_;
static boost::unordered_map<int, double> anchors_;
static Point_set_2 objectset_;
static boost::unordered_map<
  int, boost::unordered_map<int, double> > probs_;

bool
RangeQuery::random_window(double ratio)
{
  wins_.clear();
  anchors_.clear();

  double r = 1 - std::sqrt(ratio);
  boost::random::uniform_real_distribution<>
      unifx(0, sim_.g_.xmax_ * r), unify(0, sim_.g_.ymax_ * r);

  double xmin = unifx(gen), ymin = unify(gen);

  IsoRect_2 win = IsoRect_2(xmin, ymin,
                            xmin + sim_.g_.xmax_ * (1 - r),
                            ymin + sim_.g_.ymax_ * (1 - r));

  // Intersection with rooms
  for (size_t i = 0; i < sim_.g_.rooms_.size(); ++i) {
    auto res = CGAL::intersection(win, sim_.g_.rooms_[i]);
    // if the query intersects with a room, then the intersected part
    // extends to the whole room.
    if (res) {
      const IsoRect_2 tmp = *boost::get<IsoRect_2>(&*res);
      const IsoRect_2 room = sim_.g_.rooms_[i];
      wins_.push_back(std::make_pair(tmp, tmp.area() / room.area()));

      std::vector<int> anchors =
          sim_.g_.anchors_in_win(wins_.back().first);
      double r = wins_.back().second;

      for (auto j = anchors.begin(); j != anchors.end(); ++j)
        anchors_[*j] = r;
    }
  }

  // Intersection with halls.
  for (size_t i = 0; i < sim_.g_.halls_.size(); ++i) {
    auto res = CGAL::intersection(win, sim_.g_.halls_[i]);
    if (res) {
      const IsoRect_2 tmp = *boost::get<IsoRect_2>(&*res);
      const IsoRect_2 hall = sim_.g_.halls_[i];
      if (0 == sim_.g_.dirs_[i])
        wins_.push_back(std::make_pair(
            IsoRect_2(Point_2(tmp.xmin(), hall.ymin()),
                      Point_2(tmp.xmax(), hall.ymax())),
            (tmp.ymax() - tmp.ymin()) / (hall.ymax() - hall.ymin())));
      else
        wins_.push_back(std::make_pair(
            IsoRect_2(Point_2(hall.xmin(), tmp.ymin()),
                      Point_2(hall.xmax(), tmp.ymax())),
            (tmp.xmax() - tmp.xmin()) / (hall.xmax() - hall.xmin())));

      std::vector<int> anchors =
          sim_.g_.anchors_in_win(wins_.back().first);
      double r = wins_.back().second;

      for (auto j = anchors.begin(); j != anchors.end(); ++j)
        anchors_[*j] = r;
    }
  }

  return !anchors_.empty();
}

void
RangeQuery::prepare(double t)
{
  std::vector<landmark_t> tmp = sim_.positions(t);
  std::vector<Point_2> points;
  std::vector<int> infos;
  for (size_t i = 0; i < tmp.size(); ++i) {
    points.push_back(sim_.g_.coord(tmp[i]));
    infos.push_back(i);
  }

  probs_ = sim_.predict(t);

  objectset_.clear();
  objectset_.insert(
      boost::make_zip_iterator(boost::make_tuple(points.begin(),
                                                 infos.begin())),
      boost::make_zip_iterator(boost::make_tuple(points.end(),
                                                 infos.end())));
}

boost::unordered_map<int, double>
RangeQuery::query()
{
  boost::unordered_map<int, double> result;
  for (auto w = wins_.begin(); w != wins_.end(); ++w) {
    std::vector<vertex_handle> tmp;
    objectset_.range_search(w->first.vertex(0), w->first.vertex(1),
                            w->first.vertex(2), w->first.vertex(3),
                            std::back_inserter(tmp));
    for (auto it = tmp.begin(); it != tmp.end(); ++it)
      result[(*it)->info()] = w->second;
  }
  return result;
}

boost::unordered_map<int, double>
RangeQuery::predict()
{
  boost::unordered_map<int, double> result;
  for (auto i = anchors_.begin(); i != anchors_.end(); ++i) {

    auto p = probs_.find(i->first);
    if (p == probs_.end()) continue;

    const boost::unordered_map<int, double> &prob = p->second;
    for (auto j = prob.begin(); j != prob.end(); ++j)
      result[j->first] += j->second * i->second;
  }
  return result;
}

}
