/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _strongcomp.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/




//------------------------------------------------------------------------------
// Strong Connected Components 
// new version (10/2008)
//------------------------------------------------------------------------------



#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/b_stack.h>

LEDA_BEGIN_NAMESPACE


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




#if defined(OLD_VERSION)

static void scc_dfs(const graph& G, node v, node_array<int>& compnum,
                                            node_array<int>& dfsnum,
                                            node_slist& unfinished,
                                            list<node>& roots,
                                            int& count1, int& count2 );


int STRONG_COMPONENTS(const graph& G, node_array<int>& compnum)
{
  // int STRONG_COMPONENTS(graph& G, node_array<int>& compnum)
  // computes strong connected components (scc) of digraph G
  // returns m = number of scc 
  // returns in node_array<int> compnum for each node an integer with
  // compnum[v] = compnum[w] iff v and w belong to the same scc
  // 0 <= compnum[v] <= m-1 for all nodes v

  list<node>     roots;
  node_slist     unfinished(G);
  node_array<int> dfsnum(G,-1);

  int count1 = 0; 
  int count2 = 0;

  node v;

  forall_nodes(v,G) 
      if (dfsnum[v] == -1) 
       scc_dfs(G,v,compnum,dfsnum,unfinished,roots,count1,count2);

  return count2;
}


static void scc_dfs(const graph& G, node v, node_array<int>& compnum,
                                            node_array<int>& dfsnum,
                                            node_slist& unfinished,
                                            list<node>& roots,
                                            int& count1, int& count2 )
{
  node w;

  dfsnum[v] = ++count1;
  unfinished.push(v);
  roots.push(v);

  forall_adj_nodes(w,v)
    { if (dfsnum[w]==-1) 
       scc_dfs(G,w,compnum,dfsnum,unfinished,roots,count1,count2);
      else 
       if (unfinished(w))
        while (dfsnum[roots.head()]>dfsnum[w])  roots.pop();
     }

  if (v==roots.head()) 
   { do { w=unfinished.pop();
          /* w is an element of the scc with root v */
          compnum[w] = count2;
         } while (v!=w);
     roots.pop(); 
     count2++;
    }
}

#endif


LEDA_END_NAMESPACE

