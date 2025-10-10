#include <LEDA/graph/graph.h>
#include <LEDA/graph/node_map.h>
#include <LEDA/graph/edge_map.h>
#include <LEDA/core/sortseq.h>
#include <LEDA/system/assert.h>

#define TRACE(s)  {}
#define TRACEN(s) {}


LEDA_BEGIN_NAMESPACE

template <class PNT, class INF>
class GRAPH_node_positions_lt 
{
  const GRAPH<PNT,INF>& G_;
public:
 GRAPH_node_positions_lt(const GRAPH<PNT,INF>& G) : G_(G) {}
 bool operator()(const node& v1, const node& v2) const
 { return PNT::cmp_xy(G_[v1],G_[v2]); }
};


inline int sign(double f) 
{ return (f<0 ? -1 : (f>0 ? 1 : 0)); }

//template <class _POINT>
//inline int angcmp(const _POINT& p1, const _POINT& p2, const _POINT& p3)

template <class _POINT, class NT>
inline int angcmp(const _POINT& p1, const _POINT& p2, const _POINT& p3, NT)
{
  // precond vectors not trivial!

//typedef typename _POINT::coord_type NT;

  NT x1 = (p2-p1).coord(0);
  NT y1 = (p2-p1).coord(1);

  NT x2 = (p3-p1).coord(0);
  NT y2 = (p3-p1).coord(1);

  // both vectors have non-zero length
  int sx1 = sign(x1); int sx2 = sign(x2);
  int half1 = ( sx1 ? sx1 : sign(y1) );
  int half2 = ( sx2 ? sx2 : sign(y2) );  
  // the positive y axis belongs to the positive half
  // the negative y axis belongs to the negative half

  if ( half1 == half2 ) return sign(x2*y1 - x1*y2);
  // half1 != half2
  return (half2 - half1)/2;
}

template <class PNT, class INF>
class GRAPH_outedges_lt { 
  const GRAPH<PNT,INF>& G_;
public:
  GRAPH_outedges_lt(const GRAPH<PNT,INF>& G): G_(G) {}
  bool operator()(const edge& e, const edge& f) const
  { assert(source(e)==source(f));
  //return angcmp<PNT>(G_[source(e)],G_[target(e)],G_[target(f)]) < 0; }
    return angcmp(G_[source(e)],G_[target(e)],G_[target(f)],COORD(0)) < 0; }
};


template <class PNT,class SEG>
class cmp_segs_at_sweepline : public leda_cmp_base<SEG>
{
  const PNT& p;

public:

 cmp_segs_at_sweepline(const PNT& pi) : p(pi) {}

 int operator()(const SEG& s1, const SEG& s2) const
 { // Precondition:
   // [[p]] is identical to the left endpoint of either [[s1]] or [[s2]]. 
 
   if (identical(s1,s2)) return 0;
   int s = 0;
   if ( identical(p,s1.source()) )
     s = orientation(s2,p);
   else
     if ( identical(p,s2.source()) )
       s = orientation(s1,p);
    else 
       error_handler(1,"compare error in sweep.");
 
   if (s || s1.is_trivial() || s2.is_trivial()) 
     return s;
  
   s = orientation(s2,s1.target());


   if (s==0) // overlapping segments are forbidden
     error_handler(1,"compare error: two edges overlap.");
   return s;
 }
 
};






template <class _POINT, class _SEGMENT, class _LINE, class _VECTOR, class _COORD>
class leda_constr_triang_traits {
 
public:

/*
  typedef _POINT              POINT;
  typedef _SEGMENT            SEGMENT;
  typedef _LINE               LINE;
  typedef _VECTOR             VECTOR;
*/

//  typedef typename _POINT::coord_type  _COORD;

  class INPUT_ITERATOR {};
  typedef GRAPH<_POINT, _SEGMENT>  OUTPUT;

  class SWEEP_STATUS_STRUCTURE : public sortseq<_SEGMENT,edge> 
  {
    cmp_segs_at_sweepline<_POINT,_SEGMENT> comp;
  public:
    SWEEP_STATUS_STRUCTURE(const _POINT& p) : 
      sortseq<_SEGMENT,edge>(comp), comp(p) {}
  };
  typedef seq_item             ss_handle;
  typedef edge_map<seq_item>   EMAP;
  typedef map<seq_item,edge>   SMAP;

