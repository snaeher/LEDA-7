/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _g_random.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/graph_misc.h>

//------------------------------------------------------------------------------
// generators for random graph
//
// S. Naeher
//------------------------------------------------------------------------------


// we use the global random integer source "rand_int"


// to become part of _g_random.c

LEDA_BEGIN_NAMESPACE

void random_graph_noncompact(graph& G, int n, int m)
{ 
  node*  V = new node[n];
  int i;

  G.clear();

  for(i=0; i<n; i++) V[i] = G.new_node();

  for(i = 0; i < m; i++) 
    G.new_edge(V[rand_int(0,n-1)],V[rand_int(0,n-1)]);

  delete[] V;
}

LEDA_END_NAMESPACE

#include <LEDA/graph/node_map2.h>
#include <LEDA/core/array.h>  

LEDA_BEGIN_NAMESPACE

void random_graph_compact(graph& G, int n, int m, 
                          bool no_anti_parallel_edges,
                          bool loopfree,
                          bool no_parallel_edges)
{ if ( n == 0 && m > 0 )
    LEDA_EXCEPTION(1,"random graph: m to big");
  if ( n == 1 && m > 0 && loopfree )
    LEDA_EXCEPTION(1,"random graph: m to big");

  node*  V = new node[n];
  int* deg = new int[n];
  int i;

  G.clear();

  for (i = 0; i < n; i++) 
  { V[i] = G.new_node();
    deg[i] = 0;
  }

  if ( !no_anti_parallel_edges && !no_parallel_edges )
  { 
    for (i = 0; i < m; i++) deg[rand_int(0,n-1)]++;

    for (i = 0; i < n; i++) 
    { node v = V[i];
      int  d = deg[i];
      while ( d > 0 ) 
      { int j = rand_int(0,n-1);
        if ( loopfree && j == i ) continue;
        G.new_edge(v,V[j]); 
        d--;
      }
    }
  }
  else { 
         
         double md = m; double nd = n;

         if ( no_parallel_edges && !loopfree && 
              !no_anti_parallel_edges && md > nd*nd)
           LEDA_EXCEPTION(1,"random graph: m too big");

         
         if ( no_parallel_edges && loopfree && 
              !no_anti_parallel_edges && md > nd*(nd-1))
           LEDA_EXCEPTION(1,"random graph: m too big");

         if ( no_parallel_edges && loopfree && 
              no_anti_parallel_edges && md > nd*(nd-1)/2)
           LEDA_EXCEPTION(1,"random graph: m too big");

         if ( no_parallel_edges && !loopfree && 
              no_anti_parallel_edges && md > nd + nd*(nd-1)/2)
           LEDA_EXCEPTION(1,"random graph: m too big");



         node_map2<bool> C(G,true);
         array<list<node> > E(n);

         int i = m;
         while ( i > 0 )
         { int vi = rand_int(0,n-1);
           node v = V[vi];
           node w = V[rand_int(0,n-1)];
           if ( (v == w && loopfree) || !C(v,w) ) continue;
           E[vi].append(w);
           if ( no_parallel_edges ) C(v,w) = false;
           if ( no_anti_parallel_edges ) C(w,v) = false;
           i--;
         }
         for (i = 0; i < n; i++)
         { node v = V[i];
           node w = 0;
           forall(w,E[i]) G.new_edge(v,w);
         }
 }

  delete[] V;
  delete[] deg;
}


void random_graph(graph& G, int n, double p)
{ // G_{n,p} model

  node*  V = new node[n];
  random_source S;
  int i, j;

  G.clear();

  for(i=0; i<n; i++) V[i] = G.new_node();

  for(i = 0; i < n; i++) 
    for (j = 0; j < n; j++)
    { double c; S >> c;
      if (c < p) G.new_edge(V[i],V[j]);
    }

  delete[] V;
}




