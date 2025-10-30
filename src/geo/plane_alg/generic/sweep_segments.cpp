/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  sweep_segments.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/core/sortseq.h>
#include <LEDA/graph/graph.h>
#include <LEDA/core/p_queue.h>
#include <LEDA/core/map.h>
#include <LEDA/core/map2.h>
#include <LEDA/core/quicksort.h>



LEDA_BEGIN_NAMESPACE


#if (KERNEL == RAT_KERNEL)
#define sweep_cmp sweep_rat_cmp
#define sweep_cmp_edges sweep_cmp_rat_edges
#elif (KERNEL == REAL_KERNEL)
#define sweep_cmp sweep_real_cmp
#define sweep_cmp_edges sweep_cmp_real_edges
#endif



class sweep_cmp : public leda_cmp_base<SEGMENT>
{
  POINT p_sweep;

public:

 sweep_cmp(const POINT& p) : p_sweep(p) {}

 void set_position(const POINT& p) { p_sweep = p; }

 int operator()(const SEGMENT& s1, const SEGMENT& s2) const
 { // Precondition:
   // p_sweep is identical to the left endpoint of either s1 or s2. 
 
   if (identical(s1,s2)) return 0;
  
   int s = 0;
  
   if ( identical(p_sweep,s1.source()) )
    s = orientation(s2,p_sweep);
   else
    if ( identical(p_sweep,s2.source()) )
       s = -orientation(s1,p_sweep);
    else 
       LEDA_EXCEPTION(1,"compare error in sweep");
 
   if (s || s1.is_trivial() || s2.is_trivial()) return s;
  
   s = orientation(s2,s1.target());
 
   // overlapping segments will be ordered by their ID_numbers :
 
   return s ? s : (ID_Number(s1) - ID_Number(s2));
 }
 
};




static void compute_intersection(
                    sortseq<POINT,seq_item>& X_structure,
                    sortseq<SEGMENT,seq_item>& Y_structure,
                    seq_item sit0)
{ seq_item sit1 = Y_structure.succ(sit0);
  SEGMENT  s0   = Y_structure.key(sit0);
  SEGMENT  s1   = Y_structure.key(sit1);

  if ( orientation(s0,s1.target()) <= 0 && 
       orientation(s1,s0.target()) >= 0 )
  { POINT q;
    s0.intersection_of_lines(s1,q);
    Y_structure.change_inf(sit0, X_structure.insert(q,sit0));
  }
}


static void compute_intersection(
                    sortseq<POINT,seq_item>& X_structure,
                    sortseq<SEGMENT,seq_item>& Y_structure, 
                    const map2<SEGMENT,SEGMENT,seq_item>& inter_dic,
                    seq_item sit0)
{ seq_item sit1 = Y_structure.succ(sit0);
  SEGMENT  s0   = Y_structure.key(sit0);
  SEGMENT  s1   = Y_structure.key(sit1);

  if ( orientation(s0,s1.target()) <= 0 && 
       orientation(s1,s0.target()) >= 0 )
  { 
    seq_item it = inter_dic.operator()(s0,s1);
    //seq_item it = inter_dic(s0,s1);
    if ( it == nil)
    { POINT q;
      s0.intersection_of_lines(s1,q);
      it = X_structure.insert(q,sit0);
    }
    Y_structure.change_inf(sit0, it);
  }
}




class sweep_cmp_edges : public leda_cmp_base<edge>
{
  const GRAPH<POINT,SEGMENT>& G;

public:

  sweep_cmp_edges(const GRAPH<POINT,SEGMENT>& g): G(g) {}

  int operator()(const edge& e1, const edge& e2) const
  { const SEGMENT& s1 = G[e1];
    const SEGMENT& s2 = G[e2];
    int c = cmp_slopes(s1,s2);
    if (c == 0) c = compare(ID_Number(s1),ID_Number(s2));
    return c;
  }
};


class sweep_cmp_edges_approx : public leda_cmp_base<edge>
{
  const GRAPH<POINT,SEGMENT>& G;

public:

