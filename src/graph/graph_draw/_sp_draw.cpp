/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _sp_draw.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
//
// sp_draw.c
//
// Jens Loewe, 1998/99
//------------------------------------------------------------------------------


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/series_parallel.h>


LEDA_BEGIN_NAMESPACE

static void get_in_lmr(node n, node_array<vector>& coords, 
                               edge_array<vector>& bends, 
                               int& l, int& m, int& r)
{
  // counts edges entering n left, middle and right of node center
  l = m = r = 0;

  edge e;
  forall_in_edges(e, n)
    {
      if((float)bends[e][0] < (float)coords[n][0]) l++;
      else if((float)bends[e][0] == (float)coords[n][0]) m = 1;
      else  r++;
    }
}


static void get_out_lmr(node n, node_array<vector>& coords, 
                                edge_array<vector>& bends, 
                                int& l, int& m, int& r)
{
  // counts edges leaving n left, middle and right of node center

  l = m = r = 0;

  edge e;
  forall_out_edges(e, n)
    {
      if((float)bends[e][0] < (float)coords[n][0]) l++;
      else if((float)bends[e][0] == (float)coords[n][0]) m = 1;
      else r++;
    }
}





class br_node
{
 public:
  char type;

  node s, t;
  edge e;

  double x0, y0, x1, y1;

  list<br_node *> children;

  void destroy();
};


void br_node::destroy()
{
  br_node *b;
  forall(b, children) b->destroy();
  delete this;
}


// returns maximum of the number of children that siblings of start have
int get_max_childnum(spq_node start, spq_node parent)
{
  if(parent == NULL) return start->children.size();
  
  int num = 0;
  spq_node n;
  forall(n, parent->children)
    if(n->children.size() > num) num = n->children.size();

  return num;
}



static br_node* make_brtree(spq_node start, double x0, double y0, 
                                            double x1, double y1,
		                            node_array<list<edge> >& in_edges, 
                                            node_array<list<edge> >& out_edges,
                       		            node_array<double>& box_width, 
                                            spq_node start_parent = NULL)
{
  // constucts BR-tree from SPQ-tree 
  // (computes bounding rectangles for each component)
  // computes maximum possible node with (returned in box_width)

  br_node *result = new br_node;
  double dx, dy, d0;
  spq_node s;

  result->x0 = x0;
  result->y0 = y0;
  result->x1 = x1;
  result->y1 = y1;
  result->type = start->type;

  switch(start->type) {
    case 'Q':
    {
      result->e = start->e;
      result->s = source(start->e);
      result->t = target(start->e);
      
      out_edges[result->s].append(result->e);
      in_edges[result->t].append(result->e);
      
      break;
    }
    case 'S':
    { dy = (y1 - y0) / start->children.size();
      d0 = y1;
      forall(s, start->children)
	{
	  result->children.append(make_brtree(s, x0, d0 - dy, x1, d0, in_edges, out_edges, box_width, start));
	  d0 -= dy;
	}
      result->s = result->children.front()->s;
      result->t = result->children.back()->t;
      break;
     }

    case 'P':
      { int num_cells = get_max_childnum(start, start_parent);
        if((num_cells & 1) == 0) num_cells++;
        int even = !(start->children.size() & 1);
        dx = (x1 - x0) / num_cells; //(start->children.size() + even);
        d0 = x0 + dx * (num_cells - start->children.size() - even) / 2;

        int count = start->children.size() / 2;
        forall(s, start->children)
	{ count--;
	  result->children.append(make_brtree(s, d0, y0, d0 + dx, y1, in_edges, out_edges, box_width, start));
	  d0 += dx;
	  if(even && count == 0) d0 += dx;
	}
        result->s = result->children.front()->s;
        result->t = result->children.back()->t;
      break;
     }
    }

  box_width[result->s] = result->x1 - result->x0;
  box_width[result->t] = result->x1 - result->x0;

  return result;
}


