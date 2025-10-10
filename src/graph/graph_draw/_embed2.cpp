/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _embed2.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//-------------------------------------------------------------------------
// Dirk Ambras  1995 
//-------------------------------------------------------------------------


#include <LEDA/graph/graph_alg.h>

LEDA_BEGIN_NAMESPACE

static void Contract(graph& G, node a, node b , node c, list<node>& L)
{
  node		   v,w;
  list<node>       cand;
  node_array<bool> marked(G,false);			// betrachtete Knoten
  node_array<int>  deg(G,0);				// # virtuelle Nachbarn

  int N = G.number_of_edges();

  marked[a] = marked[b] = marked[c] = true;			// Init

  deg[a] = deg[b] = deg[c] = N;

  forall_adj_nodes(v,a)
  { marked[v]=true;
    forall_adj_nodes(w,v) deg[w]++;		// mache v bekannt bei den w's
   }

  { forall_adj_nodes(v,a)                         // lade Kandidaten
     if (deg[v]<=2) cand.append(v);	
   }

  while (!cand.empty())
  {  node u=cand.pop();
     if (deg[u] == 2)
     {  L.push(u);
        deg[u]=N;
	forall_adj_nodes(v, u)
	{ deg[v]--;				// u ist virtuell geloescht
          if (!marked[v])			// v ist neuer Nachbar von a
          { marked[v]=true;
            forall_adj_nodes(w,v) deg[w]++;	// mache v bekannt bei den w's
            if (deg[v] <= 2) cand.append(v);	// lade Kandidaten
          } else
          if (deg[v] == 2) cand.append(v);
        }
     }
  }
}


static void Realizer(graph& G, const list<node>& L, 
                     node a, node b, node c,
                     GRAPH<node, int>& T, node_array<node>& v_in_T)
{
  int  i=0;
  node v;
  edge e;
  node_array<int> ord(G,0);

  ord[b] = i++; 
  ord[c] = i++;
  node u;
  forall(u,L) ord[u]=i++;				// V(G) numerieren
  ord[a] = i++; 

  forall_nodes(v, G) v_in_T[v] = T.new_node();		// T = copy of G

  forall(v, L)
  { node u = v_in_T[v];   // u is copy of v in T

    forall_adj_edges(e, v)
       if (ord[G.target(e)] > ord[v]) break;

    edge e1 = e;
    while(ord[G.target(e1)] > ord[v]) e1 = G.cyclic_adj_succ(e1);
    T.new_edge(v_in_T[G.target(e1)], u, 2);

    edge e2 = e;
    while(ord[G.target(e2)] > ord[v]) e2 = G.cyclic_adj_pred(e2);
    T.new_edge(v_in_T[G.target(e2)], u, 3);

    for(e=G.cyclic_adj_succ(e1); e != e2; e=G.cyclic_adj_succ(e))
      T.new_edge(u, v_in_T[G.target(e)], 1);
  }

  // special treatement of a,b,c

  node a_in_T = v_in_T[a];
  node b_in_T = v_in_T[b];
  node c_in_T = v_in_T[c];

  forall_adj_edges(e,a)			
  T.new_edge(a_in_T, v_in_T[G.target(e)], 1);

  T.new_edge(b_in_T, a_in_T, 2);
  T.new_edge(b_in_T, c_in_T, 2);

  T.new_edge(c_in_T, a_in_T, 3);
  T.new_edge(c_in_T, b_in_T, 3);

}


static void Subtree_Sizes(GRAPH<node, int>& T, int i, node r,
                          node_array<int>& size)
{
  // computes sizes of all subtrees of tree with root r in T(i)

  int  sum=0;
  edge e;
  forall_adj_edges(e, r)  
    if (T[e]==i)
    { node w=T.target(e);
      Subtree_Sizes(T, i, w, size);
      sum+=size[w];
    }
  size[r]=sum+1;
}


static void Prefix_Sum(GRAPH<node, int>& T, int i, node r,
                       const node_array<int>& val, node_array<int>& sum)
{

  // computes for every node u in the subtree of T(i) with root r
  // the sum of all val[v] where v is a node on the path from r to u

  list<node> Q;

  Q.append(r);
  sum[r] = val[r];

  while (!Q.empty())
  { node v=Q.pop();
    edge e;
    forall_adj_edges(e, v)  
      if (T[e] == i)
      { node w=T.target(e);
        Q.append(w);
        sum[w] = val[w] + sum[v];
      }
  }
}