/* is now derived from the function above in graph_gen.c 

void random_graph(graph& G, int n, int m)
{ // random graph with n nodes and m edges  

  node*  V = new node[n];
  int* deg = new int[n];
  int i;

  G.clear();

  for(i=0; i<n; i++) 
  { V[i] = G.new_node();
    deg[i] = 0;
   }

  for(i=0; i<m; i++) deg[rand_int(0,n-1)]++;

  for(i=0; i<n; i++) 
  { node v = V[i];
    int  d = deg[i];
    while (d--) G.new_edge(v,V[rand_int(0,n-1)]);
   }

  delete[] V;
  delete[] deg;
}
*/


void random_ugraph(graph& G, int n, int m)
{ int i;
  node* V = new node[n];

  G.clear();

  for(i=0;i<n;i++) V[i] = G.new_node();

  while (m--) G.new_edge(V[rand_int(0,n-1)],V[rand_int(0,n-1)]);
}


void random_bigraph(graph& G, int na, int nb, int m,
            list<node>& A, list<node>& B, int k)
{  G.clear();
   if ( na < 0 || nb < 0 || m < 0 ) 
     LEDA_EXCEPTION(1,"random_bigraph: one of na, nb, or m < 0");
   node* AV = new node[na];
   node* BV = new node[nb];

   A.clear();
   B.clear();
   int a,b;

   for(a = 0; a < na; a++)  A.append(AV[a] = G.new_node());
   for(b = 0; b < nb; b++)  B.append(BV[b] = G.new_node());

   if ( na == 0 || nb == 0 || m == 0 ) return;
   if ( k < 1) LEDA_EXCEPTION(1,"random_bigraph: k < 1");

   int  d = m/na;
   if (k > na) k = na; 
   if (k > nb) k = nb; 
   int  Ka = na/k;    // group size in A
   int  Kb = nb/k;    // group size in B

   node v;
   int i;
   a = 0;

   forall(v,A)
   { int l = a/Ka;   // group of v
     if ( l == k) break;
     int base1 = (l == 0 ? (k-1)*Kb : (l-1)*Kb); 
     int base2 = (l == k-1 ? 0 : (l+1)*Kb); 
     for(i = 0; i < d; i++)
     { int b = ( rand_int(0,1) == 0? base1 : base2 );
       G.new_edge(v,BV[b + rand_int(0,Kb-1)]);
     }
     a++;
   }
   int r = m - a*d;

   while (r--) 
     G.new_edge(AV[rand_int(0,na-1)], BV[rand_int(0,nb-1)]);

   delete[] AV;
   delete[] BV;
}



/* replaced by the function above which comes from 
   bipartite cardinality matching 
void random_bigraph(graph& G,int n1,int n2,int m,list<node>& A,list<node>& B)
{
   int  d = m/n1; 
   int  r = m%n1; 

   node* AV = new node[n1];
   node* BV = new node[n2];

   A.clear();
   B.clear();
   G.clear();

   for(int a = 0; a < n1; a++)  A.append(AV[a] = G.new_node());
   for(int b = 0; b < n2; b++)  B.append(BV[b] = G.new_node());

   node v;
   int i;

   forall(v,A)
     for(i=0;i<d;i++)
       G.new_edge(v,BV[rand_int(0,n2-1)]);

   while (r--) G.new_edge(AV[rand_int(0,n1-1)], BV[rand_int(0,n2-1)]);

   delete[] AV;
   delete[] BV;
}

*/


//------------------------------------------------------------------------------
// random planar graph
//------------------------------------------------------------------------------

#if defined(__aCC__)

void random_planar_graph(graph&,node_array<double>&,node_array<double>&,int)
{ LEDA_EXCEPTION(1,"random_planar_graph(G,xc,yc,n) not implemented.");
  }

void random_planar_graph(graph& G, int n)
{ 
  int m = n + rand_int(n/2,n);

  random_planar_graph(G,n,m);

  node v;
  forall_nodes(v,G)
  { if (G.degree(v) <= 4) continue;
    edge e;
    forall_inout_edges(e,v)
    { if (G.degree(v) <= 4) break;
      G.del_edge(e);
     }
   }

  list<edge> L;
  Make_Connected(G,L);
  if (L.size() > 1) 
    G.new_edge(target(L.tail()),source(L.head()));
 }

