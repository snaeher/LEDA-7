/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  dc_delaunay.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/array.h>
#include <LEDA/graph/graph.h>
#include <LEDA/geo/geo_global_enums.h>

LEDA_BEGIN_NAMESPACE

inline edge make_edge(GRAPH<POINT,int>& G, node v, node w)
{ edge e_vw = G.new_edge(v,w,DIAGRAM_EDGE);
   edge e_wv = G.new_edge(w,v,DIAGRAM_EDGE);
   G.set_reversal(e_vw,e_wv);
   return e_wv;
 }


inline edge make_triangle(GRAPH<POINT,int>& G, node v, node w, node x)
{ // precondition: (v,w,x) is a leftturn
   edge e_wv = make_edge(G,v,w);
   make_edge(G,v,x);
   make_edge(G,w,x);
   return e_wv;
}


inline bool leftturn(const GRAPH<POINT,int>& G, edge e, node v)
{ return orientation(G[source(e)],G[target(e)],G[v]) > 0; }


inline int side_of_circle(const GRAPH<POINT,int>& G, node v, edge e, edge f)
{ return side_of_circle(G[source(e)],G[target(e)],G[target(f)],G[v]); }



static void merge_halves(GRAPH<POINT,int>& G, edge e_l, edge e_r,
                                                  edge& e_i, edge& e_j)
{
   edge lstair, rstair;
      {
         bool liftable_l = true, liftable_r = true;
         edge f_l = e_l;
         edge f_r = e_r;
         while (liftable_l || liftable_r)
         { 
            liftable_l = leftturn(G,f_l,G.target(f_r));
            if (liftable_l)
              f_l = G.face_cycle_succ(f_l);
            liftable_r = leftturn(G,f_r,G.source(f_l));
            if (liftable_r)
              f_r = G.face_cycle_pred(f_r);
         }
         lstair = G.new_edge(
           f_l,G.target(f_r),DIAGRAM_EDGE,leda::behind
         );
         rstair = G.new_edge(
           G.reversal(f_r),G.source(f_l),DIAGRAM_EDGE,leda::before
         );
         G.set_reversal(lstair,rstair);
      }

   if (G.target(rstair) == G.target(e_i))
      e_i = rstair;
   if (G.source(rstair) == G.source(e_j))
      e_j = rstair;

       
   edge lcand = G.cyclic_adj_succ(lstair);
   edge rcand = G.cyclic_adj_pred(rstair);
   bool lcand_valid = leftturn(G,lstair,G.target(lcand) );
   bool rcand_valid = leftturn(G,rcand ,G.target(rstair));

   while (lcand_valid || rcand_valid)
   {
      if (lcand_valid)
        {
           edge lcand_rev = G.reversal(lcand), lnext;
           node rnode = G.source(rstair);
           int side=1;
           bool lnext_valid=true;
           while ((side > 0) && lnext_valid)
           {
              lnext = G.cyclic_adj_succ(lcand);
              lnext_valid = (lnext != lstair) 
                            && leftturn(G,lcand,G.target(lnext));
              lcand_rev = G.reversal(lcand);
              if (lnext_valid)
              {
                 side = side_of_circle(G,rnode,lcand,lnext);
                 if (side == 0)
                    G[lcand_rev] = G[lcand] = NON_DIAGRAM_EDGE;
                 if (side > 0)
                 {
                    G.del_edge(lcand);
                    G.del_edge(lcand_rev);
                    lcand = lnext;
                 }
              }
           }
        }

      if (rcand_valid)
        {
           edge rcand_rev = G.reversal(rcand), rnext;
           node lnode = G.source(lstair);
           int side=1;
           bool rnext_valid=true;
           while ((side > 0) && rnext_valid)
           {
              rnext = G.cyclic_adj_pred(rcand);
              rnext_valid = (rnext != rstair) 
                            && leftturn(G,rnext,G.target(rcand));
              rcand_rev = G.reversal(rcand);
              if (rnext_valid)
              {
                 side = side_of_circle(G,lnode,rnext,rcand);
                 if (side == 0)
                    G[rcand_rev] = G[rcand] = NON_DIAGRAM_EDGE;
                 if (side > 0)
                 {
                    G.del_edge(rcand);
                    G.del_edge(rcand_rev);
                    rcand = rnext;
                 }    
              }
           }
        }

              
      {
         int side = -1;
         bool take_lcand = lcand_valid, take_rcand = rcand_valid;
         if (lcand_valid && rcand_valid)
         {
            side = side_of_circle(G,G.target(rcand),lstair,lcand);
            if (side <= 0) take_rcand = false;
            else           take_lcand = false;    
         }
         if (take_lcand)
         {
            rstair = G.new_edge(
               rstair,G.target(lcand),DIAGRAM_EDGE,leda::before
            );
            lstair = G.new_edge(
               G.reversal(lcand),G.source(rstair),DIAGRAM_EDGE,leda::behind
            );
            if (side == 0)
               G[rstair] = G[lstair] = NON_DIAGRAM_EDGE;
         }
         if (take_rcand)
         {
            lstair = G.new_edge(
               lstair,G.target(rcand),DIAGRAM_EDGE,leda::behind
            );
            rstair = G.new_edge(
               G.reversal(rcand),G.source(lstair),DIAGRAM_EDGE,leda::before
            );
         }
         G.set_reversal(lstair,rstair);
      }





      lcand = G.cyclic_adj_succ(lstair);
      lcand_valid = leftturn(G,lstair,G.target(lcand) );
      rcand = G.cyclic_adj_pred(rstair);
      rcand_valid = leftturn(G,rcand ,G.target(rstair));
   }
   
}
  

