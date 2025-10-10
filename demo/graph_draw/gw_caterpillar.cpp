#include<LEDA/graphics/graphwin.h>
#include<LEDA/graph/graph_alg.h>

using namespace leda;


list<node> TestCaterpillar(const graph& G, node_array<int>& leaves,
                                           edge_array<bool>& on_path)
{
  // returns path (backbone) as list of nodes (empty if G is not a caterpillar)
  // computes number of leaves of each node v (leaves[v])
  // marks each edge e as path or non-path edge (on_path[e])

  // G is directed:
  // we assume that each backbone node has exactly one incoming edge
  // (these edges define the backbone path) and all other edges are
  // directed towards a leaf node.
  // A node v is a leaf node exactly if outdeg(v) = 0.

  // the root (start of backbone) is the unique node v with indeg(v) = 0
  // all other nodes have an indegree > 0


  node v;
  forall_nodes(v,G) leaves[v] = 0;

  edge e;
  forall_edges(e,G) on_path[e] = false;


  // find root node v (node with indeg 0 in first component);

  forall_nodes(v,G) {
     if (G.indeg(v) == 0) break;
  }

  assert(v != nil && G.indeg(v) == 0);



  // traverse backbone and append nodes to path list

  list<node> path;

  while (v)
  {
    path.append(v);

    node next_v = nil;

    edge e;
    forall_out_edges(e,v) {

       node w = G.target(e);

       if (G.outdeg(w) == 0) {
         // edge to leaf node w
         leaves[v]++;
       }
       else
       { // backbone edge
         on_path[e] = true;
         next_v = w;
        }
    }

    v = next_v;
  }

  return path;
}



void DrawLeaves(const graph& G, const node_array<node>& sibling,
                node v, double& x, double& y, double d,
                node_array<point>& pos,
                edge_array<bool>& on_path)
{
  edge e;
  forall_out_edges(e,v) {
    if (on_path[e]) continue; // ignore backbone edges
    node w  = G.target(e);
    node ws = sibling[w];
    pos[w]  = point(x,y);
    pos[ws] = point(x+d,y-0.5);
    x += 2*d;
  }
}




bool DrawCaterpillar(GraphWin& gw, const node_array<node>& sibling,
                                   double x, double y, node_array<point>& pos)
{
  graph& G = gw.get_graph();

  // computes positions pos[v] and pos[sibling[v]] for each node v
  // return false if G is not a caterpillar graph

  // (x,y) position of root node (path.front())


  node_array<int> leaves(G);   // leaves[v] = #number of leaves of v
  edge_array<bool> on_path(G); // on_path[e] iff e lies on backbone path

  list<node> path = TestCaterpillar(G,leaves,on_path);

  int k = path.size(); // number of backbone vertices v1 ... vk

  cout << endl;
  cout << "Backbone: ";

  node v;
  forall(v,path) {
     cout << G.index(v) << " ";
     gw.set_color(v,green2);
     gw.set_color(sibling[v],blue2);
  }
  cout << endl;
  cout << endl;
  
  if (k == 0) {
   // Graph is not a caterpillar
   return false;
  }


  // compute a and b

  double h = 0.25;
  double pvr = sqrt((k*k)/2 + h*h);
  double alpha = asin(h/pvr);

  double a = pvr/sin(alpha);
  double b = k;

  node v1 = path.front();
  node u1 = sibling[v1];

  pos[v1] = point(x,y);

  if (leaves[v1] == 0)
    pos[u1] = point(x + (a+b/2), y - (a+0.5));
  else
    pos[u1] = point(x, y - (2*a+0.5));

  // main loop: iterate over all backbone nodes (v1 ... vk)

  list_item it;
  forall_items(it,path)
  { 
    // Let v = path[it] be the current vertex (v_i in Carolina's Code)

    // Invariant: positions of v and all path redecessors of v are defined

    // Let w be the successor of v on the backbone (v_(i+1))
    // and u be the predecessor of v on the backbone (v_(i-1)) 
    // Note: u or w might not exist !

    // i-1    i    (i+1)
    //  u-----v------w

    // we compute the position of w and positions of the leaves of v
    // and of the corresponding sibling nodes in the second component

     node v  = path[it];
     node u = (it == path.first()) ? nil : path[path.pred(it)];
     node w = (it == path.last())  ? nil : path[path.succ(it)];

     node vs = sibling[v];
     node us = u ? sibling[u] : nil;
     node ws = w ? sibling[w] : nil;


     if (w == nil) 
     { // v is last backbone node
       // EMBED LEAVES of v !
       // missing in Carolina's Algorithm ?

       double dist = b/(2*leaves[v] - 1);
       double x = pos[v].xcoord() - b/2;
       double y = pos[v].ycoord() - a;

       DrawLeaves(G,sibling,v,x,y,dist,pos,on_path);

       continue;
     }


     if (leaves[v] == 0)
     { 
       double x1 = pos[v].xcoord();
       double y1 = pos[v].ycoord();

       double x2 = pos[vs].xcoord();
       double y2 = pos[vs].ycoord();

       pos[w] = point(x1 + (a+b), y1);

       if (leaves[v] == 0)
         pos[ws] = point(x2 + (a+b), y2);
       else 
         pos[ws] = point(x2 + b/2, y2 - a);
      
       continue;
     }

     // here we have: leaves[v] > 0
     
     if ((u == nil || leaves[u] > 0) && leaves[w] > 0)
     { 
       double dist = b/(2*leaves[v] - 1);
       double x = pos[v].xcoord() - b/2;
       double y = pos[v].ycoord() - a;

       DrawLeaves(G,sibling,v,x,y,dist,pos,on_path);

       pos[w]  = pos[v].translate(a+b,0);
       pos[ws] = pos[vs].translate(a+b,0);

       continue;
      }


     if ((u == nil || leaves[u] > 0) && leaves[w] == 0)
     { 
       double dist = b/(2*leaves[v]);
       double x = pos[v].xcoord() - b/2;
       double y = pos[v].ycoord() - a;

       DrawLeaves(G,sibling,v,x,y,dist,pos,on_path);

       pos[w]  = point(x,y);
       pos[ws] = pos[vs].translate(a+b,0);

       continue;
      }

     if (u && leaves[u] == 0 && leaves[w] > 0)
     { 
       double dist = b/(2*leaves[v]);
       double x = pos[us].xcoord() + dist;
       double y = pos[us].ycoord() + 0.5;

       DrawLeaves(G,sibling,v,x,y,dist,pos,on_path);

       pos[w]  = pos[v].translate(a+b,0);
       pos[ws] = pos[vs].translate(a+b,0);

       continue;
      }

     if (u && leaves[u] == 0 && leaves[w] == 0)
     { 
       double dist = b/(2*leaves[v] + 1);
       double x = pos[us].xcoord() + dist;
       double y = pos[us].ycoord() + 0.5;

       DrawLeaves(G,sibling,v,x,y,dist,pos,on_path);

       pos[w]  = point(x,y);
       pos[ws] = pos[vs].translate(a+b,0);

       continue;
      }

   }

   return true;
}



