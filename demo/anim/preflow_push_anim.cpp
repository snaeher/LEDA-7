/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  preflow_push_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/anim/TextLoader.h>

#include <LEDA/core/queue.h>

#include <math.h>

using namespace leda;

using std::ifstream;


string doc_file = "doc/preflow_push/preflow_push_anim.doc";

node s;
node t;
int  cur_cap = 1;
bool do_anim = true;
bool wait_click = true;
bool interact = false;


double delay=1.0;
//TextLoader TL(500,450,"Preflow-Push Pseudo Code" );
TextLoader TL(1200,1200,"Preflow-Push Pseudo Code" );


void showText( int line )
{ if (line > 0)  
  { if (line > 13) 
       line += 6;
    else
       if (line > 6) line += 3;
    TL.set_string_style("\\red", line, " ");
   }
  
  TL.load_text_with_display(doc_file);  
  leda_wait( delay );
}



void compute_dist (const GRAPH<double,int>& G, node s, node t,
                                               const edge_array <int> &flow, 
                                               node_array <int> &dist) 
{
  node u, v;
  edge e;
  int N = G.number_of_nodes ();
  int d;
  queue <node> Q;

  Q.append (t);
  dist[t] = 0;
  while (!Q.empty ()) {
    v = Q.pop ();
    d = dist[v] + 1;
    forall_in_edges (e, v) {
      if (G[e] - flow[e] > 0) {
	u = source (e);
	if (/*u != s &&*/ dist[u] == N) {
	  dist[u] = d;
	  Q.append (u);
	}
      }
    }
    forall_out_edges (e, v) {
      if (flow[e] > 0) {
	u = target (e);
	if (/*u != s &&*/ dist[u] == N) {
	  dist[u] = d;
	  Q.append (u);
	}
      }
    }
  }
}


void draw_graph (GraphWin& gw,
                 const node_array <double> &cur_height, 
                 const node_array <double> &cur_exc, 
                 const node_array <color> &cur_col, 
                 const edge_array <double> &cur_flow, edge mark) 
{
  double x, y1, y2, xt, yt, xDir, yDir, xd, yd;
  double xWidth, yWidth, xSource, ySource, xTarget, yTarget, xCen, yCen;
  point p, q;
  node v, w;
  edge e;

  window& win = gw.get_window();

  GRAPH<double,int>& G = (GRAPH <double, int>&) gw.get_graph ();

  forall_edges (e,G) 
     gw.set_label (e, string ("%d|%d", (int)cur_flow[e], G[e]));

  win.start_buffering ();
  win.clear();

  gw.draw_graph();

/*
  forall_edges (e, G) {
    v = source (e);
    w = target (e);
    win.draw_edge (gw.get_position (v), gw.get_position (w), grey2);
  }
*/

  double pix = win.pix_to_real(1);

  forall_nodes (v, G) {
    p = gw.get_position (v);
    x = p.xcoord ();
    y1 = p.ycoord ();
    y2 = y1 + cur_height[v]*1.5;

    win.draw_filled_ellipse (x,y1, 3.0, 2.0, grey2);
    win.draw_ellipse (x,y1, 3.0, 2.0, black);
    win.draw_box (x-3.1, y1, x+3+pix, y2, grey2);

    for(int i=1; i<cur_height[v]; i++)
    { double y = y1 + 1.5*i;
      win.draw_filled_ellipse (x,y, 3.0, 2.0, grey2);
      win.draw_ellipse (x,y, 3.0, 2.0, grey3);
     }

    win.draw_segment (x-3.0, y1, x-3.0, y2, black);
    win.draw_segment (x+3+pix, y1, x+3+pix, y2, black);
    win.draw_filled_ellipse (x, y2, 3.0, 2.0, cur_col[v]); 
    win.draw_ellipse (x, y2, 3.0, 2.0, black);

/*
    if (v==s) 
       win.draw_ctext (x, y2, "s", black);
    else 
       if (v==t) 
          win.draw_ctext (x, y2, "t", black);
       else 
*/
    win.draw_ctext(x,y2,
                   string("%d",(int)cur_height[v]),cur_col[v].text_color());


    forall_out_edges (e, v) {
      w = target (e);
      q = gw.get_position (w);
      xt = q.xcoord ();
      yt = q.ycoord () + cur_height[w]*1.5;
      xd = xt-x;  yd = yt-y2;
      xDir = (xd>=0)?1.0:-1.0;
      yDir = (yd>=0)?1.0:-1.0;
      xWidth = 3.0/sqrt(1+(yd*yd)/(xd*xd));
      yWidth = 2.0/sqrt(1+(xd*xd)/(yd*yd));
      xSource = x  + xWidth*xDir;
      ySource = y2 + yWidth*yDir;
      xTarget = xt - xWidth*xDir;
      yTarget = yt - yWidth*yDir;
      xCen = (cur_flow[e]/(double)G[e])*(xTarget-xSource) + xSource;
      yCen = (cur_flow[e]/(double)G[e])*(yTarget-ySource) + ySource;
      if (e==mark) win.set_line_width (2);

      if (cur_flow[e] > 0) 
           win.draw_edge_arrow (xCen, yCen, xSource, ySource, cyan);

      if (G[e] > cur_flow[e]) 
          win.draw_edge_arrow (xCen, yCen, xTarget, yTarget, black);

      if (e==mark) win.set_line_width (1);
    }

    if (v != s && cur_exc[v] > 0)
    { win.draw_box (x-1.5, y2+3.0, x+1.5, y2+3.0+cur_exc[v]*0.75, blue);
      win.draw_rectangle (x-1.5, y2+3.0, x+1.5, y2+3.0+cur_exc[v]*0.75, black);
      win.draw_ctext (x, y2+5.0+cur_exc[v]*0.75, string("%d", (int)cur_exc[v]), black);    
    }
  }  
  win.flush_buffer ();
  win.stop_buffering ();
}


