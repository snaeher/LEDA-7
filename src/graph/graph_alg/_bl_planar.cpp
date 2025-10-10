/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _bl_planar.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/pq_tree.h>
#include <LEDA/core/array.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

static void LMDFS(graph& G, node v, 
                          const node_array<list<edge> >& L_n,
                          node_array<int>& st_num, 
                          list<edge>& embed_list)
{ 
  if (st_num[v] < 0) return;
 
  st_num[v] = -1;

  edge e;
  forall(e,L_n[v])
  { embed_list.push(G.reversal(e));
    LMDFS(G,target(e),L_n,st_num,embed_list);
  }
}


static void embedding(graph& G, node t, 
                      node_array<int>& st_num,
                      node_array<list<edge> >& L_n)
{
  list<edge> embed_list;

  node v; edge e;
  forall_nodes(v,G)
    forall(e,L_n[v]) embed_list.append(e);  

  LMDFS(G,t,L_n,st_num,embed_list);

  // append self-loops at the end of the list

  edge_map<bool> treated(G,false);
  forall_nodes(v,G)
  { edge e;
    forall_adj_edges(e,v) 
      if (target(e) == v && !treated[e]) 
      { embed_list.append(e); embed_list.append(G.reversal(e));
        treated[e] = treated[G.reversal(e)] = true;
      }
   }

  G.sort_edges(embed_list);
}


static void KURATOWSKI_SIMPLE(graph& G, list<edge>& K)
{ K.clear();

  if ( BL_PLANAR(G,false) )
    LEDA_EXCEPTION(1,"KURATOWSKI_SIMPLE: G is planar");

  list<edge> L = G.all_edges();
  edge e;
  forall(e,L) 
  { G.hide_edge(e);
    if (BL_PLANAR(G,false)) 
    { G.restore_edge(e);
      K.append(e);
    }
  }
  G.restore_all_edges();
}


enum { K_PLUS_1 = 0, OTHERS = 1};

ostream& operator<<(ostream& o, const node_array<node>&)
{ return o;}
istream& operator>>(istream& i, node_array<node>&)
{ return i;}


void DFS(const graph& G, node v, 
         list<node>& dfs_list, node_array<int>& dfs_num, 
         int& dfs_count, node_array<edge>& tree_edge)
{ dfs_list.append(v);
  dfs_num[v] = dfs_count++;
  edge e;
  forall_adj_edges(e,v)
  { node w = G.target(e);
    if ( dfs_num[w] == -1 )
    { tree_edge[w] = e;
      DFS(G,w,dfs_list,dfs_num,dfs_count,tree_edge);
    }
  }
}


void assign(node& x, const node& y)
{ if ( x == nil) x = y; }


node join(node a, node b, node c, 
          const node_array<edge>& tree_edge,
          graph& B,
          list<edge>& L)
{ L.clear();
  node_array<int> num_desc(B,0);
  array<node> A(3); A[0] = a; A[1] = b; A[2] = c;
  int i;
  for (i = 0; i < 3; i++)
  { node v = A[i];
    num_desc[v]++;
    while ( tree_edge[v] != nil )
    { v = B.source(tree_edge[v]);
      num_desc[v]++;
    }    
  }
  node r=0;
  for (i = 0; i < 3; i++)
  { node v = A[i];
    while (num_desc[v] < 3)
    { L.append(tree_edge[v]);
      num_desc[v] = 3;
      v = B.source(tree_edge[v]);        
    }
    if ( i == 0 ) r = v;
  }
  return r;
}

void translate_to_G(list<edge>& L, const GRAPH<node,edge>& B)
{ list_item it;
  forall_items(it,L) L[it] = B[L[it]];
}


