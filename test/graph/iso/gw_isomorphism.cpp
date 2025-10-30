/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_isomorphism.cpp
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/error.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/file_panel.h>
#include <LEDA/system/file.h>
#include <LEDA/graph/graph_morphism.h>

/*
#include "_binary_graph.h"
*/

#include "_binary_graph.cpp"


using namespace leda;


class GraphWinGDB : public GraphWin
{
  public:
  GraphWinGDB(graph& g, int width, int height, const char* win_label = "");
  static void load_graph_database_file(GraphWin& g);
  static void save_graph_database_file(GraphWin& g);
  static string filename;
};


string GraphWinGDB::filename;


enum ALGORITHM {
VF2 = 0, 
CONAUTO = 1, 
VF2_SIMPLE = 2, 
VF2_BETTER = 3, 
VF2_BEST = 4, 
VF2_EXP = 5, 
CONAUTO_BASIC = 6, 
CONAUTO_FAST = 7, 
LAST_ALGORITHM = 8
};


const string ALGORITHM_NAMES[LAST_ALGORITHM] = {
"vf2", 
"conauto", 
"vf2-simple", 
"vf2-better", 
"vf2-best", 
"vf2-exp", 
"conauto-basic", 
"conauto-fast"
};


enum MORPHISM {ISO, SUB, MONO, AUTO};

const char* names[] = {
"Graph Isomorphism", 
"Subgraph Isomorphism", 
"Graph Monomorphism", 
"Graph Automorphism"
};


int chosen = VF2;


GraphWinGDB::GraphWinGDB(graph& g, int width, int height, const char* win_label)
: GraphWin(g, width, height, win_label)
{
//add_simple_call(GraphWinGDB::load_graph_database_file, "gdb graph", 2);
//add_simple_call(GraphWinGDB::save_graph_database_file, "gdb graph", 3);

  int gdb_menu = add_menu("graph data base");
  add_simple_call(load_graph_database_file, "load gdb",gdb_menu);
  add_simple_call(save_graph_database_file, "save gdb",gdb_menu);
}


void load_gdb_graph(string _filename)
{
  GraphWinGDB::filename = _filename;
}


void GraphWinGDB::load_graph_database_file(GraphWin& gw)
{
  string fname = gw.get_graphname();
  string dname = gw.get_dirname();

  file_panel P(gw.get_window(), fname, dname);

  P.set_load_handler(load_gdb_graph);
  P.set_load_string("Load Graph from Graph Database File");
  P.set_pattern("Graph Database Files (*.A*)","*.A*");

  string dir = get_directory();

  P.open();

  gw.set_graphname(GraphWinGDB::filename);

  node_map<int> numbering(gw.get_graph());
  binary_graph::read_binary_leda_graph(GraphWinGDB::filename, 
                                             gw.get_graph(), numbering);
  gw.update_graph();
  gw_circle_embed(gw);
}



void GraphWinGDB::save_graph_database_file(GraphWin& gw)
{
  string fname = gw.get_graphname();
  string dname = gw.get_dirname();

  file_panel P(gw.get_window(), fname, dname);

  P.set_save_handler(load_gdb_graph);
  P.set_save_string("Load Graph from Graph Database File");
  P.set_pattern("Graph Database Files (*.A*)","*.A*");

  string dir = get_directory();

  P.open();

  gw.set_graphname(GraphWinGDB::filename);

  node_map<int> numbering(gw.get_graph());

  graph& g = gw.get_graph();
  node n;
  int i = 0;
  forall_nodes(n, g) numbering[n] = i++;
  binary_graph::write_binary_leda_graph(GraphWinGDB::filename, g, numbering);
}


