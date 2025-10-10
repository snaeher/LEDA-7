#include <LEDA/graph/planar_map.h>
#include <LEDA/graph/node_map.h>
#include <LEDA/graph/edge_map.h>

#define EPS ""
#define REV(e) P.reversal(e)
#define SUCC(e) P.face_cycle_succ(e)
#define PRED(e) P.face_cycle_pred(e)
#define IS_CAGE(f)  (P.get_cage(P.first_face_edge(f)))
#define IS_OUTER(f) (!P.get_inner(P.first_face_edge(f)))
#define NEXT(d) ((direction)((d+5)%4))
#define PREV(d) ((direction)((d+3)%4))
#define OPP(d)  ((direction)((d+6)%4))

#define ERR_EMPTY_GRAPH         "ORTHO: input graph is empty"
#define ERR_NOT_CONNECTED       "ORTHO: input graph is not connected"
#define ERR_NO_PLANAR_MAP       "ORTHO: this is no planar map"
#define ERR_INVALID_NETWORK     "ORTHO: invalid network"
#define ERR_NO_FEASIBLE_FLOW    "ORTHO: no feasible flow"
#define ERR_NO_ORTHO_REP        "ORTHO: orthogonal rep. not valid"
#define ERR_BAD_STRING          "ORTHO: trying to set nonempty string"
#define ERR_BAD_ANGLE           "ORTHO: bad angle"
#define ERR_BAD_DIRECTION       "ORTHO: bad direction"


LEDA_BEGIN_NAMESPACE

enum direction {
  north, east, south, west, unexplored
};

enum v_type {
  v_real, v_bend, v_dissection, v_big
};


class ortho_map : public planar_map 
{

  node_map<node> v_in_G;
  node_map<v_type> v_T;
  edge_map<edge> e_in_G;
  edge_map<edge> e_in_P;
  edge_map<int> a;
  edge_map<string> s;
  edge_map<bool> inner;
  edge_map<bool> cage;
  edge_array<direction> dir;
  edge_array<int> length;
  node_array<int> x,y;

public:

  LEDA_MEMORY(ortho_map)

  edge next_level_edge(node, const direction);
  ortho_map(const graph&);
  void print();
  void init_maps(edge, int, const string, bool, bool);

  node      get_orig(node v) const;
  v_type    get_type(node v) const;
  int       get_x(node v) const;
  int       get_y(node v) const;

  edge      get_orig(edge e) const;
  direction get_dir(edge e) const;
  edge      get_copy(edge e) const;
  int       get_a(edge e) const;
  bool      get_inner(edge e) const;
  bool      get_cage(edge e) const;
  string    get_s(edge e) const;
  int       get_length(edge e) const;

  void set_inner(edge e, bool t);
  void set_cage(edge e, bool t);
  void set_type(node v, const v_type t);
  void set_a(edge e, int angle);
  void set_s(edge e, const string s_new);
  void set_s(edge e, int, int, bool);
  void set_rev_s(edge e, int, int, bool);
  void set_length(edge e, int l_new);

  edge split_map_edge(edge);
  edge new_edge(edge, edge);
  edge split_bend_edge(edge);

  void PrintEdge(edge);
  void init_rest();
  void assign_directions(edge e, direction d);
  void determine_position(node, int x, int y, node_array<bool> &);
  void norm_positions();
  edge succ_corner_edge(edge);

  ~ortho_map();

};

LEDA_END_NAMESPACE