void change_height (GraphWin& gw, 
                    const node_array <int> &dist, 
                    node_array <double> &cur_height, 
                    node_array <double> &cur_exc, 
                    const edge_array <double> &cur_flow, node u) 
{

  graph& G = gw.get_graph();

  int i;
  node v;
  node_array <color> cur_col (G, yellow);
  
  cur_col[s] = green;
  cur_col[t] = violet;
  if (u!=NULL) cur_col[u] = red;

  for (i=0; i<15; i++) {
    forall_nodes (v, G) {
      cur_height[v] = cur_height[v] + ((double)dist[v]-cur_height[v])/(15-i);
    }
    draw_graph (gw,cur_height, cur_exc, cur_col, cur_flow, NULL);
    leda_wait (0.1);
  }
  leda_wait (1.0);
}


void change_exc (GraphWin& gw,
                 const node_array <double> &cur_height, 
                 const node_array <int> &excess, 
                 node_array <double> &cur_exc, 
                 const edge_array <int> &flow, 
                 edge_array <double> &cur_flow, 
                 node u1, node u2, edge mark) 
{
  int i;
  node v;
  edge e;

  graph& G = gw.get_graph();

  node_array <color> cur_col (G,yellow);
  
  cur_col[s] = green;
  cur_col[t] = violet;

  cur_col[u1] = red;
  cur_col[u2] = orange;

  for (i=0; i<15; i++) {   
    forall_nodes (v, G) {
      cur_exc[v] = cur_exc[v] + ((double)excess[v]-cur_exc[v])/(15-i);
    }
    forall_edges (e, G) {
      cur_flow[e] = cur_flow[e] + ((double)flow[e]-cur_flow[e])/(15-i);
    }
    draw_graph (gw,cur_height, cur_exc, cur_col, cur_flow, mark);
    leda_wait (0.1);
  }
  leda_wait (1.0);
}

void colorize (GraphWin&gw, 
               const node_array <double> &cur_height, 
               node_array <double> &cur_exc, 
               const edge_array <double> &cur_flow, 
               node u1, node u2, edge em,color mark1, color mark2) 
{
  node_array <color> cur_col (gw.get_graph(), yellow);
  
  cur_col[s] = green;
  cur_col[t] = violet;
  if (u1!=NULL) cur_col[u1] = mark1;
  if (u2!=NULL) cur_col[u2] = mark2;    
  draw_graph (gw,cur_height, cur_exc, cur_col, cur_flow, em);
}

void WaitForAction(GraphWin& gw, double time, string text) 
{ 
  //if (wait_click) text += " (press done to continue)";

  window& win = gw.get_window();
  win.del_messages();
  win.message(text);
   
  if (wait_click)
     do_anim = gw.wait();
  else 
     leda_wait(time);
}

void ComputeEdges (GRAPH<double,int>& G, node v, list <edge>& EL, const edge_array <int>& flow, const node_array <int>& dist, const node_array <int>& excess) {
  node w;
  edge e;
  int rcap;

  EL.clear ();
  forall_inout_edges (e, v) {
    rcap = (v == source(e)) ? G[e] - flow[e] : flow[e];
    if (rcap > 0) {
      w = opposite (v, e);
      if ((dist[v] == dist[w] + 1) && (excess[v]>0)) EL.append (e);
    }
  }
}


