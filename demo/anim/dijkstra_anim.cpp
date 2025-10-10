/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  dijkstra_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/node_pq.h>

#include <stdlib.h>
                                                      
using namespace leda;


static const char intro_text[] =
{ "\\tt This demo illustrates the Dijkstra algorithm. \
   It solves the single source shortest path \
   problem on a network with non-negative edge costs."
};


static const char create_graph_text[] =
{ "\\tt\\red Construct a Network. \\black\\n\
   Construct a graph with at least one node. \\n\
   The cost values can be modified by moving the edge slider\\n \
   or by pressing the left mouse button on that edge. \\n \
   When you're ready press the \\blue done \\black button."
};   


static const char modify_graph_text[] =
{ "\\tt\\red Modify the Network. \\black\\n\
   Now you can modify the existing network. You can change the cost values, \\n\
   or you can add/remove some edges or nodes. When you're ready press \\n \
   the \\blue done \\black button."
};   


static const char empty_graph_text[] =
{ "\\tt Sorry the network is empty, it must have at least one node!"  };


static const char select_node_text[] =
{ "\\tt\\red Select the start node. \\black\\n\
   Select now the start node with the left mouse button. It will be \\n\
   highlighted after doing so. When you're done click on \\n \
   the \\blue done \\black  button again."
};


static const char start_text[] =
{ "\\tt\\red After the init phase. \\black\\n\
   We passed the init phase of the algorithm. Your selected start node s is \\n\
   temporary labeled and has distance 0. All the other nodes are grey \\n\
   that means that these nodes are not labeled, the distance \\n\
   from s to all of them is infinity. \\n\
   Press \\blue done \\black to continue."
};


static const char perm_text[] =
{ "\\tt\\blue Permanent Labeling. \\black\\n\
    The node with the current minimal \\green2 temporary label \\black will be colored \\blue blue \\black .\\n \
    Blue nodes are permanent labeled nodes. Ways between permanent labeled \\n\
    nodes become \\blue blue \\black and belong to the shortest path. The new permanent \\n\
    labeled node will be highlighted. Press \\blue done \\black  to see the result."
};


static const char candidate_textA[] =
{ "\\tt\\green2 Candidates. \\black\\n\
   The bold edges will show which nodes may get a new distance label.\\n\
   Press \\blue done \\black  to see the result."
};


static const char candidate_textB[] =
{ "\\tt\\green2 Candidates. \\black\\n\
   Press \\blue done \\black  to continue."
};


static const char temp_textA[] =
{ "\\tt\\green2 Temporary Labeling. \\black\\n\
   The black bordered node has not been labeled yet. It will be colored \\n\
   green2 to show that it is now temporary labeled. \\n \
   Press \\blue done \\black  to see the result."
};


static const char temp_textB[] =
{ "\\tt \\green2 Temporary Labeling \\black\\n\
   The black bordered node got a new distance label\\n \
   Press \\blue done \\black  to continue."
};


static const char decrease_textA[] =
{ "\\tt\\green2 Shorter Way. \\black\\n\
  Way through highlighted node is shorter than \\n\
  through current temporary way.\\n \
  The black bordered node gets a new distance label.\\n \
  Press \\blue done \\black  to continue."
};


static const char check_inequalityT[] =
{ "\\tt\\green2 Update Temporary Label. \\black\\n\
   Can you get a shorter path  through the current selected node?\\n \
   The way to the black bordered node is tested. Press \\blue done \\black  to continue."
};


static const char check_inequalityF[] =
{ "\\tt \\green2 No Better Way.\\black\\n\
   Way through highlighted node is not shorter.\\n \
   Edge is colored simple black to show that it has already been tested.\\n \
   Press \\blue done \\black  to continue."
};


static const char check_inequalityP[] =
{ "\\tt \\green2 No Better Way. \\black\\n\
   The target node is already permanent labeled. We can not find a \
   shorter way through the highlighted node.\\n\
   Press \\blue done \\black  to continue."
};


static const char pred_text[] =
{ "\\tt \\green2 Temporary Edge. \\black\\n\
   The new temporary way will be marked green. \\n\
   Press \\blue done \\black  to continue."
};