#else


#define YNIL seq_item(nil)
#define XNIL pq_item(nil)


#define EPS  0.00001
#define EPS2 0.0000000001

class LOCAL_POINT;
class LOCAL_SEGMENT;
typedef LOCAL_POINT* point_ptr;
typedef LOCAL_SEGMENT* segment_ptr;


enum point_type {Intersection=0,Rightend=1,Leftend=2}; 

class LOCAL_POINT
{
  friend class LOCAL_SEGMENT;

  segment_ptr seg;
  int     kind;
  double  x;
  double  y;

  public:

  LOCAL_POINT(double a,double b)  
  { 
    x=a; y=b; seg=0; kind=Intersection;
   }


  LEDA_MEMORY(LOCAL_POINT);

  friend double    get_x(point_ptr p)    { return p->x; }
  friend double    get_y(point_ptr p)    { return p->y; }
  friend int       get_kind(point_ptr p) { return p->kind; }
  friend segment_ptr get_seg(point_ptr p)  { return p->seg; }   

  friend bool intersection(segment_ptr, segment_ptr, point_ptr&);
};



int compare(const point_ptr& p1, const point_ptr& p2)
{ 
  if (p1==p2) return 0;

  double diffx = get_x(p1) - get_x(p2);
  if (diffx >  EPS2 ) return  1;
  if (diffx < -EPS2 ) return -1;

  int    diffk = get_kind(p1)-get_kind(p2);
  if (diffk != 0) return diffk;

  double diffy = get_y(p1) - get_y(p2);
  if (diffy >  EPS2 ) return  1;
  if (diffy < -EPS2 ) return -1;

  return 0;
}

class LOCAL_SEGMENT
{
  point_ptr startpoint;
  point_ptr endpoint;
  double  slope;
  double  yshift;
  node  left_node;
  int   orient;
  int   color;
  int   name;


  public:

  LEDA_MEMORY(LOCAL_SEGMENT);

  LOCAL_SEGMENT(point_ptr, point_ptr,int,int);     

 ~LOCAL_SEGMENT() { delete startpoint; delete endpoint; }     



  friend point_ptr get_startpoint(segment_ptr seg)     { return seg->startpoint; }
  friend point_ptr get_endpoint(segment_ptr seg)       { return seg->endpoint; }
  friend double get_slope(segment_ptr seg)           { return seg->slope; }
  friend double get_yshift(segment_ptr seg)          { return seg->yshift; }
  friend node get_left_node(segment_ptr seg)         { return seg->left_node; }
  friend void set_left_node(segment_ptr seg, node v) { seg->left_node = v; }

  friend bool intersection(segment_ptr, segment_ptr, point_ptr&);
};




LOCAL_SEGMENT::LOCAL_SEGMENT(point_ptr p1,point_ptr p2,int c, int n)    
  {
    left_node  = nil;
    color      = c;
    name       = n;

    if (compare(p1,p2) < 0)
     { startpoint = p1; 
       endpoint = p2; 
       orient = 0;
      }
    else
     { startpoint = p2; 
       endpoint = p1; 
       orient = 1;
      }

    startpoint->kind = Leftend; 
    endpoint->kind = Rightend; 
    startpoint->seg = this; 
    endpoint->seg = this;

    if (endpoint->x != startpoint->x)
    {
      slope = (endpoint->y - startpoint->y)/(endpoint->x - startpoint->x);
      yshift = startpoint->y - slope * startpoint->x;

      startpoint->x -= EPS;
      startpoint->y -= EPS * slope;
      endpoint->x += EPS;
      endpoint->y += EPS * slope;
    }
    else //vertical segment
    { startpoint->y -= EPS;
      endpoint->y   += EPS;
      slope = 0;
      yshift = 0;
     }
  }


