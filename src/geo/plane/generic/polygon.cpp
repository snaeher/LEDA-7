/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  polygon.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/




//------------------------------------------------------------------------------
// POLYGON
//
// 07/99  operator== added
//
// 08/00  check_simplicity added
//
// 02/01  new compute_area (faster)
//
//------------------------------------------------------------------------------


/*
#define SEGMENT_INTERSECTION  MULMULEY_SEGMENTS
*/

#define SEGMENT_INTERSECTION  SWEEP_SEGMENTS

LEDA_BEGIN_NAMESPACE 

#ifndef TESTPHASE
/*
static int sign(int x)
{ if ( x == 0 ) return 0;
  if ( x > 0  ) return 1;
  return -1;
}
*/

static bool False(string s)
{ cerr << s; return false; }

#endif


POLYGON::CHECK_TYPE POLYGON::input_check_type = POLYGON::WEAKLY_SIMPLE;


static RAT_TYPE compute_area0(const list<SEGMENT>& seg_list)
{
  if (seg_list.length() < 3) return 0;

  list_item it = seg_list.get_item(1);
  POINT     p  = seg_list[it].source();

  it = seg_list.succ(it);

  RAT_TYPE A  = 0;

  while (it)
  { SEGMENT s = seg_list[it];
    A += p.area(s.source(),s.target());
    it = seg_list.succ(it);
   }

  return A;
}



static RAT_TYPE compute_area1(const list<SEGMENT>& L)
{
  if (L.length() < 3) return 0;

  RAT_TYPE A = 0;

  SEGMENT s;
  forall(s,L)
  { POINT a = s.source();
    POINT b = s.target();
    A += a.xcoord()*b.ycoord() - b.xcoord()*a.ycoord(); 
  }

  return A/2;
}

static RAT_TYPE compute_area(const list<SEGMENT>& L)
{
  if (L.length() < 3) return 0;

  RAT_TYPE A = 0;

  SEGMENT last = L.tail();

  POINT a = last.source();
  POINT b = last.target();


  SEGMENT s;
  forall(s,L)
  {
    POINT c = s.target();
    A += b.xcoord()*(c.ycoord() - a.ycoord()); 
    a = b;
    b = c;
  }

  return A/2;
}



static int compute_orientation(const list<SEGMENT>& seg_list)
{ list_item q_it = seg_list.first();
  POINT q = seg_list[q_it].source();
  
  list_item it;
  forall_items(it,seg_list)
   if ( compare(seg_list[it].source(),q) < 0 )
   { q_it = it;
     q = seg_list[q_it].source();
   }

  POINT p = seg_list[seg_list.cyclic_pred(q_it)].source();
  POINT r = seg_list[seg_list.cyclic_succ(q_it)].source();

  return orientation(p,q,r);
}


POLYGON_REP::POLYGON_REP(const list<SEGMENT>& sl,int orientation) : 
seg_list(sl), orient(orientation)
{ if (!sl.empty())
  { SEGMENT s = sl.head();
    POINT x0 = s.source();
    POINT y0 = s.source();
    POINT x1 = s.source();
    POINT y1 = s.source();
    forall(s,sl)
    { POINT p = s.source();
      pt_list.append(p);
      if (POINT::cmp_xy(p,x0) < 0) x0 = p;
      if (POINT::cmp_xy(p,x1) > 0) x1 = p;
      if (POINT::cmp_yx(p,y0) < 0) y0 = p;
      if (POINT::cmp_yx(p,y1) > 0) y1 = p;
    }
    xmin = x0;
    xmax = x1;
    ymin = y0;
    ymax = y1;

    if (orient == 0) orient = compute_orientation(sl);
#ifdef DEBUG
    if ( orient != ( compute_area(sl) > 0 ? +1 : -1))
    { cout << "\n\nSOMETHING WRONG with orientation " << orient; }
#endif
  }
}


polygon POLYGON::to_polygon() const
{ list<point> L;
  SEGMENT s;
  forall(s,ptr()->seg_list) L.append(s.source().to_point());
  return polygon(L,polygon::NO_CHECK,polygon::RESPECT_ORIENTATION);
}


void POLYGON::normalize() const
{ 
  POINT p;
  forall(p,ptr()->pt_list) p.normalize();
  ptr()->xmin.normalize();
  ptr()->xmax.normalize();
  ptr()->ymin.normalize();
  ptr()->ymax.normalize();
}


polygon POLYGON::to_float() const { return to_polygon(); }

bool  POLYGON::operator==(const POLYGON& P1) const
{
  if (size() != P1.size()) return false;
  if (size() ==0) return true;
  
  list<POINT>& L1 = ptr()->pt_list;
  list<POINT>& L2 = P1.ptr()->pt_list;

  int n = L1.size(),i;
  list_item it1 = L1.first();
  list_item it2 = L2.first();
  
  for(i=0;i<n;i++){
     if (L1[it1]==L2[it2]) break;
     it2 = L2.succ(it2);
  }
  
  if (it2==nil) return false; 
  
  for(i=0;i<n;i++){
    if (L1[it1] != L2[it2]) return false;
     it1= L1.cyclic_succ(it1);
     it2= L2.cyclic_succ(it2);
  }
  return true;
}