static const char finish_text[] =
{ "\\tt \\red Finshed! \\black \\n \
   The blue way (if it exists) is the shortest way from the\\n \
   start node (distance 0) to all the other nodes. The shortest \\n \
   distances are written into the nodes.\\n\
   Press \\blue done \\black to continue."
};

static int LOW        = 1; 
static int HIGH       = 10;
static int MAX        = 400;
static int node_height  = 50;                           
static int node_width = 90;
static int max_nodes  = 20000;                        
static int max_edges  = 20000;                        

static gw_node_shape node_shape = ovalrect_node;      

edge_array<int> cost;                                                                                       


//-----------------------------------------------------------------------------
//
// dijkstra observer
//
//-----------------------------------------------------------------------------


class dijkstra_observer
{
  GraphWin& GW;
  
  node s;
  const edge_array<int>&  cost;
  const node_array<int>&  dist;
  const node_array<edge>& pred;
  
  node_array<string> label;
  
  color temporary_node_c;
  color permanent_node_c;
  color unlabeled_node_c;
  
  color path_edge_c; 
  color path_temp_edge_c;
  color nopath_edge_c;  
  color unvisited_edge_c;
  
  bool  forever;
  float delay;
    
  void layout(list<node>& V, array<node>& V1, node_array<point>& pos)
  {  window& W = GW.get_window();

    double th = 5 * W.text_height("Z") + 10;

    double nh = W.pix_to_real(GW.get_node_height());

    double x0 = GW.get_xmin();
    double y0 = GW.get_ymin();
    double x1 = GW.get_xmax();
    double y1 = GW.get_ymax() - th;

    double bx0, by0, bx1, by1;
    GW.get_bounding_box(bx0,by0,bx1,by1);

    double ux0 = x0, ux1 = x1;
    double uy0 = (y1-y0) / 2 + y0 + nh, uy1 = y1;

    double lx0 = x0, lx1 = x1;
    double ly0 = y0, ly1 = (y1-y0) / 2 + y0 - nh;
    
    double DX = x1 - x0;
    double DY = by1 - by0;
    
    double ufx = (DX > 0) ? (ux1 - ux0) / DX : 1.0;
    double ufy = (DY > 0) ? (uy1 - uy0) / DY : 1.0;
    double udx = (ux0 + ux1 - (x0 + x1) * ufx) / 2;
    double udy = (uy0 + uy1 - (by0 + by1) * ufy) / 2;
       
    double lfx = (DX > 0) ? (lx1 - lx0) / DX : 1.0;
    double lfy = (DY > 0) ? (ly1 - ly0) / DY : 1.0;
    double ldx = (lx0 + lx1 - (x0 + x1) * lfx) / 2;
    double ldy = (ly0 + ly1 - (by0 + by1) * lfy) / 2;
  
    node v;
    forall(v,V)
    { point p = GW.get_position(v);
      pos[v] = point(ldx + lfx * p.xcoord(), ldy + lfy * p.ycoord());  
    }
  
    for (int i = V1.low(); i <= V1.high(); i++)
    { if (V1[i] == 0) continue;      
      point p = GW.get_position(V1[i]);
      pos[V1[i]] = point(udx + ufx * p.xcoord(), udy + ufy * p.ycoord());
    }
  } 


  void extend_graph(list<node>&)
  { bool B = GW.set_flush(false);
    
    graph& G = GW.get_graph();   

    node_array<point> pos(G);
    GW.get_position(pos);
      
    int low = G.index(G.first_node());
    int high = G.index(G.last_node());
    
    array<node> V1(low,high); 

    int i;
    for (i = V1.low(); i <= V1.high(); i++) V1[i] = 0;

    list<node> V = G.all_nodes();
    list<edge> E = G.all_edges();
    
    node v;
    forall(v,V)
    { V1[G.index(v)] = GW.new_node(pos[v]); 
      node w = V1[G.index(v)];

      GW.set_user_label(w, label[v]);             
      GW.set_shape(w, GW.get_shape(v));
      GW.set_height(w, GW.get_height(v));
      GW.set_width(w, GW.get_width(v)); 
    }    
 
    edge e;
    forall(e,E)
    { node s = G.source(e);
      node t = G.target(e);
      edge f = GW.new_edge(V1[G.index(s)],V1[G.index(t)]);
      GW.set_user_label(f, GW.get_user_label(e));
    }
        
    node_array<point> POS(G);
    layout(V,V1,POS);          
        
    GW.set_flush(true);
    GW.set_layout(POS);

    GW.wait();    
    
    GW.set_flush(false);
        
    for (i = V1.low(); i <= V1.high(); i++) 
      if (V1[i] != 0) GW.del_node(V1[i]);
    
    GW.set_flush(true);
    GW.set_layout(pos);
    GW.set_flush(B);
  }

