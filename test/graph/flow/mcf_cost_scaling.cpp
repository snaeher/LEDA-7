#define LEDA_CHECKING_OFF


#include <LEDA/graph/graph.h>
#include <LEDA/graph/static_fgraph.h>
#include <LEDA/graph/dimacs_graph.h>


#include <LEDA/graph/node_slot1.h>
#include <LEDA/graph/edge_slot1.h>

#define MCF_STATIC_GRAPH
#include <LEDA/graph/templates/mcf_cost_scaling.h>

#include <stdlib.h>

using namespace leda;

using std::cin;
using std::cout;
using std::cerr;
using std::flush;
using std::endl;

int main(int argc, char** argv)
{

  typedef mcf_cost_scaling<int,double,double>::node_data nd_type;
  typedef mcf_cost_scaling<int,double,double>::edge_data ed_type;

  const int ns = sizeof(nd_type)/4;
  const int es = sizeof(ed_type)/4;

  typedef static_fgraph<opposite_graph,data_slots<ns>,data_slots<es+3> > st_graph;
  //typedef st_graph::node node;
  //typedef st_graph::edge edge;

  typedef node_slot1<nd_type,st_graph,0> node_data_array;
  typedef edge_slot1<ed_type,st_graph,0> edge_data_array;

  st_graph G;
  node_array<int,st_graph>  supply(G);
  edge_array<int,st_graph>  lcap(G);

  edge_slot1<int,st_graph,es+0> flow(G);
  edge_slot1<int,st_graph,es+1> ucap(G);
  edge_slot1<int,st_graph,es+2> cost(G);

  dimacs_mcf D(cin);
  D.translate(G,supply,lcap,ucap,cost);

/*
  edge e;
  forall_edges(e,G) cost[e] = 0;
*/

  double n = G.number_of_nodes();
  double m = G.number_of_edges();

  cout << endl;
  cout << string("%6.0f nodes",n) << endl;
  cout << string("%6.0f edges",m) << endl;
  cout << endl;
  cout << "node slots: " << ns << endl;
  cout << "edge slots: " << es << endl;
  cout << endl;

  mcf_cost_scaling<int,double,double, st_graph, node_data_array, 
                                                edge_data_array> mcf;

  bool do_check = false;
  bool dimacs_statistics = false;

  int i = 1;
  while (i < argc)
  { string s = argv[i];
    if (s[0] != '-') break;
    if (s == "-l") mcf.set_write_log(true);
    if (s == "-c") do_check = true;
    if (s == "-s") dimacs_statistics = true;
    i++;
   }

  if (i < argc) { 
    int x = atoi(argv[i++]); 
    if (x != 0) mcf.set_scale_factor(x); 
   }

  if (i < argc) { 
    double x = atof(argv[i++]) * std::sqrt((double)G.number_of_nodes()); 
    if (x != 0) mcf.set_af_factor(x);
   }

  if (i < argc) { 
    double x = atof(argv[i++]); 
    if (x != 0) mcf.set_af_update(x);
   }

  if (i < argc) { 
    double x = atof(argv[i++]); 
    if (x != 0) mcf.set_eps_factor(x);
   }


  if (dimacs_statistics)
  { D.print_statistics(cout);
    cout << endl;
   }
  
  bool feasible = mcf.run(G,lcap,ucap,cost,supply,flow) ;

  double c = mcf.total_cost(G,cost,flow);

  if (feasible)
  { //cout << string("time: %6.2f cost: %14.0f  a = %d  af = %.0f (%.0f)  rf = %d  rs = %d",
    cout << string("time: %6.2f  cost: %12.0f  a = %d  af = %.0f (%.0f)  rf = %d  rs = %d",
                                            mcf.cpu_time(), 
                                            c,
                                            mcf.scale_factor(),
                                            mcf.af(), 
                                            mcf.af_start(), 
                                            mcf.refines(),
                                            mcf.restarts()) << endl;
     mcf.statistics(cout);

     if (do_check)
     { float T = used_time();
       string msg;
       bool b = mcf.check(G,lcap,ucap,cost,supply,flow,msg);
       cout << string("checking time: %.2f sec",used_time(T)) << endl;
       if (!b) 
       { cerr << "mcf::check failed: " + msg << endl;
         return 2;
        }
      }
     return 0;
   }

  cout << endl;
  cout << "Infeasible MCF-Problem." << endl;
  cout << endl;

  return 1;
}