ostream& operator<<(ostream& out, const POLYGON& p) 
{ p.vertices().print(out);
  out << endl;
  return out;
 } 

istream& operator>>(istream& in,  POLYGON& p) 
{ list<POINT> L; 
  L.read(in,'\n'); 
  p = POLYGON(L,POLYGON::input_check_type);
  return in;
}


static bool check_simplicity_segments(const list<SEGMENT>& seg_list)
{ GRAPH<POINT,SEGMENT> G;
  SEGMENT_INTERSECTION(seg_list,G);

  if (G.number_of_nodes() < seg_list.length()) return false;

  if (G.number_of_nodes() > 1)
  { node v;
    forall_nodes(v,G)
      if (G.degree(v) != 2) return false;
   }
  return true;
}

bool POLYGON::is_simple() const
{ return check_simplicity_segments(ptr()->seg_list); }

bool POLYGON::is_convex() const
{
  if (orientation() == -1) return false;
  
  if (size() < 4) return true;
  
  if (! is_simple()) return false;
  
  list<POINT> verts = vertices();
  
  list_item it,before,next;
  forall_items(it,verts) {
    before = verts.cyclic_pred(it);
    next = verts.cyclic_succ(it);

    int ori = verts[before].orientation(verts[it], verts[next]);
   
    if (ori == -1) return false;
  }
  
  return true;  
}


static bool check_weak_simplicity(const list<SEGMENT>& seg_list)
{ 
  // M. Baesken (06/2001): polygons with less than 4 points are simple
  if (seg_list.size() < 4) return true;

  GRAPH<POINT,SEGMENT> G;
  SEGMENT_INTERSECTION(seg_list,G,true);

  if ( G.number_of_edges() != 2*seg_list.length() ) return false;

  node v; edge e;
  forall_edges(e,G)
   if ( target(e) == target(G.cyclic_adj_succ(e)) ) return false;

// the remainder is copied from check_rep

map<SEGMENT,edge> segment_to_edge;

forall_edges(e,G)
{ SEGMENT s = G[e];
  node v = G.source(e);
  segment_to_edge[s] = ( s.source() == G[v] ? e : G.reversal(e) );
}

edge_array<int> label(G);
int count = 0;
//forall(P,pol_list)
{ list_item it;
  //const list<SEGMENT>& seg_list = P.segments();
  forall_items(it,seg_list) 
  { SEGMENT s1 = seg_list[it];

if (s1.is_trivial()) continue; // s.n. (Oct 2006)

    edge e = segment_to_edge[s1]; 
    e = G.reversal(e);

    SEGMENT s2 = seg_list[seg_list.cyclic_succ(it)];

if (s2.is_trivial()) continue; // s.n. (Oct 2006)

    edge f = segment_to_edge[s2];

    assert(e);
    assert(f);
    label[e] = label[f] = count++;
  } 
}


forall_nodes(v,G)
{ stack<int> S;
  forall_adj_edges(e,v)
  { if ( S.empty() || label[e] != S.top() ) 
      S.push(label[e]);
    else
      S.pop();
  }
  //if ( !S.empty() ) return False("check_rep: crossing");
  if ( !S.empty() ) return false;
}
    

return true;
}
  

bool POLYGON::is_weakly_simple() const
{ return check_weak_simplicity(ptr()->seg_list); }



// the following variant returns all proper intersections

bool POLYGON::is_weakly_simple( list<POINT>& IP ) const
{
  // Joerg Schwerdt 
  // Algorithmic Solutions
  // June 2001

  const list<SEGMENT>& seg_list = ptr()->seg_list;

  GRAPH<POINT,SEGMENT> G;
  SEGMENT_INTERSECTION(seg_list,G,true);

/// aus check_weak_simplicity
  map<SEGMENT,edge> segment_to_edge;
  edge e;
  forall_edges(e,G)
  { SEGMENT s = G[e];
    node v = G.source(e);
    segment_to_edge[s] = ( s.source() == G[v] ? e : G.reversal(e) );
  }

  edge_array<int> label(G);
  int count = 0;
  { list_item it;
    forall_items(it,seg_list)
    { SEGMENT s1 = seg_list[it];
      if (s1.is_trivial()) continue; // s.n. (Oct 2006)
      edge e = segment_to_edge[s1];
      e = G.reversal(e);
      SEGMENT s2 = seg_list[seg_list.cyclic_succ(it)];
      if (s2.is_trivial()) continue; // s.n. (Oct 2006)
      edge f = segment_to_edge[seg_list[seg_list.cyclic_succ(it)]];
      label[e] = label[f] = count++;
    }
  }


  // search initial point of polygon in G
  POINT p = seg_list.head().source();
  node n;
  forall_nodes(n,G)
  {
    if(G[n]==p)
      break;
  }

  IP.clear();
  list_item it;
  forall_items(it,seg_list)
  {
    POINT p(seg_list[it].source());

    if( G.degree(n) > 4 )  // gerichtete Kanten in beide Richtungen
    {
      if( G[n]!=p )  // echter Schnittpunkt
      {
        IP.append(G[n]);
      }
      else
      {
/// aus check_weak_simplicity
        stack<int> S;
        edge e;
        forall_adj_edges(e,n)
        { if ( S.empty() || label[e] != S.top() )
            S.push(label[e]);
          else
            S.pop();
        }
        if ( !S.empty() )
        {
          IP.append(G[n]);
        }
////////////
      }
    }
    list<edge> EL = G.out_edges(n);
    list_item ite;
    forall_items(ite,EL)
    {
      if(seg_list[it].contains(G[G.target(EL[ite])]))
      {
        n = G.target(EL[ite]);
        break;
      }
    }
  }
  return( IP.empty() );
}




