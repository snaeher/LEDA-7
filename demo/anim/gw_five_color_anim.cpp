/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_five_color_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/stack.h>
#include <LEDA/core/array.h>
#include <LEDA/system/event.h>

using namespace leda;

class alg_five_color {

  node_array<list<node> >* _merged_nodes;
  node_array<node>*        _node_copy;

  public :

  EVENT1<alg_five_color&>           start_event;
  EVENT1<alg_five_color&>           finish_event;
  EVENT2<alg_five_color&,node>      touch_node_event;
  EVENT3<alg_five_color&,node,node> merge_nodes_event;
  EVENT2<alg_five_color&,node>      remove_node_event;
  EVENT3<alg_five_color&,node,int>  color_node_event;

  const list<node>& get_merged_nodes(node v) { 
    return (*_merged_nodes)[(*_node_copy)[v]]; 
  }

  void operator () (const graph& G, node_array<int>& C);
};



static void  find_independent_neighbors(const graph& G, 
                                        const node_array<int>& col,
                                        node v, node& u, node& w)
{ 
  array<node> A(G.degree(v));

  int d = 0;
  node x;
  forall_adj_nodes(x,v)
      if (col[x] != -1) A[d++] = x;

  bool indep = false;

  for(int i=0; i<d; i++)
  { u = A[i];
    for(int j=i+1; j<d; j++)
    { w = A[j];
      indep = true;
      forall_adj_nodes(x,w) 
      { if (x == u) 
        { indep = false;
          break;
         }
       }
      if (indep) i = j = d;
     }
   }

  if (!indep)
    LEDA_EXCEPTION(1,"FIVE_COLOR: internal error (no independent neighbors).");
}


static int unused_adj_colors(node v, const node_array<int>& col) {
  int used[6];
  int c;
  for(c = 0; c < 6; c++) used[c] = 0;
  node x;
  forall_adj_nodes(x,v) {
    c = col[x];
    if (c != -1) used[c] = 1;
  }
  c = 0; 
  while(used[c]) c++;
  return c;
}



void alg_five_color::operator()(const graph& G, node_array<int>& C) 
{
  start_event(*this);

  /*
    if (!PLANAR(G))
    LEDA_EXCEPTION(1,"FIVE_COLOR: graph must be planar.");
    */
  
  // we work on a copy G1 of G

  GRAPH<node,edge> G1;

  CopyGraph(G1,G);

  //  graph G1 = G;

  node v;
  node_array<node> v_in_G1(G);
  forall_nodes(v,G1) v_in_G1[G1[v]] = v;

  _node_copy = &v_in_G1;

  // remove loops and parallel edges
  Delete_Loops(G1);
  Make_Simple(G1);

  // eliminated reversals
  edge_array<edge> rev(G1);
  Is_Bidirected(G1,rev);
  edge e;
  forall_edges(e,G1) {
    if (rev[e] == nil) continue;
    edge r = rev[e];
    G1.del_edge(e);
    rev[r] = nil;
  }
   

  G1.make_undirected();


  node_array<int>  C1(G1,0);       // C1[v] = color of node v in G1
  node_array<bool> mark(G1,false);
  node_array<int>  deg(G1);        // deg[v] = current degree of v

  list<node> small_deg;            // list of nodes with deg[v] <= 5
  node_array<list_item> I(G1,nil); // I[v] = location of v in small_deg

  node_array<list<node> > L(G1);   // L[v] = list of nodes of G represented by v
  _merged_nodes = &L;

  stack<node>  removed;            // stack of (conceptually) removed nodes

  int N;                           // current number of valid nodes of G1


  // Initialization

  N = G1.number_of_nodes();

  node u=0,w=0,x=0;

  forall_nodes(v,G1) {
    deg[v] = G1.degree(v);
    if(deg[v] <= 5) I[v] = small_deg.append(v);
    L[v].append(G1[v]);
  }

  // shrinking G1

  while (N > 0) {

//
//  forall_nodes(v,G1)
//  { int d = 0;
//    forall_adj_nodes(x,v) if (C1[x] != -1) d++;
//    assert(C1[v] == -1 || deg[v] == d); 
//   }
//


    if (small_deg.empty())
     LEDA_EXCEPTION(1,"FIVE_COLOR: no node with degree < 5 (graph not planar).");

    v = small_deg.pop();

    touch_node_event(*this,G1[v]);

    I[v] = nil;

    if (deg[v] == 5) {
      find_independent_neighbors(G1,C1,v,u,w);

      if (w == u) { 
	// parallel edges ?
        LEDA_EXCEPTION(1,"FIVE_COLOR: internal error (merging identical nodes)");
      }

      forall_adj_nodes(x,u) mark[x] = true;

      forall_adj_nodes(x,w) {
	if (x == u) 
         LEDA_EXCEPTION(1,string("FIVE_COLOR: internal error (merging adjacent nodes %d and %d  v = %d",G.index(u), G.index(w),G.index(v)));
        if (mark[x]) { 
	  deg[x]--;
	  if (deg[x] == 5) I[x] = small_deg.append(x);
	}
        else {
	  G1.new_edge(u,x);
	  if (C1[x] != -1) deg[u]++;  
	}
      }

      merge_nodes_event(*this,G1[u],G1[w]);

      forall_adj_nodes(x,u) mark[x] = false;

      deg[v]--;


      if (deg[u] > 5 && I[u] != nil) {
	small_deg.del(I[u]);
        I[u] = nil;
      }

      L[u].conc(L[w]);
      
      if (I[w] != nil) small_deg.del(I[w]);
      
      G1.del_node(w);
      
      N--;
      
    }

    remove_node_event(*this,G1[v]);
    
    //  now deg[v] <= 4

    C1[v] = -1;
    removed.push(v);

    forall_adj_nodes(x,v) if ( --deg[x] == 5) I[x] = small_deg.append(x);

    N--;
  }

   // now color the nodes in "removed" from top to bottom

   while ( ! removed.empty() ) {
     v = removed.pop();

     int c = unused_adj_colors(v,C1);

     if (c == 5) 
       LEDA_EXCEPTION(1,"FIVE_COLOR: internal error (more than 5 colors).");

     C1[v] = c;
     color_node_event(*this,G1[v],c);
     forall(x,L[v]) { C[x] = c; }
   }

   finish_event(*this);

}