void set_layout(GraphWin& gw, node_array<point>& pos)
{
  //gw.set_layout(pos);

  graph& G = gw.get_graph();
  window& W = gw.get_window();

  node_array<double> radius(G);
  edge_array<list<point> > bends(G);
  edge_array<point> anchor(G);

  node v;
  forall_nodes(v,G) {
    //radius[v] = W.pix_to_real(4);
    radius[v] = W.pix_to_real(5);
    gw.set_border_width(v,1);
  }

  edge e;
  forall_edges(e,G) {
    gw.set_width(e,1);
    gw.set_direction(e,undirected_edge);
  }

  gw.set_layout(pos,radius,radius,bends,anchor,anchor);
  gw.center_graph();
}
  



int main()
{
  GraphWin gw("Caterpillar 1");

  //gw.win_init(0,150,0);
  gw.win_init(0,200,0);

  gw.set_node_radius(4);

  gw.set_zoom_objects(false);


  gw.display();

  window& W = gw.get_window();

  double x = W.xmin() + W.pix_to_real(50);
  double y = W.ymax() - W.pix_to_real(100);

  while (gw.edit())
  { graph& G = gw.get_graph();
    int n = G.number_of_nodes();


    graph G1 = G;
    G.join(G1);

    array<node> V(2*n);
    int i = 0;

    node v;
    forall_nodes(v,G) V[i++] = v;

    node_array<node> sibling(G,nil);

    gw.update_graph();

    gw.set_flush(false);

    i = 0;
    forall_nodes(v,G) {
      if (i < n)
        sibling[v] = V[i+n];
      else
      { node u = V[i-n];
        //sibling[v] = u;
        point pos = gw.get_position(u);
        gw.set_position(v,pos.translate(0,-100));
       }
      i++;
    }

    gw.set_flush(true);

    gw.center_graph();

    gw.edit();

    node_array<point> pos(G);

    if (!DrawCaterpillar(gw,sibling,x,y,pos)) {
        cout << "ERROR" << endl;
        continue;
    }

    forall_nodes(v,G) {
      double x = pos[v].xcoord();
      double y = pos[v].ycoord();
      cout << string("%2d:  %6.2f  %6.2f",G.index(v),x,y) << endl;
      node u = sibling[v];
      if (u) pos[u] = pos[u].translate(0,-10);
    }

    set_layout(gw,pos);
  }
  
  return 0;
}