#ifdef DEBUG
static void check_before_return(const graph& G,
     const list<edge>& K, const node_array<int>& st_num,
     const array<node_array<node> >& leaf, 
     const node_array<edge>& tree_edge,
     const node_array<int>& dfs_num, int k, 
     GRAPH<node,edge>& B, const node_array<int> st_numB,
     const node& sB, const string& current_case) 
{ 
node v; edge e;
  GRAPH<node,edge> G1;  
  G1.set_node_bound(G.number_of_nodes());
  G1.set_edge_bound(G.number_of_edges());
  node_array<node> link(G); 
  forall_nodes(v,G) link[v] = G1.new_node(v);
  node_array<int> st_num1(G1);
  forall_nodes(v,G1) st_num1[v] = st_num[G1[v]];
  forall(e,K) G1.new_edge(link[G.source(e)],link[G.target(e)]);
 

  if (!CHECK_KURATOWSKI(G,K) )
  { cout << "\n\n" << current_case;

    GraphWin gw1(G1);
    gw1.set_node_label_type(user_label);
    gw1.set_edge_label_type(user_label);
    forall_nodes(v,G1)
    { string s = string("%d",st_num1[v]);
      gw1.set_user_label(v,s);
    }
    gw1.display();
    gw1.edit();

    
    GraphWin gw(B); gw.set_node_label_type(user_label);
    gw.set_directed(true);
    gw.set_node_shape(rectangle_node);
    gw.set_node_width(80);

    cout << "\nk = " << k; cout.flush();

    forall_nodes(v,B)
    { if (leaf[K_PLUS_1][v] && leaf[OTHERS][v] )
      gw.set_color(v,red);
      if (st_num[B[v]] == k + 1) gw.set_color(v,green);
      if (st_num[B[v]] > k + 1) gw.set_color(v,blue);
      string s = string("%d",dfs_num[v]) + " " + string("%d",st_numB[v]) + " " + string("%d",st_num[B[v]]);
      gw.set_user_label(v,s);
      if ( v != sB ) gw.set_color(tree_edge[v],red);
    }
    gw.display(); gw.edit();
  }
}
#endif



static bool PLANTEST(graph& G, node_array<int>& st_num, 
                               list<node>& st_list)
{ 
  int n = G.number_of_nodes();
  int m = G.max_edge_index() + 1;

  if (n < 5)  return true;

  pq_tree  T(m);
  
  int stv = 1;
  
  node v;
  forall(v,st_list)
  { 
    list<int> L, U, I;

    edge e;
    forall_inout_edges(e,v)
    { node w = G.opposite(v,e);
      int stw = st_num[w];
      if (stw < stv) L.push(index(e)+1);
      if (stw > stv) U.push(index(e)+1); 
     }

    if ( !T.replace(L,U,I) ) break;

    stv++;
   }

  return stv == n+1;
}



static int PLAN_EMBED_K(graph& G, node_array<int>& st_num,
                                     list<node>& st_list)
{ 
  int n = G.number_of_nodes();
  if ( G.number_of_edges() == 0 ) return true;
  int m = G.max_edge_index() + 1;

  // interface for pq_tree

  pq_tree  T(m);

  list<int>* I = new list<int>[n+1];
  edge* EDGE  = new edge[m+1];  // EDGE[i+1] = edge with index i

  edge  e;
  forall_edges(e,G) EDGE[index(e)+1] = e;
      
  // planarity test

  int stv = 1;
    
  node v;
  forall(v,st_list)
  { 
    list<int> L, U;

    edge e;
    forall_adj_edges(e,v)
    { int stw = st_num[target(e)];
      if (stw < stv) L.push(index(e) + 1);
      if (stw > stv) U.push(index(G.reversal(e)) + 1); 
    }
    
    if ( !T.replace(L,U,I[stv]) ) break;

    stv++;
  }


  if (stv == n+1) { 
                    node_array<list<edge> > L_n(G);

                    //const int EVEN = +1; 
                    const int ODD = -1;

                    int stv = n; 

                    forall_rev(v,st_list)  
                    { 
                      if (stv == 1) break;   // for v = t down to s+1

                      list<int>* I_v = &I[stv];

                      int d  = 1;
                      int l  = I_v->pop();

                      if ( l == ODD )
                      { d = -1;
                        l = I_v->pop();
                      }
                      int i;
                      for( i = 0; i < l; i++)
                      { int j = d * I_v->pop();
                        if (j < 0) I[-j].push(ODD);  // tell |j| that it is odd
                      }
                      if (d > 0)
                        forall(i,*I_v) L_n[v].append(EDGE[i]);
                      else
                        forall(i,*I_v) L_n[v].push(EDGE[i]);

                      stv--;
                    }

                    
                    node t = st_list.tail();
                    embedding(G,t,st_num,L_n);

 }

  delete[] EDGE;
  delete[] I;

  return stv - 1;
}