static alg_five_color FIVE_COLOR;




class gw_fc_observer {

  GraphWin& gw;
  node_array<point> pos0; 
  event_item it[6];
  bool step;
  bool coloring;


  void hide_node(node v, bool hide_edges = true);

  void wait(string s) 
  { string msg = "\\bf\\blue " + s;
    if (step)
      step = gw.wait(msg);
    else
     { gw.message(msg);
       leda_wait(0.5);
      }
   }


public :

  void on_start(alg_five_color&);
  void on_finish(alg_five_color&);
  void on_touch_node(alg_five_color&,node);
  void on_merge_nodes(alg_five_color&,node,node);
  void on_remove_node(alg_five_color&,node);
  void on_color_node(alg_five_color&,node,int);

  gw_fc_observer(alg_five_color& p, GraphWin& _gw) : gw(_gw)
  { step = true;
    coloring = false;
    it[0] = attach(p.start_event,       *this, &gw_fc_observer::on_start);
    it[1] = attach(p.finish_event,      *this, &gw_fc_observer::on_finish);
    it[2] = attach(p.touch_node_event,  *this, &gw_fc_observer::on_touch_node);
    it[3] = attach(p.merge_nodes_event, *this, &gw_fc_observer::on_merge_nodes);
    it[4] = attach(p.remove_node_event, *this, &gw_fc_observer::on_remove_node);
    it[5] = attach(p.color_node_event,  *this, &gw_fc_observer::on_color_node);
  }

 ~gw_fc_observer() { detach(it,6); }

};



void gw_fc_observer::on_start(alg_five_color&) 
{
  graph& g = gw.get_graph();

  pos0.init(g);

  // set initial layout

  gw.set_flush(false);

  node v;
  forall_nodes(v,g) {
    pos0[v] = gw.get_position(v);
    gw.deselect(v);
    gw.set_shape(v,circle_node);
/*
    gw.set_width(v,25);
*/
    gw.set_border_color(v,black);
    edge e;
    forall_adj_edges(e,v) {
      gw.deselect(e);
      gw.set_style(e,solid_edge);
      gw.set_direction(e,undirected_edge);
    }
  }
  gw.set_flush(true);
  gw.redraw();
}


void gw_fc_observer::on_finish(alg_five_color&) {
  gw.message("\\bf End of Demo");
}


void gw_fc_observer::on_touch_node(alg_five_color& a,node v) 
{
  const list<node>& L = a.get_merged_nodes(v);

  forall(v,L) 
  { gw.set_select(v,true,false);
    gw.set_shape(v,square_node);
    edge e;
    forall_inout_edges(e,v) 
    { node w = gw.get_graph().opposite(v,e);
      if (gw.get_color(w) != invisible) gw.select(e);
     }
   }
}



void gw_fc_observer::hide_node(node v, bool hide_edges) 
{ if (hide_edges) 
  { edge e;
    forall_inout_edges(e,v)
    { gw.deselect(e);
      gw.set_color(e,invisible);
      gw.set_label_color(e,invisible);
     }
   }
  gw.set_select(v,false,false);
  gw.set_color(v,invisible);
  gw.set_border_color(v,invisible);
  gw.set_label_color(v,invisible);
  gw.set_label_type(v,index_label);
}