int max_flow (GraphWin& gw, node s, node t,
              edge_array <int> &flow, node_array <int> &excess) 
{
  GRAPH<double,int>& G = (GRAPH <double, int>&) gw.get_graph ();



  // preprocessing
  node v, w, tmp;
  edge e, e2;
  point p, q;
  int rcap, min_dist;
  int N = G.number_of_nodes ();
  double range, cur_range;
  double pi = 1.5707963268;
  list_item cur_item;
  //bool ready = false;
  node_array <int> dist (G, N);
  node_array <double> cur_height (G, 0);
  node_array <double> cur_exc (G, 0);
  edge_array <double> cur_flow (G, 0);
  list <edge> edge_list;
  list <node> has_excess;
  
  showText(14); showText(15); showText(16); 

  if (do_anim) {
    gw.del_message();
    gw.message ("compute all distances");
  }

  showText(1); showText(2); showText(3); 
  compute_dist (G,s,t,flow, dist);
  change_height (gw, dist, cur_height, cur_exc, cur_flow, NULL);

  showText(4); 
  dist[s] = N-1;

  forall_nodes (v, G) {
    p = gw.get_position (v);
    G [v] = -p.ycoord ();
  }
  G.sort_nodes ();
  
  if (do_anim) {
    change_height (gw, dist, cur_height, cur_exc, cur_flow, NULL);
    colorize (gw,cur_height, cur_exc, cur_flow, s, s, NULL, red, red);
    //WaitForAction (gw,1.0, "saturate all edges incident to s");
    WaitForAction (gw,1.0,"");
  }
  
  showText(5);

  forall_adj_edges (e, s) {
    v = target (e);  
    if (dist[v]!=N) {
      flow[e] = G[e]; 
      excess[v] += flow[e];
      excess[s] -= flow[e];
      
      if (do_anim) {
	colorize (gw,cur_height, cur_exc, cur_flow, s, v, e, red, orange);
	//WaitForAction (gw,1.0, "push over next adjazent edge");
	change_exc (gw,cur_height, excess, cur_exc, flow, cur_flow, s, v, e);
      }
      if (v != t) has_excess.push (v);
    }
  }

  showText(6); showText(17); 

  // push 
  while (!has_excess.empty ()) {
  
    if (do_anim && interact) {
      colorize (gw,cur_height, cur_exc, cur_flow, NULL, NULL, NULL, 0, 0);
      v = gw.ask_node();

/*
      window& win = gw.get_window();
      win.del_messages();
      win.message("select source-node");
      do {
        gw.get_window().read_mouse(p);
	range = MAXDOUBLE;
	forall (tmp, has_excess) {
	  q = gw.get_position (tmp);
	  cur_range = p.distance (q.translate (pi, dist[tmp]*1.5));
	  if (range > cur_range) {
	    range = cur_range;
	    v = tmp;
	  }
	}
	if ((range > 2.0) || (v==t)) v = NULL;
      } while (v == NULL);
*/
    if (do_anim) colorize (gw,cur_height, cur_exc, cur_flow, v, NULL, NULL, red, 0);
      cur_item = has_excess.search (v);
    } else {
      cur_item = has_excess.last ();
      v = has_excess[cur_item];
    }

    ComputeEdges (G,v, edge_list, flow, dist, excess);

    while (!edge_list.empty () && excess[v]>0) {
      if (do_anim && interact) {
      	gw.message ("select target-node");
      	do {
      	  gw.get_window().read_mouse (p);
      	  range = MAXDOUBLE;
          w = NULL;
      	  forall (e2, edge_list) {
	    tmp = opposite (v, e2);
      	    q = gw.get_position (tmp);
      	    cur_range = p.distance (q.translate (pi, dist[tmp]*1.5));
	    
      	    if (range > cur_range) {
      	      range = cur_range;
      	      w = tmp; e = e2;
      	    }
      	  } /* for all */
	  
      	  if (range > 2.0) w = NULL;
      	
	} while (w == NULL);
	
      	edge_list.remove (e);

      } else {
      	e = edge_list.pop ();
	w = opposite (v, e);
      }

      rcap = (v == source (e)) ? G[e] - flow[e] : flow[e];
      if (rcap > excess[v]) rcap = excess [v];  //rcap = min{rcap, excess}
      excess[v] -= rcap;
      if ((w != t) && (excess[w] == 0)) has_excess.push (w);
      excess[w] += rcap;
      
      if (v == source (e)) {
      	flow[e] += rcap;     // nuetzliche Vorwaertskante
		
	showText(17);
        showText(18);
      	//WaitForAction (gw,1.5, "");
      	colorize (gw,cur_height, cur_exc, cur_flow, v, w, e, red, yellow);

        showText(19);  /* main loop */

    	showText(7);
        showText(8); 
	showText(9); 
        showText(10); /*push*/ 

      	if (do_anim) {
      	  colorize (gw,cur_height, cur_exc, cur_flow, v, w, e, red, orange);
      	  WaitForAction (gw,1.5, "push flow from red to orange node");
      	  change_exc (gw, cur_height, excess, cur_exc, flow, cur_flow, v, w, e);
      	}
				
	showText(13); showText(17); /* back to main loop */
      
      } else {
      	  flow[e] -= rcap;     // nuetzliche Rueckwaertskante
	
      	  showText(17); showText(18); showText(19); /* main loop */
      	  showText(7); showText(8); 
	  showText(9); showText(10); /*push*/ 

      	  if (do_anim) {
      	    colorize (gw,cur_height, cur_exc, cur_flow, v, w, e, red, orange);
      	    WaitForAction (gw,1.0, "push flow from red to orange node");
      	    change_exc (gw, cur_height, excess, cur_exc, flow, cur_flow, v, w, e);
      	  }			
					
      	  showText(13); showText(17); /* back to main loop */
        }  /* else */ 
      }

    // relabel
    if (excess[v] > 0) {
      min_dist = MAXINT;
      forall_out_edges (e, v) {
      	rcap = G[e] - flow[e];
      	if (rcap > 0) {
      	  w = target (e);
      	  if (min_dist > dist[w]) min_dist = dist[w];
      	}
      }
    
      forall_in_edges (e, v) {
        rcap = flow[e];
        if (rcap > 0) {
      	  w = source (e);
      	  if (min_dist > dist[w]) min_dist = dist[w];
        }
      }
  
     dist[v] = min_dist + 1;
		
      showText(17); showText(18); showText(19); /* main loop */
      showText(7); showText(8); showText(9);
      showText(11); showText(12); /* relabel */

      if (do_anim) {
       colorize (gw,cur_height, cur_exc, cur_flow, v, v, NULL, red, red);      		
       WaitForAction (gw,1.0, "relabel red node");
       change_height (gw, dist, cur_height, cur_exc, cur_flow, v);
      }
			
      showText(13); showText(17); /* back to main loop */
    } else has_excess.erase (cur_item);
  
  } /* end while */

  showText(20); 
  TL.set_string_style("\\black", 20, "\\");
  TL.load_text_with_display(doc_file);  
	
  if (do_anim) {
    WaitForAction (gw, 2.0, "finished");
    forall_nodes (v, G) dist[v] = 0;
    change_height (gw, dist, cur_height, cur_exc, cur_flow, NULL);
  }
  
  return excess[t];
}

