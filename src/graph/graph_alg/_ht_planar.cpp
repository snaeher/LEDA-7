/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _ht_planar.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/stack.h>
#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_alg.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

const int left_side = 1; 
const int right_side = 2; 

void dfs_in_reorder(list<edge>& Del, node v, int& dfs_count,
node_array<bool>& reached,  
node_array<int>& dfsnum,
node_array<int>& lowpt1, node_array<int>& lowpt2, 
node_array<node>& parent); 


class block
{ 
  private:
  list<int> Latt, Ratt; //list of attachments
  list<edge> Lseg,Rseg; //list of segments represented by their defining edges

  public:

  LEDA_MEMORY(block);

  block(edge e, list<int>& A)
  { 
    Lseg.append(e); 
    Latt.conc(A); 
    // the other two lists are empty
  }

  ~block()  {}

  void flip()
  { 
    list<int> ha; 
    list<edge> he; 
    /* we first interchange |Latt| and |Ratt| and then |Lseg| and |Rseg| */
    ha.conc(Ratt); Ratt.conc(Latt); Latt.conc(ha); 
    he.conc(Rseg); Rseg.conc(Lseg); Lseg.conc(he); 
  }

  int head_of_Latt() { return Latt.head();  }

  bool empty_Latt()  { return Latt.empty(); }

  int head_of_Ratt() { return Ratt.head();  }

  bool empty_Ratt()  { return Ratt.empty(); }

  bool left_interlace(stack<block*>& S)
  { 
    /* check for interlacing with the left side of the topmost block of
    |S| */
    if (Latt.empty()) LEDA_EXCEPTION(1,"Latt is never empty"); 

    if (!S.empty() && !((S.top())->empty_Latt()) && 
    Latt.tail() < (S.top())->head_of_Latt())
    return true; 
    else return false; 
  }

  bool right_interlace(stack<block*>& S)
  { 
    /* check for interlacing with the right side of the topmost block of
    |S| */
    if (Latt.empty()) LEDA_EXCEPTION(1,"Latt is never empty"); 

    if (!S.empty() && !((S.top())->empty_Ratt()) && 
    Latt.tail() < (S.top())->head_of_Ratt())
    return true; 
    else return false; 
  }

  void combine(block* const & Bprime)
  { 
    /* add block Bprime to the rear of |this| block */
    Latt.conc(Bprime -> Latt); 
    Ratt.conc(Bprime -> Ratt); 
    Lseg.conc(Bprime -> Lseg); 
    Rseg.conc(Bprime -> Rseg); 
    delete(Bprime); 
  }

  bool clean(int dfsnum_w, edge_array<int>& alpha)
  { 
    /* remove all attachments to |w|; there may be several */
    while (!Latt.empty() && Latt.head() == dfsnum_w) Latt.pop(); 
    while (!Ratt.empty() && Ratt.head() == dfsnum_w) Ratt.pop(); 

    if (!Latt.empty() || !Ratt.empty()) return false; 

    /*|Latt| and |Ratt| are empty; we record the placement of the subsegments
    in |alpha|. */

    edge e; 
    forall(e,Lseg) alpha[e] = left_side; 

    forall(e,Rseg) alpha[e] = right_side; 

    return true; }

    void add_to_Att(list<int>& Att, int dfsnum_w0,
    edge_array<int>& alpha)
    { 
      /* add the block to the rear of |Att|. Flip if necessary */
      if (!Ratt.empty() && head_of_Ratt() > dfsnum_w0) flip(); 

      Att.conc(Latt); 
      Att.conc(Ratt); 
      /* This needs some explanation. Note that |Ratt| is either empty
      or $\{ w0\ }$. Also if |Ratt| is non - empty then all subsequent sets are contained
      in $\{ w0\ }$. So we indeed compute an ordered set of attachments. */

      edge e; 
      forall(e,Lseg) alpha[e] = left_side; 

      forall(e,Rseg) alpha[e] = right_side; 
    }


  }; 


static node_array<int> node_ord;
static int source_ord(const edge& e) { return node_ord[source(e)]; }
static int target_ord(const edge& e) { return node_ord[target(e)]; }


