/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _stnumb.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//-----------------------------------------------------------------------------
//
// ST_NUMBERING(const graph& G, node_array<int>& st_num, list<node>& st_list)
//
// computing an st-numbering of a biconnected graph G
//
// R. Hesse, E. Kalliwoda, S. Naeher  (1996)
//
//-----------------------------------------------------------------------------

#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/node_slist.h>

LEDA_BEGIN_NAMESPACE


static void ST_DFS(const graph& G, node v, int& count, 
                                                 node_array<int>&  dfsnum, 
                                                 node_array<int>&  lownum, 
                                                 node_array<edge>& tree_in,
                                                 node_array<edge>& low_out)
{ dfsnum[v] = count;
  lownum[v] = count;
  count++;
  
  edge e;
  forall_inout_edges(e,v)
  { 
    node w = G.opposite(v,e);

    if (v == w) continue;
  
    if (dfsnum[w] == 0 ) // first visit
      { tree_in[w] = e;
        ST_DFS(G,w,count,dfsnum,lownum,tree_in,low_out);
        if (lownum[w] < lownum[v]) 
        { lownum[v] = lownum[w]; 
          low_out[v] = e;
         }
       }
    else 
      if (dfsnum[w] < lownum[v]) 
      { lownum[v] = dfsnum[w]; 
        low_out[v] = e;
       }
   }
 }



static void FINDPATH(const graph& G, node w, node_array<bool>& used_node,
                                             edge_array<bool>& used_edge,
                                             node_array<edge>& pred_edge,
                                             node_slist& PATH)
{ while ( !used_node[w] )
  { edge x = pred_edge[w];  
    PATH.push(w);
    used_node[w] = true; 
    used_edge[x] = true;
    w = G.opposite(w,x);
   } 
}
  


node ST_NUMBERING(const graph& G, node_array<int>& st_num, list<node>& st_list,
                                                           edge e_st) 
{

//if (!Is_Biconnected(G)) 
//  LEDA_EXCEPTION(1,"st_numbering: Graph must be biconnected.");

  st_list.clear();

  int n = G.number_of_nodes();

  if (n == 0) return nil;

  node s;
  node t;

  node_array<int>  dfsnum(G,0);
  node_array<int>  lownum(G,0);
  node_array<edge> tree_in(G,nil);
  node_array<edge> low_out(G,nil);

  if ( e_st != nil ) 
    { s = G.source(e_st); 
      t = G.target(e_st); 
     } 
  else
    { t = G.first_node();
      if ( n == 1) 
      { st_num[t] = 1; 
        st_list.append(t);
        return t;
       }
      edge e = nil;
      forall_inout_edges(e,t)
         if (source(e) != target(e)) break;

      e_st = e;
      s = G.opposite(t,e);
    } 

  tree_in[t] = e_st;  // dummy
  
  // int count = 1;
  // ST_DFS(G,t,count,dfsnum,lownum,tree_in,low_out);
  //
  // force e_st to be the first edge visited by ST_DFS(t)

  dfsnum[t] = 1;
  lownum[t] = 1;
  tree_in[s] = e_st;
  int count = 2;

  ST_DFS(G,s,count,dfsnum,lownum,tree_in,low_out);

  edge_array<bool> used_edge(G,false);
  node_array<bool> used_node(G,false);

  used_node[s] = true;
  used_node[t] = true;
  used_edge[e_st] = true;
  
  st_list.push(t);
  st_list.push(s);

  int st_count = 1;

  node_slist P(G);

  for (list_item top = st_list.first(); top; top = st_list.succ(top))
  { node v = st_list[top];
    st_num[v] = st_count++;

    edge e;
    forall_inout_edges(e,v)
    { 
      if (used_edge[e]) continue;
      used_edge[e] = true;

      node w = G.opposite(v,e);

      if (v == w) continue;

      // node_slist P(G);  // sn: since this takes linear time now
                           //     we move P outside of the loop

      if (tree_in[w] == e)          // unused tree edge (v,w)
         FINDPATH(G,w,used_node,used_edge,low_out,P);
      else 
         if (dfsnum[v] < dfsnum[w]) // unused back edge (w,v)
            FINDPATH(G,w,used_node,used_edge,tree_in,P);


      // forall(w,P)  st_list.insert(w,top); 

      // sn: since P is global we have to make it empty

      while (!P.empty()) st_list.insert(P.pop(),top);
    }
  }

 return s;

}



/*
static void check_stnum(const graph& G, node_array<int>& st_num, 
                                        list<node>& st_list) 
{
  G.print();
  cout << endl;

  node s = st_list.head();
  node t = st_list.tail();
  
  int count = 0;
  
  node v;
  cout << endl;
  forall(v,st_list)
  { G.print_node(v);
    if (st_num[v] != ++count) LEDA_EXCEPTION(1,"error1 in stnumb");
   }
  
  forall_nodes(v,G)
  { if (v == s || v == t) continue;
    bool b1 = false;
    bool b2 = false;
    edge e;
    forall_inout_edges(e,v)
    { node w = G.opposite(v,e);
      if (st_num[w] > st_num[v]) b1 = true;
      if (st_num[w] < st_num[v]) b2 = true;
     }
 
    if (!(b1 && b2)) LEDA_EXCEPTION(1,"error2 in stnumb");
   }
  
  bool b = false;
  edge e;
  forall_inout_edges(e,s)
  { node w = G.opposite(s,e);
    if (w == t) b = true;
   }
  
  if (!b) 
  { G.print_node(s);
    G.print_node(t);
    LEDA_EXCEPTION(1,"error3 in stnumb");
   }
  
}
*/

LEDA_END_NAMESPACE