POLYGON::CHECK_TYPE POLYGON::check_simplicity() const
{
  const list<SEGMENT>& seg_list = segments();

  GRAPH<POINT,SEGMENT> G;
  SEGMENT_INTERSECTION(seg_list,G,true);

  // Kurt: I moved the following line from further down.
  if (G.number_of_edges() != 2*seg_list.length()) 
        return NOT_WEAKLY_SIMPLE;
  
  bool flag = true;
  if (G.number_of_nodes() > 1)
  { node v;
    forall_nodes(v,G){
      // degree 4 because of reversals ...
      if (G.degree(v) != 4) flag = false; // not simple, maybe weakly simple?
    }  
   }
  if (flag ) return SIMPLE;           
  



  node v; edge e;
  forall_edges(e,G)
   if ( target(e) == target(G.cyclic_adj_succ(e)) ) return NOT_WEAKLY_SIMPLE;

  // the remainder is copied from check_rep
  map<SEGMENT,edge> segment_to_edge;

  forall_edges(e,G)
  { SEGMENT s = G[e];
    node v = G.source(e);
    segment_to_edge[s] = ( s.source() == G[v] ? e : G.reversal(e) );
  }

  edge_array<int> label(G);
  int count = 0;

  list_item it;

  forall_items(it,seg_list) 
  { edge e = segment_to_edge[seg_list[it]]; 
    e = G.reversal(e);
    edge f = segment_to_edge[seg_list[seg_list.cyclic_succ(it)]];
    label[e] = label[f] = count++;
  } 
  
  forall_nodes(v,G)
  { stack<int> S;
    forall_adj_edges(e,v)
    { if ( S.empty() || label[e] != S.top() ) 
        S.push(label[e]);
      else
        S.pop();
    }
    if ( !S.empty() ) return NOT_WEAKLY_SIMPLE; 
  }
  
  return WEAKLY_SIMPLE;
}



POLYGON::POLYGON(const list<SEGMENT>& sl,int orientation)
{ PTR = new POLYGON_REP(sl,orientation); }
 

POLYGON::POLYGON(const list<POINT>& pl, POLYGON::CHECK_TYPE check, 
RESPECT_TYPE respect_orientation)
{ 
  list<SEGMENT> seglist;

  list_item it;
  forall_items(it,pl)
      seglist.append(SEGMENT(pl[it],pl[pl.cyclic_succ(it)]));

  // changed by sn  12/2007
  // construct legal object even in presence of errors (exceptions !)

  if ( check == SIMPLE && !check_simplicity_segments(seglist) )
  { PTR = new POLYGON_REP(list<SEGMENT>());
    LEDA_EXCEPTION(1,"POLYGON: POLYGON is not simple");
   }

  if ( check == WEAKLY_SIMPLE && !check_weak_simplicity(seglist) )
  { PTR = new POLYGON_REP(list<SEGMENT>());
    LEDA_EXCEPTION(1,"POLYGON: POLYGON is not weakly simple");
   }

 
  if ( respect_orientation == DISREGARD_ORIENTATION 
                                        && compute_area(seglist) < 0)
  { // reverse edge list
    seglist.clear();
    forall_rev_items(it,pl)
       seglist.append(SEGMENT(pl[it],pl[pl.cyclic_pred(it)]));
   }

  PTR = new POLYGON_REP(seglist);
}