static void embed_brnode(br_node *start, node_array<vector>& coords, 
                                         edge_array<vector>& top_bends, 
                                         edge_array<vector>& bottom_bends,
                                         double p)
{
  // compute bend position for all edges from BR-tree

  br_node *b;
  double d;

  switch(start->type) {
    case 'Q':
    { coords[start->s][0] = (start->x0 + start->x1) / 2;
      coords[start->s][1] = start->y1;
      coords[start->t][0] = (start->x0 + start->x1) / 2;
      coords[start->t][1] = start->y0;
      d = (start->y1 - start->y0) * p;
      top_bends[start->e] = vector((start->x0 + start->x1) / 2, start->y1 - d);
      bottom_bends[start->e] = vector((start->x0 + start->x1) / 2, start->y0 + d);

      break;
     }
  
    case 'S':
    { // embed all children
      forall(b, start->children)
	embed_brnode(b, coords, top_bends, bottom_bends, p);
      break;
     }


    case 'P':
    { // embed all children
      forall(b, start->children)
	embed_brnode(b, coords, top_bends, bottom_bends, p);
      
      // center source & target
      coords[start->s][0] = (start->x0 + start->x1) / 2;
      coords[start->s][1] = start->y1;
      coords[start->t][0] = (start->x0 + start->x1) / 2;
      coords[start->t][1] = start->y0;
      break;
     }
    }
}


class LMR {

// node information: how many nodes are entering/leaving node left,
// middle and right of node center
 
 public:
  int il, im, ir; // entering edges
  int ol, om, oor; // leaving edges

  int max_l() { return il < ol ? ol : il; }
  int max_m() { return im < om ? om : im; }
  int max_r() { return ir < oor ? oor : ir; }

  int max_o() { return ol > oor ? ol : oor; }
  int max_i() { return il > ir ? il : ir; }

};


static void compute_lmr(const graph& G, node_array<vector>& coords, 
                                        node_array<double>& node_width, 
                                        edge_array<vector>& top_bends,
                                        edge_array<vector>& bottom_bends,
                                        node_array<LMR>& lmr, 
                                        double& d_max, double rel_node_height)
{
 // computes LMR for all nodes
 // computes maximum possible distance between edges (d_max)

  node n;
  edge e;
  double d;

  forall_nodes(n, G)
    {
      get_in_lmr(n, coords, bottom_bends, lmr[n].il, lmr[n].im, lmr[n].ir);
      get_out_lmr(n, coords, top_bends, lmr[n].ol, lmr[n].om, lmr[n].oor);
    }


  forall_nodes(n, G)
    {
      forall_in_edges(e, n) 
	{
	  d = (coords[source(e)][1] - coords[n][1]) / (rel_node_height + lmr[n].max_i() + lmr[source(e)].max_o() + 1);
	  if(d < d_max) d_max = d;
	}

      d = node_width[n] / (2 * lmr[n].max_i() + 1 + lmr[n].im);
      if(d < d_max) d_max = d;
      
 
      forall_out_edges(e, n) 
	{
	  d = (coords[n][1] - coords[target(e)][1]) / (rel_node_height + lmr[n].max_o() + lmr[target(e)].max_i() + 1);
	  if(d < d_max) d_max = d;
	}

      d = node_width[n] / (2 * lmr[n].max_o() + 1 + lmr[n].om);
      if(d < d_max) d_max = d;
    }
}