  void wait(string message = "") 
  { if (!forever) 
    { GW.message(message);
      if (!GW.edit()) throw(1); 
    } else
        if (!GW.wait(delay)) throw(1); 
  }


  public:
  
  dijkstra_observer(GraphWin& gw, node v, 
                    const edge_array<int>& cost0,
                    const node_array<int>& dist0,
                    const node_array<edge>& pred0)
                    : GW(gw), s(v), cost(cost0), dist(dist0), pred(pred0) 
  {
    permanent_node_c = color(100,100,255); //blue
    //temporary_node_c = color(100,200,100);   // green 
    temporary_node_c = color(100,210,100);   // green 
    unlabeled_node_c  = color(200,200,200); // grey
    
    path_edge_c      = permanent_node_c;
    path_temp_edge_c = temporary_node_c;
    nopath_edge_c    = black;
    unvisited_edge_c = unlabeled_node_c;
    
    delay = 1.5;
    forever = false;    
    
    graph& G = GW.get_graph();
    
    label.init(G);

    node w;
    forall_nodes(w,G)
      label[w] = GW.get_label(w);
  }

  void run() 
  { wait("\\tt Press \\blue done \\black to continue to the end."); 
    forever = true;  
  }

  void start() 
  { graph& G = GW.get_graph();
    
    GW.set_flush(false);

    node v;
    forall_nodes(v,G)
    { if (v == s)
        GW.set_label(v, string("%d", dist[v]));
      else
        GW.set_label(v, string("oo"));
     
      GW.set_color(v, unlabeled_node_c);
    }
    
    edge e;
    forall_edges(e,G)
    { GW.set_color(e, unvisited_edge_c);    
      GW.set_label_color(e, unvisited_edge_c);    
    }
    
    GW.set_label_color(s, black);
    GW.set_color(s, temporary_node_c);

    GW.set_flush(true);
    GW.redraw();
    
    wait(start_text);
  }
  
  void del_min(node v)
  { wait(perm_text);
    
    if (!GW.get_selected_nodes().empty())
    { node w = GW.get_selected_nodes().head();
   
      GW.set_flush(false);
      graph& G = GW.get_graph();    
    
      node tmp = G.new_node();
        
      GW.set_label(tmp,"");
      GW.set_color(tmp,invisible);
      GW.set_label_color(tmp,invisible);
      GW.set_shape(tmp, GW.get_shape(w));
      GW.set_height(tmp, GW.get_height(w));
      GW.set_width(tmp, GW.get_width(w));
      GW.set_border_color(tmp,black);
      GW.set_position(tmp,GW.get_position(w));
      GW.set_animation_steps(100);
      
      GW.deselect(w);
      GW.select(tmp);

      GW.set_flush(true);
    
      GW.set_position(tmp,GW.get_position(v));
      GW.deselect(tmp);
    
      G.del_node(tmp);
    }
    
    GW.select(v);
        
    GW.set_label_color(v, white);
    GW.set_color(v, permanent_node_c);

    if (pred[v])
      GW.set_color(pred[v],path_edge_c);

    graph& G = GW.get_graph();    

    if (!G.adj_nodes(v).empty())
    { wait(candidate_textA);

      edge e;
      forall_adj_edges(e,v)
      { GW.set_width(e,2);
        GW.set_color(e,black);
      } 
    
      wait(candidate_textB);
    
      forall_adj_edges(e,v)
      { GW.set_width(e,1);
        GW.set_color(e,unvisited_edge_c);    
      }  
    }  
  }
  
 
  void insert(node v, int C)
  { wait(temp_textA);
    GW.set_label(v, string("%d", C));
    GW.set_color(v, temporary_node_c);    
    wait(temp_textB);    
    GW.set_border_width(v,1);
  }
  