  typedef GRAPH_node_positions_lt<_POINT,_SEGMENT> nodePOS_LT;
  typedef GRAPH_outedges_lt<_POINT,_SEGMENT>       ADJedgeS_LT;
  struct CHECKBELOW {
    bool operator() (const _POINT& p, 
                     const _SEGMENT& s0, const _SEGMENT& s1) const
    {
      _LINE sl(p, _VECTOR((_COORD)0,(_COORD)1));
      _POINT p0,p1;
      sl.intersection(_LINE(s0),p0);
      sl.intersection(_LINE(s1),p1);
      if (s0.is_vertical()) p0=p;
      if (s1.is_vertical()) p1=p;
      if (p0.ycoord() > p1.ycoord() ) return false;
      if (p0.ycoord() == p1.ycoord() && 
	 ((p0.ycoord() <= p.ycoord() && cmp_slopes(s0,s1) >= 0) ||
	  (p0.ycoord() > p.ycoord()  && cmp_slopes(s0,s1) <= 0)))
	return false;
      return true;
    }      
  };

  struct SEGFROMIT {
    _SEGMENT operator()(const typename sortseq<_SEGMENT,edge>::
                       const_iterator it) const
    { return (*it).first; }
  };  

  node                    event;
  _POINT                   p_sweep;
  OUTPUT&                 EG;
  SWEEP_STATUS_STRUCTURE  SweepLine;
  // EVENT_STRUCTURE is given implicit by graph iteration
  EMAP                    SLItem;
  SMAP                    VisEdge;
  node                    v_low,v_high; // framing nodes!
  edge                    e_low,e_high;

  _SEGMENT nil_seg;

  leda_constr_triang_traits(INPUT_ITERATOR&, INPUT_ITERATOR&, OUTPUT& out) :
    p_sweep(),EG(out),SweepLine(p_sweep),SLItem(EG,0),
    VisEdge(0),v_low(0),v_high(0) { }
  