static double x_sweep;
static double y_sweep;


int compare(const segment_ptr& s1, const segment_ptr& s2)
{
  double y1 = get_slope(s1)*x_sweep+get_yshift(s1);
  double y2 = get_slope(s2)*x_sweep+get_yshift(s2);

  double diff = y1-y2;
  if (diff >  EPS2 ) return  1;
  if (diff < -EPS2 ) return -1;

  if (get_slope(s1) == get_slope(s2)) 
        return compare(get_x(get_startpoint(s1)), get_x(get_startpoint(s2)));

  if (y1 <= y_sweep+EPS2)
        return compare(get_slope(s1),get_slope(s2));
  else
        return compare(get_slope(s2),get_slope(s1));

}

LEDA_END_NAMESPACE

#include <LEDA/core/sortseq.h>
#include <LEDA/core/p_queue.h>

LEDA_BEGIN_NAMESPACE

static p_queue<point_ptr,seq_item>  X_structure;
static sortseq<segment_ptr,pq_item> Y_structure;


bool intersection(segment_ptr seg1,segment_ptr seg2, point_ptr& inter)
{
  if (seg1->slope == seg2->slope)
    return false;
  else
  { 
    double cx = (seg2->yshift - seg1->yshift) / (seg1->slope - seg2->slope);
 
    if (cx <= x_sweep) return false;

    if (seg1->startpoint->x > cx || seg2->startpoint->x > cx ||
        seg1->endpoint->x < cx || seg2->endpoint->x < cx ) return false;

    inter = new LOCAL_POINT(cx,seg1->slope * cx + seg1->yshift);

    return true;
  }
}



inline pq_item Xinsert(point_ptr p, seq_item i) 
{ return X_structure.insert(p,i); }


inline point_ptr Xdelete(pq_item i) 
{ point_ptr p = X_structure.prio(i);
  X_structure.del_item(i);
  return p;
 }