list<POLYGON> POLYGON::simple_parts() const
{
  list<SEGMENT>& seglist = ptr()->seg_list;

  GRAPH<POINT,SEGMENT> G;
  SEGMENT_INTERSECTION(seglist,G,true);

  list<POLYGON> result;


  edge_array<bool> considered(G,0);
  node_array<int>  mark(G,0);
  int cur_mark = 0;

  // compute faces

  edge e;
  forall_edges(e,G)
  { 
    if (considered[e]) continue;

    cur_mark++;

    POINT a = G[source(e)];
    POINT b = G[target(e)];

    edge x = e;

    list<SEGMENT> pol;
    RAT_TYPE A = 0;

    bool ignore = false;

    do { if (mark[target(x)] == cur_mark) ignore = true;
         mark[target(x)] = cur_mark;
         POINT p = G[source(x)];
         POINT q = G[target(x)];
         pol.push(SEGMENT(q,p));
         considered[x] = true;
         A += a.area(p,q);
         x = G.face_cycle_succ(x);
    }
    while (x != e);

    if (ignore) continue;

    POLYGON P(pol);
    if (A < 0) result.push(P);
    if (A > 0) result.append(P);
   } 

  if (result.head().area() < 0) // all faces have negative area
  { list_item it;
    forall_items(it,result)
      result[it] = result[it].complement();
   }

  if (is_weakly_simple() && result.length() == 2 &&
      G.number_of_nodes() <= seglist.length())
  { POLYGON A = result.head();
    POLYGON B = result.tail();
    if (A.area() > 0 && B.area() < 0) 
    { result.clear();
      result.append(*this);
     }
   }
  
  return result;
}
   

/* This is a proposal for replacing the original simple_parts function.
   The idea is the following: We construct the planar map induced by the 
   segments of P (with a segment sweep). Then we extract those face cycles 
   which have the property that at least one edge on the cycle points in the 
   same direction as the associated edge. We discard face cycles consisting 
   only of edges that are antiparallel to their segments.
   Hopefully, the list of resulting polygons has the property that their union
   has the same inner points as P.
*/

static
void add_polygon_for_cycle(list<POLYGON>& polys, 
						   const list<edge>& cycle, 
						   const GRAPH<POINT,SEGMENT>& G,
						   bool strict)
{
    list<POINT> vertices;

	bool all_parallel = true;
	bool all_antiparallel = true;

	edge e;
	forall(e, cycle) {
		POINT p = G[source(e)];
		POINT q = G[target(e)];
		SEGMENT s = G[e];
		if ( (q - p) * (s.target() - s.source()) > 0 ) 
		  all_antiparallel = false; // edge and associated segment point in same direction
		else
		  all_parallel = false; // edge and associated segment point in opposite directions
		vertices.append(p);
    }

	if (!vertices.empty() 
		&& ( (strict && all_parallel) || (!strict && !all_antiparallel) ) )
    {
		POLYGON P(vertices, POLYGON::NO_CHECK);
		if (P.orientation() > 0) polys.push(P);
		if (P.orientation() < 0) polys.append(P);
	}
}

/*
	More documentation:
	First we compute the (embedded) map G induced by the segments of the 
	polygon.
	By definition, a polygonal chain is weakly simple if its segments are
	disjoint except for common endpoints.
	Hence, each face-cycle of G is almost a weakly simple polygonal chain.
	Why almost? A face-cycle may be self-intersecting, but since every isolated
	intersection point corresponds to a vertex in G, isolated intersections
	are no problem. (Here isolated means that there is an environment around 
	the intersection point that does not contain any other intersection point.)
	However, there may be non-isolated intersection which correspond to 
	anti-parallel edges in G.
	The function below works as follows: It traces the face-cylces of G.
	While looking at a particular face-cycle f it constructs a list<edge> 
	cycle, which contains (a subset of) the edges of f.
	For every new edge c=(u,v) on f it checks whether c is anti-parallel to 
	some edge c'=(v,u) in the list cycle. In order to this efficiently it 
	stores for every node n that has been visited already the list_item in 
	cycle that corresponds to the last visited edge on f that leaves n.
	If two anti-parallel edges are found we are in the following situation:
	cycle = e1, e2, ..., c'=(v,u), g1, g2, ..., gl, c=(u,v)
	where l may be zero.
	The function extracts the subcyle g1, g2, ..., gl from cycle and deletes
	the edges c and c' from cycle and continues with the next edge on f.
	Note that in the ccw-ordering around the node u there can be no edge 
	between reversal(c) and g1 and no edge between reversal(gl) and c'.
	Hence, if all edges parallel to c or c' are deleted from G, then the 
	subcycle g1, g2, ..., gl would become a face cycle of G.
*/