static void compute_Delaunay_Triangulation( GRAPH<POINT,int>& G,
                                            const array<POINT>& A,
                                            int i, int j,
                                            edge& e_i, edge& e_j)
{
  // precondition: A[i] < A[i+1] < ... A[j] 
  // (in lexicographic order)
  if (j <= i+2)
     {
        if (j == i+1) 
          {
             node v = G.new_node(A[i]); 
             node w = G.new_node(A[j]); 
             e_i = e_j = make_edge(G,v,w);
          }
  
        if (j == i+2) 
          {
             node v = G.new_node(A[i]);
             node w = G.new_node(A[i+1]); 
             node x = G.new_node(A[i+2]); 
             
             int side = orientation(A[i],A[i+1],A[i+2]);
             if (side > 0)
             {
                e_i = make_triangle(G,v,w,x);
                e_j = G.face_cycle_pred(e_i);
             }
             if (side < 0) 
                e_i = e_j = make_triangle(G,v,x,w);
             if (side == 0)
             {
                e_i = make_edge(G,v,w);
                e_j = make_edge(G,w,x);
             }  
          }
 
     }

  else
  {
     int m = (i+j)/2;
     edge e_l, e_r;
     compute_Delaunay_Triangulation(G, A,  i , m, e_i, e_l);
     compute_Delaunay_Triangulation(G, A, m+1, j, e_r, e_j);
     merge_halves(G,e_l,e_r,e_i,e_j);
  }
}


// CB: die naechste Prozedur hab ich eingefuegt, um die labels in
// Ordnung zu bringen.
// Dies ersetzt den Abschnitt ''remove completion edges'' im .nw file!
// Dadurch musste auch die Funktion check_Delaunay_Graph geandert werden

static void cleanup_labels(GRAPH<POINT,int>& G, edge start)
{
   edge e = start;
   do
   { 
// SN:  
//   edge e_rev = G.reversal(e);
//   G[e_rev] = G[e] = HULL_EDGE;
     G[e] = HULL_EDGE;
     e = G.face_cycle_succ(e);
   }
   while (e != start);
}


static void check_Delaunay_Graph(GRAPH<POINT,int>& G, list<POINT> L, edge start_edge)
{
   {
      L.sort();

      node v;
      list<POINT> GL;
      forall_nodes(v,G)
        GL.append(G.inf(v));
      GL.sort();

      if (L.length() != GL.length())
         LEDA_EXCEPTION(1,"error: number of points wrong");

      list_item it1 =  L.first();
      list_item it2 = GL.first();
      while(it1)
      {
         if (L.contents(it1) != GL.contents(it2))
            LEDA_EXCEPTION(1,"error: points in graph wrong");
         it1 =  L.succ(it1);
         it2 = GL.succ(it2);
      }

   }

   // CB Der folgende Block ist neu geschrieben, der check erweitert
   {
      node v;
      edge e = start_edge, e_rev;
      node start_node = G.source(start_edge);
      int number_hull_edges=0, labelled_hull_edges=0; 
      do
      {
         v = G.source(e);
         e = G.face_cycle_succ(e);
         e_rev = G.reversal(e);
         number_hull_edges+=2;
         if ((G[e] != HULL_EDGE) || (G[e_rev] != HULL_EDGE) )
            LEDA_EXCEPTION(1,"error: hull label wrong");
         if (leftturn(G,e,v))
            LEDA_EXCEPTION(1,"error: hull not locally convex");
         if (leftturn(G,e,start_node))
            LEDA_EXCEPTION(1,"error: hull not convex or not simple");
         if (leftturn(G,start_edge,v))
            LEDA_EXCEPTION(1,"error: hull not convex or not simple");
      }
      while (e!= start_edge);
      forall_edges(e,G)
         if (G[e] == HULL_EDGE)
           labelled_hull_edges++;
      if (number_hull_edges != labelled_hull_edges)
         LEDA_EXCEPTION(1,"error: hull label wrong");
   }


   {
      node v;
      edge e, e_rev, e_pre, e_opp;
      forall_edges(e,G)
      {
         e_rev = G.reversal(e);
         e_pre = G.face_cycle_pred(e);
         e_opp = G.face_cycle_succ(e_rev);
         v = G.target(e_opp);

         // CB: Die Aenderung in cleanup_labels erzwingt diese 
         //     Aenderung in check_Delaunay_graph:

         if (G[e] != G[e_rev])
           LEDA_EXCEPTION(1,"error: edge label wrong");
         switch(G[e])
         {
            case DIAGRAM_EDGE:
               if (side_of_circle(G,v,e,G.reversal(e_pre)) >= 0)
                  LEDA_EXCEPTION(1,
                    "check_Delaunay_Graph: incircle test failed");
               break;
            case NON_DIAGRAM_EDGE:
               if (side_of_circle(G,v,e,G.reversal(e_pre)) != 0)
                  LEDA_EXCEPTION(1,
                    "check_Delaunay_Graph: NON_DIAGRAM_EDGE wrong");
            case HULL_EDGE:
               break;
         }

      }
   }

}


