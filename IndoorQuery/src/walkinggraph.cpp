#include <cmath>
#include <fstream>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

#include <boost/iterator/zip_iterator.hpp>

#include <boost/algorithm/string.hpp>

#include "walkinggraph.h"
#include "global.h"

namespace simulation {

Point_2
linear_interpolate(const Point_2 &p0, const Point_2 &p1, double a)
{
  return p0 + (p1 - p0) * a;
}

WalkingGraph::graph_::graph_(const WalkingGraph::graph_ &o)
    :g(o.g)
{
  v2n = boost::get(boost::vertex_name, g);
  n2v.clear();
  for (auto it = boost::vertices(g); it.first != it.second;
       ++it.first)
    n2v[vid(*it.first)] = *it.first;

  e2n = boost::get(boost::edge_name, g);
  n2e.clear();
  for (auto it = boost::edges(g); it.first != it.second;
       ++it.first)
    n2e[eid(*it.first)] = *it.first;
}

WalkingGraph::graph_ &
WalkingGraph::graph_::operator= (const WalkingGraph::graph_ &o)
{
  g = o.g;

  v2n = boost::get(boost::vertex_name, g);
  n2v.clear();
  for (auto it = boost::vertices(g); it.first != it.second;
       ++it.first)
    n2v[vid(*it.first)] = *it.first;

  e2n = boost::get(boost::edge_name, g);
  n2e.clear();
  for (auto it = boost::edges(g); it.first != it.second;
       ++it.first)
    n2e[eid(*it.first)] = *it.first;

  return *this;
}

WalkingGraph::WalkingGraph()
{
  initialize();
  insert_anchors();
}

void
WalkingGraph::initialize()
{
  const std::string Commenter("//");
  const std::string DataRoot("/home/gongzhitaao/Documents/"
                             "simulator/data/jiao/");

  // Read in vertices for walking graph.
  {
    std::ifstream fin(DataRoot + "node.txt");
    std::string line;
    while (std::getline(fin, line)) {
      boost::algorithm::trim_left(line);
      if (line.compare(0, Commenter.size(), Commenter) == 0) continue;

      std::stringstream ss(line);
      int name, type;
      double x, y;
      ss >> name >> x >> y >> type;
      wg_.addv(name);
      vp_[name] = {Point_2(x, y), (vertex_color_enum) type};
    }
    fin.close();
  }

  // Read in edge for walking graph
  {
    std::ifstream fin(DataRoot + "edge.txt");
    std::string line;
    for (int id = 0; std::getline(fin, line); ++id) {
      boost::algorithm::trim_left(line);
      if (line.compare(0, Commenter.size(), Commenter) == 0) continue;

      std::stringstream ss(line);
      int n1, n2;
      ss >> n1 >> n2;
      wg_.adde(n1, n2, id);
      ep_[id].weight = std::sqrt(
          CGAL::squared_distance(coord(n1), coord(n2)));
    }
    fin.close();
  }

  // Read in RFID readers.
  {
    std::vector<int> infos;
    std::vector<Point_2> points;

    std::ifstream fin(DataRoot + "rfid2.txt");
    std::string line;
    while (std::getline(fin, line)) {
      boost::algorithm::trim_left(line);
      if (line.compare(0, Commenter.size(), Commenter) == 0) continue;

      std::stringstream ss(line);
      int id, v1, v2;
      double x, y;
      ss >> id >> x >> y >> v1 >> v2;

      Point_2 p0(x, y), p1 = coord(v1), p2 = coord(v2);
      double p = std::sqrt(CGAL::squared_distance(p0, p1) /
                           CGAL::squared_distance(p1, p2));
      infos.push_back(id);
      points.push_back(p0);
      readermap_[id] = {v1, v2, p};
    }
    fin.close();

    readerset_.insert(
        boost::make_zip_iterator(boost::make_tuple(points.begin(),
                                                   infos.begin())),
        boost::make_zip_iterator(boost::make_tuple(points.end(),
                                                   infos.end())));
  }

  // Read in rooms configuration.
  {
    std::ifstream fin(DataRoot + "room.txt");
    std::string line;
    while (std::getline(fin, line)) {
      boost::algorithm::trim_left(line);
      if (line.compare(0, Commenter.size(), Commenter) == 0) continue;

      std::stringstream ss(line);
      double x0, y0, x1, y1;
      int id;
      ss >> id >> x0 >> y0 >> x1 >> y1;
      rooms_.push_back(IsoRect_2(x0, y0, x1, y1));

      if (x1 > xmax_) xmax_ = x1;
      if (y1 > ymax_) ymax_ = y1;
    }
    fin.close();
  }

  // Read in halls configuration
  {
    std::ifstream fin(DataRoot + "hall.txt");
    std::string line;
    while (std::getline(fin, line)) {
      boost::algorithm::trim_left(line);
      if (line.compare(0, Commenter.size(), Commenter) == 0) continue;
      std::stringstream ss(line);
      double x0, y0, x1, y1;
      int dir;
      ss >> x0 >> y0 >> x1 >> y1 >> dir;
      halls_.push_back(IsoRect_2(x0, y0, x1, y1));
      dirs_.push_back(dir);
    }
    fin.close();
  }
}

class InsertAnchor : public boost::default_bfs_visitor
{
 public:
  InsertAnchor(WalkingGraph &g, double unit = 20.0)
      : g_(g), name_(g_.ANCHORID), unit_(unit) { }

