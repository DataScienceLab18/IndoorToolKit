#ifndef SRC_WALKINGGRAPH_H_
#define SRC_WALKINGGRAPH_H_

#pragma once

#include <vector>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/random.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Point_set_2.h>

namespace simulation {

// <source, target, perscentage>
typedef boost::tuple<int, int, double> landmark_t;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef K::Iso_rectangle_2 IsoRect_2;
typedef K::Circle_2 Circle_2;

typedef CGAL::Triangulation_vertex_base_with_info_2<int, K> Vb;
typedef CGAL::Triangulation_data_structure_2<Vb> Tds;
typedef CGAL::Point_set_2<K, Tds> Point_set_2;
typedef Point_set_2::Vertex_handle vertex_handle;

enum vertex_color_enum { HALL, DOOR, ROOM, VERTEX_COLOR_ENUM };

struct VP {
  Point_2 coord;
  vertex_color_enum color;
};

struct EP {
  double weight;
  std::vector<std::pair<int, double> > anchors;
};

typedef boost::property<boost::vertex_name_t, int> VertexProperty;
typedef boost::property<boost::edge_name_t, int> EdgeProperty;

typedef boost::adjacency_list<
  boost::vecS, boost::vecS, boost::undirectedS,
  VertexProperty, EdgeProperty> UndirectedGraph;
typedef boost::graph_traits<UndirectedGraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<UndirectedGraph>::edge_descriptor Edge;

typedef boost::property_map<UndirectedGraph,
                            boost::vertex_name_t>::type vertex_name_t;
typedef boost::property_map<UndirectedGraph,
                            boost::edge_name_t>::type edge_name_t;

Point_2
linear_interpolate(const Point_2 &p0, const Point_2 &p1, double a);

class WalkingGraph
{
  friend class InsertAnchor;
  friend class RangeQuery;
  friend class NearestNeighbor;

  enum { ANCHORID = 100, OBJECTID = 1000, USEREDGE = 10000};

  // Convinient wrapper for boost::graph, including map between vertex
  // descriptor and vertex name, edge descriptor and edge name.  This
  // is not labeled graph, so we need an extra map from name to
  // descriptor.
  class graph_
  {
   public:
    graph_()
        : v2n(boost::get(boost::vertex_name, g))
        , e2n(boost::get(boost::edge_name, g))
    { }

    graph_(const graph_ &o);

    graph_ &
    operator= (const graph_ &o);

    UndirectedGraph &
    operator() ()
    { return g; }

    const UndirectedGraph &
    operator() () const
    { return g; }

    UndirectedGraph g;

    vertex_name_t v2n;
    boost::unordered_map<int, Vertex> n2v;

    edge_name_t e2n;
    boost::unordered_map<int, Edge> n2e;

    Vertex
    v(int id) const
    { return n2v.at(id); }

    Vertex &
    v(int id)
    { return n2v.at(id); }

    Vertex
    addv(int id)
    { return n2v[id] = boost::add_vertex(id, g); }

    int
    vid(Vertex v) const
    { return boost::get(v2n, v); }

    Edge
    e(int id) const
    { return n2e.at(id); }

    Edge &
    e(int id)
    { return n2e.at(id); }

    Edge
    e(int s, int d) const
    { return boost::edge(v(s), v(d), g).first; }

    int
    eid(Edge e) const
    { return boost::get(e2n, e); }

    Edge
    adde(int s, int d, int id)
    { return n2e[id] = boost::add_edge(v(s), v(d), id, g).first; }

    Edge
    adde(Vertex s, Vertex d, int id)
    { return n2e[id] = boost::add_edge(s, d, id, g).first; }
  };

 public:

  WalkingGraph();

  // set the enter room probability
  void
  enter_room(double p)
  { enter_room_ = p; }

  // set the knock door probability
  void
  knock_door(double p)
  { knock_door_ = p; }

  // get the coordinate of a vertex by its name
  Point_2
  coord(int v) const
  { return vp_.at(v).coord; }

  // get the coordinate of a landmark
  Point_2
  coord(const landmark_t &pos) const
  {
    return linear_interpolate(coord(pos.get<0>()),
                              coord(pos.get<1>()),
                              pos.get<2>());
  }

  // Get the weight of and edge
  double
  weight(int u, int v) const
  { return ep_.at(wg_.eid(wg_.e(u, v))).weight; }

  // Get the color of a vertex, HALL, DOOR or ROOM.
  vertex_color_enum
  color(int v) const
  { return vp_.at(v).color; }

  // Generate a random vertex name in walking graph
  template <typename Generator>
  int
  random_vertex(Generator gen) const
  { return wg_.vid(boost::random_vertex(wg_(), gen)); }

  // Given current vertex and where we were, generate a random next
  // step.
  int
  random_next(int to, int from = -1) const;

  // Generate a random position along the walking graph.
  landmark_t
  random_pos() const;

  // Generate a random position inside a reader
  landmark_t
  reader_pos(int i) const { return readermap_.at(i); }

  // Given a window ratio, generate a random window and intersect the
  // window with the hall way and room.  Each newly generated window
  // is associated with a probability.
  std::vector<std::pair<IsoRect_2, double> >
  random_window(double ratio) const;

  // Find out the index of the reader the landmark is in.
  int
  detected_by(const landmark_t &pos, double radius);

  // Align a landmark, align it to the nearest anchor point.
  int
  align(const landmark_t &p);

  // Get all the anchors with a rect.
  std::vector<int>
  anchors_in_win(const IsoRect_2 &w);

  // Print information about the walking graph.  This is more of a
  // debugging helper.
  void
  print(std::ostream &os) const;

 // protected:
  void
  initialize();

  // Insert anchors into the walking graph.
  void
  insert_anchors(double unit = 20.0);

  // Generate a random window
  IsoRect_2 random_window_aux(double ratio) const;

  // walkinggraph, anchorgraph
  graph_ wg_, ag_;

  // vertex property
  boost::unordered_map<int, VP> vp_;
  // edge property
  boost::unordered_map<int, EP> ep_;
  // anchor poiht property
  boost::unordered_map<int, landmark_t> ap_;

  Point_set_2 anchorset_;

  Point_set_2 readerset_;
  boost::unordered_map<int, landmark_t> readermap_;

  std::vector<IsoRect_2> rooms_;
  std::vector<IsoRect_2> halls_;
  std::vector<int> dirs_;

  double xmax_, ymax_;

  double enter_room_, knock_door_;
};

}  // namespace simulation

#endif  // SRC_WALKINGGRAPH_H_