void start_maxflow(int) 
{
  window* wp = window::get_call_window();

  GraphWin& gw = *(wp->get_graphwin());

  GRAPH<double,int>& G = (GRAPH <double, int>&) gw.get_graph ();

  node v;
  edge e;
  edge_array <int> flow (G, 0);
  node_array <int> excess (G, 0);

  gw.deselect_all_edges();
  forall_edges (e, G) 
  { gw.set_color(e,grey2);
    gw.set_label_color(e,grey2);
   }

  int mflow = max_flow (gw,s,t,flow, excess);

  forall_nodes (v, G) {
    if (v==s) gw.set_label (v, "s"); 
    else if (v==t) gw.set_label (v, "t");
    else gw.set_label (v, "0");
  }

  forall_edges (e, G) 
  { gw.set_color(e,black);
    gw.set_label_color(e,black);
    gw.set_label (e, string ("%d|%d", flow[e], G[e]));
   }

  gw.del_message();
  gw.message (string ("MaxFlow = %d", mflow));
  gw.redraw ();
}

void change_cap (int cur_cap) {
  window* wp = window::get_call_window();
  GraphWin* gw = wp->get_graphwin();
  GRAPH<double,int>& G = (GRAPH<double,int>&)gw->get_graph();
  edge e;
  forall_edges (e, G) {
    if (gw->is_selected (e)) {
      G[e] = cur_cap;
      gw->set_label (e, string ("0|%d", G[e]));
    }
  }
  gw->redraw ();
}