static void copy_simplified_graph(const graph& G, GRAPH<node,edge>& H, 
                       node_array<node>& v_in_H, edge_array<edge>& e_in_H,
                       edge_array<int>& offset, bool Gin_is_map)
{
  node v;
  forall_nodes(v,G) v_in_H[v] = H.new_node(v);

  list<edge> el = G.all_edges();

  if ( el.empty() ) return;
  
  node_ord.init(G,0);
  int  nr = 0;
  int  n = G.number_of_nodes();
 
  forall_nodes(v,G) node_ord[v] = nr++;
  el.bucket_sort(0,n-1,&source_ord);
  el.bucket_sort(0,n-1,&target_ord);
  
  // now parallel edges are adjacent in el

  if ( !Gin_is_map )
  { // we delete self loops and parallel edges
    list_item it = el.first();   
    while ( it )
    { edge e0 = el[it];
      node v = G.source(e0); node w = G.target(e0);
      if ( v == w )
        e_in_H[e0] = nil;
      else 
        e_in_H[e0] = H.new_edge(v_in_H[v],v_in_H[w],e0); 
        
      list_item it1 = el.succ(it); 
      while (it1 && 
             source(e0) == source(el[it1]) && 
             target(e0) == target(el[it1]) ) 
      { edge e = el[it1];
        e_in_H[e] = e_in_H[e0]; 
        it1 = el.succ(it1); 
      }
      it = it1;
    }
    return;
  }
  
  // we need to work harder because we want to embed    
    
  list_item it = el.first();   
  while ( it )
  { edge e0 = el[it];
    if ( offset[e0] != 0 ) { it = el.succ(it); continue; }
    edge r0 = G.reversal(e0);
    node v = G.source(e0); node w = G.target(e0);
    if ( v == w )
      { e_in_H[e0] = nil; e_in_H[r0] = nil; }
    else 
    { e_in_H[e0] = H.new_edge(v_in_H[v],v_in_H[w],e0); 
      e_in_H[r0] = H.new_edge(v_in_H[w],v_in_H[v],r0); 
      H.set_reversal(e_in_H[e0],e_in_H[r0]);
    }
    list_item it1 = it; int off = 1;
    while (it1 && 
          source(e0) == source(el[it1]) && 
          target(e0) == target(el[it1])) 
    { edge e = el[it1];
      e_in_H[e] = e_in_H[e0]; e_in_H[G.reversal(e)] = e_in_H[r0];
      if (offset[e] == 0)
      { offset[e] = off; offset[G.reversal(e)] = -off;  
        off++;
      }
      it1 = el.succ(it1);
    }
    it = it1;   
  }

//assert(H.is_map());
//edge e;
//forall_edges(e,G) assert(offset[e] == -offset[G.reversal(e)]);
//forall_edges(e,G) assert(G.source(e) == G.target(e) || e_in_H[e] != nil);
//forall_edges(e,G) assert(offset[e] != 0);

}


void reorder(graph& G,node_array<int>& dfsnum, 
node_array<node>& parent)
{ 
  node v; 
  node_array<bool> reached(G,false); 
  int dfs_count = 0; 
  list<edge> Del; 
  node_array<int> lowpt1(G), lowpt2(G); 

  dfs_in_reorder(Del,G.first_node(),dfs_count,reached,dfsnum,lowpt1,lowpt2,parent); 
      
  /* remove forward and reversals of tree edges */

  edge e; 
  forall(e,Del)   G.del_edge(e); 

  /* we now reorder adjacency lists as described in \cite[page 101]{ Me:book } */

  node w; 
  edge_array<int> cost(G); 
  forall_edges(e,G)
  { v = source(e); w = target(e); 
    cost[e] = ((dfsnum[w] < dfsnum[v]) ?
    2*dfsnum[w]  : 
    ( (lowpt2[w] >= dfsnum[v]) ?
    2*lowpt1[w] : 2*lowpt1[w] + 1)); 
  }

  G.bucket_sort_edges(cost); 

}