void random_planar_graph(graph& G, node_array<double>& xcoord,
                                   node_array<double>& ycoord, int n)
{
  point_ptr    p,inter;
  segment_ptr  seg, l,lsit,lpred,lsucc,lpredpred;
  pq_item  pqit,pxmin;
  seq_item sitmin,sit,sitpred,sitsucc,sitpredpred;

  //Kurt

  int MAX_X = 1;
  while (MAX_X < n) MAX_X *= 2;
  int MAX_Y = MAX_X;


  int N = n;  // number of random segments

  G.clear();

  xcoord.init(G,n,0);
  ycoord.init(G,n,0);


  int count=1;
 
  //initialization of the X-structure

  for (int i = 0; i < N; i++)
   { // Kurt
     //point p = new POINT(rand_int(0,MAX_X/3), rand_int(0,MAX_Y));
     //point q = new POINT(rand_int(2*MAX_X/3,MAX_X), rand_int(0,MAX_Y));
     // Stefan
     //point p = new POINT(rand_int(0,MAX_X-1), rand_int(0,MAX_Y-1));
     //point q = new POINT(rand_int(0,MAX_X-1), rand_int(0,MAX_Y-1));
     point_ptr p = new LOCAL_POINT(rand_int(0,MAX_X/2-1),     rand_int(0,MAX_Y-1));
     point_ptr q = new LOCAL_POINT(rand_int(MAX_X/2,MAX_X-1), rand_int(0,MAX_Y-1));

     seg = new LOCAL_SEGMENT(p,q,0,count++);
     Xinsert(get_startpoint(seg),YNIL);
   }

  x_sweep = -MAXINT;
  y_sweep = -MAXINT;

  while( !X_structure.empty()  && G.number_of_nodes() < n )
  {
    pxmin = X_structure.find_min();
    p = X_structure.prio(pxmin);

    sitmin = X_structure.inf(pxmin);

    Xdelete(pxmin);

    if (sitmin == YNIL) //left endpoint
    { 
      l = get_seg(p); 
      x_sweep = get_x(p);
      y_sweep = get_y(p);

      node w = G.new_node();
      xcoord[w] = x_sweep;
      ycoord[w] = y_sweep;
      set_left_node(l,w);

      sit = Y_structure.insert(l,XNIL);

      Xinsert(get_endpoint(l),sit);

      sitpred = Y_structure.pred(sit);
      sitsucc = Y_structure.succ(sit);

      if (sitpred != YNIL) 
      { if ((pqit = Y_structure.inf(sitpred)) != XNIL)
          delete Xdelete(pqit);

        lpred = Y_structure.key(sitpred);

        Y_structure.change_inf(sitpred,XNIL);

        if (intersection(lpred,l,inter))
            Y_structure.change_inf(sitpred,Xinsert(inter,sitpred));
      }


      if (sitsucc != YNIL)
      { lsucc = Y_structure.key(sitsucc);
        if (intersection(lsucc,l,inter))
           Y_structure.change_inf(sit,Xinsert(inter,sit));
      }

    }
    else if (get_kind(p) == Rightend)
         //right endpoint
         { 
           x_sweep = get_x(p);
           y_sweep = get_y(p);

           sit = sitmin;

           sitpred = Y_structure.pred(sit);
           sitsucc = Y_structure.succ(sit);

           segment_ptr seg = Y_structure.key(sit);

           Y_structure.del_item(sit);

           delete seg;

           if((sitpred != YNIL)&&(sitsucc != YNIL))
           {
             lpred = Y_structure.key(sitpred);
             lsucc = Y_structure.key(sitsucc);
             if (intersection(lsucc,lpred,inter))
                Y_structure.change_inf(sitpred,Xinsert(inter,sitpred));
           }
         }
         else // intersection point p
         { 
           node w = G.new_node();
           xcoord[w] = get_x(p);
           ycoord[w] = get_y(p);

           /* Let L = list of all lines intersecting in p 
 
              we compute sit     = L.head();
              and        sitpred = L.tail();

              by scanning the Y_structure in both directions 
              starting at sitmin;

           */

           /* search for sitpred upwards from sitmin: */

           Y_structure.change_inf(sitmin,XNIL);

           sitpred = Y_structure.succ(sitmin);


           while ((pqit=Y_structure.inf(sitpred)) != XNIL)
           { point_ptr q = X_structure.prio(pqit);
             if (compare(p,q) != 0) break; 
             X_structure.del_item(pqit);
             Y_structure.change_inf(sitpred,XNIL);
             sitpred = Y_structure.succ(sitpred);
            }


           /* search for sit downwards from sitmin: */

           sit = sitmin;

           seq_item sit1;
           
           while ((sit1=Y_structure.pred(sit)) != YNIL)
           { pqit = Y_structure.inf(sit1);
             if (pqit == XNIL) break;
             point_ptr q = X_structure.prio(pqit);
             if (compare(p,q) != 0) break; 
             X_structure.del_item(pqit);
             Y_structure.change_inf(sit1,XNIL);
             sit = sit1;
            }



           // insert edges to p for all segments in sit, ..., sitpred into G
           // and set left node to w 

           lsit = Y_structure.key(sitpred);

           node v = get_left_node(lsit);
           if (v!=nil && w!=nil) G.new_edge(v,w);
           set_left_node(lsit,w);

           for(sit1=sit; sit1!=sitpred; sit1 = Y_structure.succ(sit1))
           { lsit = Y_structure.key(sit1);

             v = get_left_node(lsit);
             if (v!=nil && w!=nil) G.new_edge(v,w);
             set_left_node(lsit,w);
            }

           lsit = Y_structure.key(sit);
           lpred=Y_structure.key(sitpred);
           sitpredpred = Y_structure.pred(sit);
           sitsucc=Y_structure.succ(sitpred);


           if (sitpredpred != YNIL)
            { 
              lpredpred=Y_structure.key(sitpredpred);

              if ((pqit = Y_structure.inf(sitpredpred)) != XNIL)
                delete Xdelete(pqit);

              Y_structure.change_inf(sitpredpred,XNIL);


              if (intersection(lpred,lpredpred,inter))
                Y_structure.change_inf(sitpredpred,
                                       Xinsert(inter,sitpredpred));
             }


           if (sitsucc != YNIL)
            {
              lsucc=Y_structure.key(sitsucc);

              if ((pqit = Y_structure.inf(sitpred)) != XNIL)
                delete Xdelete(pqit);
                 
              Y_structure.change_inf(sitpred,XNIL);

              if (intersection(lsucc,lsit,inter))
                  Y_structure.change_inf(sit,Xinsert(inter,sit));
             }


// reverse the subsequence sit, ... ,sitpred  in the Y_structure

           x_sweep = get_x(p);
           y_sweep = get_y(p);

           Y_structure.reverse_items(sit,sitpred);

          delete p;

         } // intersection

  }


  pq_item xit;
  forall_items(xit,X_structure)
  { point_ptr      p = X_structure.prio(xit);
    seq_item sit = X_structure.inf(xit);
    if (get_kind(p) == Leftend)         delete get_seg(p);
    if (get_kind(p) == Rightend && sit) delete Y_structure.key(sit);
    if (get_kind(p) == Intersection)    delete p;
   }

  X_structure.clear();
  Y_structure.clear();

  Make_Connected(G);

  // normalize x and y coordinates
  while (MAX_X/2 >= x_sweep) MAX_X /= 2; 
  node v;
  forall_nodes(v,G) 
  { xcoord[v] /= MAX_X;
    ycoord[v] /= MAX_Y;
   }
}


