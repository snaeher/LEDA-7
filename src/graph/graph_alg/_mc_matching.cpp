/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _mc_matching.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/mc_matching.h>
#include <LEDA/graph/node_slist.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/array.h>

#include <LEDA/system/assert.h>


LEDA_BEGIN_NAMESPACE

enum LABEL {ODD, EVEN, UNLABELED};

/*
class node_partition {
public:

 node_partition(const graph& G) {}

 node find(node v) const { return v; }
 node operator()(node v) const { return find(v); }
 void union_blocks(node v, node w) {}
 void make_rep(node v) {}
 void split(const list<node>& L) {}
};
*/

#define return_false(s) \
{ cerr << "CHECK_MAX_CARD_MATCHING: " << s << endl; return false; }

bool CHECK_MAX_CARD_MATCHING(const graph& G, const list<edge>& M,
                                    const node_array<int>& OSC)
{ int n = leda_max(2,G.number_of_nodes());
  int K = 1;
  array<int> count(n);
  int i;
  for (i = 0; i < n; i++) count[i] = 0;
  node v; edge e;

  forall_nodes(v,G) 
  { if ( OSC[v] < 0 || OSC[v] >= n ) 
     return_false("negative label or label larger than n - 1");
    count[OSC[v]]++;
    if (OSC[v] > K) K = OSC[v];
  }

  int S = count[1];
  for (i = 2; i <= K; i++) S += count[i]/2;
  if ( S != M.length() )
    return_false("OSC does not prove optimality");

  forall_edges(e,G)
  { node v = G.source(e);
    node w = G.target(e);
    if ( v == w || OSC[v] == 1 || OSC[w] == 1 ||
            ( OSC[v] == OSC[w] && OSC[v] >= 2) ) continue;
    return_false("OSC is not a cover");
  }
  return true;
}




static void shrink_path(node b, node v, node w, 
          node_partition& base, node_array<node>& mate, 
          node_array<node>& pred, node_array<node>& source_bridge, 
          node_array<node>& target_bridge, node_slist& Q)
{ node x = base(v);
  while (x != b)
  { base.union_blocks(x,b);
    x = mate[x];
    base.union_blocks(x,b);
    base.make_rep(b);
    Q.append(x);
    source_bridge[x] = v;  
    target_bridge[x] = w;
    x = base(pred[x]);
  }
}


static void find_path(list<node>& P, node x, node y,
                 node_array<int>&  label, node_array<node>& pred,
                 node_array<node>& mate, 
                 node_array<node>& source_bridge,
                 node_array<node>& target_bridge)
{ if ( x == y ) 
  {
    P.append(x);  
    return;
  }

  if ( label[x] == EVEN ) 
  { 
    P.append(x);
    P.append(mate[x]);
    find_path(P,pred[mate[x]],y,label,pred,mate,
                   source_bridge,target_bridge);   
    return;
  }
  else // x is ODD
  {  
    P.append(x);

    list<node> P2;
    find_path(P2,source_bridge[x],mate[x],label,pred,mate,
                             source_bridge,target_bridge);  
    P2.reverse_items();
    P.conc(P2);

    find_path(P,target_bridge[x],y,label,pred,mate,
                      source_bridge,target_bridge);
  
    return;
  }
}