list<POLYGON> POLYGON::split_into_weakly_simple_parts(bool strict) const
{
	list<POLYGON> result;

	list<SEGMENT>& seglist = ptr()->seg_list;

	GRAPH<POINT,SEGMENT> G;
	SEGMENT_INTERSECTION(seglist, G, true);

	if ( G.number_of_edges() == 2*G.number_of_nodes() 
		   && G.number_of_nodes() == seglist.length() ) {
		// *this is simple (cf. make_chains_simple in gen_polygon.cpp)
		result.append(*this);
		return result;
	}

    // BugFix 19.5.2006
    // If there are parallel segments, G has parallel edges. These parallel edges are
    // sorted randomly in the adjacency lists. It may be that a cycle takes the wrong
    // edge, i.e. the edge not corresponding to the correct segment. Then it may
    // also be that the cycle orientation gets false, as the orientation from the
    // incorrect segment is used. There is an example where this happened.
    // Our idea: We go through the outedges of every node and check, if the 
    // predecessor (which gets the cyclic_adj_succ) has a parallel edge. If this is
    // the case we search for the parallel edge corresponding to the current outedge.
    // This is found using the corresponding number of the segment. 
    // When the corresponding edge is not the predecessor edge, we switch the 
    // predecessor edge and the corresponding edge.
    map<SEGMENT, int> number;

    int idx = 0;
    SEGMENT s;
    forall(s, seglist)
        number[s] = idx++;

    node u;
    forall_nodes(u, G) {
        if (G.outdeg(u) < 3) continue;

        // Since the code below may change the order of the adjacency list of u
        // we use copy for the iteration!
        list<edge> out_edges = G.out_edges(u);
        edge e;
        forall(e, out_edges) {
            edge first_pred = G.cyclic_adj_pred(e);
            edge second_pred = G.cyclic_adj_pred(first_pred);

            if (target(first_pred) != target(second_pred)) continue;

            // now we know that first_pred and second_pred are parallel

            int corresp_number_for_e;

            if (G[e].source() == G[u]) {
                // segment of e starts in node u
                corresp_number_for_e = number[ G[e] ] - 1;
                if ( corresp_number_for_e < 0 )
                    corresp_number_for_e = seglist.size() - 1;
            }
            else if (G[e].target() == G[u]) {
                // segment of e ends in node u
                corresp_number_for_e 
                    = (number[ G[e] ] + 1) % seglist.size();
            }
            else {
                // segment of e passes through node u, i.e. u is an inner point
                corresp_number_for_e = number[ G[e] ];
            }

            if (number[ G[first_pred] ] == corresp_number_for_e) continue;

            node v = target(first_pred);

            while (number[ G[second_pred] ] != corresp_number_for_e
                   && v == target(second_pred) && second_pred != e)
            {
                second_pred = G.cyclic_adj_pred(second_pred);
            }

            if (v == target(second_pred) && second_pred != e)
            {
/*
                cout << ++ii << ") u: " << G[u].to_float() << endl;

                cout << "e: " << number[ G[e] ] 
                     << G[e].to_float() << endl;

                cout << "corresp_number_for_e: " << corresp_number_for_e << endl;

                cout << "first: " << number[ G[first_pred] ] 
                     << G[first_pred].to_float() << endl;

                cout << "second: " << number[ G[second_pred] ] 
                     << G[second_pred].to_float() << endl;
                cout << endl;
*/
                edge first_rev = G.reversal(first_pred);
                edge second_rev = G.reversal(second_pred);

                G.move_edge(second_pred, first_pred, v, leda::behind);
                G.move_edge(second_rev, first_rev, u, leda::before);
            }
        }
    }

    // end BugFix

	edge_array<bool>      considered(G, 0);
	node_array<int>       mark(G, 0);
	node_array<list_item> out_edge_item(G, nil);
	int cur_mark = 0;

	// compute face cycles

	edge e;
	forall_edges(e, G) {
		if (considered[e]) continue;
		++cur_mark;

		list<edge> cycle;
		edge c = e;
		do {
			node u = source(c), v = target(c);
			list_item out_it_u = out_edge_item[u] = cycle.append(c);
#if ( KERNEL == FLOAT_KERNEL )
                        if (mark[v] == cur_mark && out_edge_item[v] == nil) {     
                          LEDA_EXCEPTION( 1, "map incosistent, switch to rational kernel!" );
                          return list<POLYGON>();
                        }
#endif      
			if (mark[v] == cur_mark && target(cycle[out_edge_item[v]]) == u) {
				// found two antiparallel edges on the cycle ...
				// NOTE: out_edge_item[v] is only valid if mark[v] == cur_mark,
				//       i.e. v has already been visited by the current cycle

				// extract the subcycle (which is edge-simple and contains no overlapping edges)
				list_item in_it_u = out_edge_item[v];
				list<edge> subcycle;
				cycle.extract(in_it_u, out_it_u, subcycle, false);
#if ( KERNEL == FLOAT_KERNEL )
                                edge ss;
                                forall(ss, subcycle) out_edge_item[source(ss)] = nil;
                                out_edge_item[v] = nil;
#endif                
				add_polygon_for_cycle(result, subcycle, G, strict);

				// remove the two parallel edges between u and v from the cycle
				cycle.del_item(out_it_u);
				cycle.del_item(in_it_u);				
			}
			mark[v] = cur_mark;
			considered[c] = true;
			c = G.face_cycle_succ(c);
		} while (c != e);
		add_polygon_for_cycle(result, cycle, G, strict);
	}

	return result;
}

/*
GEN_POLYGON POLYGON::make_weakly_simple(bool with_neg_parts, bool strict) 
is implemented in gen_polygon.cpp
*/

/*
GEN_POLYGON POLYGON::buffer(RAT_TYPE distance, int p = 3) const
is implemented in gen_polygon.cpp
*/

