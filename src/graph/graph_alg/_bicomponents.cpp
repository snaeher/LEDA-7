/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _bicomponents.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------------
// BICONNECTED COMPONENTS                                                      
//                                                                              
// last modified: 
//
// March 98  (isolated nodes)
//
//------------------------------------------------------------------------------


#include <LEDA/graph/graph_alg.h>
#include <LEDA/core/stack.h>

LEDA_BEGIN_NAMESPACE

static void bcc_dfs(const graph& G,node v,edge_array<int>& compnum,
                    node_array<int>& dfsnum,node_array<int>& lowpt,
                    node_array<node>& father,stack<node>& current,
                    int& count1,int& count2);


int BICONNECTED_COMPONENTS(const graph& G, edge_array<int>& compnum)
{
  // computes the biconnected components of the underlying  undirected
  // graph,  returns m = number of biconnected components and 
  // in edge_array<int> compnum for each edge an integer with
  // compnum[x] = compnum[y] iff edges x and y belong to the same component 
  // and 0 <= compnum[e] <= m-1 for all edges e
  // running time : O(|V|+|E|)
  //
  // (problem  with self-loops ? )

  stack<node> current;
  node_array<int> dfsnum(G,-1);
  node_array<int> lowpt(G,0);
  node_array<node> father(G,nil);
  int count1 = 0; 
  int count2 = 0;
  int num_isolated = 0;
  node v;

  forall_nodes(v,G)
   if (dfsnum[v] == -1)
    { dfsnum[v] = ++count1;
      bool is_isolated = true;

      edge e;
      list<edge> L = G.adj_edges(v);
      if (G.is_directed()) {
         forall_in_edges(e,v) L.append(e);
      }

      //forall_adj_undirected_edges(e,v)
      forall(e,L)
        if ( G.opposite(v,e) != v ) { is_isolated = false; break; }

      if ( is_isolated ) 
	{ num_isolated++; }
      else
	{ current.push(v);
          bcc_dfs(G,v,compnum,dfsnum,lowpt,father,current,count1,count2);
          current.pop();
        }
     }

  return(count2 + num_isolated);

 } // BI_COMPONENTS



static void bcc_dfs(const graph& G, node v, edge_array<int>& compnum,
                                            node_array<int>& dfsnum,
                                            node_array<int>& lowpt,
                                            node_array<node>& father,
                                            stack<node>& current,
                                            int& count1, int& count2)
 {

  lowpt[v] = dfsnum[v];

   edge e;
   //forall_adj_undirected_edges(e,v)
   list<edge> L = G.adj_edges(v);
   if (G.is_directed()) {
       forall_in_edges(e,v) L.append(e);
   }

   forall(e,L)
   { node w = G.opposite(v,e);
     if (dfsnum[w] == -1)
     { dfsnum[w] = ++count1;
       current.push(w);
       father[w] = v;
       bcc_dfs(G,w,compnum,dfsnum,lowpt,father,current,count1,count2);
       lowpt[v] = leda_min(lowpt[v],lowpt[w]);
      }
     else
       lowpt[v] = leda_min(lowpt[v],dfsnum[w]);
    }

  if (father[v] && (lowpt[v] == dfsnum[father[v]]))
  { node w;
    do { w = current.pop();
         edge e;
         //forall_adj_undirected_edges(e,w)
         list<edge> L = G.adj_edges(w);
         if (G.is_directed()) {
            forall_in_edges(e,w) L.append(e);
         }

         forall(e,L)
            if (dfsnum[w] > dfsnum[G.opposite(w,e)]) compnum[e] = count2;

        } while (w != v);

    count2++;
  }

 } // bcc_dfs


LEDA_END_NAMESPACE
