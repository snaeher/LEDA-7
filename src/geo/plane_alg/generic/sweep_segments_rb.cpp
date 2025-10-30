/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  sweep_segments_rb.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/

// 2016/04/26
// bug fixed by sn  (incorrect number n of segments)


#include <LEDA/core/sortseq.h>
#include <LEDA/graph/graph.h>
#include <LEDA/core/p_queue.h>
#include <LEDA/core/map.h>

LEDA_BEGIN_NAMESPACE


#if (KERNEL == RAT_KERNEL)
#define sweep_cmp_rb sweep_rat_cmp_rb
#define sweep_cmp_edges_rb sweep_cmp_rat_edges_rb
#elif (KERNEL == REAL_KERNEL)
#define sweep_cmp_rb sweep_real_cmp_rb
#define sweep_cmp_edges_rb sweep_cmp_real_edges_rb
#endif


class sweep_cmp_rb : public leda_cmp_base<SEGMENT>
{
  POINT p_sweep;

public:

 sweep_cmp_rb(const POINT& p) : p_sweep(p) {}

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


inline int cmp_seg_by_source(const SEGMENT& s1, const SEGMENT& s2)
{ return POINT::cmp_xy(s1.source(),s2.source()); }


static void compute_intersection_rb(
                    sortseq<POINT,seq_item>& X_structure,
                    sortseq<SEGMENT,seq_item>& Y_structure,
                    const map<SEGMENT,int>& seg_color,
                    seq_item sit0)
{ 
  seq_item sit1 = Y_structure.succ(sit0);
  SEGMENT  s0   = Y_structure.key(sit0);
  SEGMENT  s1   = Y_structure.key(sit1);

  POINT t0 = s0.target(); 
  POINT t1 = s1.target(); 


  if (seg_color[s0] == seg_color[s1])
  { if (identical(t0,t1))
       Y_structure.change_inf(sit0, X_structure.insert(t0,sit0));
    return;
   }


  if ( orientation(s0,t1) <= 0 && orientation(s1,t0) >= 0 )
  { POINT q;
    s0.intersection_of_lines(s1,q);
    Y_structure.change_inf(sit0, X_structure.insert(q,sit0));
  }
}



class sweep_cmp_edges_rb : public leda_cmp_base<edge>
{
  const GRAPH<POINT,SEGMENT>& G;

public:

  sweep_cmp_edges_rb(const GRAPH<POINT,SEGMENT>& g): G(g) {}

  int operator()(const edge& e1, const edge& e2) const
  { SEGMENT s1 = G[e1];
    SEGMENT s2 = G[e2];
    int c = cmp_slopes(s1,s2);
    if (c == 0) c = compare(ID_Number(s1),ID_Number(s2));
    return c;
  }
};


static void construct_embedding_rb(GRAPH<POINT,SEGMENT>& G)
{  
  list<edge> R = G.all_edges();

  sweep_cmp_edges_rb cmp(G);
  R.sort(cmp);

  edge e;
  forall(e,R)
  { edge r = G.new_edge(target(e),source(e),G[e]);
    G.set_reversal(e,r);
  }
}


void SWEEP_SEGMENTS_RB(const list<SEGMENT>& S1, 
                       const list<SEGMENT>& S2, 
                       GRAPH<POINT, SEGMENT>& G, 
                       bool embed,
                       bool use_optimization)
{ 

  
  int num_segments =  S1.length() + S2.length();

  int n =  0; // considered segments (no zero-length segments)


  list<SEGMENT>        internal;
  SEGMENT s;

  map<SEGMENT,SEGMENT> original; 


  POINT p_sweep;
  sweep_cmp_rb cmp(p_sweep);

  sortseq<POINT,seq_item>     X_structure;
  sortseq<SEGMENT, seq_item>  Y_structure(cmp);


  map<SEGMENT,node>           last_node(nil);


  //p_queue<POINT,SEGMENT>      seg_queue;
  array<SEGMENT> seg_queue(num_segments+1); // one additional segment (sentinel)

  int sq_next = 0;

  map<SEGMENT,int>            seg_color(0);

/*
  list<SEGMENT> S;
  SEGMENT x;
  forall(x,S1) { S.append(x); seg_color[x] = 0; } 
  forall(x,S2) { S.append(x); seg_color[x] = 1; }
*/

  
  G.clear();
    
  COORD Infinity = 1;

const list<SEGMENT>* A[2];
A[0] = &S1; 
A[1] = &S2; 
    

for(int i=0; i<2; i++)
{
  forall(s,*(A[i])) 
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
    internal.append(s1);

    //seg_queue.insert(s1.source(),s1);
    seg_queue[n++] = s1;

    seg_color[s1] = i;
  }
}

    
  SEGMENT lower_sentinel(-Infinity,-Infinity,Infinity,-Infinity);
  SEGMENT upper_sentinel(-Infinity, Infinity,Infinity, Infinity);
    
  p_sweep = lower_sentinel.source();
  cmp.set_position(p_sweep);

  Y_structure.insert(upper_sentinel,seq_item(nil));
  Y_structure.insert(lower_sentinel,seq_item(nil));
    
  POINT pstop(Infinity,Infinity);
  //seg_queue.insert(pstop,SEGMENT(pstop,pstop));
  seg_queue[n++] = SEGMENT(pstop,pstop);

  seg_queue.sort(cmp_seg_by_source,0,n-1);


  //SEGMENT next_seg = seg_queue.inf(seg_queue.find_min());
  SEGMENT next_seg = seg_queue[sq_next++];

  
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
  //seq_item sit_pred_succ = nil;
    seq_item sit_first =     nil;

    if (sit != nil)  
    { 
      // walk up
      while ( Y_structure.inf(sit) == event || 
              Y_structure.inf(sit) == Y_structure.succ(sit) )
        sit = Y_structure.succ(sit);
                
      sit_succ = Y_structure.succ(sit);
                
                
      // walk down 
                
      bool overlapping;
      do 
      { overlapping = false;
        s = Y_structure.key(sit);

        if ( !embed && s.source() == original[s].source() )
          G.new_edge(last_node[s], v, s);
        else            
          G.new_edge(v, last_node[s], s );
                    
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
    //sit_pred_succ = sit_first;  

      
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
        //sit_pred_succ = sit_succ;  
      }
              
      // delete minimum and assign new minimum to next_seg
              
      //seg_queue.del_min();
      //next_seg = seg_queue.inf(seg_queue.find_min());
      next_seg = seg_queue[sq_next++];
    }

    
    if ( sit_pred != nil ) 
    { Y_structure.change_inf(sit_pred,seq_item(nil));
      compute_intersection_rb(X_structure, Y_structure, seg_color,sit_pred);
      sit = Y_structure.pred(sit_succ);
      if ( sit != sit_pred )
         compute_intersection_rb(X_structure, Y_structure, seg_color,sit);
    }
          

    X_structure.del_item(event);
  }

  
  if (embed) construct_embedding_rb(G);

  edge e;
  forall_edges(e, G) G[e] = original[G[e]];

}


void SWEEP_SEGMENTS_RB(const list<SEGMENT>& S1, 
                       const list<SEGMENT>& S2, list<POINT>& L)
{ GRAPH<POINT,SEGMENT> G;
  SWEEP_SEGMENTS_RB(S1,S2,G,false,true);
  L.clear();
  node v;
  forall_nodes(v,G)
   if (G.degree(v) > 1) L.append(G[v]);
}


LEDA_END_NAMESPACE