  sweep_cmp_edges_approx(const GRAPH<POINT,SEGMENT>& g): G(g) {}

  int operator()(const edge& e1, const edge& e2) const
  { const SEGMENT& s1 = G[e1];
    const SEGMENT& s2 = G[e2];
    int c = cmp_slopes(s1.to_float(),s2.to_float());
    if (c == 0) c = compare(ID_Number(s1),ID_Number(s2));
    return c;
  }
};




class sweep_smaller_exact
{
  const sweep_cmp_edges& cmp;

public:
  sweep_smaller_exact(const sweep_cmp_edges& c): cmp(c) {}
  bool operator()(const edge& e1, const edge& e2) const
  { return cmp(e1,e2) < 0; }
};




class sweep_smaller_approx
{
  const sweep_cmp_edges_approx& cmp;

public:
  sweep_smaller_approx(const sweep_cmp_edges_approx& c): cmp(c) {}
  bool operator()(const edge& e1, const edge& e2) const
  { return cmp(e1,e2) < 0; }
};





static void construct_embedding(GRAPH<POINT,SEGMENT>& G)
{  
  sweep_cmp_edges cmp(G);

/*
  list<edge> R = G.all_edges();
  R.sort(cmp);
  edge e;
  forall(e,R) G.set_reversal(e, G.new_edge(target(e),source(e),G[e]));
*/


  int m = G.number_of_edges();
  array<edge> A(m);
  int i = 0;
  edge e;
  forall_edges(e,G) A[i++] = e;
  A.sort(cmp);
  forall(e,A) G.set_reversal(e, G.new_edge(target(e),source(e),G[e]));



/*
list<edge> L1 = G.all_edges();
list<edge> L2 = G.all_edges();
int m = G.number_of_edges();
array<edge> A(m);
array<edge> B(m);
array<edge> C(m);
int i = 0;
forall_edges(e,G) { A[i] = B[i] = C[i] = e; i++; }

//sweep_smaller_exact  smaller(G);
sweep_smaller_exact2  smaller(cmp);
float T1 = used_time();
L1.sort_smaller(smaller);
cout << string("list sort1: %.2f ",used_time(T1)) << endl;

leda_smaller_cmp_obj<edge,leda_cmp_base<edge> > smaller2(cmp);
float T2 = used_time();
L2.sort_smaller(smaller2);
cout << string("list sort2: %.2f",used_time(T2));
cout << endl;

sweep_cmp_edges cmp1(G);
float T3 = used_time();
A.sort(cmp1);
cout << string("arraysort0: %.2f ",used_time(T3)) << endl;

sweep_smaller_exact smaller1(G);
//sweep_smaller_exact2 smaller1(cmp);
float T4 = used_time();
B.sort_smaller(smaller1);
cout << string("arraysort1: %.2f ",used_time(T4)) << endl;

leda_smaller_cmp_obj<edge,leda_cmp_base<edge> > smaller3(cmp);
float T5 = used_time();
C.sort_smaller(smaller3);
cout << string("arraysort1: %.2f ",used_time(T5)) << endl;
*/

}