int STRAIGHT_LINE_EMBEDDING2(graph& G,node_array<int>& xcoord,
                                      node_array<int>& ycoord)
{
  int n = G.number_of_nodes();

  if (n < 3)
  { int max_c = 1;
    if (n > 0) 
    { node a = G.first_node();
      xcoord[a] = 1;
      ycoord[a] = 1;
     }
    if (n > 1) 
    { node b = G.last_node();
      xcoord[b] = 2;
      ycoord[b] = 2;
      max_c = 2;
     }
    return max_c;
   }

  node		   v;
  list<node>       L;
  GRAPH<node, int> T;
  node_array<node> v_in_T(G);

   
  list<edge> el = G.triangulate_map();

  // choose outer face a,b,c


  node a=G.first_node();  
  edge e=G.first_adj_edge(a);	
  node c=G.target(e);  
  node b = G.target(G.adj_succ(e));


  Contract(G,a,b,c,L);

  Realizer(G,L,a,b,c,T,v_in_T);				// T aufbauen

  node_array<int>  t1(T);
  node_array<int>  t2(T);
  node_array<int>  val(T,1);

  node_array<int>  P1(T);
  node_array<int>  P3(T);
  node_array<int>  v1(T);
  node_array<int>  v2(T);

  Subtree_Sizes(T, 1, v_in_T[a], t1);
  Subtree_Sizes(T, 2, v_in_T[b], t2);

  Prefix_Sum(T, 1, v_in_T[a], val, P1);
  Prefix_Sum(T, 3, v_in_T[c], val, P3);
  // now Pi  =  depth of all nodes in Tree T(i) (depth[root] = 1)


  Prefix_Sum(T, 2, v_in_T[b], t1, v1);
  v1[v_in_T[a]] = t1[v_in_T[a]];	// Sonderrolle von a

  // in v1[v] steht jetzt die Summe (Anzahl der Knoten im T1-UBaum[x])
  // fuer jeden Knoten x im Pfad in T2 von b nach v

  Prefix_Sum(T, 3, v_in_T[c], t1, val);
  val[v_in_T[a]]=t1[v_in_T[a]];				// Sonderrolle von a

  // in val[v] steht jetzt die Summe (Anzahl der Knoten im T1-UBaum[x])
  // fuer jeden Knoten x im Pfad in T3 von c nach v
  // es ist r1[v]=v1[v]+val[v]-t1[v] die Anzahl der Knoten in der
  // Region 1 von v

  forall_nodes(v, T) v1[v] += val[v]-t1[v]-P3[v];	// v1' errechnen

  Prefix_Sum(T, 3, v_in_T[c], t2, v2);
  v2[v_in_T[b]]=t2[v_in_T[b]];				// Sonderrolle von b

  Prefix_Sum(T, 1, v_in_T[a], t2, val);
  val[v_in_T[b]]=t2[v_in_T[b]];				// Sonderrolle von b

  forall_nodes(v, T) v2[v] += val[v]-t2[v]-P1[v];	// v2' errechnen

  int maxcoord = 0;

  forall_nodes(v, G)					// x- & y-Feld kopieren
  { xcoord[v] = v1[v_in_T[v]];  
    ycoord[v] = v2[v_in_T[v]]; 
    maxcoord = leda_max(maxcoord,leda_max(xcoord[v],ycoord[v]));
   }

  forall(e, el) G.del_edge(e);	                        // eingefuegte Kanten
                                                        // loeschen
  return maxcoord;
}


void STRAIGHT_LINE_EMBEDDING2(graph& G,node_array<double>& x, node_array<double>& y)
{ 
  node_array<int> x0(G);
  node_array<int> y0(G);

  int maxc = STRAIGHT_LINE_EMBEDDING2(G,x0,y0);

  node v;
  forall_nodes(v,G) 
  { x[v] = double(x0[v])/maxc;
    y[v] = double(y0[v])/maxc; 
   }
}

LEDA_END_NAMESPACE