bool SP_ORTHO_EMBEDDING(const graph& G, double dist, 
                        double rel_node_width, double rel_node_height,
			node_array<vector>& coords,
			node_array<double>& node_width,
			node_array<double>& node_height,
			edge_array<list<vector> >& bends,
			edge_array<vector>& source_anchor,
			edge_array<vector>& target_anchor,
			double dx, double dy, double x0, double y0)
{
  // computes orthogonal embedding
  // dist:   edge distance
  // rel_node_width/height: relative width/height of nodes (unit: edge distance)
  // dx, dy: width and height of embedding rectangle
  // x0, y0: coordinate offset

  spq_tree T(G);
  edge_array<vector> top_bends(G, vector(2));
  edge_array<vector> bottom_bends(G, vector(2));
  edge_array<vector> first_bends(G, vector(2));
  edge_array<vector> last_bends(G, vector(2));
  node_array<list<edge> > in_edges(G), out_edges(G);
  node_array<LMR> lmr(G);

  if (!T.is_series_parallel()) return false;

  // construct BR-tree
  br_node *b = make_brtree(T.get_root_node(), x0, y0, x0 + dx, y0 + dy, in_edges, out_edges, node_width);

  // embed BR-tree
  embed_brnode(b, coords, top_bends, bottom_bends, 0.45);

  // compute LMR and maximum possible distance
  compute_lmr(G, coords, node_width, top_bends, bottom_bends, lmr, dist, rel_node_height);

  node n;
  edge e;

  forall_nodes(n, G)
    {
      int lr_max = lmr[n].max_l() > lmr[n].max_r() ? lmr[n].max_l() : lmr[n].max_r();

      // compute node width
      node_width[n] -= dist;
      double width = rel_node_width > 2 * lr_max + lmr[n].max_m() ? rel_node_width :  2 * lr_max + lmr[n].max_m();
      double nwidth = width * dist;
      if(node_width[n] > nwidth) node_width[n] = nwidth;

      int count = 0;

      // set target anchor and last bend position for all edges entering this node
      forall(e, in_edges[n])
	{
	  count++;
	  if(count <= lmr[n].il)
	    {
	      bottom_bends[e][1] = coords[n][1] + dist * (rel_node_height / 2) + count * dist;
	      last_bends[e][0] = coords[n][0] - (lmr[n].il - count + 1) * dist + (lmr[n].im > 0 ? 0 : dist / 2);
	      last_bends[e][1] = bottom_bends[e][1];
	    }
	  else if(count == lmr[n].il + 1 && lmr[n].im > 0)
	    {
	      bottom_bends[e][1] = coords[n][1] + dist * (rel_node_height / 2);
	      last_bends[e][0] = coords[n][0];
	      last_bends[e][1] = bottom_bends[e][1];
	    }
	  else
	    {
	      int cur_pos = count - lmr[n].il - lmr[n].im - 1; // 0...lmr[n].il - 1
	      bottom_bends[e][1] = coords[n][1] + ((rel_node_height / 2) + lmr[n].ir - cur_pos) * dist;
	      last_bends[e][0] = coords[n][0] + (lmr[n].im > 0 ? dist : dist / 2) + cur_pos * dist;
	      last_bends[e][1] = bottom_bends[e][1];
	    }

	  target_anchor[e][0] = (last_bends[e][0] - coords[n][0]) / (node_width[n] / 2);
	  target_anchor[e][1] = 1;

	}
      

      count = 0;

      // set source anchor and first bend position for all edges leaving this node
      forall(e, out_edges[n])
	{
	  count++;
	  if(count <= lmr[n].ol)
	    {
	      top_bends[e][1] = coords[n][1] - dist * (rel_node_height) / 2 - count * dist;
	      first_bends[e][0] = coords[n][0] - 
		(lmr[n].ol - count + 1) * dist + (lmr[n].om > 0 ? 0 : dist / 2);
	      first_bends[e][1] = top_bends[e][1];
	    }
	  else if(count == lmr[n].ol + 1 && lmr[n].om > 0)
	    {
	      top_bends[e][1] = coords[n][1] - dist * (rel_node_height / 2);
	      first_bends[e][0] = coords[n][0];
	      first_bends[e][1] = top_bends[e][1];
	    }
	  else
	    {
	      int cur_pos = count - lmr[n].ol - lmr[n].om - 1; // 0...lmr[n].ol - 1
	      
	      top_bends[e][1] = coords[n][1] - ((rel_node_height / 2) + lmr[n].oor - cur_pos) * dist;
	      first_bends[e][0] = coords[n][0] + (lmr[n].om > 0 ? dist : dist / 2) + cur_pos * dist;
	      first_bends[e][1] = top_bends[e][1];
	    }

	  source_anchor[e][0] = (first_bends[e][0] - coords[n][0]) / (node_width[n] / 2);
	  source_anchor[e][1] = -1;	  
	}
    }

 
  // remove unnecessary bends in parallel edges
  forall_nodes(n, G)
    {
      edge prev_edge = NULL;
      forall(e, in_edges[n])
	{
	  if(prev_edge != NULL && source(prev_edge) == source(e) &&
	     bottom_bends[prev_edge][0] < last_bends[prev_edge][0] &&
	     last_bends[prev_edge][0] < coords[n][0] &&
	     coords[n][0] < last_bends[e][0] &&
	     last_bends[e][0] < bottom_bends[e][0])
	    {
	      top_bends[prev_edge][0] = bottom_bends[prev_edge][0] = last_bends[prev_edge][0];
	      top_bends[e][0] = bottom_bends[e][0] = last_bends[e][0];
	      prev_edge = NULL;
	    }
	  else prev_edge = e;
	}

      prev_edge = NULL;
      forall(e, out_edges[n])
	{
	  if(prev_edge != NULL && target(prev_edge) == target(e) &&
	     top_bends[prev_edge][0] < first_bends[prev_edge][0] &&
	     first_bends[prev_edge][0] < coords[n][0] &&
	     coords[n][0] < first_bends[e][0] &&
	     first_bends[e][0] < top_bends[e][0])
	    {
	      top_bends[prev_edge][0] = bottom_bends[prev_edge][0] = first_bends[prev_edge][0];
	      top_bends[e][0] = bottom_bends[e][0] = first_bends[e][0];
	      prev_edge = NULL;
	    }
	  else prev_edge = e;
	}
    }


  forall_edges(e, G)
    {
      bends[e].append(first_bends[e]);
      if(top_bends[e] != first_bends[e]) bends[e].append(top_bends[e]);
      if(bottom_bends[e] != last_bends[e]) bends[e].append(bottom_bends[e]);
      bends[e].append(last_bends[e]);
    }

  b->destroy();

  forall_nodes(n, G) node_height[n] = dist * rel_node_height;

  return true;
}