void random_caps (int) {
  window* wp = window::get_call_window();
  GraphWin* gw = wp->get_graphwin();
  GRAPH<double,int>& G = (GRAPH<double,int>&)gw->get_graph();
  edge e;
  forall_edges (e, G) 
  { G[e] = rand_int(1,15);
    gw->set_label (e, string ("0|%d", G[e]));
   }
  gw->redraw ();
}


void choose_s (int) 
{ window* wp = window::get_call_window();
  GraphWin* gw = wp->get_graphwin();
  node v = gw->ask_node();
  gw->set_flush (true);
  gw->set_color(s,gw->set_color(v,green));
  gw->set_label(v,"s");
  gw->set_label(s,"0");
  s = v;
  //gw->redraw ();
  gw->set_flush (false);
}

void choose_t (int) 
{ window* wp = window::get_call_window();
  GraphWin* gw = wp->get_graphwin();
  node v = gw->ask_node();
  gw->set_flush (true);
  gw->set_color(t,gw->set_color(v,violet));
  gw->set_label(v,"t");
  gw->set_label(t,"0");
  t = v;
  //gw->redraw ();
  gw->set_flush (false);
}

void new_edge_handler (GraphWin& gw, edge e) {
  GRAPH<double,int>& G = (GRAPH <double, int>&) gw.get_graph ();
  G[e] = cur_cap;
  gw.set_label (e, string ("0|%d", G[e]));
  gw.redraw ();
}


void new_node_handler (GraphWin& gw, node v) {
  gw.set_flush (true);
  gw.set_label (v, "0"); 
  gw.set_flush (false);
}


bool del_node_handler (GraphWin& gw, node v) {
  return ( v != s &&  v != t);
}


void init_graph_handler (GraphWin& gw) 
{
  node v;
  edge e;
  GRAPH<double,int>& G = (GRAPH <double, int>&) gw.get_graph ();

  if (G.number_of_nodes () == 0) {
    gw.set_flush (true);
    s = gw.new_node (point (-10.0, -10.0));
    t = gw.new_node (point (110.0, -10.0));
    gw.set_label (s, "s");
    gw.set_label (t, "t");
    gw.set_color (s, green);
    gw.set_color (t, violet);
    gw.set_position (s, point (20.0, 50.0));
    gw.set_position (t, point (80.0, 50.0));
    gw.set_flush (false);
  } else {
    s = G.first_node ();
    t = G.last_node ();
  }

  gw.set_flush (false);

  forall_nodes (v, G) gw.set_label (v, "0");

  gw.set_label (s, "s"); 
  gw.set_color (s, green);

  gw.set_label (t, "t");
  gw.set_color (t, violet);

  forall_edges (e, G) gw.set_label (e, string ("0|%d", G[e]));

  gw.redraw ();
}



int main() 
{

  if (!is_directory(("doc/preflow_push")))
  { string lroot; 
    if (get_environment("LEDAROOT",lroot))
       doc_file = lroot + "/demo/animation/" + doc_file;
   }


  GRAPH<double,int> G;

  GraphWin gw(G, "LEDA: Preflow Push");

  gw.add_help_text("preflow_push_anim");

  gw.finish_menu_bar();

  window& win = gw.get_window();

  win.set_node_width(0);

/*
  win.int_item("capacity", cur_cap, 1, 15, change_cap);
  win.button("choose `s'", choose_s);
  win.button("choose `t'", choose_t);
  win.button("random caps", random_caps);
  win.button ("start", start_maxflow);

  win.bool_item ("Animation", do_anim);
  win.bool_item ("WaitForClick", wait_click);
*/
/*
  win.bool_item ("Interactive", interact);
  win.display(window::center,window::center);
*/

  gw.set_init_graph_handler (init_graph_handler);
  gw.set_new_edge_handler   (new_edge_handler); 
  gw.set_new_node_handler   (new_node_handler);
  gw.set_del_node_handler   (del_node_handler);
  gw.set_directed (true);

  gw.display(window::min,window::center);

  int x0,y0,x1,y1;
  gw.get_window().frame_box(x0,y0,x1,y1);

  TL.set_scrollbar( true );
  TL.load_text_with_display(doc_file,x1+1,y0);  
  
  
  gw.win_init(0,100,0);

  gw.set_node_shape (ellipse_node);
  gw.set_node_radius1(3.0); 
  gw.set_node_radius2(2.0); 
  gw.set_flush(false);

  init_graph_handler(gw);

  gw.display_help_text("preflow_push_anim");

  while(gw.edit())
  { do_anim = true;
    random_caps(0);
    start_maxflow(0); 
   }
  
  TL.close();
  
  return 0;
} 

 