  leda_constr_triang_traits(OUTPUT& out) :
    p_sweep(),EG(out),SweepLine(p_sweep),SLItem(EG,0),
    VisEdge(0),v_low(0),v_high(0)
  { /* SETDTHREAD(7); */ }
  
  

edge new_bi_edge(node v1, node v2)
{ // apended at v1 and v2 ajd list
  edge e1 = EG.new_edge(v1,v2,nil_seg);
  edge e2 = EG.new_edge(v2,v1,nil_seg);
  EG.set_reversal(e1,e2);
  return e1;
}

edge new_bi_edge(edge e_bf, edge e_af)
{ // ccw before e_bf and after e_af 
  edge e1 = EG.new_edge(e_bf,source(e_af),nil_seg,leda::before);
  edge e2 = EG.new_edge(e_af,source(e_bf),nil_seg,leda::behind);
  EG.set_reversal(e1,e2);
  return e1;
}

edge new_bi_edge(node v, edge e)
{ // appended at v's adj list and after e
  edge e1 = EG.new_edge(v,source(e),nil_seg);
  edge e2 = EG.new_edge(e,v,nil_seg,leda::before);
  EG.set_reversal(e1,e2);
  return e1;
}


  
bool edge_is_visible_from(node v, edge e)
{
  _POINT p =  EG[v];
  _POINT p1 = EG[EG.source(e)];
  _POINT p2 = EG[EG.target(e)];
  return left_turn(p1,p2,p);
}


void triangulate_up(edge& e_apex)
{
  TRACEN("triangulate_up" << seg(e_apex));
  node v_apex = EG.source(e_apex);
  while (true) {
    edge e_vis = EG.face_cycle_pred(EG.reversal(e_apex));
    bool in_sweep_line = (SLItem[e_vis] != 0);
    bool not_visible = !edge_is_visible_from(v_apex,e_vis);
      TRACE(" checking" << in_sweep_line << not_visible << seg(e_vis));
    if ( in_sweep_line || not_visible) {
      TRACEN(" stop"); return;
    }
    edge e_back = new_bi_edge(e_apex,e_vis);
    e_apex = e_back;
    TRACEN(" produced" << seg(e_apex));
  }
}

void triangulate_down(edge& e_apex)
{
  TRACEN("triangulate_down" << seg(e_apex));
  node v_apex = source(e_apex);
  while (true) {
    edge e_vis = EG.face_cycle_succ(e_apex);
    bool in_sweep_line = (SLItem[e_vis] != 0);
    bool not_visible = !edge_is_visible_from(v_apex,e_vis);
      TRACE(" checking" << in_sweep_line << not_visible << seg(e_vis));
    if ( in_sweep_line || not_visible) {
        TRACEN(" stop"); return;
    }
    edge e_vis_rev = EG.reversal(e_vis);
    edge e_forw = new_bi_edge(e_vis_rev,e_apex);
    e_apex = EG.reversal(e_forw);
    TRACEN(" produced" << seg(e_apex));
  }
}


void triangulate_between(edge e_upper, edge e_lower)
{
  // we triangulate the interior of the whole chain between
  // target(e_upper) and target(e_lower)
  assert(EG.source(e_upper)==EG.source(e_lower));
  TRACEN("triangulate_between" << seg(e_upper) << seg(e_lower));
  edge e_end = EG.reversal(e_lower);
  while (true) {
    edge e_vis =  EG.face_cycle_succ(e_upper);
    edge en_vis = EG.face_cycle_succ(e_vis);
    TRACEN(" working on base e_vis " << seg(e_vis));
    TRACEN(" next is " << seg(en_vis));
    if (en_vis == e_end) return;
    e_upper = EG.reversal(new_bi_edge(EG.reversal(e_vis),e_upper));
    TRACEN(" produced" << seg(e_upper));
  } 
}


void process_event() 
{
    TRACEN("PROCESS_EVENT " << p_sweep);
  edge e_first,e_last, e_end, e = EG.last_adj_edge(event);
  /* we have to search for the last ingoing edge */
  ADJedgeS_LT adjedge_lt(EG);
  edge ep=e;
  while (e && adjedge_lt(ep = EG.cyclic_adj_pred(e),e)) e=ep;
  e_last = e_end = e;
  e_first = e = ep;
  ss_handle sit_pred=0;
  ss_handle sit=0;

  if (e)
  { /* does not work with Borland C++
       sit = sit_pred = (SLItem[e] ? SweepLine.pred(SLItem[e]) : 
		      SweepLine.locate_pred(seg(e)));
     */
    if (SLItem[e]) 
       sit = SweepLine.pred(SLItem[e]);
    else
       sit = SweepLine.locate_pred(seg(e));
    sit_pred = sit;
  }

    // take either known entry point into SweepLine
    // or determine by a search
  else // event is isolated node
    sit_pred = SweepLine.locate_pred(_SEGMENT(EG[event],EG[event]));
    // look for trivial segment

  bool ending_edges(0), starting_edges(0);
  while (e) { // walk adjacency list clockwise
    if ( SLItem[e] ) {  // ACTIONS ON INGOING BUNDLE 
      /* here e is directed backwards, rev(e) is ending in event
	 for each wedge between two ending edges we triangulate
	 the whole face; we also delete all ending edges from the
	 SweepLine and mark the edges as such. */
        TRACEN("ending " << seg(e));
      if (ending_edges) 
        triangulate_between(e,EG.cyclic_adj_succ(e));

      ending_edges = true;
      SweepLine.del_item(SLItem[e]);
      link_bi_edge_to(e,0);
      // not in SL anymore
    } else {  // ACTIONS ON OUTGOING BUNDLE
      /* here e is directed forward, starting in event
 	 we only insert the segments into the SweepLine
	 and keep track of the last edge in ingoing 
	 bundle */
        TRACEN("starting " << seg(e));
      sit = SweepLine.insert_at(sit,seg(e),e);
      link_bi_edge_to(e,sit);
      if (!starting_edges) {
        // !starting_edges marks upperst edge of outgoing bundle
        e_last = EG.cyclic_adj_succ(e);
      }
      VisEdge[sit] = e;
      starting_edges = true;
    }
    if (e == e_end) break;
    e = EG.cyclic_adj_pred(e);
  }

  if (!ending_edges) {
    /* we first determine the node and a candidate edge for
       visibility search along a chain of edges bounding
       the face which contains event */
    edge e_vis = VisEdge[sit_pred];
    edge e_vis_n = EG.cyclic_adj_succ(e_vis);

    e_first = e_last = new_bi_edge(event,e_vis_n); 
    TRACEN(" producing link " << seg(e_first) << " before" << seg(e_vis_n));

  }
  triangulate_up(e_last);
  triangulate_down(e_first);
  VisEdge[sit_pred] = e_first;
}


bool event_exists() 
{ 
  if (event) { 
    // event is set at end of loop and in init
    // we stop at the right lower framing node
    p_sweep = EG[event];
    return true;
  }
  return false; 
}

void procede_to_next_event() 
{ event = EG.succ_node(event); }

  
void link_bi_edge_to(edge e, ss_handle sit)
{ 
  SLItem[e] = sit;
  SLItem[EG.reversal(e)] = sit; 
}

_SEGMENT seg(edge e)
{ return _SEGMENT(EG[EG.source(e)],EG[EG.target(e)]); }
_COORD xpos(node v) { return EG[v].xcoord(); }
_COORD ypos(node v) { return EG[v].ycoord(); }
node new_first_node(const _POINT& p) 
{ return EG.new_node(EG.first_node(),p,leda::before); }

_COORD my_abs(const _COORD& x) { if (x >= 0) return x; else return -x; } 


void initialize_structures()
{
    TRACEN("initialize_structures ");TRACEN(debug(EG));

  _COORD infty = 1; node v;
  forall_nodes(v,EG) {
    _COORD x = my_abs(xpos(v));
    _COORD y = my_abs(ypos(v));
    while (x >= infty || y >= infty) infty *= 2;
  }

  _POINT p_mm(-infty,-infty);
  _POINT p_mp(-infty, infty);
  _POINT p_pm( infty,-infty);
  _POINT p_pp( infty, infty);
  _SEGMENT s_min(p_mm,p_pm);
  _SEGMENT s_max(p_mp,p_pp);

  p_sweep = p_mm;
  ss_handle sit_low  = SweepLine.insert(s_min,0);
  p_sweep = p_mp;
  ss_handle sit_high = SweepLine.insert(s_max,0);
  // inserting frame

  event = EG.first_node();
  if (!event) return;
  p_sweep = EG[event];
  edge e;
  forall_out_edges(e,event) {
    ss_handle sit = SweepLine.insert(seg(e),e);
    link_bi_edge_to(e,sit);
    VisEdge[sit] = e;
  }
  // we move to the second node;

  v_high = new_first_node(p_mp);
  v_low  = new_first_node(p_mm);
  edge high_e = new_bi_edge(event,v_high);
  edge low_e  = new_bi_edge(event,v_low);
  // embedded ccw by appending edges

  SweepLine[sit_low]  = low_e;
  SweepLine[sit_high] = high_e;

  link_bi_edge_to(low_e ,sit_low);
  link_bi_edge_to(high_e,sit_high);
  // by marking them as in sweep line they will never be used
  // for triangulation edges to the sentinel structure !!!

  VisEdge[sit_low] = low_e;
  // source(e_vis) is visible from any point between the sentinels
  procede_to_next_event();
  event_exists(); 
  // set p_sweep for check invariants
}


  
void complete_structures() 
{
  if(v_low) {
    EG.del_node(v_low);
    EG.del_node(v_high);
  } // removing the frame
}


  
/*
#include "checker_modules.h"
#define CHECKINVARS
*/

void check_invariants()
{
#ifdef CHECKINVARS
  if (!event) return;
  sweep_line_consistency_checker<SWEEP_STATUS_STRUCTURE,
    _POINT,_SEGMENT,CHECKBELOW> C1(SweepLine);
  C1.check_at(p_sweep);
  ccw_local_embedding_checker<OUTPUT,node,edge,ADJedgeS_LT> C2(EG);
  C2.check_at(event);
  node_order_checker<OUTPUT,node,edge,nodePOS_LT>           C3(EG);
  C3.check_at(event);
#endif
}

void check_final()
{
  if (!Is_Plane_Map(EG))
    error_handler(1,"check_final::graph not planar!");
  if (!Is_Bidirected(EG))
    error_handler(1,"check_final::graph not bidirected!");
}

};



LEDA_END_NAMESPACE

