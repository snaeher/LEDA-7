/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_scc_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/b_stack.h>

using namespace leda;


static void scc_dfs(const graph& G, node v, node_array<int>& compnum,
                                            b_stack<node>& unfinished,
                                            b_stack<int>& roots,
                                            int& count1, int& count2 )
{ int cv = --count1;
  compnum[v] = cv;
  unfinished.push(v);
  roots.push(cv);

  edge e;
  forall_out_edges(e,v)
  { node w =G.target(e);
    int cw = compnum[w];
    if (cw == -1)
      scc_dfs(G,w,compnum,unfinished,roots,count1,count2);
    else
      if (cw < -1)
         while (roots.top() < cw)  roots.pop();
   }

  if (roots.top() == cv)
  { node u;
    do { u = unfinished.pop(); // u is a node of the scc with root v
         compnum[u] = count2;
        } while (v != u);
    roots.pop();
    count2++;
   }
}


int STRONG_COMPONENTS(const graph& G, node_array<int>& compnum)
{
  int n = G.number_of_nodes();

  b_stack<int>  roots(n);
  b_stack<node> unfinished(n);

  int count1 = -1;
  int count2 = 0;

  node v;
  forall_nodes(v,G) compnum[v] = -1;

  forall_nodes(v,G)
    if (compnum[v] == -1)
       scc_dfs(G,v,compnum,unfinished,roots,count1,count2);

  return count2;
}



static color text_color(color col)
{ if (col==black || col==red || col==blue || col==violet || col==brown 
      || col==pink || col==blue2 || col==grey3) 
     return white;
  else
     return black;
}



void display_scc(GraphWin& gw)
{ graph& G = gw.get_graph();
  node_array<int> comp_num(G);
  STRONG_COMPONENTS(G,comp_num);
  bool f = gw.set_flush(false);
  node v;
  forall_nodes(v,G) 
  { gw.set_color(v,color::get(comp_num[v] % 16));  
    gw.set_user_label(v,string("%d",comp_num[v]));
    gw.set_label_type(v,user_label);
  }
  gw.redraw();
  gw.set_flush(f);
}

void new_edge_handler(GraphWin& gw, edge)  { display_scc(gw); }
void new_node_handler(GraphWin& gw, node)  { display_scc(gw); }
 

static int  small_width = 20;
static int  large_width = 36;
static int  color_shift = 5;

static bool step = true;

void message(GraphWin& gw, string msg)
{ msg += "\\5 \\blue press done \\black"; 
  if (step && !gw.wait(msg)) step = false;
}


static window state_win(320,60,"State Of The Algorithm");

static void state_redraw(window* wp) { wp->flush_buffer(); }


void state_info(GraphWin& gw, const list<node>& unfinished, 
                              const list<node>& roots,
                              const node_array<int>& dfsnum, node cur_v)
{ 
  if (!state_win.is_open())
  { state_win.set_bg_color(grey1);
    state_win.set_redraw(state_redraw);
    state_win.display(-gw.get_window().xpos()+8,0);
    state_win.init(0,320,0);
    state_win.start_buffering();
   }

  state_win.clear();

  double th = state_win.text_height("H");

  double x0 = state_win.text_width("Unfinished") + 2*th;
  double y1 = state_win.ymax() - 1.75*th;
  double y2 = state_win.ymax() - 3.20*th;

  double d = 18;

  state_win.set_bold_font();
  state_win.draw_text(5,y1+(d+th)/2,"Unfinished");
  state_win.draw_text(5,y2+(d+th)/2,"Roots");
  state_win.set_text_font();


  list_item r_it = roots.first();

  double x = x0;

  list_item u_it;
  forall_items(u_it, unfinished)
  { node v = unfinished[u_it];
    color col = gw.get_color(v);
    int   dn  = dfsnum[v];

    state_win.draw_box(x,y1,x+d,y1+d,col);
    state_win.draw_rectangle(x,y1,x+d,y1+d,black);
    state_win.draw_ctext(x+d/2,y1+d/2,string("%d",dn),text_color(col));

    if ( v == roots[r_it] )
    { state_win.draw_box(x,y2,x+d,y2+d,col);
      state_win.draw_rectangle(x,y2,x+d,y2+d,black);
      state_win.draw_ctext(x+d/2,y2+d/2,string("%d",dn),text_color(col));
      r_it = roots.succ(r_it);
     }
    else
      state_win.draw_box(x+1,y2,x+d,y2+d,white);
      

    x += d;
   }

  if (x > x0)
  { state_win.draw_rectangle(x0,y1,x,y1+d,black);
    state_win.draw_rectangle(x0,y2,x,y2+d,black);
   }

  state_win.flush_buffer();
}