static bool PLAN_EMBED(graph& G, node_array<int>& st_num,
                                     list<node>& st_list)
{ return PLAN_EMBED_K(G,st_num,st_list) == G.number_of_nodes(); }



bool BL_PLANAR(graph& G, bool embed) 
{ if (G.number_of_edges() <= 0)  return true;
  
  // prepare graph

  list<edge> el;

  if (embed)
  { if ( !G.make_map() )
      LEDA_EXCEPTION(1,"BL_PLANAR: can only embed maps.");
     Make_Biconnected(G,el); 
     edge e;
     forall(e,el)
     { edge x = G.new_edge(target(e),source(e));
       el.push(x);
       G.set_reversal(e,x);
     }
  }
  else
    Make_Biconnected(G,el); 
  
  
  node_array<int> st_num(G);
  list<node> st_list;
  ST_NUMBERING(G,st_num,st_list);


  bool plan;

  if (embed)
    plan = PLAN_EMBED(G,st_num,st_list);
  else
    plan = PLANTEST(G,st_num,st_list);


  // restore graph

  edge e; forall(e,el) G.del_edge(e);

  return plan;
}



static void Kuratowski(graph& G, list<edge>& K)
{ node v; edge e; 
  string current_case;  // for debugging purposes

  
  node_array<int> st_num(G);
  list<node> st_list;
  ST_NUMBERING(G,st_num,st_list);
  node s = st_list.head();
  node t = st_list.tail();
      

  int k = PLAN_EMBED_K(G,st_num,st_list);
  if ( k == G.number_of_nodes() ) 
    LEDA_EXCEPTION(1,"Kuratowski: G must be non-planar");

  
  GRAPH<node,edge> B;
  list<node>      st_listB;

  node_array<node> v_in_B(G,nil);

  forall(v,st_list)
  { if ( st_num[v] > k ) break;
    node vB = v_in_B[v] = B.new_node(v);
    st_listB.append(vB);
  }

  node top_B = B.new_node();

  forall_nodes(v,G)
  { if (st_num[v] > k) continue;
    forall_adj_edges(e,v)
    { node w = G.target(e);
      if ( st_num[w] < st_num[v] )  continue;
      edge r = G.reversal(e);
      node wB;
      if ( st_num[w] > k )
      { wB = B.new_node(w);
        st_listB.append(wB);
        B.set_reversal(B.new_edge(wB,top_B),B.new_edge(top_B,wB));
      } 
      else
        wB =  v_in_B[w];
      edge e1 = B.new_edge(v_in_B[v],wB,e);
      edge r1 = B.new_edge(wB,v_in_B[v],r);
      B.set_reversal(e1,r1);
    }
  }

  node sB = v_in_B[s];  
  node tB = 0;
  forall_adj_edges(e,sB)
    if ( B[B.target(e)] == t) tB = B.target(e); 

  B.set_reversal(B.new_edge(sB,top_B),B.new_edge(top_B,sB));

  st_listB.append(top_B);

  node_array<int> st_numB(B);
  int stn = 1;
  forall(v,st_listB) st_numB[v] = stn++;

  PLAN_EMBED(B,st_numB,st_listB); // destroys st-numbers

  stn = 1;
  forall(v,st_listB) st_numB[v] = stn++;

  B.del_node(top_B); st_listB.Pop();  // remove top_B


  
  array<node> z(3);
  array<node> spec(3);


  list<node> dfs_list;
  node_array<edge> tree_edge(B,nil);
  node_array<int> dfs_num(B,-1);
  int dfs_count = 0;

  DFS(B,sB,dfs_list,dfs_num,dfs_count,tree_edge);

  edge_array<int> comp_num(B);
  int num_comps = BICONNECTED_COMPONENTS(B,comp_num);

  node_array<edge> up_tree_edge(G,nil);

  array<node_array<node> > leaf(2);
  leaf[K_PLUS_1] = leaf[OTHERS] = node_array<node>(B,nil);

  array<node_array<node> > leaf_in_upper_part(2);
  leaf_in_upper_part[K_PLUS_1] = 
         leaf_in_upper_part[OTHERS] = node_array<node>(B,nil);

  node_array<int>  num_mixed_non_root_comps(B,0);

  node_array<node> spec_leaf_in_root_comp(B,nil);

  array<node_array<node> > child(1,2);  // want indices one and two
  child[1] = child[2] = node_array<node>(B,nil);


  forall_nodes(v,G)
  { if (st_num[v] <= k ) continue;
    edge e;
    forall_adj_edges(e,v)
    { node w = G.target(e);
      if ( st_num[w] > st_num[v] )
      { up_tree_edge[v] = G.reversal(e); break; }
    }
  }


  forall_nodes(v,B)
  { if (st_numB[v] <= k) continue;
    if ( st_num[B[v]] == k + 1 ) 
      leaf[K_PLUS_1][v] = v;
    else 
      leaf[OTHERS][v] = v;
  }

  forall_rev(v,dfs_list)  // down the tree 
  { if (v == sB) continue;
    node pv = B.source(tree_edge[v]);
    assign(leaf[K_PLUS_1][pv],leaf[K_PLUS_1][v]);
    assign(leaf[OTHERS][pv],  leaf[OTHERS][v]);
  }


  forall_rev(v,dfs_list)   // down the tree
  { if (num_mixed_non_root_comps[v] >= 3) 
    { 
      current_case = "three mixed non-root components";

      int i = 0;

      forall_adj_edges(e,v)
      { node w = B.target(e);
        if ( w == sB || v != B.source(tree_edge[w]) ) continue;
        if ( leaf[K_PLUS_1][w] && leaf[OTHERS][w] )
        { z[i] = leaf[OTHERS][w]; spec[i] = leaf[K_PLUS_1][w]; 
          i++;
          if ( i == 3) break;
        }
      }

      
      // reroot the DFS-tree at v
       
      dfs_list.clear();
      dfs_num.init(B,-1);
      tree_edge.init(B,nil);
      int dfs_count = 0;
      DFS(B,v,dfs_list,dfs_num,dfs_count,tree_edge);

      list<edge> join_edges;
      for (i = 0; i < 3; i++)
      { join(z[i],spec[i],v,tree_edge,B,join_edges);
        translate_to_G(join_edges,B); K.conc(join_edges);
      }

      join(B[z[0]],B[z[1]],B[z[2]],up_tree_edge,G,join_edges);
      K.conc(join_edges);

#ifdef DEBUG
      check_before_return(G,K,st_num,leaf,tree_edge,dfs_num,k,
      B,st_numB,sB,current_case); 
#endif

     return;
 
 } 

    if ( v == sB) continue;
    node pv = B.source(tree_edge[v]);
    if ( pv == sB 
        || comp_num[tree_edge[v]] != comp_num[tree_edge[pv]] )
    { if ( leaf[K_PLUS_1][v] && leaf[OTHERS][v] )
        num_mixed_non_root_comps[pv]++;
      assign(leaf_in_upper_part[K_PLUS_1][pv],leaf[K_PLUS_1][v]); 
      assign(leaf_in_upper_part[OTHERS][pv],leaf[OTHERS][v]);   
    } 
  }


  forall_rev(v,dfs_list)  // down the tree
  { if (v == sB) continue;
    node pv = B.source(tree_edge[v]);
    if ( pv != sB && 
        comp_num[tree_edge[v]] == comp_num[tree_edge[pv]] )
      assign(spec_leaf_in_root_comp[pv],leaf[K_PLUS_1][v]);
  }


  node_array<node>  spec_leaf_via_tree_edge(B,nil);

  forall(v,dfs_list)  // up the tree 
  { assign(spec_leaf_in_root_comp[v],spec_leaf_via_tree_edge[v]);
    
    if ( num_mixed_non_root_comps[v] == 2 &&
         spec_leaf_in_root_comp[v] )
    { 
      current_case = "two mixed non-root and a mixed root component";

      z[0] = tB; 
      spec[0] = spec_leaf_in_root_comp[v];

      int i = 1; 

      forall_adj_edges(e,v)
      { node w = B.target(e);
        if ( w == sB || v != B.source(tree_edge[w]) ) continue;
        if ( v != sB && comp_num[e] == comp_num[tree_edge[v]] ) 
          continue;
        if ( leaf[K_PLUS_1][w] && leaf[OTHERS][w] )
        { z[i] = leaf[OTHERS][w]; spec[i] = leaf[K_PLUS_1][w]; 
          i++;
          if ( i == 3) break;   
        }
      }

      
      // reroot the DFS-tree at v
       
      dfs_list.clear();
      dfs_num.init(B,-1);
      tree_edge.init(B,nil);
      int dfs_count = 0;
      DFS(B,v,dfs_list,dfs_num,dfs_count,tree_edge);

      list<edge> join_edges;
      for (i = 0; i < 3; i++)
      { join(z[i],spec[i],v,tree_edge,B,join_edges);
        translate_to_G(join_edges,B); K.conc(join_edges);
      }

      join(B[z[0]],B[z[1]],B[z[2]],up_tree_edge,G,join_edges);
      K.conc(join_edges);

#ifdef DEBUG
      check_before_return(G,K,st_num,leaf,tree_edge,dfs_num,k,
      B,st_numB,sB,current_case); 
#endif

      return;

 }

    if ( spec_leaf_via_tree_edge[v] != nil )
    { forall_adj_edges(e,v)
      { node c = B.target(e);
        if ( c == sB || v != B.source(tree_edge[c]) ) continue;
        spec_leaf_via_tree_edge[c] = spec_leaf_via_tree_edge[v];
      }
    }     
    else
    { forall_adj_edges(e,v)
      { node c = B.target(e);
        if ( c == sB || v != B.source(tree_edge[c]) ) continue;
        if ( leaf[K_PLUS_1][c] )
        { if ( child[1][v] == nil )
            child[1][v] = c; 
          else
            child[2][v] = c;
        }
      }
      if ( child[1][v] )
      { forall_adj_edges(e,v)
        { node c = B.target(e);
          if ( c == sB || v != B.source(tree_edge[c]) ) continue;
          if ( c != child[1][v] )
            spec_leaf_via_tree_edge[c] = 
                                     leaf[K_PLUS_1][child[1][v]];
          else
            if ( child[2][v] )
             spec_leaf_via_tree_edge[c] =
                                     leaf[K_PLUS_1][child[2][v]];
        }
      }
    }
  }


  
  array<bool> treated_component(num_comps); 
  edge f;

  forall(v,dfs_list)       // upwards
  { forall_adj_edges(f,v)
    { edge e1 = B.cyclic_adj_succ(f);
      edge e_pred = B.cyclic_adj_pred(f);
      if ( comp_num[e1] != comp_num[f] || 
           comp_num[f] == comp_num[e_pred] ) continue;
      if ( treated_component[comp_num[f]] ) continue;

      list<edge> cycle_edges;
      treated_component[comp_num[f]] = true; 

      
      edge e0 = f;
      node y0 = v;
      edge e = f; // e1 was set to B.cyclic_adj_succ(f) above
          
      do
      { while ( comp_num[B.cyclic_adj_succ(e1)] == comp_num[e] &&
               B.cyclic_adj_succ(e1) != e )
        { e1 = B.cyclic_adj_succ(e1); }
        cycle_edges.append(e1);
        e = B.reverse(e1); 
        e1 = B.cyclic_adj_succ(e);
      } while ( e != e0 );


      node spec_leaf_in_opposite_part = nil;

      
      spec_leaf_in_opposite_part = spec_leaf_in_root_comp[v]; 

      for (int i = 1; i <= 2; i++)
      { node c = child[i][v];
        if ( spec_leaf_in_opposite_part == nil
            && c &&  comp_num[tree_edge[c]] != comp_num[e0] )
          spec_leaf_in_opposite_part = leaf[K_PLUS_1][c];
      }

    
      

      list<int> kinds;
      kinds.append(OTHERS); kinds.append(K_PLUS_1);

      int y0_type;

      forall(y0_type, kinds)
      { array<node> y(4);
        y[0] = y0; y[1] = y[2] = y[3] = nil;

        array<node> z(4);

        bool break_loop = false;  // sn: workaround for a bug in g++-2.97

        if (y0_type == OTHERS)
        { z[0] = tB; 
          current_case = 
               "cycle with 4 attachment nodes; y_0 connects to t";
        }
        else
        { z[0] = spec_leaf_in_opposite_part; 
          current_case = 
                "cycle with 4 attachment nodes; y_0 connects to k + 1";
          //if ( !spec_leaf_in_opposite_part ) break;  //sn: see above
          if ( !spec_leaf_in_opposite_part ) break_loop = true;
        }

     if (!break_loop)  // sn
     {                 // sn
        list_item it0 = cycle_edges.first();
        list_item it = cycle_edges.cyclic_succ(it0);

        int i = 1; 
        while (it != it0)
        { node v = B.source(cycle_edges[it]);
          int kind = (i == 2 ? y0_type : 1 - y0_type);
          if ( leaf_in_upper_part[kind][v] )
          { y[i] = v; 
            z[i] = leaf_in_upper_part[kind][v];
            i++;
          }
          if ( i == 4 ) 
          { 
            translate_to_G(cycle_edges,B); K.conc(cycle_edges);

            list<edge> join_edges;

            int i;
            for (i = 0; i < 4; i++)
            { join(y[i],z[i],z[i],tree_edge,B,join_edges);
              translate_to_G(join_edges,B); K.conc(join_edges);
            }

            // subtree of T_t spanned by k+1 and two large leaves.

            if (y0_type == OTHERS) i = 0; else i = 3;
            join(B[z[i]],B[z[1]],B[z[2]],up_tree_edge,G,join_edges);

            K.conc(join_edges); 

#ifdef DEBUG
            check_before_return(G,K,st_num,leaf,tree_edge,dfs_num,k,
            B,st_numB,sB,current_case);
#endif


            return;
          }
          it = cycle_edges.cyclic_succ(it);
        }
       }           // sn
       else break; // sn
      }



      if ( spec_leaf_in_opposite_part ) 
      { 
        array<node> y(3);
        array<node> spec_leaf_opposing(3);
        array<node> other_leaf_opposing(3);

        y[0] = y0; 
        spec_leaf_opposing[0] = spec_leaf_in_opposite_part;
        other_leaf_opposing[0] = tB;

        int i = 1;

        list_item it0 = cycle_edges.first(); 
        list_item it = cycle_edges.cyclic_succ(it0);
        while (it != it0)
        { node v = B.source(cycle_edges[it]);
          if ( leaf_in_upper_part[OTHERS][v] && 
              leaf_in_upper_part[K_PLUS_1][v] )
          { y[i] = v;
            spec_leaf_opposing[i] = leaf_in_upper_part[K_PLUS_1][v];
            other_leaf_opposing[i] = leaf_in_upper_part[OTHERS][v];
            i++;
          }
          if ( i == 3 )
          { 
            current_case = "obstructing cycle with three mixed attachments";

            translate_to_G(cycle_edges,B); K.conc(cycle_edges); 

            list<edge> join_edges;

            for(int j = 0; j <= 2; j++)
            { join(spec_leaf_opposing[j], other_leaf_opposing[j], y[j],
                   tree_edge,B,join_edges);
              translate_to_G(join_edges,B); K.conc(join_edges);
            }

            node r = join(B[other_leaf_opposing[1]], 
                          B[other_leaf_opposing[2]],
                          B[spec_leaf_opposing[0]],
                          up_tree_edge,G,join_edges);

            K.conc(join_edges);

            join(r,r,t,up_tree_edge,G,join_edges);

            K.conc(join_edges);

            { 
              node v; edge e;

              edge_array<bool> in_K(G,false);
              node_array<int> deg_in_K(G,0);
              forall(e,K) 
              { in_K[e] = true; 
                deg_in_K[G.source(e)]++;
                deg_in_K[G.target(e)]++;
              }

              GRAPH<node,list<edge> > AG;
              node_array<node> link(G,nil);

              forall_nodes(v,G) 
                if ( deg_in_K[v] > 2 ) link[v] = AG.new_node(v);
               
              forall_nodes(v,G)
              { if ( !link[v] ) continue;
                edge e; 
                forall_inout_edges(e,v)
                { if ( in_K[e] )
                  { // trace path starting with e
                    list<edge> path;
                    edge f = e; node w = v;
                    while (true)
                    { in_K[f] = false; path.append(f);
                      w = G.opposite(w,f);
                      if ( link[w] ) break;
                      // observe that w has degree two and hence ...
                      forall_inout_edges(f,w)
                        if ( in_K[f] ) break;
                    }
                    edge e_new = AG.new_edge(link[v],link[w]);
                    AG[e_new].conc(path);  // O(1) assignment 
                  }
                }
              }
               
              list<edge> el;
              KURATOWSKI_SIMPLE(AG,el);

              K.clear();
              forall(e,el) K.conc(AG[e]);
 }


#ifdef DEBUG
            check_before_return(G,K,st_num,leaf,tree_edge,dfs_num,k,
            B,st_numB,sB,current_case);
#endif

            return;
          }
          it = cycle_edges.cyclic_succ(it);
        }
 }
    }
  }

  #ifdef DEBUG
  GraphWin gw(B); gw.set_node_label_type(user_label);
  gw.set_directed(true);
  gw.set_node_shape(rectangle_node);
  gw.set_node_width(55);

  cout << "\nk = " << k; cout.flush();

  read_char("Type any character to proceed: did not find one of the cases");

  forall_nodes(v,B)
  { if (leaf[K_PLUS_1][v] && leaf[OTHERS][v] )
      gw.set_color(v,red);
    if (st_num[B[v]] == k + 1) gw.set_color(v,green);
    if (st_num[B[v]] > k + 1) gw.set_color(v,blue);
    string s = string("%d",dfs_num[v]) + " " + string("%d",st_numB[v]) + " " + string("%d",st_num[B[v]]);
    gw.set_user_label(v,s);
    if ( v != sB ) gw.set_color(tree_edge[v],red);
  }
  gw.display(); gw.edit();


  LEDA_EXCEPTION(1,"did not find one of the cases");
  #endif


}



