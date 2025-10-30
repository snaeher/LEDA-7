/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  sweep_segments_old.c
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
#include <LEDA/std/math.h>

LEDA_BEGIN_NAMESPACE


#if defined(NO_FLOAT_ALGORITHMS)
#define sweep_cmp sweep_rat_cmp
#define cmp_edges cmp_rat_edges
#endif


class sweep_cmp:public leda_cmp_base < SEGMENT >
{
  POINT p_sweep;

    public:

    sweep_cmp (const POINT & p):p_sweep (p)
  {
  }

  void set_position (const POINT & p)
  {
    p_sweep = p;
  }

  int operator () (const SEGMENT & s1, const SEGMENT & s2) const
  {				// Precondition:
    // [[p_sweep]] is identical to the left endpoint of either [[s1]] or [[s2]]. 

    if (identical (s1, s2))
      return 0;

    int s = 0;

    if (identical (p_sweep, s1.source ()))
        s = orientation (s2, p_sweep);
    else if (identical (p_sweep, s2.source ()))
        s = orientation (s1, p_sweep);
    else
        LEDA_EXCEPTION (1, "compare error in sweep");

    if (s || s1.is_trivial () || s2.is_trivial ())
        return s;

      s = orientation (s2, s1.target ());

    // overlapping segments will be ordered by their ID_numbers :

      return s ? s : (ID_Number (s1) - ID_Number (s2));
  }

};

#if defined(__WATCOMC__)
inline int
compare (const SEGMENT & s1, const SEGMENT & s2)
{
  LEDA_EXCEPTION (1, "sweep: compare called.");
  return 0;
}
#endif


static void
compute_intersection (sortseq < POINT, seq_item > &X_structure,
		      sortseq < SEGMENT, seq_item > &Y_structure,
		      const map2 < SEGMENT, SEGMENT, seq_item > &inter_dic,
		      seq_item sit0)
{


  seq_item sit1 = Y_structure.succ (sit0);
  SEGMENT s0 = Y_structure.key (sit0);
  SEGMENT s1 = Y_structure.key (sit1);



  if (orientation (s0, s1.target ()) <= 0 && orientation (s1, s0.target ()) >= 0)
    {
      seq_item it = inter_dic.operator ()(s0, s1);
      //seq_item it = inter_dic(s0,s1);
      if (it == nil)
	{
	  POINT q;
	  s0.intersection_of_lines (s1, q);
	  it = X_structure.insert (q, sit0);
	}
      Y_structure.change_inf (sit0, it);
    }
}




// Sort Edges ...

class cmp_edges:public leda_cmp_base < edge >
{
  GRAPH < POINT, SEGMENT > *g_ptr;

  public:

  cmp_edges (GRAPH < POINT, SEGMENT > &G)
  {
    g_ptr = &G;
  }

  int operator () (const edge & e1, const edge & e2) const
  {
    SEGMENT s1 = (*g_ptr)[e1];
    SEGMENT s2 = (*g_ptr)[e2];
    int c = cmp_slopes (s1, s2);
    if (c == 0)
        c = compare (ID_Number (s1), ID_Number (s2));
      return c;
  }
};


static void
construct_embedding (GRAPH < POINT, SEGMENT > &G)
{
  list < edge > E = G.all_edges ();

  cmp_edges cmp (G);
  E.sort (cmp);

  edge e;
  forall (e, E)
  {
    edge r = G.new_edge (target (e), source (e), G[e]);
    G.set_reversal (e, r);
  }
}