  void
  initialize_vertex(Vertex v, const UndirectedGraph &)
  { dist_[v] = 0.0; }

  void
  tree_edge(Edge e, const UndirectedGraph &g)
  {
    Vertex source = boost::source(e, g),
           target = boost::target(e, g);
    int id = g_.wg_.eid(e);

    double d = dist_[source],
           w = g_.ep_.at(id).weight;

    g_.ep_.at(id).anchors.push_back(
        std::make_pair(g_.wg_.vid(source), 0.0));
    while (d < w) {
      g_.ep_.at(id).anchors.push_back(std::make_pair(name_++, d / w));
      d += unit_;
    }
    g_.ep_.at(id).anchors.push_back(
        std::make_pair(g_.wg_.vid(target), 1.0));
    dist_[target] = d - w;
  }

 private:
  boost::unordered_map<Vertex, double> dist_;
  WalkingGraph &g_;
  int name_;
  double unit_;
};

void
WalkingGraph::insert_anchors(double unit)
{
  // calculate the anchor points in a bfs fashion
  InsertAnchor vis(*this, 20);
  boost::breadth_first_search(wg_(), boost::random_vertex(wg_(), gen),
                              boost::visitor(vis));

  ag_ = wg_;

  int id = boost::num_edges(ag_());

  std::vector<int> infos;
  std::vector<Point_2> points;

  for (auto it = boost::edges(wg_()); it.first != it.second;
       ++it.first, ++id) {

    const std::vector<std::pair<int, double> > &vec =
        ep_.at(wg_.eid(*it.first)).anchors;
    int size = vec.size();

    if (size <= 2) continue;

    Vertex source = ag_.v(vec[0].first),
           target = ag_.v(vec.back().first),
                u = source;

    int from = vec[0].first,
          to = vec.back().first;

    double w = weight(vec[0].first, vec.back().first);
    boost::remove_edge(source, target, ag_());

    for (int i = 1; i < size - 1; ++i) {
      Point_2 p = linear_interpolate(coord(from),
                                     coord(to), vec[i].second);

      Vertex v = ag_.addv(vec[i].first);
      vertex_color_enum c = HALL;
      if (color(from) == ROOM || color(to) == ROOM) c = ROOM;

      vp_[vec[i].first] = {p, c};
      ap_[vec[i].first] = {from, to, vec[i].second};

      points.push_back(p);
      infos.push_back(vec[i].first);

      ag_.adde(u, v, id);
      ep_[id].weight = w * (vec[i].second - vec[i-1].second);

      u = v;
    }

    ag_.adde(u, ag_.v(vec.back().first), id);
    ep_[id].weight = w * (1 - vec[size-2].second);
  }

  for (auto it = boost::vertices(ag_()); it.first != it.second;
       ++it.first) {
    int name = ag_.vid(*it.first);
    infos.push_back(name);
    points.push_back(coord(name));
  }

  anchorset_.insert(
      boost::make_zip_iterator(boost::make_tuple(points.begin(),
                                                 infos.begin())),
      boost::make_zip_iterator(boost::make_tuple(points.end(),
                                                 infos.end())));
}

// Randomly choose next vertex to advance to.  If u which is where the
// particle came from is present, then the randomly chosen vertex may
// not be u unless the out degree of v is only one in which we have no
// choice.  In this way, the particle preserves its direction.
int
WalkingGraph::random_next(int to, int from) const
{
  if (color(from) == ROOM) {
    boost::random::uniform_real_distribution<> unifd(0, 1);
    if (unifd(gen) > .2)
      return from;
  }

  Vertex cur = wg_.v(to);
  Vertex pre = from < 0 ? wg_().null_vertex() : wg_.v(from);

  boost::unordered_set<Vertex> outs;
  auto pit = boost::out_edges(cur, wg_());
  for (auto it = pit.first; it != pit.second; ++it)
    outs.insert(boost::target(*it, wg_()));

  if (outs.size() > 1)
    outs.erase(pre);

  boost::random::uniform_int_distribution<> unifi(0, outs.size() - 1);
  return wg_.vid(*boost::next(outs.begin(), unifi(gen)));

  // boost::unordered_set<Vertex> hall, door, room;

  // auto pit = boost::out_edges(cur, wg_());
  // for (auto it = pit.first; it != pit.second; ++it) {
  //   Vertex v = boost::target(*it, wg_());
  //   switch (color(wg_.vid(v))) {
  //     case HALL: hall.insert(v); break;
  //     case DOOR: door.insert(v); break;
  //     case ROOM: room.insert(v); break;
  //     default: break;
  //   }
  // }

  // if (hall.size() == 0 || wg_().null_vertex() == pre) {
  //   if (hall.size() > 0) {
  //     boost::random::uniform_int_distribution<>
  //         unifi(0, hall.size() - 1);
  //     return wg_.vid(*boost::next(hall.begin(), unifi(gen)));
  //   }
  //   return wg_.vid(*door.begin());
  // }

  // boost::random::uniform_real_distribution<> unifd(0, 1);

  // if (room.size() > 0 && color(wg_.vid(pre)) != ROOM &&
  //     unifd(gen) < enter_room_)
  //   return wg_.vid(*room.begin());

  // if (door.size() > 0 && (color(wg_.vid(cur)) == ROOM ||
  //                         unifd(gen) < knock_door_))
  //   return wg_.vid(*door.begin());

  // if (hall.size() > 1) hall.erase(pre);

  // boost::random::uniform_int_distribution<> unifi(0, hall.size() - 1);
  // return wg_.vid(*boost::next(hall.begin(), unifi(gen)));
}

landmark_t
WalkingGraph::random_pos() const
{
  Edge e = boost::random_edge(wg_(), gen);
  boost::random::uniform_real_distribution<> unifd(0, 1);
  return boost::make_tuple(wg_.vid(boost::source(e, wg_())),
                           wg_.vid(boost::target(e, wg_())),
                           unifd(gen));
}

IsoRect_2
WalkingGraph::random_window_aux(double ratio) const
{
  /* METHOD 1: Ramdom rectangular window */
  double r = 1 - std::sqrt(ratio);
  boost::random::uniform_real_distribution<>
      unifx(0, xmax_ * r), unify(0, ymax_ * r);
  double xmin = unifx(gen), ymin = unify(gen);

  return IsoRect_2(xmin, ymin, xmin + xmax_ * (1 - r),
                   ymin + ymax_ * (1 - r));

  /* METHOD 2: Random square window with center fixed on edges. */
  // double r = std::sqrt(xmax_ * ymax_ * ratio) / 2.0;

  // Edge e = boost::random_edge(wg_(), gen);
  // boost::random::uniform_real_distribution<> unifd(0, 1);
  // Point_2 center = linear_interpolate(
  //     coord(wg_.vid(boost::source(e, wg_()))),
  //     coord(wg_.vid(boost::target(e, wg_()))),
  //     unifd(gen));

  // return IsoRect_2(center.x() - r, center.y() - r, center.x() + r,
  //                  center.y() + r);

  /* METHOD 3: Fix query window in the hall way. */
  // boost::random::uniform_int_distribution<>
  //     unifi(0, halls_.size() - 1);
  // IsoRect_2 hall = halls_[unifi(gen)];

  // double r = std::sqrt(ratio),
  //     rx = r * (hall.xmax() - hall.xmin()),
  //     ry = r * (hall.ymax() - hall.ymin());
  // boost::random::uniform_int_distribution<>
  //     unifx(hall.xmin(), hall.xmax() - rx),
  //     unify(hall.ymin(), hall.ymax() - ry);

  // double x = unifx(gen), y = unify(gen);

  // return IsoRect_2(x, y, x + rx, y + ry);
}

std::vector<std::pair<IsoRect_2, double> >
WalkingGraph::random_window(double ratio) const
{
  IsoRect_2 win = random_window_aux(ratio);

  std::vector<std::pair<IsoRect_2, double> > results;

  // Intersection with rooms
  for (size_t i = 0; i < rooms_.size(); ++i) {
    auto res = CGAL::intersection(win, rooms_[i]);
    // if the query intersects with a room, then the intersected part
    // extends to the whole room.
    if (res) {
      const IsoRect_2 tmp = *boost::get<IsoRect_2>(&*res);
      const IsoRect_2 room = rooms_[i];
      results.push_back(std::make_pair(tmp, tmp.area() / room.area()));
    }
  }

  // Intersection with halls.
  for (size_t i = 0; i < halls_.size(); ++i) {
    auto res = CGAL::intersection(win, halls_[i]);
    if (res) {
      const IsoRect_2 tmp = *boost::get<IsoRect_2>(&*res);
      const IsoRect_2 hall = halls_[i];
      if (0 == dirs_[i])
        results.push_back(std::make_pair(
            IsoRect_2(Point_2(tmp.xmin(), hall.ymin()),
                      Point_2(tmp.xmax(), hall.ymax())),
            (tmp.ymax() - tmp.ymin()) / (hall.ymax() - hall.ymin())));
      else
        results.push_back(std::make_pair(
            IsoRect_2(Point_2(hall.xmin(), tmp.ymin()),
                      Point_2(hall.xmax(), tmp.ymax())),
            (tmp.xmax() - tmp.xmin()) / (hall.xmax() - hall.xmin())));
    }
  }

  return results;
}

int
WalkingGraph::detected_by(const landmark_t &pos, double radius)
{
  Point_2 center = linear_interpolate(
      coord(pos.get<0>()), coord(pos.get<1>()), pos.get<2>());
  Circle_2 circle(center, radius * radius);

  std::vector<vertex_handle> res;
  readerset_.range_search(circle, std::back_inserter(res));

  if (res.empty()) return -1;

  return (*res.begin())->info();
}

void
WalkingGraph::print(std::ostream &os) const
{
  // boost::graph_traits<UndirectedGraph>::vertex_iterator vi, end;
  // for (boost::tie(vi, end) = boost::vertices(wg_); vi != end; ++vi)
  //   os << boost::get(coords_, *vi) << std::endl;

  boost::graph_traits<UndirectedGraph>::edge_iterator ei, eend;
  for (boost::tie(ei, eend) = boost::edges(wg_()); ei != eend; ++ei) {
    Vertex v = boost::source(*ei, wg_());
    Vertex u = boost::target(*ei, wg_());
    os << coord(wg_.vid(v)) << ' ' << coord(wg_.vid(u)) << std::endl;
  }
}

int
WalkingGraph::align(const landmark_t &p)
{
  Point_2 pt = coord(p);
  return anchorset_.nearest_neighbor(pt)->info();
}

std::vector<int>
WalkingGraph::anchors_in_win(const IsoRect_2 &w)
{
  std::vector<vertex_handle> tmp;
  anchorset_.range_search(w.vertex(0), w.vertex(1),
                          w.vertex(2), w.vertex(3),
                          std::back_inserter(tmp));
  std::vector<int> results;
  std::transform(tmp.begin(), tmp.end(), std::back_inserter(results),
                 [] (const vertex_handle vh) {
                   return vh->info();
                 });
  return results;
}

}