  void decrease_p(node v, int C)
  { wait(decrease_textA);
    GW.set_label(v, string("%d", C));
    GW.set_border_width(v,1);
  }
  
  void set_pred(node v, edge e)
  { wait(pred_text);
    
    if (pred[v]) 
    { GW.set_width(pred[v],1);
      GW.set_color(pred[v],black);
    }
    
    GW.set_color(e,path_temp_edge_c);
  }
  
  void check_inequality(node_pq<int>& PQ, node v, node w, edge e, bool B)
  { GW.set_width(e,2);
    GW.set_color(e,black);
    GW.set_label_color(e,black);
    GW.set_border_width(w,2);

    if (pred[w] != 0 &&  !PQ.member(w)) 
    {
      wait(check_inequalityP);
      GW.set_width(e,1);
      GW.set_border_width(w,1);    
    }
    else    
    { wait(check_inequalityT);
    
      if (!B) 
      { wait(check_inequalityF);  
        GW.set_width(e,1);
        GW.set_border_width(w,1);
      }
    }
  }
  
  void finish() 
  { graph& G = GW.get_graph();
    GW.deselect_all_nodes();

    GW.message(finish_text);    

    gw_node_shape shape = GW.get_shape(s);
    
    int height = GW.get_height(s);
    int width  = GW.get_width(s);
    
    GW.set_flush(false);
    
    GW.deselect_all_nodes();

    list<node> L;
    extend_graph(L);
 
    node v;
    forall_nodes(v,G)
    {
      GW.set_color(v,ivory);
      GW.set_label_color(v, black);

      GW.set_shape(v, shape);
      GW.set_height(v, height);
      GW.set_width(v, width);

      GW.set_user_label(v, label[v]);
    }

    edge e;
    forall_edges(e,G)
    {
      GW.set_color(e, black);    
      GW.set_width(e, 1);
      GW.set_label_color(e, black);        
    }

    GW.set_flush(true);

    GW.del_message();
    GW.redraw();
  }
};


//-----------------------------------------------------------------------------
//
// dijkstra algorithm
//
//-----------------------------------------------------------------------------

void dijkstra(const graph& G, node s, 
              const edge_array<int>& cost,
              node_array<int>& dist, 
              node_array<edge>& pred,
              dijkstra_observer& OB)
{
  int INFTY = MAX * G.number_of_nodes();
  
  node_pq<int> PQ(G);
  
  node v;
  forall_nodes(v,G)
  { dist[v] = INFTY;
    pred[v] = 0;
  }
  
  dist[s] = 0;
  
  OB.start();

  PQ.insert(s,0);

  while (!PQ.empty())
  { v = PQ.del_min();

    OB.del_min(v);
    
    edge e;
    forall_adj_edges(e,v)
    { node w = G.target(e);
     
      OB.check_inequality(PQ,v,w,e,dist[v] + cost[e] < dist[w]);
     
      int C = dist[v] + cost[e];
      if (C < dist[w])
      { if (pred[w] == 0)
        { OB.insert(w,C);
          PQ.insert(w,C);
        }
        else
        { OB.decrease_p(w,C);
          PQ.decrease_p(w,C);
        } 
        
        dist[w] = C;

        OB.set_pred(w,e);
        pred[w] = e;      
      }          
    }
  }
  
  OB.finish();
}
                          

//-----------------------------------------------------------------------------
//
// some graphwin handler
//
//-----------------------------------------------------------------------------
                                                      
