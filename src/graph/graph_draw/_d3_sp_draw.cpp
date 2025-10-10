/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_sp_draw.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/series_parallel.h>


LEDA_BEGIN_NAMESPACE

// 3d sp-embedding

const double pi = LEDA_PI;

class embed_3dresult
{
 public:
  double r, h;
  list<node> touched_nodes;
  list<edge> touched_edges;
  node s, t;
  
  node_array<vector> *coords;
  edge_array<vector> *top_bend;
  edge_array<vector> *bottom_bend;
};


istream& operator>>(istream& in, embed_3dresult&) { return in; }
ostream& operator<<(ostream& out, const embed_3dresult&) { return out; }


static void embed3d_move_y(embed_3dresult& res, double dy)
{
  // nodes
  node v;
  forall(v, res.touched_nodes)
    {
      (*res.coords)[v][1] += dy;
    }

  // edges
  edge e;
  forall(e, res.touched_edges)
    {
      (*res.top_bend)[e][1] += dy;
      (*res.bottom_bend)[e][1] += dy;
    }
}


static void embed3d_scale_y(embed_3dresult& res, double new_h)
{
  // nodes
  node v;
  forall(v, res.touched_nodes)
    {
      (*res.coords)[v][1] *= new_h / res.h;
    }

  // edges
  edge e;
  forall(e, res.touched_edges)
    {
      (*res.top_bend)[e][1] *= new_h / res.h;
      (*res.bottom_bend)[e][1] *= new_h / res.h;
    }
 
  res.h = new_h;
}


static void embed3d_move_xz(embed_3dresult& res, double dx, double dz)
{
  // nodes
  node v;
  forall(v, res.touched_nodes)
    {
      (*res.coords)[v][0] += dx;
      (*res.coords)[v][2] += dz;
    }

  // edges
  edge e;
  forall(e, res.touched_edges)
    {
      (*res.top_bend)[e][0] += dx;
      (*res.top_bend)[e][2] += dz;

      (*res.bottom_bend)[e][0] += dx;
      (*res.bottom_bend)[e][2] += dz;
    }
}


static void embed3d_scale_radius(embed_3dresult& res, double new_rad)
{
  // nodes
  node v;
  forall(v, res.touched_nodes)
    {
      (*res.coords)[v][0] *= new_rad / res.r;
      (*res.coords)[v][2] *= new_rad / res.r;
    }

  // edges
  edge e;
  forall(e, res.touched_edges)
    {
      (*res.top_bend)[e][0] *= new_rad / res.r;
      (*res.top_bend)[e][2] *= new_rad / res.r;

      (*res.bottom_bend)[e][0] *= new_rad / res.r;
      (*res.bottom_bend)[e][2] *= new_rad / res.r;
    }
 
  res.r = new_rad;
}


static double embed3d_move_to_angle(embed_3dresult& res, double rad, 
                                                         double start_angle, 
                                                         double end_angle)
{
  // returns radius of sub-circle

  double dx = rad * cos((start_angle + end_angle) / 2);
  double dz = rad * sin((start_angle + end_angle) / 2);

  double phi = (end_angle - start_angle) / 2;
  if(phi > pi / 2) phi = pi / 2;

  double new_rad = rad * sin(phi);
  
  embed3d_scale_radius(res, new_rad);
  embed3d_move_xz(res, dx, dz);

  return new_rad;
}