/*
//change by sn (06/2007)
POLYGON POLYGON::translate(RAT_TYPE dx, RAT_TYPE dy) const
{ list<SEGMENT> sl;
  SEGMENT s;
  forall(s,ptr()->seg_list) sl.append(s.translate(dx,dy));
  return POLYGON(sl);
}

POLYGON POLYGON::translate(const VECTOR& v) const
{ list<SEGMENT> sl;
  SEGMENT s;
  forall(s,ptr()->seg_list) sl.append(s.translate(v));
  return POLYGON(sl);
}

POLYGON POLYGON::rotate90(const POINT& p, int i) const
{ list<SEGMENT> sl;
  SEGMENT s;
  forall(s,ptr()->seg_list) sl.append(s.rotate90(p,i));
  return POLYGON(sl);
}


POLYGON POLYGON::reflect(const POINT& p, const POINT& q) const
{ list<SEGMENT> sl;
  SEGMENT s;
  forall(s,ptr()->seg_list) sl.append(s.reflect(p,q));
  return POLYGON(sl);
}


POLYGON POLYGON::reflect(const POINT& p) const
{ list<SEGMENT> sl;
  SEGMENT s;
  forall(s,ptr()->seg_list) sl.append(s.reflect(p));
  return POLYGON(sl);
}

*/


POLYGON POLYGON::translate(RAT_TYPE dx, RAT_TYPE dy) const
{ list<POINT> L;
  POINT p;
  forall(p,ptr()->pt_list) L.append(p.translate(dx,dy));
  return POLYGON(L,POLYGON::NO_CHECK,POLYGON::RESPECT_ORIENTATION);
}

POLYGON POLYGON::translate(const VECTOR& v) const
{ list<POINT> L;
  POINT p;
  forall(p,ptr()->pt_list) L.append(p.translate(v));
  return POLYGON(L,POLYGON::NO_CHECK,POLYGON::RESPECT_ORIENTATION);
}



POLYGON POLYGON::rotate90(const POINT& p, int i) const
{ list<POINT> L;
  POINT q;
  forall(q,ptr()->pt_list) L.append(q.rotate90(p,i));
  return POLYGON(L,POLYGON::NO_CHECK,POLYGON::RESPECT_ORIENTATION);
}


POLYGON POLYGON::reflect(const POINT& p, const POINT& q) const
{ list<POINT> L;
  POINT r;
  forall(r,ptr()->pt_list) L.append(r.reflect(p,q));
  return POLYGON(L,POLYGON::NO_CHECK,POLYGON::RESPECT_ORIENTATION);
}


POLYGON POLYGON::reflect(const POINT& p) const
{ list<POINT> L;
  POINT q;
  forall(q,ptr()->pt_list) L.append(q.reflect(p));
  return POLYGON(L,POLYGON::NO_CHECK,POLYGON::RESPECT_ORIENTATION);
}




list<POINT> POLYGON::intersection(const SEGMENT& s) const
{ list<POINT> result;
  SEGMENT t;
  forall(t,ptr()->seg_list) 
  { POINT p;
    if (!s.intersection(t,p)) continue;
    if (result.empty() || (p != result.tail() && p != result.head())) 
       result.append(p);
   }
  return result;
}



list<POINT> POLYGON::intersection(const LINE& l) const
{ list<POINT> result;
  SEGMENT t;
  forall(t,ptr()->seg_list) 
  { POINT p;
    if (!l.intersection(t,p)) continue;
    if (result.empty() || (p != result.tail() && p != result.head())) 
       result.append(p);
   }

  return result;
}


POLYGON POLYGON::intersect_halfplane(const LINE& l) const
{ list<POINT> result;
  POINT old_p = ptr()->pt_list.tail();

  int old_orient = l.orientation(old_p);

  POINT p;
  forall(p,ptr()->pt_list) 
  { 
    int orient = l.orientation(p);

    if ((orient > 0 && old_orient < 0) || (orient < 0 && old_orient > 0))
    { // append point of intersection
      POINT q;
      assert(l.intersection(SEGMENT(old_p,p),q));
      result.append(q);
    }
    if (orient >= 0) result.append(p);
    old_p = p;
    old_orient = orient;
  }
  return POLYGON(result);
}



POLYGON POLYGON::complement() const
{ list<SEGMENT> R;
  SEGMENT s;
  forall(s,ptr()->seg_list) R.push(SEGMENT(s.target(),s.source()));
  return POLYGON(R, - orientation());
}


POLYGON POLYGON::eliminate_colinear_vertices() const
{ list<POINT> L = vertices();
  if (L.length() > 2)
  { list_item it;
    forall_items(it,L)
    { list_item it1 = L.cyclic_pred(it);
      list_item it2 = L.cyclic_succ(it);
      if (L.length() > 2 && L[it1].orientation(L[it],L[it2]) == 0) 
          L.del_item(it);
     }
  }

  return POLYGON(L,POLYGON::NO_CHECK);
}