void SWEEP_SEGMENTS(const list<SEGMENT>& S, 
                    GRAPH<POINT, SEGMENT>& G, 
                    bool embed,
                    bool use_optimization)
{ 
//  list<SEGMENT>        internal; // ST: seems to be unused
  map<SEGMENT,SEGMENT> original; 
  map<SEGMENT,node>    last_node(0);

  POINT p_sweep;
  sweep_cmp cmp(p_sweep);

  sortseq<POINT,seq_item>     X_structure;
  sortseq<SEGMENT, seq_item>  Y_structure(cmp);
  p_queue<POINT,SEGMENT>      seg_queue;

  map2<SEGMENT,SEGMENT,seq_item>   inter_dic(0);

  
  G.clear();
    
  COORD Infinity = 1;
    
  SEGMENT s;
  forall(s,S) 
  {
    COORD x1 = s.xcoord1(); 
    COORD y1 = s.ycoord1(); 
    COORD x2 = s.xcoord2(); 
    COORD y2 = s.ycoord2(); 
    if (x1 < 0) x1 = -x1;
    if (y1 < 0) y1 = -y1;
    if (x2 < 0) x2 = -x2;
    if (y2 < 0) y2 = -y2;

    while (x1 >= Infinity || y1 >= Infinity || 
          x2 >= Infinity || y2 >= Infinity )   Infinity *= 2;
        
    seq_item it1 = X_structure.insert(s.source(), seq_item(nil));
    seq_item it2 = X_structure.insert(s.target(), seq_item(nil));
        
    if (it1 == it2) continue;  // ignore zero-length segments
        
    POINT p = X_structure.key(it1);
    POINT q = X_structure.key(it2);
        
    SEGMENT s1; 

    if ( compare(p,q) < 0 ) 
      s1 = SEGMENT(p,q);
    else
      s1 = SEGMENT(q,p);

    original[s1] = s; 
//    internal.append(s1); // ST: seems to unused
    seg_queue.insert(s1.source(),s1);
  }
    
  SEGMENT lower_sentinel(-Infinity,-Infinity,Infinity,-Infinity);
  SEGMENT upper_sentinel(-Infinity, Infinity,Infinity, Infinity);
    
  p_sweep = lower_sentinel.source();
  cmp.set_position(p_sweep);

  Y_structure.insert(upper_sentinel,seq_item(nil));
  Y_structure.insert(lower_sentinel,seq_item(nil));
    
  POINT pstop(Infinity,Infinity);
  seg_queue.insert(pstop,SEGMENT(pstop,pstop));
  SEGMENT next_seg = seg_queue.inf(seg_queue.find_min());

  while ( !X_structure.empty() ) 
  { seq_item event = X_structure.min();
    p_sweep = X_structure.key(event);

    cmp.set_position(p_sweep);

    node v = G.new_node(p_sweep);

    seq_item sit = X_structure.inf(event);

          
    if (sit == nil)
      sit = Y_structure.lookup(SEGMENT(p_sweep,p_sweep));

    seq_item sit_succ  =     nil;
    seq_item sit_pred  =     nil;
    seq_item sit_pred_succ = nil;
    seq_item sit_first =     nil;

    if (sit != nil)  
    { 
      // walk up
      while ( Y_structure.inf(sit) == event || 
              Y_structure.inf(sit) == Y_structure.succ(sit) )
        sit = Y_structure.succ(sit);
                
      sit_succ = Y_structure.succ(sit);
      seq_item sit_last = sit;
                
      if ( use_optimization )
      { 
        seq_item xit = Y_structure.inf(sit_last);
        if (xit) 
        { SEGMENT s1 = Y_structure.key(sit_last);
          SEGMENT s2 = Y_structure.key(sit_succ);
          inter_dic(s1,s2) = xit;
         }
       }
                
      // walk down 
                
      bool overlapping;
      do 
      { overlapping = false;
        s = Y_structure.key(sit);
        node u = last_node[s];

        if ( !embed && s.source() == original[s].source() )
          G.new_edge(u,v,s);
        else            
          G.new_edge(v,u,s);


        if ( identical(p_sweep,s.target()) )   // ending segment
        { 
           seq_item it = Y_structure.pred(sit);
           if ( Y_structure.inf(it) == sit )
           { overlapping = true;
             Y_structure.change_inf(it, Y_structure.inf(sit));
           }

           Y_structure.del_item(sit);
           sit = it; 
         }
         else  // passing segment
         { 
           if ( Y_structure.inf(sit) != Y_structure.succ(sit) )
             Y_structure.change_inf(sit, seq_item(nil));
           last_node[s] = v;
           sit = Y_structure.pred(sit); 
         }
      } while ( Y_structure.inf(sit) == event || overlapping ||
               Y_structure.inf(sit) == Y_structure.succ(sit) );
                
      sit_pred = sit;
      sit_first = Y_structure.succ(sit_pred);
      sit_pred_succ = sit_first;  

      
      sit = sit_first;
              
      // reverse subsequences of overlapping segments  (if existing)

      while ( sit != sit_succ ) 
      { seq_item sub_first = sit;
        seq_item sub_last  = sub_first;
                    
        while (Y_structure.inf(sub_last) == Y_structure.succ(sub_last))
          sub_last = Y_structure.succ(sub_last);
                    
        if (sub_last != sub_first)
          Y_structure.reverse_items(sub_first, sub_last);
                    
        sit = Y_structure.succ(sub_first);
      }
                
      // reverse the entire bundle

      if ( sit_first != sit_succ )
        Y_structure.reverse_items(Y_structure.succ(sit_pred),
                                  Y_structure.pred(sit_succ));  

    }

    
    while ( identical(p_sweep,next_seg.source()) )
    { seq_item s_sit = Y_structure.locate(next_seg);
      seq_item p_sit = Y_structure.pred(s_sit);
              
      s = Y_structure.key(s_sit);
              
      if ( orientation(s, next_seg.start()) == 0 &&
          orientation(s, next_seg.end()) == 0 )
         sit = Y_structure.insert_at(s_sit, next_seg, s_sit);
      else 
         sit = Y_structure.insert_at(s_sit, next_seg, seq_item(nil));

      s = Y_structure.key(p_sit);

      if ( orientation(s, next_seg.start()) == 0 &&
          orientation(s, next_seg.end()) == 0 )
        Y_structure.change_inf(p_sit, sit);
              
      X_structure.insert(next_seg.end(), sit);
      last_node[next_seg] = v;
              
      if ( sit_succ == nil )  
      { sit_succ = s_sit; 
        sit_pred = p_sit;  
        sit_pred_succ = sit_succ;  
      }
              
      // delete minimum and assign new minimum to next_seg
              
      seg_queue.del_min();
      next_seg = seg_queue.inf(seg_queue.find_min());
    }

    
    if ( sit_pred != nil ) 
    { if ( !use_optimization )
      { Y_structure.change_inf(sit_pred,seq_item(nil));
        compute_intersection(X_structure, Y_structure, sit_pred);
        sit = Y_structure.pred(sit_succ);
        if ( sit != sit_pred )
          compute_intersection(X_structure, Y_structure, sit);
      }
      else
      { 
        seq_item xit = Y_structure.inf(sit_pred);
        if ( xit ) 
        { SEGMENT s1 = Y_structure.key(sit_pred);
          SEGMENT s2 = Y_structure.key(sit_pred_succ);  // sit_first
          inter_dic(s1,s2) = xit;
          Y_structure.change_inf(sit_pred, seq_item(nil));
        }
                   
        compute_intersection(X_structure, Y_structure,inter_dic,sit_pred);
        sit = Y_structure.pred(sit_succ);
        if ( sit != sit_pred )
          compute_intersection(X_structure, Y_structure,inter_dic,sit);
       }
    }
          

    X_structure.del_item(event);
  }

  
  if (embed) construct_embedding(G);

  edge e;
  forall_edges(e, G) G[e] = original[G[e]];

}


/*
void SWEEP_SEGMENTS(const list<SEGMENT>& S, GRAPH<POINT,SEGMENT>& G, bool embed)
{ sortseq<POINT,seq_item>     X_structure;
  sortseq<SEGMENT, seq_item>  Y_structure(cmp_segments);
  SWEEP_SEGMENTS(S,G,X_structure,Y_structure,embed);
}


void SWEEP_SEGMENTS0(const list<SEGMENT>& S, GRAPH<POINT,SEGMENT>& G, bool embed)
{ sortseq<POINT,seq_item>     X_structure;
  sortseq<SEGMENT, seq_item>  Y_structure;
  SWEEP_SEGMENTS(S,G,X_structure,Y_structure,embed);
}
*/
  

void SWEEP_SEGMENTS(const list<SEGMENT>& S, list<POINT>& L)
{ GRAPH<POINT,SEGMENT> G;
  SWEEP_SEGMENTS(S,G,false,true);
  L.clear();
  node v;
  forall_nodes(v,G)
   if (G.degree(v) > 1) L.append(G[v]);
}


LEDA_END_NAMESPACE