list<edge> MAX_CARD_MATCHING_EDMONDS(const graph& G, node_array<int>& OSC, 
                                                     int heur)
{ 

    node_array<node> mate(G,nil);

    node_partition base(G);    // now base(v) = v for all nodes v

{ node v;
  forall_nodes(v,G) assert(base(v) == v);
}

    node_array<int>  label(G,EVEN);     
    node_array<node> pred(G,nil);


    double strue = 0;
    node_array<double>  path1(G,0);
    node_array<double>  path2(G,0);


    node_array<node> source_bridge(G,nil);
    node_array<node> target_bridge(G,nil);
  
    switch (heur) {

    case 0: break;

    case 1: { edge e;
              forall_edges(e,G)
              { node v = G.source(e); node w = G.target(e);
                if ( v != w && mate[v] == nil && mate[w] == nil)
                { mate[v] = w; label[v] = UNLABELED;
                  mate[w] = v; label[w] = UNLABELED;
                }
              }
              break;
            }
    }


  node v; edge e;

  forall_nodes(v,G)
  { if ( mate[v] != nil ) continue;
    
    node_slist Q(G); Q.append(v);
    list<node> T; T.append(v);
    bool breakthrough = false;
  
    while (!breakthrough && !Q.empty()) // grow tree rooted at v
    {
      node v = Q.pop();
      
      forall_inout_edges(e,v)
      { node w = G.opposite(v,e); 

        if ( base(v) == base(w) || label[base(w)] == ODD ) 
           continue;   // do nothing

        if ( label[w] == UNLABELED )
          { 
            label[w] = ODD;            T.append(w);
            pred[w] = v;
            label[mate[w]] = EVEN;     T.append(mate[w]);
            Q.append(mate[w]);
 }
        else  // base(w) is EVEN
          { 
            node hv = base(v);
            node hw = base(w);

            strue++; 
            path1[hv] = path2[hw] = strue;
              
            while ((path1[hw] != strue && path2[hv] != strue) &&
                   (mate[hv] != nil || mate[hw] != nil) )
            { 
              if (mate[hv] != nil)
              { hv = base(pred[mate[hv]]);
                path1[hv] = strue;
              }
              
              if (mate[hw] != nil)
              { hw = base(pred[mate[hw]]);
                path2[hw] = strue;
              }
            }
              
            if (path1[hw] == strue || path2[hv] == strue) 
              { 
                node b = (path1[hw] == strue) ? hw : hv;    // Base

                shrink_path(b,v,w,base,mate,pred,source_bridge,target_bridge,Q);
                shrink_path(b,w,v,base,mate,pred,source_bridge,target_bridge,Q);
 } 
            else  
              { 
                list<node> P;

                find_path(P,v,hv,label,pred,mate,source_bridge,target_bridge);
                P.push(w);

                while(! P.empty())
                { node a = P.pop();
                  node b = P.pop();
                  mate[a] = b;
                  mate[b] = a;
                }

                T.append(w);
                forall(v,T) label[v] = UNLABELED;  
                base.split(T);

                breakthrough = true;
                break;        
 } 
 } 
      } 
  
    }
  }


  list<edge> M;
  
   forall_edges(e,G) 
   { node v = source(e);
     node w = target(e);
     if ( v != w  &&  mate[v] == w ) 
     { M.append(e);
       mate[v] = v;
       mate[w] = w;
      }
    }


  forall_nodes(v,G) OSC[v] = -1;

  int number_of_unlabeled = 0;
  node arb_u_node = 0;

  forall_nodes(v,G) 
   if ( label[v] == UNLABELED ) 
   { number_of_unlabeled++;
     arb_u_node = v;
   }

  int L = 0;
  if ( number_of_unlabeled > 0 )
  { OSC[arb_u_node] = 1;
    if (number_of_unlabeled > 2) L = 2;
    forall_nodes(v,G) 
      if ( label[v] == UNLABELED && v != arb_u_node ) OSC[v] = L;
  }

  int K = ( L == 0? 2 : 3);

  forall_nodes(v,G)
   if ( base(v) != v && OSC[base(v)] == -1 ) OSC[base(v)] = K++;

  forall_nodes(v,G)
  { if ( base(v) == v && OSC[v] == -1 )
    { if ( label[v] == EVEN ) OSC[v] = 0;
      if ( label[v] == ODD  ) OSC[v] = 1;
    }
    if ( base(v) != v ) OSC[v] = OSC[base(v)];
  }

 return M;
}


list<edge> MAX_CARD_MATCHING_EDMONDS(const graph& G, int heur)
{ 
  if (G.is_directed())
  { node_array<int> OSC(G);
    return MAX_CARD_MATCHING_EDMONDS(G,OSC,heur);
   }

  // undirected graphs (sn. Jan 2010)
  // construct directed copy G1 ofG 

  GRAPH<node,edge> G1;
  node_array<node> V(G,NULL);
  edge_array<edge> E(G,NULL);

  node u;
  forall_nodes(u,G) V[u] = G1.new_node(u);

  edge x;
  forall_edges(x,G) 
  { node a = V[G.source(x)];
    node b = V[G.target(x)];
    E[x] = G1.new_edge(a,b,x);
   }

  // run matching on directed graph
  
  list<edge> M = MAX_CARD_MATCHING_EDMONDS(G1,heur);

  // translate result back to original undirected graph

  list<edge> result;
  edge e;
  forall(e,M) result.append(G1[e]);

  return result;
}

LEDA_END_NAMESPACE