void dfs_in_reorder(list<edge>& Del, node v, int& dfs_count,
         node_array<bool>& reached,  
         node_array<int>& dfsnum,
         node_array<int>& lowpt1, node_array<int>& lowpt2, 
         node_array<node>& parent)
{
node w;
edge e;


  dfsnum[v] =  dfs_count++;
  lowpt1[v] = lowpt2[v] = dfsnum[v];
  reached[v] = true;
  forall_adj_edges(e,v)
    {
     w = target(e);
     if( !reached[w] )
      {/* e is a tree edge */
       parent[w] = v;
       dfs_in_reorder(Del,w,dfs_count,reached,dfsnum,lowpt1,lowpt2,
           parent);
       lowpt1[v] = leda_min(lowpt1[v],lowpt1[w]);
      } //end tree edge
     else {lowpt1[v] = leda_min(lowpt1[v],dfsnum[w]); // no effect for forward edges
          if(( dfsnum[w] >= dfsnum[v])  || w == parent[v] ) 
               /* forward edge or reversal of tree edge */
               Del.append(e); 
          } //end non-tree edge
          
               
     } // end forall

  /* we know |lowpt1[v]| at this point and now make a second pass over all
     adjacent edges of |v| to compute |lowpt2| */
  
  forall_adj_edges(e,v)
    {w = target(e);
     if (parent[w] ==v)
     { /* tree edge */
       if (lowpt1[w] != lowpt1[v]) lowpt2[v] = leda_min(lowpt2[v],lowpt1[w]);
       lowpt2[v] = leda_min(lowpt2[v],lowpt2[w]);
     } //end tree edge
     else // all other edges 
          if (lowpt1[v] != dfsnum[w]) lowpt2[v] = leda_min(lowpt2[v],dfsnum[w]);
     } //end forall
} //end dfs


bool strongly_planar(edge e0, graph& G, list<int>& Att, 
edge_array<int>& alpha, 
node_array<int>& dfsnum,
node_array<node>& parent)
{ 
  
node x = source(e0); 

node y = target(e0); 

edge e = G.first_adj_edge(y); 

node wk = y; 

while (dfsnum[target(e)] > dfsnum[wk])  // e is a tree edge
{ wk = target(e); 
  e= G. first_adj_edge(wk); 
}

node w0 = target(e); 

node w = wk; 

stack<block*> S; 

while (w != x)
{ int count = 0; 
  forall_adj_edges(e,w)
  { count++; 
    if (count != 1) //no action for first edge
    { 
list<int> A; 

if (dfsnum[w] < dfsnum[target(e)])
{ /* tree edge */
  if (!strongly_planar(e,G,A,alpha,dfsnum,parent)) 
  { while (!S.empty()) delete(S.pop()); 
    return false; 
  }
        
}
else A.append(dfsnum[target(e)]); // a back edge
        
block* B = new block(e,A); 

while (true)
{ 
  if (B->left_interlace(S)) (S.top())->flip(); 

  if (B->left_interlace(S)) 
  { delete(B); 
    while (!S.empty()) delete(S.pop()); 
    return false; 
  }; 

  if (B->right_interlace(S)) B->combine(S.pop()); 
  else break; 
} //end while

S.push(B); 

    } // end if
  } //end forall
  
while (!S.empty() && (S.top())->clean(dfsnum[parent[w]],alpha)) delete(S.pop()); 
  w = parent[w]; 
} //end while

  
Att.clear(); 
while (! S.empty())
{ 
  block* B = S.pop(); 

  if (!(B->empty_Latt()) && !(B->empty_Ratt()) &&
  (B->head_of_Latt()>dfsnum[w0]) && (B->head_of_Ratt() > dfsnum[w0]))
  { delete(B); while (!S.empty()) delete(S.pop()); return false; }

  B->add_to_Att(Att,dfsnum[w0],alpha); 

  delete(B); 
} //end while

/* Let's not forget (as the book does) that $w0$ is an attachment of $S(e0)$
except if $w0 = x$. */

if (w0 != x) Att.append(dfsnum[w0]); 

  return true; 
}


