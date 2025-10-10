/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _ortho_map.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include "ortho_map.h"

LEDA_BEGIN_NAMESPACE

static int angle(char c)
{
  int result = 0;
  switch (c) {
    case '0':
      result = 90;
      break;
    case '1':
      result = 270;
      break;
    default:
      LEDA_EXCEPTION(1, ERR_BAD_ANGLE);
  }
  return result;
}


static bool Euler(const graph& G)
{ int n = 0;
  node v;
  forall_nodes(v, G) 
    if (outdeg(v) != 0) n++;
  int m = G.number_of_edges()/2;
  int f = G.number_of_faces();
  return (m == n + f - 2);
}



node ortho_map::get_orig(node v) const {
  return v_in_G[v];
}

edge ortho_map::get_orig(edge e) const {
  return e_in_G[e];
}

direction ortho_map::get_dir(edge e) const {
  return dir[e];
}

edge ortho_map::get_copy(edge e) const {
  return e_in_P[e];
}

v_type ortho_map::get_type(node v) const {
  return v_T[v];
}

void ortho_map::set_inner(edge e, bool t) {
  inner[e] = t;
}

void ortho_map::set_cage(edge e, bool t) {
  cage[e] = t;
}

bool ortho_map::get_inner(edge e) const {
  return inner[e];
}

bool ortho_map::get_cage(edge e) const {
  return cage[e];
}

void ortho_map::set_type(node v, const v_type t) {
  v_T[v] = t;
}

int ortho_map::get_a(edge e) const {
  return a[e];
}

void ortho_map::set_a(edge e, int angle) {
  a[e] = angle;
}

string ortho_map::get_s(edge e) const {
  return s[e];
}

void ortho_map::set_s(edge e, const string s_new) {
  s[e] = s_new;
}


int ortho_map::get_x(node v) const {
  return x[v];
}

int ortho_map::get_y(node v) const {
  return y[v];
}

void ortho_map::set_length(edge e, int l_new) {
  length[e] = l_new;
}

int ortho_map::get_length(edge e) const {
  return length[e];
}

void ortho_map::init_rest() {
  dir.init(*this, unexplored);
  length.init(*this, 0);
  x.init(*this, -1);
  y.init(*this, -1);
}


ortho_map::~ortho_map() { clear(); }


void ortho_map::assign_directions(edge e, direction d) 
{
  while (dir[e] == unexplored) 
  { dir[e] = d;
    edge r = reversal(e);

    if (dir[r] == unexplored) {
      if (inner[r])
         assign_directions(r, OPP(d));
      else
         dir[r] = OPP(d);
    } 

    switch (a[e]) {
     case 90: d = NEXT(d);
              break;
     case 270:d = PREV(d);
              break;
     case 360:d = OPP(d);
              break;
    } 

    e = face_cycle_succ(e);
  }
}



edge ortho_map::next_level_edge(node v, const direction d)
{
  edge e;
  switch (d) {
    case east: {
        forall_out_edges(e,v) 
          if (dir[e] == north) return e;
        break;
      }
    case west: {
        forall_in_edges(e,v) 
          if (dir[e] == north) return e;
        break;
      }
    case north: {
        forall_in_edges(e,v) 
          if (dir[e] == east) return e;
        break;
      }
    case south: {
        forall_out_edges(e,v) 
          if (dir[e] == east) return e;
        break;
      }
    default:
      break;
  }
  return NULL;
}

ortho_map::ortho_map(const graph& G):planar_map(G)
{
  if (G.number_of_nodes() == 0)
    LEDA_EXCEPTION(1, ERR_EMPTY_GRAPH);

  if (!Is_Connected(G))
    LEDA_EXCEPTION(1, ERR_NOT_CONNECTED);

  if (!Euler(G))
    LEDA_EXCEPTION(1, ERR_NO_PLANAR_MAP);

  v_in_G.init(*this);
  e_in_G.init(*this);
  e_in_P.init(G);

  node v_P = first_node(), v_G = G.first_node();
  while (v_P) {
    v_in_G[v_P] = v_G;
    v_P = succ_node(v_P);
    v_G = G.succ_node(v_G);
  }
  forall_nodes(v_P, *this) {
    v_G = v_in_G[v_P];
    edge e_P = first_adj_edge(v_P), e_G = G.first_adj_edge(v_G);
    while (e_P) {
      e_in_G[e_P] = e_G;
      e_in_P[e_G] = e_P;
      e_G = adj_succ(e_G);
      e_P = adj_succ(e_P);
    }
  }

  a.init(*this, 90);
  s.init(*this, EPS);
  v_T.init(*this, v_real);
  inner.init(*this, true);
  cage.init(*this, false);
}