void SCC_DFS(node v, const graph& G, node_array<int>& dfsnum,
             node_array<int>& comp_num, list<node>& unfinished,
             list<node>& roots, int& dfscount, int& comp_count, GraphWin& gw,
             const node_array<point>& perm_pos)
{ 
gw.select(v);

dfsnum[v] = dfscount++;
unfinished.push(v); 
roots.push(v); 

color clr = color::get((color_shift + roots.size()) % 16);
gw.set_color(v,clr);

gw.set_user_label(v,string("%d",dfsnum[v]));
gw.set_shape(v,rectangle_node); 
gw.set_width(v,large_width);
 
string msg;
msg += "A new node has been reached.\\n ";
msg += "It got the dfs-number ";
msg += string("%d ",dfsnum[v]);
msg += "and it is the new current node. ";
msg += "It is the root of a new tentative component";

state_info(gw,unfinished,roots,dfsnum,v);
message(gw,msg);


node w; edge e;
forall_adj_edges(e,v)
{ w = G.target(e);

  gw.set_style(e,solid_edge);
  gw.set_color(e,red); 

  string msg = "I am exploring the red edge.\\n ";

  if (dfsnum[w] == - 1)  
   { 
     msg += "It's a tree edge and I am making a recursive call."; 
     message(gw,msg);
     state_info(gw,unfinished,roots,dfsnum,v);

gw.deselect(v);
gw.set_color(e,blue); 
//gw.set_width(e,3);
gw.set_width(e,2);
       
SCC_DFS(w,G,dfsnum,comp_num,unfinished,roots,dfscount,
                                         comp_count,gw, perm_pos); 
 
gw.set_width(e,1);
gw.set_color(e,black);
gw.select(v);


state_info(gw,unfinished,roots,dfsnum,0);

message(gw,"I returned from a recursive call. The node with number " 
              + string("%d ",dfsnum[v]) + " got reactivated");


                                       }
  else if (comp_num[w] == - 1) 
        { 

msg += "It's a non-tree edge into a tentative component. This edge may merge \
        several components into one. More precisely: all components whose \
        root is larger than " + string("%d ",dfsnum[w]);
msg += "cease to exist and are merged into the component containing the \
        node with dfs-number " + string("%d. ",dfsnum[w]);
msg += "Algorithmically, this amounts to removing all roots larger \
        than " + string("%d ",dfsnum[w]);
msg += "from the stack of roots. I do so one by one. Removal of a node from \
        the stack of roots turns its shape from rectangular to circular.";

message(gw,msg);
state_info(gw,unfinished,roots,dfsnum,v);


while (dfsnum[roots.head()] > dfsnum[w])
{ node z = roots.pop();
  gw.set_shape(z,circle_node);  
  gw.set_width(z,small_width);
  state_info(gw,unfinished,roots,dfsnum,v);
  leda_wait(0.25);  
}


node u;
forall(u,unfinished) {
  if (dfsnum[u] >= dfsnum[roots.head()] ) 
  { color clr = color::get((color_shift + roots.size()) % 16);
    gw.set_color(v,clr);
   }
}

state_info(gw,unfinished,roots,dfsnum,0);
message(gw,string("Now all roots are removed and the newly formed\
                   component has been recolored. The current node\
                   is still: %d.", dfsnum[v]));


 gw.set_color(e,black); 
   }
      else 
        { 

msg += "It's a non-tree edge into a permanent component. I do nothing.";

message(gw,msg);
state_info(gw,unfinished,roots,dfsnum,v);

gw.set_color(e,black); 

                                                     } 
}

if (v == roots.head())
{ 

string msg = "Node " + string("%d",dfsnum[v]) + " has been completed.";
msg += "It is a root and hence we have identified a permanent component. ";
msg += "The permanent component consists of all nodes  in unfinished whose ";
msg += "dfs-number is at least as large as " + string("%d",dfsnum[v]) + ".";
msg += "\\n I move all nodes in the component to the left and indicate ";
msg += "their dfs-number and their component number.";

state_info(gw,unfinished,roots,dfsnum,0);
message(gw,msg);

do { w = unfinished.pop(); 
     if (v == w) roots.pop();
     comp_num[w] = comp_count;
     gw.set_shape(w,rectangle_node);
     gw.set_width(w,large_width);
     gw.set_color(w,color::get(comp_count % 16));
     gw.set_user_label(w,string("%d|%d",dfsnum[w],comp_num[w]));
     state_info(gw,unfinished,roots,dfsnum,0);
     gw.set_position(w,perm_pos[w]);
} while ( w != v); 

comp_count++; 
}

gw.deselect(v);

}


