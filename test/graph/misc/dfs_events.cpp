/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  dfs_events.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/system/event.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;




class graph_explorer {
public :
  EVENT2<graph_explorer&,node> reach_node_event;
  EVENT2<graph_explorer&,node> finish_node_event;
  EVENT2<graph_explorer&,edge> touch_edge_event;
  EVENT1<graph_explorer&>      start_event;
  EVENT1<graph_explorer&>      finish_event;
};



class depth_first_search : public graph_explorer {

  void explore(node s, node_array<bool>& reached) 
  { reached[s] = true;
    reach_node_event(*this,s);
    edge e;
    forall_adj_edges(e,s) 
    { touch_edge_event(*this,e);
      node t = target(e);
      if (!reached[t]) explore(t,reached);
    }
    finish_node_event(*this,s);
  }
  
  public :

  void operator () (const graph& G) 
  { start_event(*this);
    node_array<bool> reached(G,false);
    node v;
    forall_nodes(v,G)
       if (!reached[v]) explore(v,reached);
    finish_event(*this);
   }
};


depth_first_search DFS;




class dfs_observer {

  node_array<int>& _dfsnum;
  node_array<int>& _compnum;
  
  int count1;
  int count2;
  
  event_item it[3];
  
  void on_start(graph_explorer&)               { count1 = count2 = 0;  }
  void on_reach_node(graph_explorer&, node v)  { _dfsnum[v] = count1++;  }
  void on_finish_node(graph_explorer&, node v) { _compnum[v] = count2++; }

public:
  
  dfs_observer(graph_explorer& exp, node_array<int>& dfsnum,
                                    node_array<int>& compnum) 
  : _dfsnum(dfsnum), _compnum(compnum)
  { 
    it[0] = attach(exp.reach_node_event,  *this, &on_reach_node); 
    it[1] = attach(exp.finish_node_event, *this, &on_finish_node); 
    it[2] = attach(exp.start_event,       *this, &on_start); 
   }
  
 ~dfs_observer() { detach(it,3); }

};




int main() {

  graph g;

  test_graph(g);

  node_array<int> dfsnum(g);
  node_array<int> compnum(g);
  dfs_observer obs(DFS,dfsnum,compnum);

  DFS(g);

  return 0;
}