void init_graph_handler(GraphWin& GW)                 
{                                                     
  graph& G = GW.get_graph();                          
                                                      
  GW.set_flush(false);                                

  edge e;                                             
  forall_edges(e,G)                                   
  { if (GW.get_user_label(e) != "") 
    { int c = atoi(GW.get_user_label(e)); 
      cost[e] = c;      

      if (c < LOW) LOW = c;
      if (c > HIGH) HIGH = c;
    }
    else
      cost[e] = rand_int(LOW, HIGH);                                                     

    GW.set_width(e, 1);
    GW.set_color(e, black);    
    GW.set_label(e, string("%d", cost[e]));
    GW.set_label_color(e, black);        
    GW.set_slider_value(e, double(cost[e])/HIGH, 0);  
  }

  int num = 0;

  node v;
  forall_nodes(v,G)
  { GW.set_color(v,ivory);
    GW.set_label_color(v, black);
    GW.set_shape(v, node_shape);
    GW.set_width(v, node_width);
    GW.set_height(v, node_height);
      
    if (GW.get_user_label(v) != "")
      num = atoi(GW.get_user_label(v));
    else
      num++;
    
    GW.set_user_label(v, string("%d",num));
  }
  
  GW.set_flush(true);
  GW.redraw();
}

void new_edge_handler(GraphWin& GW, edge e)
{ cost[e] = rand_int(LOW, HIGH);
  GW.set_label(e, string("%d", cost[e]));
  GW.set_slider_value(e, double(cost[e])/HIGH, 0);
}

void new_node_handler(GraphWin& GW, node v) 
{ GW.set_shape(v, node_shape);
  GW.set_height(v,node_height);
  GW.set_width(v, node_width);
  GW.set_user_label(v, string("%d", index(v))); 
}


void set_cost(GraphWin& GW, const point&)
{ edge e = GW.get_edit_edge();
 
  int val = cost[e];
  
  panel P;
  P.int_item("edge cost", val);
  P.fbutton("&Apply",0);
  P.button("&Cancel",1);

  if (GW.open_panel(P)) return;
    
  while (val < LOW || val > HIGH)
    if (GW.open_panel(P)) return;
  
  cost[e] = val;
  GW.set_label(e, string("%d", cost[e]));
  GW.set_slider_value(e, double(cost[e])/HIGH, 0);
}

void del_edge(GraphWin& GW, const point&)
{ GW.del_edge(GW.get_edit_edge());
  GW.redraw();  
}

void del_node(GraphWin& GW, const point&)
{ GW.del_node(GW.get_edit_node());
  GW.redraw();  
}

void select_node(GraphWin& GW, const point&)
{ node v = GW.get_edit_node();
  
  if (GW.is_selected(v))
    GW.deselect(v);
  else
  { GW.deselect_all_nodes();
    GW.select(v);
  }
}

void new_node(GraphWin& GW, const point& p)
{ GW.set_flush(false); 
  node v = GW.new_node(p);
  new_node_handler(GW,v); 
  GW.set_flush(true);
  GW.redraw();
}


void edge_slider_handler(GraphWin& GW, edge e, double f) 
{ cost[e] = int(double((HIGH-LOW) * f + 0.01)) + LOW; 
  GW.set_label(e, string("%d", cost[e]));
}


void update_edge_cost(GraphWin& GW)
{ graph& G = GW.get_graph();

  GW.set_flush(false);

  edge e;
  forall_edges(e,G)
    edge_slider_handler(GW,e,GW.get_slider_value(e));

  GW.set_flush(true);
  GW.redraw();
}

void edge_cost_panel(GraphWin& GW)
{ int LOW_OLD = LOW, HIGH_OLD = HIGH;

  panel P;
  P.text_item("\\tt Here you can adjust the edge cost:");
  P.int_item("Minimum cost", LOW, 0 , MAX);
  P.int_item("Maximum cost", HIGH, 0, MAX);
  P.fbutton("Apply",1);
  P.button("Cancel",0);
  
  while (1)
    switch (GW.open_panel(P))
    { case 0: 
        LOW  = LOW_OLD; 
        HIGH = HIGH_OLD; 
        return;

      case 1: 
        if (LOW < HIGH)
        { update_edge_cost(GW);
          return;
        }
    };
}


void move_graph(GraphWin& GW)
{
  window& W = GW.get_window();
 
  double th = 5 * W.text_height("Z") + 10;
  double x0 = GW.get_xmin() + 5;
  double y0 = GW.get_ymin() + 5;
  double x1 = GW.get_xmax() - 5;
  double y1 = GW.get_ymax() - th;
  
  double bx0, by0, bx1, by1;
  GW.get_bounding_box(bx0,by0,bx1,by1);

  if (y1 < by1) GW.place_into_box(x0,y0,x1,y1);
}  