region_kind POLYGON::region_of(const POINT& p) const
{
  // use extreme vertices for a quick test.

  int cx1 = POINT::cmp_xy(p,ptr()->xmin);
  int cx2 = POINT::cmp_xy(p,ptr()->xmax);
  int cy1 = POINT::cmp_yx(p,ptr()->ymin);
  int cy2 = POINT::cmp_yx(p,ptr()->ymax);

  if (cx1 < 0 || cx2 > 0 || cy1 < 0 || cy2 > 0) 
    return UNBOUNDED_REGION;

  list<SEGMENT>& seglist = ptr()->seg_list;

  // check boundary segments

  list_item it;
  forall_items(it,seglist)
  { SEGMENT s = seglist[it];
    if (s.contains(p)) return ON_REGION;
  }

  // count intersections with vertical ray starting in p

  int count  = 0;

  forall_items(it,seglist)
  { SEGMENT s = seglist[it];
    POINT a = s.source();  POINT b = s.target();

    int orient = POINT::cmp_x(a,b);
    if ( orient == 0 ) continue;
    if ( orient > 0 )
    { // a is right of b
      leda_swap(a,b);
    }

    if (POINT::cmp_x(a,p)<=0 && POINT::cmp_x(p,b)<0 && a.orientation(b,p)<0)
      count++;
  }
  return ( count % 2 == 0 ? UNBOUNDED_REGION : BOUNDED_REGION );

}


int POLYGON::side_of(const POINT& p) const
{ region_kind k = region_of(p);
  switch (k) {
    case ON_REGION:        return 0;
    case BOUNDED_REGION:   return   ptr()->orient;
    case UNBOUNDED_REGION: return -(ptr()->orient);
    default:               assert( 0 == 1); return 0;
  }
}
  


RAT_TYPE  POLYGON::area0() const { return compute_area0(ptr()->seg_list); }

RAT_TYPE  POLYGON::area1() const { return compute_area1(ptr()->seg_list); }

RAT_TYPE  POLYGON::area() const { return compute_area(ptr()->seg_list); }


POLYGON   reg_n_gon(int n, CIRCLE C, double epsilon)
{ 
  list<POINT> L;
  double two_pi = 2* LEDA_PI;
  for (int i = 0; i < n; i++)
  { double alpha = two_pi * i / n;
    L.append(C.point_on_circle(alpha,epsilon)); 
  }

  return POLYGON(L);
}


POLYGON   n_gon(int n, CIRCLE C, double epsilon)
{ 
#if ( KERNEL == FLOAT_KERNEL ) || ( KERNEL == REAL_KERNEL )
return reg_n_gon(n,C,epsilon);
#else
circle c = C.to_circle();
polygon P = reg_n_gon(n,c,epsilon);

point p;
int k = 1; double two_to_k = 2;
forall(p,P.vertices()) 
{ while (p.xcoord() > two_to_k ) { k++; two_to_k *= 2; }
  while (p.ycoord() > two_to_k ) { k++; two_to_k *= 2; }
}
if ( k > 10 ) 
  cerr << "n_gon: conversion to rat_polygon looses significant precision";

int prec = 30 - k; 
return POLYGON(P,prec);
#endif
}


static void A(int, list<point>&, double&, double&, double, double);
static void B(int, list<point>&, double&, double&, double, double);
static void C(int, list<point>&, double&, double&, double, double);
static void D(int, list<point>&, double&, double&, double, double);

static void A(int i, list<point>& L, double& x, double& y, double dx, double dy)
{ if (i > 0)
  { D(i-1,L,x,y,dx,dy); x-=dx; L.append(point(x,y));
    A(i-1,L,x,y,dx,dy); y-=dy; L.append(point(x,y));
    A(i-1,L,x,y,dx,dy); x+=dx; L.append(point(x,y));
    B(i-1,L,x,y,dx,dy);
   }
 }

static void B(int i, list<point>& L, double& x, double& y, double dx, double dy)
{ if (i > 0)
  { C(i-1,L,x,y,dx,dy); y+=dy; L.append(point(x,y));
    B(i-1,L,x,y,dx,dy); x+=dx; L.append(point(x,y));
    B(i-1,L,x,y,dx,dy); y-=dy; L.append(point(x,y));
    A(i-1,L,x,y,dx,dy);
   }
 }

static void C(int i, list<point>& L, double& x, double& y, double dx, double dy)
{ if (i > 0)
  { B(i-1,L,x,y,dx,dy); x+=dx; L.append(point(x,y));
    C(i-1,L,x,y,dx,dy); y+=dy; L.append(point(x,y));
    C(i-1,L,x,y,dx,dy); x-=dx; L.append(point(x,y));
    D(i-1,L,x,y,dx,dy);
   }
 }

static void D(int i, list<point>& L, double& x, double& y, double dx, double dy)
{ if (i > 0)
  { A(i-1,L,x,y,dx,dy); y-=dy; L.append(point(x,y));
    D(i-1,L,x,y,dx,dy); x-=dx; L.append(point(x,y));
    D(i-1,L,x,y,dx,dy); y+=dy; L.append(point(x,y));
    C(i-1,L,x,y,dx,dy);
   }
 }