bool SP_EMBEDDING(const graph& G, double dist,
		  node_array<vector>& coords,
		  edge_array<list<vector> >& bends,
		  double dx, double dy, double x0, double y0)
{
  // computes simple embedding
  // dist:   edge distance
  // dx, dy: width and height of embedding rectangle
  // x0, y0: coordinate offset

  spq_tree T(G);

  node_array<list<edge> > in_edges(G), out_edges(G);
  node_array<double> node_width(G);
  edge_array<vector> top_bends(G, vector(2)), bottom_bends(G, vector(2));

  if(!T.is_series_parallel()) return false;

  br_node *b = make_brtree(T.get_root_node(), x0, y0, x0 + dx, y0 + dy, in_edges, out_edges, node_width);
  embed_brnode(b, coords, top_bends, bottom_bends, 0.45);

  node n;
  edge e;

  forall_nodes(n, G)
    {
      forall(e, in_edges[n])
	if((coords[source(e)][1] - coords[n][1]) / 2 < dist) dist = (coords[source(e)][1] - coords[n][1]) / 2;

      forall(e, out_edges[n])
	if((coords[n][1] - coords[target(e)][1]) / 2 < dist) dist = (coords[n][1] - coords[target(e)][1]) / 2;
    }

  forall_nodes(n, G)
    {
      forall(e, in_edges[n]) bottom_bends[e][1] = coords[n][1] + dist;
      forall(e, out_edges[n]) top_bends[e][1] = coords[n][1] - dist;
    }

  forall_edges(e, G)
    {
      bends[e].append(top_bends[e]);
      bends[e].append(bottom_bends[e]);
    }

  b->destroy();

  return true;
}


bool SP_EMBEDDING(graph& G, node_array<double>& x_coord,
                            node_array<double>& y_coord,
                            node_array<double>& x_radius,
                            node_array<double>& y_radius,
                            edge_array<list<double> >& x_bends,
                            edge_array<list<double> >& y_bends,
                            edge_array<double>& x_sanch,
                            edge_array<double>& y_sanch,
                            edge_array<double>& x_tanch,
                            edge_array<double>& y_tanch)
{
  double bend_dist = 10;
  double x0 = 0;
  double y0 = 0;
  double dx = 500;
  double dy = 500; 
  double rel_node_width = 10;
  double rel_node_height = 2;

  node_array<vector> coords(G,vector(2));
  node_array<double> node_width(G);
  node_array<double> node_height(G);
  edge_array<list<vector> > bends(G);
  edge_array<vector> source_anchor(G,vector(2));
  edge_array<vector> target_anchor(G,vector(2));


  bool b =  SP_ORTHO_EMBEDDING(G, bend_dist, rel_node_width, rel_node_height,
			coords, node_width, node_height, bends,
			source_anchor, target_anchor,
			dx, dy, x0, y0);

  node v;
  forall_nodes(v,G)
  { x_coord[v] = coords[v][0];
    y_coord[v] = coords[v][1];
    x_radius[v] = node_width[v]/2;
    y_radius[v] = node_height[v]/2;
   }
   
  edge e;
  forall_edges(e,G)
  { vector v;
    forall(v,bends[e])
    { x_bends[e].append(v[0]);
      y_bends[e].append(v[1]);
     }
    x_sanch[e] = source_anchor[e][0];
    y_sanch[e] = source_anchor[e][1];
    x_tanch[e] = target_anchor[e][0];
    y_tanch[e] = target_anchor[e][1];
   }

  return b;
}

LEDA_END_NAMESPACE