void
SWEEP_SEGMENTS (const list < SEGMENT > &S, GRAPH < POINT, SEGMENT > &G, bool embed)
{

  POINT p_sweep;
  sweep_cmp cmp (p_sweep);

  // we use two sorted sequences ...

  sortseq < POINT, seq_item > X_structure;
  sortseq < SEGMENT, seq_item > Y_structure (cmp);

  // three maps ...

  map < SEGMENT, SEGMENT > original;
  map < SEGMENT, node > last_node (nil);
  map2 < SEGMENT, SEGMENT, seq_item > inter_dic (nil);

  // a list

  list < SEGMENT > internal;

  // and a priority queue of segments ordered by their left endpoints

  p_queue < POINT, SEGMENT > seg_queue;



  G.clear ();

  COORD Infinity = 1;

  SEGMENT s;
  forall (s, S)
  {
    COORD x1 = s.xcoord1 ();
    COORD y1 = s.ycoord1 ();
    COORD x2 = s.xcoord2 ();
    COORD y2 = s.ycoord2 ();
    if (x1 < 0)
      x1 = -x1;
    if (y1 < 0)
      y1 = -y1;
    if (x2 < 0)
      x2 = -x2;
    if (y2 < 0)
      y2 = -y2;

    while (x1 >= Infinity || y1 >= Infinity ||
	   x2 >= Infinity || y2 >= Infinity)
      Infinity *= 2;

    seq_item it1 = X_structure.insert (s.source (), seq_item (nil));
    seq_item it2 = X_structure.insert (s.target (), seq_item (nil));

    if (it1 == it2)
      continue;			// ignore zero-length segments

    POINT p = X_structure.key (it1);
    POINT q = X_structure.key (it2);

    SEGMENT s1;

    if (compare (p, q) < 0)
      s1 = SEGMENT (p, q);
    else
      s1 = SEGMENT (q, p);

    original[s1] = s;
    internal.append (s1);
    seg_queue.insert (s1.source (), s1);
  }

  // insert a lower and an upper sentinel segment to avoid special
  // cases when traversing the Y-structure

  SEGMENT lower_sentinel (-Infinity, -Infinity, Infinity, -Infinity);
  SEGMENT upper_sentinel (-Infinity, Infinity, Infinity, Infinity);

  // the sweep begins at the lower left corner

  p_sweep = lower_sentinel.source ();
  cmp.set_position (p_sweep);

  Y_structure.insert (upper_sentinel, seq_item (nil));
  Y_structure.insert (lower_sentinel, seq_item (nil));

  // insert a stopper into |seg_queue| and initialize |next_seg| with
  // the first segment in  the queue

  POINT pstop (Infinity, Infinity);
  seg_queue.insert (pstop, SEGMENT (pstop, pstop));
  SEGMENT next_seg = seg_queue.inf (seg_queue.find_min ());

  // Main Loop

  while (!X_structure.empty ())
    {

      seq_item event = X_structure.min ();
      p_sweep = X_structure.key (event);
      node v = G.new_node (p_sweep);

      cmp.set_position (p_sweep);



      seq_item sit = X_structure.inf (event);

      if (sit == nil)
	{


	  sit = Y_structure.lookup (SEGMENT (p_sweep, p_sweep));
	}

      seq_item sit_succ = nil;
      seq_item sit_pred = nil;
      seq_item sit_first = nil;


      if (sit != nil)		// key(sit) is an ending or passing segment

	{


	  while (Y_structure.inf (sit) == event ||
		 Y_structure.inf (sit) == Y_structure.succ (sit))
	    sit = Y_structure.succ (sit);

	  sit_succ = Y_structure.succ (sit);

	  seq_item xit = Y_structure.inf (sit);
	  if (xit)
	    {
	      SEGMENT s1 = Y_structure.key (sit);
	      SEGMENT s2 = Y_structure.key (sit_succ);
	      inter_dic (s1, s2) = xit;
	    }



	  bool upperst;
	  do
	    {
	      upperst = false;
	      s = Y_structure.key (sit);


	      if (!embed && s.source () == original[s].source ())
		G.new_edge (last_node[s], v, s);
	      else
		G.new_edge (v, last_node[s], s);

	      if (identical (p_sweep, s.target ()))	//ending segment

		{
		  seq_item it = Y_structure.pred (sit);
		  if (Y_structure.inf (it) == sit)
		    {
		      upperst = true;
		      Y_structure.change_inf (it, Y_structure.inf (sit));
		    }
		  Y_structure.del_item (sit);
		  sit = it;
		}
	      else		//passing segment

		{
		  if (Y_structure.inf (sit) != Y_structure.succ (sit))
		    Y_structure.change_inf (sit, seq_item (nil));
		  last_node[s] = v;
		  sit = Y_structure.pred (sit);
		}
	    }
	  while (Y_structure.inf (sit) == event || upperst ||
		 Y_structure.inf (sit) == Y_structure.succ (sit));

	  sit_pred = sit;
	  sit = Y_structure.succ (sit_pred);
	  sit_first = sit;	// first item of the bundle


	  // reverse subsequences of overlapping segments  (if existing)

	  while (sit != sit_succ && Y_structure.succ (sit) != sit_succ)
	    {
	      seq_item sub_first = sit;
	      seq_item sub_last = sub_first;

	      while (Y_structure.inf (sub_last) == Y_structure.succ (sub_last))
		sub_last = Y_structure.succ (sub_last);

	      if (sub_last != sub_first)
		Y_structure.reverse_items (sub_first, sub_last);

	      sit = Y_structure.succ (sub_first);
	    }

	  // reverse the bundle as awhole

	  if (Y_structure.succ (sit_pred) != sit_succ)
	    Y_structure.reverse_items (Y_structure.succ (sit_pred),
				       Y_structure.pred (sit_succ));
	}

      // insert all segments starting at |p_sweep|

      while (identical (p_sweep, next_seg.source ()))
	{


	  seq_item s_sit = Y_structure.locate (next_seg);
	  seq_item p_sit = Y_structure.pred (s_sit);

	  s = Y_structure.key (s_sit);

	  if (!orientation (s, next_seg.start ()) &&
	      !orientation (s, next_seg.end ()))
	    sit = Y_structure.insert_at (s_sit, next_seg, s_sit);
	  else
	    sit = Y_structure.insert_at (s_sit, next_seg, seq_item (nil));

	  s = Y_structure.key (p_sit);



	  if (!orientation (s, next_seg.start ()) &&
	      !orientation (s, next_seg.end ()))
	    Y_structure.change_inf (p_sit, sit);

	  X_structure.insert (next_seg.end (), sit);
	  last_node[next_seg] = v;

	  if (sit_succ == nil)
	    {

	      sit_succ = Y_structure.succ (sit);
	      sit_pred = Y_structure.pred (sit);
	      sit_first = sit_succ;
	    }

	  // delete minimum and assign new minimum to |next_seg|

	  seg_queue.del_min ();
	  next_seg = seg_queue.inf (seg_queue.find_min ());
	}



      if (sit_pred != nil)
	{


	  seq_item xit = Y_structure.inf (sit_pred);
	  if (xit)
	    {
	      SEGMENT s1 = Y_structure.key (sit_pred);
	      SEGMENT s2 = Y_structure.key (sit_first);
	      inter_dic (s1, s2) = xit;
	      Y_structure.change_inf (sit_pred, seq_item (nil));
	    }



	  compute_intersection (X_structure, Y_structure, inter_dic, sit_pred);
	  sit = Y_structure.pred (sit_succ);
	  if (sit != sit_pred)
	    compute_intersection (X_structure, Y_structure, inter_dic, sit);
	}

      X_structure.del_item (event);
    }

  if (embed)
    construct_embedding (G);

  edge e;
  forall_edges (e, G) G[e] = original[G[e]];
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


void
SWEEP_SEGMENTS (const list < SEGMENT > &S, list < POINT > &L)
{
  GRAPH < POINT, SEGMENT > G;
  SWEEP_SEGMENTS (S, G, false);
  L.clear ();
  node v;
  forall_nodes (v, G)
    if (G.degree (v) > 1)
    L.append (G[v]);
}


LEDA_END_NAMESPACE
