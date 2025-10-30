/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  MWM-demo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/grpahs/graph_alg.h>
#include <LEDA/graph/graph_gen.h>
#include <LEDA/system/file.h>
#include <LEDA/graphics/window.h>

#include <LEDA/graph/templates/mw_matching.h>

#include <stdio.h>

using namespace leda;


int    n, m, low, high, heur;             
bool   save, max, perfect, stat_inf, check;
string file_name;


void read_weighted_graph(graph &G, edge_array<int> &w, 
			 string fname) {

  // reads the graph and weight infos from a file
  // with the following file format:
  //   n m
  //   s1 t1 w1
  //   s2 t2 w2
  //   ...
  // where:
  //   - n is the number of nodes of G, 
  //   - m is the numner of edges of G,
  //   - other lines give source, target and weight information
  //     for all edges of G


  ifstream f(fname);
  if (!f)
    error_handler(1, "void read_weighted_graph(...): cannot access file " + fname );

  G.clear();
  cout << "READ weighted graph file: " << fname << "..." << flush;

  int n, m;
  f >> n;
  f >> m;

  array<node> node_of(n);
  array<edge> edge_of(m);
  array<int>  weight_of(m);

  for (int i = 0; i < n; i++)
    node_of[i] = G.new_node();

  int s, t;
  int weight;

  for(int cnt = 0; cnt < m; cnt++) {
    f >> s;
    f >> t;
    f >> weight;
    edge_of[cnt]   = G.new_edge(node_of[s], node_of[t]);
    weight_of[cnt] = weight;
  }

  w.init(G);

  for (int i = 0; i < m; i++)
    w[edge_of[i]] = weight_of[i];

  f.close();
  cout << "ready! n = " << n << " m = " << m << endl; 
}


void write_weighted_graph(graph &G, edge_array<int> &w, 
		      	  string fname, bool neg = false) {

  // writes G to the file fname
  // using the following file format:
  //   n m
  //   s1 t1 w1
  //   s2 t2 w2
  //   ...
  // where:
  //   - n is the number of nodes of G, 
  //   - m is the numner of edges of G,
  //   - other lines give source, target and weight information
  //     for all edges of G

  ofstream f(fname);
  if (!f)
    error_handler(1, "void write_weighted_graph(...): cannot access file " + fname );

  cout << "WRITE weighted graph file: " << fname << "..."; cout.flush();

  f << G.number_of_nodes() << " ";
  f << G.number_of_edges() << endl;

  int sign = (neg ? -1 : 1);
  edge e;
  forall_edges(e, G) 
    f << index(source(e)) << " " << index(target(e)) << " " << sign*w[e] << endl;

  f.close();
  cout << "ready!" << endl;
}


void create_random_weights(graph &G, edge_array<int> &w, 
			   int low, int high) {

  cout << "CREATE random weights out of range [" << low 
       << ",...," << high << "]..." << flush;

  w.init(G);

  edge e;
  forall_edges(e, G) 
    w[e] = rand_int(low, high);  // 4*rand_int(low/4, high/4);

  cout << "done!" << endl;
}


void create_random_graph(graph &G, edge_array<int> &w, 
			 int n, int m, int low, int high,
			 string fname) {

  cout << "CREATE graph with n = " << n 
       << " and m = " << m << "..."; cout.flush();
  random_graph(G, n, m, 1, 1, 1);
  cout << "ready!" << endl;
  
  create_random_weights(G, w, low, high);

  if (save) write_weighted_graph(G, w, fname);
}