edge ortho_map::split_map_edge(edge e)
{
  edge n = planar_map::split_edge(e);
  edge er = reversal(e), nr = reversal(n);
  a[er] = a[nr];
  a[n] = a[e];
  s[er] = EPS;
  s[n] = EPS;
  inner[er] = inner[nr];
  cage[er] = cage[nr];
  inner[n] = inner[e];
  cage[n] = cage[e];
  e_in_G[n] = e_in_G[e];
  e_in_G[er] = e_in_G[nr];
  v_in_G[source(n)] = NULL;

  return n;
}

edge ortho_map::split_bend_edge(edge e)
{
  string s_e = s[e];
  int a_e = a[e];
  bool inner_e = inner[e];
  bool cage_e = cage[e];
  edge er = reversal(e);
  string s_er = s[er];
  int a_er = a[er];
  bool inner_er = inner[er];
  bool cage_er = cage[er];

  edge n = split_map_edge(e);
  er = reversal(e);
  edge nr = reversal(n);

  a[e] = angle(s_e[s_e.length() - 1]);
  a[er] = a_er;
  a[n] = a_e;
  a[nr] = angle(s_er[0]);
  s[e] = s_e.head(s_e.length() - 1);
  s[er] = s_er.tail(s_er.length() - 1);
  s[n] = EPS;
  s[nr] = EPS;
  inner[e] = inner[n] = inner_e;
  cage[e] = cage[n] = cage_e;
  inner[er] = inner[nr] = inner_er;
  cage[er] = cage[nr] = cage_er;
  v_T[source(n)] = v_bend;

  return n;
}

edge ortho_map::new_edge(edge e1, edge e2)
{
  edge n = planar_map::new_edge(e1, e2);
  e_in_G[n] = NULL;
  e_in_G[reversal(n)] = NULL;
  return n;
}

void ortho_map::print()
{
  face f;
  edge e;
  forall_faces(f, (*this)) {
    if (!inner[first_face_edge(f)])
      cout << "outer ";
    if (cage[first_face_edge(f)])
      cout << "cage ";
    cout << "face: " << endl;
    forall_face_edges(e, f) {
      PrintEdge(e);
      cout << endl;
    }
  }
}

void ortho_map::set_s(edge e, int flow, int flow_rev, bool bridge)
{
  if (s[e] != EPS)
    LEDA_EXCEPTION(1, ERR_BAD_STRING);
  for (int i = 0; i<flow; i++)
    s[e] += "0";
  if (!bridge) {
    for (int i = 0; i<flow_rev; i++)
      s[e] += "1";
  }
}

void ortho_map::set_rev_s(edge e, int flow, int flow_rev,
  bool bridge)
{
  if (s[e] != EPS)
    LEDA_EXCEPTION(1, ERR_BAD_STRING);
  for (int i = 0; i<flow; i++)
    s[e] += "1";
  if (!bridge) {
    for (int i = 0; i<flow_rev; i++)
      s[e] = string('0') + s[e];
  }
}

void ortho_map::PrintEdge(edge e)
{
planar_map::print_edge(e);
  cout << "\t(" << s[e] << " " << a[e] << " [" << inner[e] << cage[e] << "])";
}

void ortho_map::init_maps(edge e, int a_new, const string s_new,
  bool i_new, bool c_new)
{
  a[e] = a_new;
  s[e] = s_new;
  inner[e] = i_new;
  cage[e] = c_new;
}

void ortho_map::determine_position(node v, int x_new,
  int y_new, node_array<bool> &seen)
{
  if (seen[v])
    return;
  edge e;
  x[v] = x_new;
  y[v] = y_new;
  seen[v] = true;
  forall_out_edges(e, v) {
    if (dir[e] == north)
      determine_position(target(e), x_new + length[e], y_new, seen);
    if (dir[e] == west)
      determine_position(target(e), x_new, y_new + length[e], seen);
  }
  forall_in_edges(e, v) {
    if (dir[e] == north)
      determine_position(source(e), x_new - length[e], y_new, seen);
    if (dir[e] == west)
      determine_position(source(e), x_new, y_new - length[e], seen);
  }
}

void ortho_map::norm_positions()
{
  int xmin = 0, ymin = 0;
  node v;
  forall_nodes(v, (*this)) if (v_T[v] != v_big) {
    xmin = leda_min(xmin, x[v]);
    ymin = leda_min(ymin, y[v]);
  }
  forall_nodes(v, (*this)) {
    x[v] -= xmin;
    y[v] -= ymin;
  }
}

edge ortho_map::succ_corner_edge(edge e)
{
  edge e_c;
  for (e_c = face_cycle_succ(e); a[e_c] == 180; e_c = face_cycle_succ(e_c));
  return e_c;
}


LEDA_END_NAMESPACE