void embedding(edge e0, int t, 
               GRAPH<node,edge>& G,
               edge_array<int>& alpha,  
               node_array<int>& dfsnum, 
               list<edge>& T, list<edge>& A, int& cur_nr,  
               edge_array<int>& sort_num, node_array<edge> & tree_edge_into, 
               node_array<node>& parent,GRAPH<node,edge>& H) //, edge_array<edge>& reversal)

{ 
node x = source(e0); 

node y = target(e0); 

tree_edge_into[y] = e0; 

edge e = G.first_adj_edge(y); 

node wk = y; 

while (dfsnum[target(e)] > dfsnum[wk])  // e is a tree edge
{ wk = target(e); 
  tree_edge_into[wk] = e; 
  e= G. first_adj_edge(wk); 
}

// node w0 = target(e); 
edge back_edge_into_w0 = e; 

  
node w = wk; 

list<edge> Al, Ar; 
list<edge> Tprime, Aprime; 

T.clear(); 
T.append(G[e]); // |e = (wk,w0)| at this point, line (2)

while (w != x)
{ int count = 0; 
  forall_adj_edges(e,w)
  { count++; 
    if (count != 1) //no action for first edge
    { 
        
if (dfsnum[w] < dfsnum[target(e)])
{ /* tree edge */
  int tprime = ((t == alpha[e]) ? left_side : right_side); 
  embedding(e,tprime,G,alpha,dfsnum,Tprime,Aprime,cur_nr,
            sort_num,tree_edge_into,parent,H); //,reversal); 

}
else         { /* back edge */
               Tprime.append(G[e]); //$e$
               Aprime.append(H.reversal(G[e])); //reversal of $e$
}

      
if (t == alpha[e])
{ Tprime.conc(T); 
  T.conc(Tprime); //$T = Tprime\ conc\ T$
        
  Al.conc(Aprime); //$Al = Al\ conc\ Aprime$
        
}
else        { T.conc(Tprime); // $ T\ = T\ conc\ Tprime $

              Aprime.conc(Ar); 
              Ar.conc(Aprime); // $ Ar\ = Aprime\ conc\ Ar$
        
}

    } // end if
  } //end forall

  
T.append(H.reversal(G[tree_edge_into[w]])); // $(w_{ j - 1 },w_j)$

forall(e,T) sort_num[e] = cur_nr++; 

/* |w|'s adjacency list is now computed; we clear |T| and prepare for the 
next iteration by moving all darts incident to |parent[w]| from |Al| and
|Ar| to |T|. These darts are at the rear end of |Al| and at the front end
of |Ar| */

T.clear(); 

while (!Al.empty() && source(Al.tail()) == G[parent[w]])   
// |parent[w]| is in |G|, |Al.tail| in |H|
{ T.push(Al.Pop()); //Pop removes from the rear
}

T.append(G[tree_edge_into[w]]); // push would be equivalent

while (!Ar.empty() && source(Ar.head()) == G[parent[w]])   // 
{ T.append(Ar.pop()); // pop removes from the front
}

  w = parent[w]; 
} //end while

  
A.clear(); 

A.conc(Ar); 
A.append(H.reversal(G[back_edge_into_w0])); 
A.conc(Al); 

}


bool HT_PLANAR(graph& Gin, bool embed)
/* |Gin| is a directed graph. HT\_PLANAR decides whether |Gin| is planar.
 * If it is and |embed == true| then it also computes a 
 * combinatorial embedding of |Gin| by suitably reordering 
 * its adjacency lists. |Gin| must be a map in that case. */