void give_statistics() {

#ifdef _STAT
  printf("\n-----------------------------------------------------\n");
  printf("%-14s %7.2f sec.\n", "INIT:",      init_t);
  if (heur) 
  printf("%-14s %7.2f sec.\n", "HEURISTIC:", heur_t);
  printf("%-14s %7.2f sec.\n", "MATCHING:",  alg_t);
  printf("%-14s %7.2f sec.\n", "EXTRACT:",   extract_t);
  if (check)
  printf("%-14s %7.2f sec.\n", "CHECKER:",   check_t);
  printf("-----------------------------------------------------\n");
  printf("%-14s %7d\n", "ADJUSTMENTS:",  adj_c);
  printf("%-14s %7d %7.2f sec. (avg. %3.2f msec.)\n", "SCAN:",         scan_c, scan_t, scan_t/scan_c*1000);
  printf("%-14s %7d %7.2f sec. (avg. %3.2f msec.)\n", "GROW:",         grow_c, grow_t, grow_t/grow_c*1000);
  printf("%-14s %7d %7.2f sec. (avg. %3.2f msec.)\n", "SHRINK:",       shrink_c, shrink_t, shrink_t/shrink_c*1000);
  printf("%-14s %7d %7.2f sec. (avg. %3.2f msec.)\n", "EXPAND:",       expand_c, expand_t, expand_t/expand_c*1000);
  if(!perfect)
  printf("%-14s %7d %7.2f sec. (avg. %3.2f msec.)\n", "ALTERNATE:",    alternate_c, alternate_t, alternate_t/alternate_c*1000);
  printf("%-14s %7d %7.2f sec. (avg. %3.2f msec.)\n", "AUGMENT:",      augment_c, augment_t, augment_t/augment_c*1000);
  printf("%-14s %7d %7.2f sec. (avg. %3.2f msec.)\n", "DESTROY TREE:", destroy_c, destroy_t, destroy_t/destroy_c*1000);
  printf("-----------------------------------------------------\n");
  printf("TOTAL TIME (without checking): %7.2f sec.\n\n", total_t);
#endif
}


void parameters_panel(panel &P) {

  list<string> M1 = get_files(".", "*.wg");
  P.load_button_font("lucidasans-12");

  P.text_item("");
  P.text_item("\\bf\\blue Parameters for Random Instances");
  P.int_item("n", n);
  P.int_item("m", m);
  P.int_item("low", low);
  P.int_item("high", high);
  P.bool_item("write graph", save);
  P.string_item("file name", file_name, M1, 8);

  P.text_item("");
  P.text_item("\\bf\\blue Algorithm Parameters");
  P.bool_item("maximum matching", max);
  P.bool_item("perfect matching", perfect);
  P.bool_item("check optimality", check);
  P.bool_item("statistics", stat_inf);
  P.choice_item("heuristic", heur, "none", "greedy", "jump start");
}


template <class NT>
NT compute_total_weight(const list<edge> &M, const edge_array<NT> &w) {

  NT W = 0;
  edge e;
  forall(e, M) { W += w[e]; }
  return W;
}

void call_func(GRAPH<int, int> &G) {

  list<edge> M;

  if (perfect) 
    if (max)
      M = MAX_WEIGHT_PERFECT_MATCHING_T(G, G.edge_data(), check, heur);
    else 
      M = MIN_WEIGHT_PERFECT_MATCHING_T(G, G.edge_data(), check, heur);
  else 
    if (max)
      M = MAX_WEIGHT_MATCHING_T(G, G.edge_data(), check, heur);
  
  cout << "Matching has cardinality " << M.length() << " and total weight " 
       << compute_total_weight(M, G.edge_data()) << endl;
  if (stat_inf) give_statistics();
  else cout << "---------------------------------------------------------------" << endl;

}




int main() {

  GRAPH<int, int> G;
  list<edge> M;

  system("clear");

  n = 10000; m = 80000; low = 1; high = 1000; heur = 2;      
  max = true; perfect = true; stat_inf = true; check = true;
  file_name = "test.wg";

  panel P("WEIGHTED MATCHING - DEMO");
  parameters_panel(P);

  P.buttons_per_line(2);
  P.button("Test on Ramdom Graph", 1);
  P.button("Read and Test Graph", 2);
  P.button("Exit", 3);

  P.display(window::min, window::min);

  char but;

  do {
    but = P.read_mouse();

    if (!max && !perfect) {
      window MSG(420, 80, "Error");
      MSG.acknowledge("Computation of minimum-weight matching supported for perfect case only!");      
    }
    else
      if (but == 1) {
        create_random_graph(G, G.edge_data(), n, m, low, high, file_name);      
        call_func(G);
        G.clear();
      }
      else if (but == 2) {
	read_weighted_graph(G, G.edge_data(), file_name);
	call_func(G);
	G.clear();
      }
  } while (but != 3);
  
  P.close();
  
}