static polygon Hilbert(int n, double x1, double y1, double x2, double y2) 
{
  double w = x2-x1;
  double h = y2-y1;

  double dx = w/(1<<n);
  double dy = h/(1<<n);

  double x = x2 - dx;
  double y = y2 - dy/2;

  list<point> L;
  L.append(point(x+dx,y));
  L.append(point(x,y));

  A(n,L,x,y,dx,dy);

  L.append(point(x+dx,y));

  return polygon(L);
}


POLYGON hilbert(int n, RAT_TYPE x1, RAT_TYPE y1, RAT_TYPE x2, RAT_TYPE y2) 
{ 
#if ( KERNEL == REAL_KERNEL )
// make sure that the double approximation of the input is good enough
x1.to_close_double();
y1.to_close_double();
x2.to_close_double();
y2.to_close_double();
#endif

#if ( KERNEL == RAT_KERNEL ) || ( KERNEL == REAL_KERNEL )
int prec = 10*(1<<n);
double d_x1 = x1.to_double();
double d_y1 = y1.to_double();
double d_x2 = x2.to_double();
double d_y2 = y2.to_double();

return POLYGON(Hilbert(n,d_x1,d_y1,d_x2,d_y2),prec); 
#else
return Hilbert(n,x1,y1,x2,y2); 
#endif
}




RAT_TYPE POLYGON::sqr_dist(const POINT& p) const
{ if ( ptr()->seg_list.empty() ) return 0;
  SEGMENT s = ptr()->seg_list.head();
  RAT_TYPE dmin = s.sqr_dist(p);
  forall(s,ptr()->seg_list) 
  { RAT_TYPE d = s.sqr_dist(p);
    if (d < dmin) dmin = d;
    if (dmin == 0) break;
  }
  return dmin;
}


#if (KERNEL == FLOAT_KERNEL)

polygon::polygon(const polygon& P,int /*prec*/) : HANDLE_BASE(polygon_rep)(P) {}

double polygon::distance(const point& p) const 
{ return ::sqrt(sqr_dist(p)); }

polygon polygon::translate(double dx, double dy, double dw) const
{  return translate(dx/dw,dy/dw); }

polygon polygon::translate_by_angle(double alpha, double d) const
{ list<segment> sl;
  segment s;
  forall(s,ptr()->seg_list) sl.append(s.translate_by_angle(alpha,d));
  return polygon(sl);
}

polygon polygon::rotate(const point& p, double alpha) const
{ list<segment> sl;
  segment s;
  forall(s,ptr()->seg_list) sl.append(s.rotate(p,alpha));
  return polygon(sl);
}

polygon polygon::rotate(double alpha) const
{ return rotate(point(0,0),alpha); }

#include <LEDA/geo/rat_polygon.h>

rat_polygon polygon::to_rational(int prec) const
{
	if (empty()) return rat_polygon();

	list<rat_point> rat_pnts;

	point p;
	forall_vertices(p, *this) rat_pnts.append( rat_point(p,prec) );

	return rat_polygon(rat_pnts);
}

#endif


#if (KERNEL == REAL_KERNEL)

POLYGON::POLYGON(const polygon& P, int prec)
{ list<POINT> pl;
  point p;
  forall_vertices(p,P) 
  { 
    pl.append(POINT(p,prec));
  }

  list<SEGMENT> seglist;

  list_item it;
  forall_items(it,pl)
  { POINT p = pl[it];
    POINT q = pl[pl.cyclic_succ(it)];
    if (p == q) continue;
    seglist.append(SEGMENT(p,q));
   }

  PTR = new POLYGON_REP(seglist);
}

NUMBER POLYGON::distance(const POINT& p) const 
{ return sqrt(sqr_dist(p)); }

POLYGON POLYGON::translate(NUMBER dx, NUMBER dy, NUMBER dw) const
{  return translate(dx/dw,dy/dw); }

#include <LEDA/geo/rat_polygon.h>

rat_polygon POLYGON::to_rational(int prec) const
{
	if (empty()) return rat_polygon();

	list<rat_point> rat_pnts;

	POINT p;
	forall_vertices(p, *this) rat_pnts.append(rat_point(p.to_point(),prec));

	return rat_polygon(rat_pnts);
}

#endif



#if (KERNEL == RAT_KERNEL)

rat_polygon::rat_polygon(const polygon& P, int prec)
{ list<rat_point> pl;
  point p;
  forall_vertices(p,P) 
  { 
    pl.append(rat_point(p,prec));
  }

  list<SEGMENT> seglist;

  list_item it;
  forall_items(it,pl)
  { POINT p = pl[it];
    POINT q = pl[pl.cyclic_succ(it)];
    if (p == q) continue;
    seglist.append(SEGMENT(p,q));
   }

  PTR = new POLYGON_REP(seglist);
}

rat_polygon rat_polygon::translate(integer dx, integer dy, integer dw) const
{  return translate(rat_vector(dx,dy,dw)); }

#endif


LEDA_END_NAMESPACE 