void exit_handler(window *wp) { if (1) throw(1); }

void gw_exit_handler(GraphWin& GW) { exit_handler(&GW.get_window()); }

void forever_handler(GraphWin& GW)
{ window& W = GW.get_window();
  dijkstra_observer& OB = *(dijkstra_observer*) W.get_client_data(2);  
  OB.run();
}

int main()
{
  graph G;
  
  node_array<int>  dist;
  node_array<edge> pred;
  
  cost.init(G,max_edges,0);
  dist.init(G,max_nodes,0);
  pred.init(G,max_nodes,0);

  GraphWin GW(G,"Visualization of Dijkstra's Algorithm");
  
  GW.set_directed(true);        
  GW.set_edge_label_type(user_label);
  GW.set_node_label_type(user_label);
  
  GW.set_init_graph_handler(init_graph_handler);
  GW.set_new_edge_handler(new_edge_handler);
  GW.set_new_node_handler(new_node_handler);
   
  GW.set_default_menu(M_FILE|M_GRAPH|M_DONE);  
  
  int M_ID = GW.add_menu("&Properties"); 
  GW.add_simple_call(edge_cost_panel,"&Edge cost",M_ID);
  
//  int M_FE = GW.add_simple_call(forever_handler,"&forever");
//  GW.disable_call(M_FE);

  window& W = GW.get_window();
  W.set_window_close_handler(exit_handler);
  
  GW.display(window::center, window::center);
 
  panel P;
  P.text_item(intro_text);
  P.fbutton("Continue",0);
  while (GW.open_panel(P)); 

  try 
  { do
    { if (G.empty()) 
        GW.message(create_graph_text);  
      else
        GW.message(modify_graph_text);

      GW.set_edge_slider_handler(edge_slider_handler);

      GW.reset_actions();
 
      GW.set_action(A_RIGHT,0);
      GW.set_action(A_MIDDLE|A_EDGE,0);   
      GW.set_action(A_MIDDLE|A_NODE,0);
      GW.set_action(A_RIGHT|A_EDGE,del_edge);
      GW.set_action(A_RIGHT|A_NODE|A_FRAME,del_node);
      GW.set_action(A_RIGHT|A_NODE,del_node);  
      GW.set_action(A_LEFT|A_EDGE,set_cost);
      GW.set_action(A_LEFT|A_DOUBLE,0);
      GW.set_action(A_LEFT,new_node);

      GW.redraw();
    
      if (!GW.edit()) break;    
      while (G.number_of_nodes() == 0)
      { GW.message(empty_graph_text);
        if (!GW.edit()) return 0;
      }        
     
      move_graph(GW);
  
      GW.set_edge_slider_handler(0);
    
      GW.set_action(A_RIGHT|A_EDGE,0);
      GW.set_action(A_MIDDLE|A_EDGE,0);   
      GW.set_action(A_MIDDLE|A_NODE,select_node);
      GW.set_action(A_RIGHT|A_NODE,select_node);
      GW.set_action(A_LEFT|A_EDGE,0);
      GW.set_action(A_LEFT|A_NODE,select_node);  
      GW.set_action(A_LEFT|A_DOUBLE,0);
      GW.set_action(A_LEFT|A_DRAG,0);
      GW.set_action(A_LEFT,0);

      GW.redraw();

      node s = G.first_node();
      if (G.number_of_nodes() > 1)
      { while (GW.get_selected_nodes().empty())
        { GW.message(select_node_text);
          if (!GW.edit()) return 0;
        }

        s = GW.get_selected_nodes().head();    
        GW.deselect_all_nodes();
      } 
  
      GW.deselect(s);
       
      GW.set_action(A_MIDDLE|A_NODE,0);
      GW.set_action(A_RIGHT|A_NODE,0);
      GW.set_action(A_LEFT|A_NODE,0);  
            
      dijkstra_observer OB(GW,s,cost,dist,pred);
      
//      W.set_client_data(&OB,2);
//      GW.enable_call(M_FE);
      dijkstra(G,s,cost,dist,pred,OB);      
//      GW.disable_call(M_FE);
    } while (1);
  } catch(int) {}

  return 0;
}