void DELAUNAY_STOLFI(const list<POINT>& S0, 
                     GRAPH<POINT,int>& G, bool with_check)
{
   list<POINT> S = S0;
   G.clear();
   if (S.empty()) return;

   S.sort();

   array<POINT> A(S.length());

/*
   int n;
   {
      list_item it, ne;
      n = 0;
      forall_items(it,S)
      {
         while (
           (ne = S.succ(it)) && (S.contents(ne) == S.contents(it))
         )
            S.del_item(ne);
         A[n++] = S.contents(it);
      }
   }
*/


   int n = 0;
   {
     A[0] = S.pop();
     POINT p;
     forall(p,S) if (p != A[n]) A[++n] = p;
     n++;
   }


   if (n == 1) 
   { G.new_node(A[0]);
     return;
    }

   edge e, f;
   compute_Delaunay_Triangulation(G, A, 0, n-1, e, f);
   // CB: der Aufruf von cleanup labels ist neu:
   cleanup_labels(G,e);
   if (with_check) check_Delaunay_Graph(G,S,e);
}




void DELAUNAY_DWYER(const list<POINT>& S0, GRAPH<POINT,int>& G, bool with_check)
{  
   list<POINT> S = S0;
   G.clear();
   if (S.empty()) return;

   S.sort(POINT::cmp_yx);

   array<POINT> A(S.length());

/*
   int n;
   {
      list_item it, ne;
      n = 0;
      forall_items(it,S)
      {
         while (
           (ne = S.succ(it)) && (S.contents(ne) == S.contents(it))
         )
            S.del_item(ne);
         A[n++] = S.contents(it);
      }
   }
*/

   int n = 0;
   {
     A[0] = S.pop();
     POINT p;
     forall(p,S) if (p != A[n]) A[++n] = p;
     n++;
   }


   if (n == 1) 
   { G.new_node(A[0]);
     return;
   }

   int m = (int) ::floor(::sqrt(double(n)*log(2.0)/log(double(n))));
   int size = (int) ::ceil(n/double(m));

   //array<edge> u(m), l(m);
   edge* u = new edge[m];
   edge* l = new edge[m];

   int low, high;
   {
      edge e, f;
      for(int i=0; i<m; i++)
      {
         low = i*size;
         high = (i+1)*size-1;
         if (high > n-1) high = n-1;
         A.sort(low,high);
         compute_Delaunay_Triangulation(G, A, low, high, e, f);
         if (POINT::cmp_yx(G[G.source(e)],G[G.target(e)]) >= 0)
            e = G.face_cycle_succ(e);
         while (POINT::cmp_yx(G[G.source(e)],G[G.target(e)]) < 0)
            e = G.face_cycle_succ(e);
         u[i] = e;
         while (POINT::cmp_yx(G[G.source(f)],G[G.target(f)]) > 0)
            f = G.face_cycle_succ(f);
         f = G.face_cycle_pred(f);
         l[i] = f;
      }
   }

   for(int k=1; k < n;k*=2)
      for(int j=0; j< m-k; j+=2*k)
      {
         merge_halves(G,u[j],l[j+k],l[j],u[j+k]);
         u[j] = u[j+k];
      }

   // CB: der Aufruf von cleanup labels ist neu:
   cleanup_labels(G,l[0]);

   if (with_check)
      check_Delaunay_Graph(G,S,l[0]);
  
   delete[] u;
   delete[] l;
}

LEDA_END_NAMESPACE