bool BL_PLANAR(graph& G, list<edge>& K, bool embed)
{
  if (BL_PLANAR(G, embed)) return true;

  edge_array<int> comp_num(G);
  int num_c = BICONNECTED_COMPONENTS(G,comp_num);

  GRAPH<node,edge> H;

  edge e;
  if ( num_c == 1 ) 
  { CopyGraph(H,G);
    Delete_Loops(H);
  }
  else
  { node_array<node> link(G,nil);

    array<list<edge> > E(num_c);
    array<list<node> > V(num_c);

    forall_edges(e,G)
    { node v = source(e);  node w = target(e); 
      if (v == w) continue;
      int c = comp_num[e];  E[c].append(e);
      V[c].append(v); V[c].append(w);
    }
    int c; node v;
    for(c = 0; c < num_c; c++)
    { H.clear();

      forall(v,V[c]) 
        if ( link[v] == nil ) link[v] = H.new_node(v);
      
      forall(e,E[c])
      { node v = source(e); node w = target(e);
        H.new_edge(link[v],link[w],e); 
      }

      forall(v,V[c]) link[v] = nil;

      if (!BL_PLANAR(H,false)) break;
    }
  }

  K.clear();

  // H is a biconnected non-planar graph; we turn it into map

  Make_Simple(H);

  list<edge> R;
  H.make_map(R);   

  forall(e,R) H[e] = H[H.reverse(e)];
 
  // auxiliary edges inherit original edge from their reversal

  Kuratowski(H,K);

  list_item it;
  forall_items(it,K) K[it] = H[K[it]];

  return false;
}

LEDA_END_NAMESPACE
