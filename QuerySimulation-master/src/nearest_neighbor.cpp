#include <algorithm>
#include <iterator>
#include <utility>

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

#include "nearest_neighbor.h"
#include "global.h"

namespace simulation {

using std::cout;
using std::endl;

template <typename ID, typename W>
class WeightMap
{
 public:
  typedef typename boost::property_traits<ID>::key_type key_type;
  typedef typename W::mapped_type value_type;
  typedef typename W::mapped_type &reference;

  typedef boost::lvalue_property_map_tag category;

  WeightMap(ID a = ID(), W b = W())
      : id(a), w(b) { }

  ID id;
  W w;
};

template <typename ID, typename W>
typename W::mapped_type
get(const WeightMap<ID, W> &w,
    typename boost::property_traits<ID>::key_type e)
{ return w.w.at(boost::get(w.id, e)); }

template <typename ID, typename W>
void
put(WeightMap<ID, W> &w,
    typename boost::property_traits<ID>::key_type e,
    const typename W::mapped_type &d)
{ w.w[boost::get(w.id, e)] = d; }

template <typename ID, typename W>
typename W::mapped_type &
at(const WeightMap<ID, W> &w,
   typename boost::property_traits<ID>::key_type e)
{ return w.w.at(boost::get(w.id, e)); }

static WalkingGraph g_;
static boost::unordered_map<
  int, std::vector<std::pair<int, double> > > landmarks_;
static WeightMap<
  edge_name_t, boost::unordered_map<int, double> > w0_, w1_;
static int object_;
static boost::unordered_map<
  int, boost::unordered_map<int, double> > probs_;

NearestNeighbor::NearestNeighbor(Simulator &sim)
    : sim_(sim)
{
  g_ = sim.g_;

  w0_ = WeightMap<edge_name_t,
                  boost::unordered_map<int, double> >(g_.wg_.e2n);
  w1_ = WeightMap<edge_name_t,
                  boost::unordered_map<int, double> >(g_.ag_.e2n);

  for (auto i = boost::edges(g_.wg_()); i.first != i.second;
       ++i.first)
    put(w0_, *i.first, g_.ep_.at(g_.wg_.eid(*i.first)).weight);

  for (auto i = boost::edges(g_.ag_()); i.first != i.second;
       ++i.first)
    put(w1_, *i.first, g_.ep_.at(g_.ag_.eid(*i.first)).weight);
}

void
NearestNeighbor::random_object()
{
  boost::random::uniform_int_distribution<>
      unifd(0, sim_.num_object_ - 1);
  object_ = unifd(gen);
  //cout<<"numL:"<<sim_.num_object_<<endl;
  //cout<<"num:"<<gen<<endl;
  //cout<<"object of random:"<<object_<<endl;
}

void
NearestNeighbor::prepare(double t)
{
  probs_ = sim_.predict(t);
  g_.wg_ = sim_.g_.wg_;
  landmarks_.clear();
//  cout<<"query time:"<<t<<endl;
//cout<<"landmarks1_:"<<landmarks_.size()<<endl;
  std::vector<landmark_t> points = sim_.positions(t);
  //cout<<points.size()<<endl;
  for (size_t i = 0; i < points.size(); ++i) {

    landmark_t &p = points[i];
//    cout<<i<<"point:"<<p.get<0>()<<" to  :  "<<p.get<1>()<<":"<<p.get<2>()<<endl;
    Edge e = g_.wg_.e(p.get<0>(), p.get<1>());
    int s = g_.wg_.vid(boost::source(e, g_.wg_()));
    //cout<<"s:"<<s<<endl;
    //cout<<"e:"<<g_.wg_.eid(e)<<endl;
    std::vector<std::pair<int, double> > &vec =
        landmarks_[g_.wg_.eid(e)];

    //cout<<"landmarks2_:"<<landmarks_.size()<<endl;
    if (vec.empty()) vec.push_back({s, 0.0});

    vec.push_back({i, s == p.get<0>() ? p.get<2>() : 1 - p.get<2>()});
  }

  {
    int eid = g_.USEREDGE;
    const int idbase_ = g_.OBJECTID;
    //cout<<"landmarks_:"<<landmarks_.size()<<endl;
    for (auto i = landmarks_.begin(); i != landmarks_.end(); ++i) {
      Edge e = g_.wg_.e(i->first);
//      cout<<"edge:"<<i->first<<endl;
      double w = get(w0_, e);
//      cout<<"w"<<w<<endl;
      std::vector<std::pair<int, double> > &vec = i->second;

      std::stable_sort(vec.begin(), vec.end(),
                       [](const std::pair<int, double> &a,
                          const std::pair<int, double> &b) {
                         return a.second < b.second;
                       });

      boost::remove_edge(e, g_.wg_());
      Vertex u = g_.wg_.v(vec.begin()->first);

      for (auto j = std::next(vec.begin()); j != vec.end(); ++j) {
        Vertex v = g_.wg_.addv(idbase_ + j->first);
        Edge t = g_.wg_.adde(u, v, eid++);
        put(w0_, t, w * (j->second - std::prev(j)->second));
        u = v;
      }

      // find the right `target'
      Vertex v = g_.wg_.v(vec.begin()->first);
      if (boost::target(e, g_.wg_()) == v)
        v = boost::source(e, g_.wg_());
      else v = boost::target(e, g_.wg_());

      Edge t = g_.wg_.adde(u, v, eid++);
//      cout<<"u:"<<u<<"v:"<<v<<"weight:"<<w * (1 - vec.back().second)<<endl;
      put(w0_, t,vec.back().second<1?w * (1 - vec.back().second):0);
    }
  }

  try {
    Vertex v = g_.ag_.v(object_ + g_.OBJECTID);
    boost::remove_vertex(v, g_.ag_());
  } catch (...) { }

  {
    int eid = g_.USEREDGE;

    Vertex v = g_.ag_.addv(object_ + g_.OBJECTID),
           s = g_.ag_.v(points[object_].get<0>()),
           d = g_.ag_.v(points[object_].get<1>());
//           cout<<"v0 :"<<g_.ag_.v(1)<<endl;
//    cout<<"new :"<<object_<<endl;
//    cout<<"new2 :"<<points[object_].get<0>()<<endl;
//    cout<<"new3 :"<<points[object_].get<1>()<<endl;
//    cout<<"v"<<v<<endl;
//    cout<<"s"<<s<<endl;
//    cout<<"d"<<d<<endl;
    Edge e1 = g_.ag_.adde(s, v, eid++),
         e2 = g_.ag_.adde(v, d, eid++);

    double p = points[object_].get<2>();
//    cout<<"p:"<<p<<endl;
    put(w1_, e1, 0.5);
    put(w1_, e2, 0.5);
  }
}

struct found_nn {};

class nearest_neighbor_visitor :
      public boost::default_d
      ijkstra_visitor
{
 public:
  nearest_neighbor_visitor(
      int k, boost::unordered_map<int, double> &result, int limit)
      : k_(k), result_(result), limit_(limit) { }

  template <typename V, typename G>
  void
  discover_vertex(V v, const G &g)
  {
    int vid = g_.wg_.vid(v);
    if (vid >= limit_) {
      result_[vid - limit_] = 1.0;

      if (--k_ == 0)
        throw found_nn();
    }
  }

 private:
  int k_;
  boost::unordered_map<int, double> &result_;
  int limit_;
};

boost::unordered_map<int, double>
NearestNeighbor::query(int k)
{
  boost::unordered_map<int, double> result;

  nearest_neighbor_visitor vis(k, result, g_.OBJECTID);
//  cout<<"objectTarget"<<object_<<endl;
//  cout<<"limit"<<g_.OBJECTID<<endl;
  try {
    boost::dijkstra_shortest_paths(
        g_.wg_(), g_.wg_.v(object_ + g_.OBJECTID),
        boost::visitor(vis).weight_map(w0_));
  } catch (found_nn) {
   cout<<"___error____"<<endl;}
//  cout<<"_______"<<endl;
   //cout<<"object"<<result.size()<<endl;
  for (auto it = result.cbegin(); it != result.cend(); ++it)
    {
//        cout<<it->first<<":"<<it->second<<endl;
    }
  return result;
}

class nearest_neighbor_prob_visitor
    : public boost::default_dijkstra_visitor
{
 public:
  nearest_neighbor_prob_visitor(
      int k, boost::unordered_map<int, double> &result)
      : k_(k), result_(result), p_(0.0) { }

  template <typename V, typename G>
  void
  discover_vertex(V v, const G &g)
  {
    int vid = g_.ag_.vid(v);
    auto it = probs_.find(vid);
    if (it != probs_.end()) {
      boost::unordered_map<int, double> &objs = it->second;
      for (auto i = objs.begin(); i != objs.end(); ++i) {
        result_[i->first] += i->second;
        p_ += i->second;
      }
      //cout<<"vid->"<<vid<<":"<<p_<<endl;
      if (p_ >= k_)
        throw found_nn();
    }
  }

 private:
  int k_;
  boost::unordered_map<int, double> &result_;
  double p_;
};

boost::unordered_map<int, double>
NearestNeighbor::predict(int k)
{
  boost::unordered_map<int, double> result;
//  cout<<"_______________K:"<<k<<endl;
  nearest_neighbor_prob_visitor vis(k, result);
//  cout<<"xx"<<k<<endl;
  try {
    boost::dijkstra_shortest_paths(
        g_.ag_(), g_.ag_.v(object_ + g_.OBJECTID),
        boost::visitor(vis).weight_map(w1_));
  } catch (found_nn) { }
//  cout<<"predict_______"<<endl;
  for (auto it = result.cbegin(); it != result.cend(); ++it)
    {
//        cout<<it->first<<":"<<it->second<<endl;
    }
  return result;
}

}