void gw_fc_observer::on_merge_nodes(alg_five_color& a, node v, node w) 
{
  const list<node>& Lv = a.get_merged_nodes(v);
  const list<node>& Lw = a.get_merged_nodes(w);

  color cv = gw.get_color(v);
  color cw = gw.get_color(w);

  node x;
  forall(x,Lv) gw.set_color(x,green2);
  forall(x,Lw) gw.set_color(x,green2);

  string s("Merging Nodes  ");
  s += gw.get_label(v) + " and " + gw.get_label(w);

  wait(s);

  const list<node>& L = a.get_merged_nodes(w);

  gw.set_position(L,gw.get_position(v));

  gw.set_color(w,cw);
  gw.set_color(v,cv);

  forall(x,Lv) gw.set_color(x,cv);

  forall(x,Lw) 
  { gw.set_color(x,cw); 
    gw.set_label(x,gw.get_label(v));
   }
}



void gw_fc_observer::on_remove_node(alg_five_color& a, node v) 
{
  const list<node>& L = a.get_merged_nodes(v);

  gw.select(v);

  string s("Removing Node %s",gw.get_label(L.head()));

  if (L.length() > 1)
  { s += "~~[ ";
    forall(v,L) s += string("%d ",index(v));
    s += "]";
   }

  wait(s);
  forall(v,L) hide_node(v);
}




void gw_fc_observer::on_color_node(alg_five_color& a, node v, int c) 
{
  static color colors[] = { yellow,red,blue,green,pink };

  if (!coloring)
  { coloring = true;
    step = true;
   }

  const list<node>& L = a.get_merged_nodes(v);

  forall(v,L)
  { gw.set_select(v,true,false);
    gw.set_color(v,ivory);
    gw.set_label_color(v,black);
    edge e;
    forall_inout_edges(e,v) 
    { node u = opposite(v,e);
      if (gw.get_color(u) != invisible)
      { gw.set_style(e,solid_edge);
        gw.set_color(e,black);
        gw.set_select(e,true);
       }
     }
   }
  
  string s("Coloring Node %s",gw.get_label(L.head()));

  if (L.length() > 1)
  { s += "~~[ ";
    forall(v,L) s += string("%d ",index(v));
    s += "]";
   }

  wait(s);

  graph& g = gw.get_graph();
  node_array<point> pos(g);

  forall_nodes(v,g) pos[v] = gw.get_position(v);

  forall(v,L) 
  { gw.set_select(v,false,false);
    gw.set_shape(v,circle_node);
    gw.set_color(v,colors[c]);
    gw.set_border_color(v,black);
    gw.set_label_color(v,gw_auto_color);
    pos[v] = pos0[v];
    edge e;
    forall_inout_edges(e,v) 
    { node u = opposite(v,e);
      if (gw.get_color(u) != invisible) gw.set_select(e,false);
     }
  }

  if (L.length() > 1) gw.set_layout(pos);
}



void draw_graph(GraphWin& gw, color col, int dx, int dy)
{
  window& W = gw.get_window();
  graph& G = gw.get_graph();

  bool fl = gw.set_flush(false);

  gw.set_node_color(invisible);
  gw.set_node_border_color(col);
  gw.set_node_label_color(col);
  gw.set_edge_color(col);

  node v;
  forall_nodes(v,G)
  { point p = gw.get_position(v);
    gw.set_position(v,p.translate(W.pix_to_real(dx),W.pix_to_real(dy)));
   }

  gw.redraw();

  gw.set_flush(fl);
}


int main() 
{
  GraphWin  gw;

  gw.set_edge_direction(undirected_edge);

  gw.display();
  gw.edit();

  double xmin = gw.get_xmin();
  double ymin = gw.get_ymin();
  double xmax = gw.get_xmax();
  double ymax = gw.get_ymax();

  gw.save_node_attributes();
  gw.save_edge_attributes();

/*
  gw.set_bg_color(grey1);

  draw_graph(gw,grey3,0,0);

  char* pm1 = gw.get_window().get_pixrect(xmin,ymin,xmax,ymax);
  gw.set_bg_pixmap(pm1,xmin,ymax);

  draw_graph(gw,white,-1,1);
*/

  draw_graph(gw,grey1,0,0);

  char* pm = gw.get_window().get_pixrect(xmin,ymin,xmax,ymax);
  gw.set_bg_pixmap(pm,xmin,ymax);

  gw.restore_node_attributes();
  gw.restore_edge_attributes();


  gw_fc_observer observer(FIVE_COLOR,gw);
  node_array<int> col(gw.get_graph());
      
  FIVE_COLOR(gw.get_graph(),col);

  gw.wait();

  return 0;
}