int STRONG_COMPONENTS(const graph& G, node_array<int>& comp_num, 
                GraphWin& gw, const node_array<point>& perm_pos)
{ list<node> unfinished; 
  list<node> roots; 
  node_array<int> dfsnum(G,-1); 
  node v; 
  forall_nodes(v,G) comp_num[v] = -1; 
  int dfscount = 0;
  int comp_count = 0;
  forall_nodes(v,G)
  if (dfsnum[v] == -1) 
  { SCC_DFS(v,G,dfsnum,comp_num,unfinished,roots,dfscount,
                                        comp_count,gw,perm_pos);
    message(gw,"This was a return from an outermost call\
                I am looking for an unreached node and \
                and start a new search.");
  }

  return comp_count;
}



int main() {
  
GraphWin gw("SCC Animation Demo");

gw.set_graphname("scc");

gw.set_directed(true);

gw.set_init_graph_handler(display_scc);    
gw.set_del_edge_handler(display_scc);
gw.set_del_node_handler(display_scc);
gw.set_new_node_handler(new_node_handler);
gw.set_new_edge_handler(new_edge_handler);

gw.add_help_text("gw_scc_anim1");
gw.add_help_text("gw_scc_anim2");
gw.add_help_text("gw_scc_anim3");
gw.add_help_text("gw_scc_anim4");

gw.display();

/*
int h_menu = gw.get_menu("Help");

gw.add_simple_call(about_scc_anim1,       "About SCC: phase 1", h_menu);
gw.add_simple_call(about_scc_anim2,       "About SCC: phase 2", h_menu);
gw.add_simple_call(about_scc_anim_core, "About SCC: core",  h_menu);
gw.add_simple_call(about_scc_anim_data_structures, 
                                     "About SCC: data structures",h_menu);
about_scc_anim1(gw);
*/

gw.display_help_text("gw_scc_anim1");

gw.message("\\blue Construct or load a graph and press done.");
leda_wait(1.75);
gw.message("");

gw.set_flush(true);

gw.edit();


gw.disable_calls();

// about_scc_anim2(gw);

gw.display_help_text("gw_scc_anim2");

//gw.remove_bends();

graph&  G = gw.get_graph();
window& W = gw.get_window();

node_array<point> perm_pos(G);

double xmin = gw.get_xmin() + W.pix_to_real(20);
double xmax = gw.get_xmax() - W.pix_to_real(20);
double ymin = gw.get_ymin() + W.pix_to_real(30);
double ymax = gw.get_ymax() - W.pix_to_real(20);

double dx = xmax - xmin;
double dy = ymax - ymin;

gw.place_into_box(xmin+dx/2,ymin,xmax,ymax-dy/5);

gw.set_flush(false);

node v; 
forall_nodes(v,G) 
{ gw.set_color(v,white);
  gw.set_label_type(v,user_label); 
  gw.set_user_label(v,"");
  gw.set_shape(v,circle_node);
  gw.set_node_width(small_width);
  double xcoord = gw.get_position(v).xcoord();
  double ycoord = gw.get_position(v).ycoord();
  perm_pos[v] = point(xcoord-dx/2,ycoord);
}

edge e;
forall_edges(e,G) gw.set_style(e,dashed_edge);

gw.redraw();
gw.set_flush(true);


message(gw,"I use a split design for the window.\\n\
            \\blue Tentative \\black components are shown in the right half\
            of the screen and \\blue permanent \\black components are shown\
            in the left half. Right now nothing is permanent and therefore\
            I moved the graph to the right.");

node_array<int> comp_num(G);

STRONG_COMPONENTS(G,comp_num,gw,perm_pos);

gw.message("\\bf Wasn\'t this a nice demo ?");
leda_wait(1);
gw.message("");

gw.fill_window();

gw.enable_calls();
gw.edit();

return 0;

}