void show_all_graph_morphisms(GraphWin& gw1, MORPHISM morphism)
{
  graph& g1 = gw1.get_graph(); 
  window& w1 = gw1.get_window();

  int x = w1.xpos() + w1.width() + 4;
  int y = w1.ypos() - 23;


  MORPHISM actual_morphism = morphism;

  graph g2;
  GraphWinGDB gw2(g2, 550, 600, names[morphism]);


  if(morphism != AUTO)
  { gw2.display(x,y);
    string msg = 
           "Please enter or load the second (small) graph and \n";
    msg += "press \"done\" when finished.";
    gw2.message(msg);
    gw2.edit();
    gw2.del_message();
   }
  else
   { CopyGraph(g2, g1);
     gw2.update_graph();
     gw2.display(x,y);
     actual_morphism = ISO;
    }

  graph_morphism_algorithm<>* alg = NULL;
  graph_morphism_algorithm<>::morphism_list morphisms;

  try
  {
    switch(chosen) {

    case VF2:        alg = new graph_morphism<graph, vf2<graph> >();
                     break;

    case VF2_SIMPLE: alg = new graph_morphism<graph, vf2_simple<graph> >();
                     break;

    case VF2_BETTER: alg = new graph_morphism<graph, vf2_better>();
                     break;

    case VF2_BEST:   alg = new graph_morphism<graph, vf2_best>();
                     break;

    case VF2_EXP:    alg = new graph_morphism<graph, vf2_exp<graph> >();
                     break;
 
    case CONAUTO:    alg = new graph_morphism<graph, conauto<graph> >();
                     break;

    case CONAUTO_BASIC: 
                     alg = new graph_morphism<graph, conauto_basic<graph> >();
                     break;

    case CONAUTO_FAST:
                     alg = new graph_morphism<graph, conauto_fast<graph> >();
                     break;

    default:         throw leda_exception("Algorithm not implemented.", 1);
                     break;
    }

   switch(actual_morphism) {

   case ISO:  alg->find_all_iso(g1, g2, morphisms);
              break;

   case SUB:  alg->find_all_sub(g1, g2, morphisms);
              break;

   case MONO: alg->find_all_mono(g1, g2, morphisms);
              break;

   case AUTO: // added (sn)
              break;
   }

  }
  catch(leda_exception& le)
  { delete alg;
    gw2.acknowledge(string("Morphism could not be computed: ") + le.get_msg());
    return;
   }

  delete alg;

  int k = morphisms.size();

  string msg;

  if (k == 0) msg = string("There is \bf no \rm %s.", names[morphism]);
  if (k == 1) msg = string("There is one %s.", names[morphism]);
  if (k >= 2) msg = string("There are %d %ss.", k, names[morphism]);

  panel alert_box;
  alert_box.text_item(msg);
  alert_box.button("ok");
  if (k > 0)
    alert_box.button("show");

  bool anim = true;
  panel go_ahead;
  go_ahead.text_item("Show next morphism?");
  go_ahead.bool_item("animation",anim);
  go_ahead.button("Abort",1);
  go_ahead.button("Next", 2);
  go_ahead.button("All",  3);

  int but = 0;

  if (alert_box.open(gw2.get_window()))
  { 
    //show morphisms
    node_array<point> pos1(g1), pos2(g2);
    edge_array<list<point> > bends2(g2);

    gw1.set_flush(false);
    gw1.save_all_attributes();
    gw1.get_position(pos1);

    graph_morphism_algorithm<>::morphism* m;
    forall(m, morphisms)
    { node n;
      forall_nodes(n, g2)
      { node n1 = m->first()[n];
        pos2[n] = pos1[n1];
        if(actual_morphism != ISO) gw1.set_color(n1, red);
       }
      edge e;
      forall_edges(e, g2)
      { edge e1 = m->second()[e];
        bends2[e] = gw1.get_bends(e1);
        if(actual_morphism != ISO) gw1.set_color(e1, red);
       }

      gw1.redraw();

      gw2.set_layout(pos2, bends2);
      if (!anim) gw2.redraw();
                        
      if (but == 3)
         leda_wait(0.3);
      else
        { but = go_ahead.open(gw2.get_window());
          gw2.set_flush(anim);
         }

      gw1.restore_all_attributes();
      if (but == 1) break;
    }

    forall(m, morphisms) delete m;
    gw1.set_flush(true);
    gw2.set_flush(true);
  }
}


void show_all_graph_isomorphism(GraphWin& gw1)
{ gw1.get_window().disable_panel();
  show_all_graph_morphisms(gw1, ISO); 
  gw1.get_window().enable_panel();
}


void show_all_subgraph_isomorphism(GraphWin& gw1)
{ gw1.get_window().disable_panel();
  show_all_graph_morphisms(gw1, SUB);
  gw1.get_window().enable_panel();
}


void show_all_graph_monomorphism(GraphWin& gw1)
{ gw1.get_window().disable_panel();
  show_all_graph_morphisms(gw1, MONO);
  gw1.get_window().enable_panel();
}


void show_all_graph_automorphism(GraphWin& gw1)
{ gw1.get_window().disable_panel();
  show_all_graph_morphisms(gw1, AUTO);
  gw1.get_window().enable_panel();
}


int main()
{
  graph G;
  GraphWinGDB gw(G, 680, 800, "LEDA Graph Morphism Demo");

  window& W = gw.get_window();

  list<string> algorithms;
  for(int i = 0; i < 2; i++) // only vf2 and conauto (s.n.)
  { string s = ALGORITHM_NAMES[i];
    algorithms.push_back(s);
   }

  W.choice_item("Algorithm", chosen, algorithms);


  gw.add_simple_call(show_all_graph_isomorphism,    "Graph-Iso");
  gw.add_simple_call(show_all_subgraph_isomorphism, "Subgraph-Iso");
  gw.add_simple_call(show_all_graph_monomorphism,   "Graph-Mono");
  gw.add_simple_call(show_all_graph_automorphism,   "Graph-Auto");

  string msg = "Please enter or load a graph and select an algorithm, \n"; 
  msg += "then click the button for the kind of morphism to be computed.";
  gw.message(msg);

  gw.display(window::min, window::center);

  set_error_handler(exception_error_handler);

  gw.edit();

  return 0;
}