void random_planar_graph(graph& G, int n)
{ 
  node_array<double> xcoord;
  node_array<double> ycoord;
  random_planar_graph(G,xcoord,ycoord,n);
}

#endif



void maximal_planar_map(graph& G, int n)
{
  G.clear();

  if (n <= 0 ) return;

  node a = G.new_node();
  n--;

  if (n == 0) return;

  node b = G.new_node();
  n--;

  edge* E = new edge[6*n];

  E[0] = G.new_edge(a,b);
  E[1] = G.new_edge(b,a);
  G.set_reversal(E[0],E[1]);

  int m = 2;

  while (n--)
  { edge e = E[rand_int(0,m-1)];
    node v = G.new_node();
    while (target(e) != v)
    { edge x = G.new_edge(v,source(e));
      edge y = G.new_edge(e,v,leda::behind);
      E[m++] = x;
      E[m++] = y;
      G.set_reversal(x,y);
      e = G.face_cycle_succ(e);
     }
   }

  delete[] E;
}


void maximal_planar_graph(graph& G, int n)
{
  maximal_planar_map(G,n);
  list<edge> E;

  edge_array<bool> marked(G,false);
  edge e;
  forall_edges(e,G)
  { if (!marked[e]) E.append(e);
    marked[e] = true;
    marked[G.reversal(e)] = true;
   }

  forall(e,E) G.del_edge(e);
}



void random_planar_map(graph& G, int n, int m)
{
  maximal_planar_map(G,n);

 list<edge> E;
 edge e;

 edge_array<bool> marked(G,false);

 forall_edges(e,G)
 { if (!marked[e]) E.append(e);
   marked[G.reversal(e)] = true;
  }

 E.permute();

 while (E.length() > m)
 { edge e = E.pop();
   G.del_edge(G.reversal(e));
   G.del_edge(e);
  }

}


void random_planar_graph(graph& G, int n, int m)
{
  random_planar_map(G,n,m);
  list<edge> L;

  edge_array<bool> marked(G,false);
  edge e;
  forall_edges(e,G)
  { if (!marked[e]) L.append(e);
    marked[e] = true;
    marked[G.reversal(e)] = true;
   }

  G.del_edges(L);

  Make_Connected(G);
}

LEDA_END_NAMESPACE