{ 
  GRAPH <node,edge> G;
  edge_array <edge> companion_in_G(Gin,nil);
  node_array <node> link(Gin);
  bool Gin_is_map = Gin.make_map();
  if ( embed && !Gin_is_map )
    LEDA_EXCEPTION(1,"HT_PLANAR: sorry: can only embed maps");

  edge_array<int> offset(Gin,0);

  copy_simplified_graph(Gin,G,link,companion_in_G,offset,Gin_is_map);

  //if (embed) assert(G.is_map());

  int n = G.number_of_nodes();
  int m = G.number_of_edges();

  if (n >= 3 && m > 6*n - 12) return false; 

  /* An undirected planar graph with three or more nodes 
   * has at most $3n - 6$ edges; a directed graph may
   * have twice as many */

  // for some strange region the planarity test and the embedding 
  // function want at least three nodes

  for (int i = 0; i < 3 - n; i++) G.new_node();

  list<edge> el = Make_Biconnected(G);

  edge e;
  forall(e,el) 
  { edge r = G.new_edge(target(e),source(e));
    G.set_reversal(e,r);
  }

  if ( !Gin_is_map ) G.make_map(el);
  //else assert(G.is_map()); 


  
  GRAPH<node,edge> H;
  edge_array<edge> companion_in_H(Gin);
  
  { node v;
    edge e;
    forall_nodes(v, G) G[v] = H.new_node(v);
    forall_edges(e, G) G[e] = H.new_edge(G[source(e)],G[target(e)],e);
    forall_edges(e, Gin) 
    { if ( Gin.source(e) != Gin.target(e) )
      companion_in_H[e] = G[companion_in_G[e]];
    }
    forall_edges(e, G) 
      H.set_reversal(G[e],G[G.reversal(e)]);
  }
 // assert(H.is_map());

  
node_array<int> dfsnum(G); 
node_array<node> parent(G,nil); 

reorder(G,dfsnum,parent); 

edge_array<int> alpha(G,0); 

{ list<int> Att; 

  alpha[G.first_adj_edge(G.first_node())] = left_side; 

  if (!strongly_planar(G.first_adj_edge(G.first_node()),G,Att,alpha,dfsnum,parent))
  return false; 
}


  if (embed) 
  { 
{ list<edge> T,A; // lists of edges of |H|

  int cur_nr = 0; 
  edge_array<int> sort_num(H); 
  node_array<edge> tree_edge_into(G); 

  embedding(G.first_adj_edge(G.first_node()),left_side,G,alpha,dfsnum,T,A,
  cur_nr,sort_num,tree_edge_into,parent,H); //,reversal); 

  /* |T| contains all edges incident to the first node except the cycle edge into it. 
  That edge comprises |A| */

  T.conc(A); 
  edge e; 

  forall(e,T) sort_num[e] = cur_nr++; 

  edge_array<double> sort_Gin(Gin,0);

  int mh = H.number_of_edges();  // mh - 1 is maximal value for sort_num
  double two_m = 2*(1 + Gin.number_of_edges());            

                                 // maximal absolute value of offset is
                                 // Gin.number_of_edges

  edge ein;
  forall_edges(ein,Gin) 
  if ( Gin.source(ein) == Gin.target(ein) )
    sort_Gin[ein] = two_m * ( mh + node_ord[Gin.source(ein)]) + offset[ein];
  else 
    sort_Gin[ein] = two_m * sort_num[companion_in_H[ein]] + offset[ein]; 
  
  Gin.sort_edges(sort_Gin); 

}

                            }
  return true; 
}


bool HT_PLANAR(graph& G, list <edge>& P, bool embed)
{ 
  if (HT_PLANAR(G, embed)) return true; 


  GRAPH<node,edge> H; 
  node_array<node> link(G); 
  node v; 
  forall_nodes(v,G) link[v] = H.new_node(v); 


  edge e; 
  forall_edges(e,G) H.new_edge(link[source(e)],link[target(e)],e); 
 

  list<edge> L = H.all_edges(); 

  edge eh; 

  forall(eh,L)
  { e = H[eh]; // the edge in |G| corresponding to |eh|
    node x = source(eh); node y = target(eh); 
    H.del_edge(eh); 
    if (HT_PLANAR(H,false)) 
    H.new_edge(x,y,e); //put a new version of |eh| back in and establish the correspondence
      
  }


  P.clear(); 

  forall_edges(eh,H) P.append(H[eh]); 

  return false; 

}


LEDA_END_NAMESPACE