void spq_3dembed(spq_node start, embed_3dresult& res, double p)
{
  res.touched_nodes.clear();
  res.touched_edges.clear();

  if(start->type == 'Q')
    {

      res.s = source(start->e);
      res.t = target(start->e);
      res.r = 1;
      res.h = 1;
      (*res.coords)[res.s][0] = 0;
      (*res.coords)[res.s][1] = res.h;
      (*res.coords)[res.s][2] = 0;

      (*res.coords)[res.t][0] = 0;
      (*res.coords)[res.t][1] = 0;
      (*res.coords)[res.t][2] = 0;

      (*res.top_bend)[start->e][0] = 0;
      (*res.top_bend)[start->e][1] = res.h * (1 - p);
      (*res.top_bend)[start->e][2] = 0;

      (*res.bottom_bend)[start->e][0] = 0;
      (*res.bottom_bend)[start->e][1] = res.h * p;
      (*res.bottom_bend)[start->e][2] = 0;

      res.touched_edges.append(start->e);
    }
  else if(start->type == 'S')
    {
      list<embed_3dresult> sub_res;

      // embed children first
      res.r = 0;
      res.h = 0;

      spq_node child;
      forall(child, start->children)
	{
	  embed_3dresult r;
	  r.coords = res.coords;
	  r.top_bend = res.top_bend;
	  r.bottom_bend = res.bottom_bend;

	  spq_3dembed(child, r, p);
	  res.h += r.h;
	  if(res.r < r.r) res.r = r.r;
	  sub_res.append(r);
	}
      
      // scale children to max_rad and put them on top of each other
      double cur_top = res.h;
      bool first = true;
      embed_3dresult r;
      forall(r, sub_res)
	{
	  embed3d_scale_radius(r, res.r);
	  cur_top -= r.h;
	  embed3d_move_y(r, cur_top);
	  
	  (*res.coords)[r.s][0] = 0;
	  (*res.coords)[r.s][1] = cur_top + r.h;
	  (*res.coords)[r.s][2] = 0;

	  (*res.coords)[r.t][0] = 0;
	  (*res.coords)[r.t][1] = cur_top;
	  (*res.coords)[r.t][2] = 0;
	  
	  if(first)
	    {
	      first = false;
	      res.s = r.s;
	    }
	  else res.touched_nodes.append(r.s);
	  
	  res.touched_nodes.conc(r.touched_nodes);
	  res.touched_edges.conc(r.touched_edges);
	  res.t = r.t;
	}
    }
  else if(start->type == 'P')
    {
      list<embed_3dresult> sub_res;

      // embed children first
      res.r = 0;
      res.h = 0;
      spq_node child;
      forall(child, start->children)
	{
	  embed_3dresult r;
	  r.coords = res.coords;
	  r.top_bend = res.top_bend;
	  r.bottom_bend = res.bottom_bend;

	  spq_3dembed(child, r, p);
	  res.r += r.r;
	  if(res.h < r.h) res.h = r.h;
	  sub_res.append(r);
	}
      
      // scale children to max_h and put them next to each other
      embed_3dresult r;
      double cur_angle = 0;
      double add_rad = 0;
      forall(r, sub_res)
	{
	  embed3d_scale_y(r, res.h);

	  double delta_angle = r.r * 2 * pi / res.r;
	  double new_add_rad = embed3d_move_to_angle(r, 1, cur_angle, cur_angle + delta_angle);
	  if(new_add_rad > add_rad) add_rad = new_add_rad;

	  cur_angle += delta_angle;
	  	 
	  res.touched_nodes.conc(r.touched_nodes);
	  res.touched_edges.conc(r.touched_edges);
	  res.s = r.s;
	  res.t = r.t;
	}

      res.r = 1 + add_rad;

      // center source & target
      (*res.coords)[res.s][0] = 0;
      (*res.coords)[res.s][1] = res.h;
      (*res.coords)[res.s][2] = 0;

      (*res.coords)[res.t][0] = 0;
      (*res.coords)[res.t][1] = 0;
      (*res.coords)[res.t][2] = 0;
    }
  else
    {
      cerr << "ERROR: invalid spq_node while 3d-embedding spq_node\n";
    }
}




bool D3_SP_EMBEDDING(const graph& G, double bend_dist,
		     node_array<vector>& node_coords,
		     edge_array<list<vector> >& bends,
		     double width, double height,
		     double x0, double y0)
{ 
  spq_tree T(G);

  edge_array<vector> top_bends(G, vector(3)), bottom_bends(G, vector(3));

  if(!T.is_series_parallel()) return false;

  embed_3dresult res;

  res.coords = &node_coords;
  res.top_bend = &top_bends;
  res.bottom_bend = &bottom_bends;

  spq_3dembed(T.get_root_node(), res, bend_dist);

  res.touched_nodes.append(res.s);
  res.touched_nodes.append(res.t);

  embed3d_scale_radius(res, width);  
  embed3d_scale_y(res, height);
  embed3d_move_xz(res, res.r + x0, res.r + y0);

  edge e;
  forall_edges(e, G)
    {
      bends[e].append(top_bends[e]);
      bends[e].append(bottom_bends[e]);
    }
  
  return true;
}

LEDA_END_NAMESPACE

