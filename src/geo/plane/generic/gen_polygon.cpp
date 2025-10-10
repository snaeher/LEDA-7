/*******************************************************************************
+
+  LEDA 6.6  
+
+
+  gen_polygon.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// GEN_POLYGON
//
//
//------------------------------------------------------------------------------


#include <LEDA/core/set.h>

/*
#define SEGMENT_INTERSECTION  MULMULEY_SEGMENTS
*/

#define SEGMENT_INTERSECTION  SWEEP_SEGMENTS

/*
#if ( KERNEL == RAT_KERNEL )
#define SEGMENT_INTERSECTION  CURVE_SWEEP_SEGMENTS
#else
#define SEGMENT_INTERSECTION  SWEEP_SEGMENTS
#endif
*/

LEDA_BEGIN_NAMESPACE 

#if ( KERNEL == FLOAT_KERNEL )
inline double abs(double d) { return d>=0 ? d : -d; }
inline int sign(double d) { return d>0 ? +1 : (d<0 ? -1 : 0); }
#endif

#ifndef TESTPHASE
/*
static int sign(int x)
{ if ( x == 0 ) return 0;
  if ( x > 0  ) return 1;
  return -1;
}
*/

static bool False(string s)
{ LEDA_EXCEPTION(0,s);
  return false; 
}

#endif


GEN_POLYGON::CHECK_TYPE GEN_POLYGON::input_check_type = 
                                     GEN_POLYGON::WEAKLY_SIMPLE;


bool GEN_POLYGON::operator==(const GEN_POLYGON&) const 
{ LEDA_EXCEPTION(1,"gen_polygon::operator== not implemented.");
  return 0;
 }


//static typedef GEN_POLYGON_REP::KIND KIND;

static GEN_POLYGON_REP::KIND opposite(GEN_POLYGON_REP::KIND k)
{ switch (k)
  { case GEN_POLYGON_REP::EMPTY: return GEN_POLYGON_REP::FULL;
    case GEN_POLYGON_REP::FULL:   return GEN_POLYGON_REP::EMPTY;
    default: LEDA_EXCEPTION(1,"should not be applied to NON_TRIVIAL"); return GEN_POLYGON_REP::NON_TRIVIAL;
  }
}


/*
  Note: We cannot simply each polygon pol by pol.eliminate_colinear_vertices().
        This might turn a weakly simply gen_polygon into a non-weakly simple 
                one. This is because two polygons P,Q might share a vertex v that is 
                colinear in P but not Q. In that case we have to keep v in P and in Q!
*/
GEN_POLYGON GEN_POLYGON::eliminate_colinear_vertices() const
{ if (trivial()) return *this;

  set<POINT> vertices_to_keep;
  POLYGON pol;
  forall(pol, polygons()) {
    pol = pol.eliminate_colinear_vertices();
        POINT v;
        forall_vertices(v, pol)
      vertices_to_keep.insert(v);
  }

  list<POLYGON> L;
  forall(pol, polygons()) {
    list<POINT> vertices;
    POINT v;
    forall_vertices(v, pol) {
      if (vertices_to_keep.member(v)) vertices.append(v);
    }
    L.append( POLYGON(vertices, POLYGON::NO_CHECK) );
  }
  return GEN_POLYGON(L, NO_CHECK);
}


region_kind GEN_POLYGON::region_of(const POINT& p) const
{ if ( empty() ) return UNBOUNDED_REGION;
  if ( full() )  return BOUNDED_REGION;
  int side = side_of(p);
  if ( side == 0 ) return ON_REGION;
  return side * polygons().head().orientation() > 0 ?
        BOUNDED_REGION : UNBOUNDED_REGION; 
}


RAT_TYPE GEN_POLYGON::area() const
{   
  RAT_TYPE A  = 0;

  if ( empty() ) return A;
  if ( full() ) LEDA_EXCEPTION(1,"area: cannot compute area of full polygon");

  POLYGON P;

  forall(P,ptr()->pol_list) A += P.area();

  return A;
}


static GEN_POLYGON make_chains_simple(const GEN_POLYGON& P);

list<GEN_POLYGON> GEN_POLYGON::regional_decomposition() const
{
        list<GEN_POLYGON> result;

        if (trivial()) {
                result.append(*this);
                return result;
        }

        GRAPH<POLYGON, int> T;

        node root = Nesting_Tree( make_chains_simple(*this), T );

        int orient = orientation();

        node father;
        forall_nodes(father, T) {
                if (T[father].orientation() == orient && father != root) {
                        list<POLYGON> chains;

                        chains.append(T[father]);

                        edge e;
                        forall_out_edges(e, father) {
                                node child = target(e);
                                chains.append(T[child]);
                        }

                        GEN_POLYGON region(chains, GEN_POLYGON::NO_CHECK);
                        result.append(region);
                }
        }

        return result;
}

GEN_POLYGON POLYGON::buffer(RAT_TYPE distance, int corners = 3) const
{
  if (!((corners == 1) || (corners == 3)))
	  LEDA_EXCEPTION(1, "create_buffer1: number of corners not 1 or 3!");
  if (sign(distance) == 0) return *this;

  list<GEN_POLYGON> buffer_parts;

  list<POINT> vertices = this->vertices();

  // we make a smooth copy of the polygon vertices
 
  list<POINT> vcopy; // surviving vertices
  list<SEGMENT> scopy; // surviving and new edges

  if (vertices.size() > 1) 
  { 
 // cout << "Vertices: " << vertices.size() << endl;
    POINT p = vertices[ vertices.last() ];
    POINT q = vertices[ vertices.first() ];
    vcopy.append(p); // first vertex in smooth copy
    scopy.append(SEGMENT(p, q)); // first segment in smooth copy

    VECTOR u = (q - p);
    double norm = u.to_float().length();
    if (norm == 0) 
    { LEDA_EXCEPTION(1, "create_buffer2: norm of u is zero!");
      return POLYGON();
    }
    u /= RAT_TYPE(norm);

    list_item it;
    forall_items(it, vertices) 
    { POINT q = vertices[ it ];
      POINT r = vertices[ vertices.cyclic_succ(it) ];

      VECTOR v = (q - r);
      double norm = v.to_float().length();
      if (norm == 0) 
      { LEDA_EXCEPTION(1, "create_buffer3: norm of v is zero!");
        return POLYGON();
      }
      v /= RAT_TYPE(norm);

      RAT_TYPE sin_alpha = u.xcoord() * v.ycoord() - u.ycoord() * v.xcoord();

      // we check for minor deviations which are considered to be errors
      if ((sin_alpha < 0.00000001)&&(-sin_alpha < 0.00000001))
      {
        scopy.Pop(); // remove last segment
        scopy.append(SEGMENT(p,r)); // insert new segment without error
        u = (r - p);
        norm = u.to_float().length();
        if (norm == 0) 
        { LEDA_EXCEPTION(1, "create_buffer2: norm of u is zero!");
          return POLYGON();
        }
        u /= RAT_TYPE(norm);
      }
      else
      {
        vcopy.append(q); // vertex did survive
        scopy.append(SEGMENT(q,r)); // segment did survive
        p = q;
        u = - v;
      }
    }

    // we have to delete the helping elements; the are either twice in the list
    // or they did not survive the smoothing
    vcopy.del(vcopy.first());
    scopy.del(scopy.first());

    // we have to replace the last segment and replace it bei the one from back to front of vcopy
    // the two might be identical, but they don't have to
    scopy.del(scopy.last());
    scopy.append(SEGMENT(vcopy.back(), vcopy.front()));
// cout << "Copy of Vertices: " << vcopy.size() << endl;
// cout << "Copy of Segments: " << scopy.size() << endl;
   
}
// end of makeing a smooth copy

// the buffer consists of a buffer for every segment
  SEGMENT s;
  forall(s, scopy) 
  {
//   cout << "In forall loop" << endl;
    if (s.is_trivial()) continue;

    POINT p = s.source();
    POINT q = s.target();

    VECTOR u = (q - p).rotate90(-1); 
    // u is perpendicular to s and point to the right of s (if dist > 0)

    double norm = u.to_float().length();
    if (norm == 0) 
    { 
      LEDA_EXCEPTION(1, "create_buffer1: norm of u is zero!");
      return POLYGON();
     }

    u *= distance / norm; // now u has length distance

    list<POINT> vertices;
    vertices.append( q );
    vertices.append( p );
    vertices.append( p + u );
    vertices.append( q + u );

    GEN_POLYGON buffer_part(vertices,GEN_POLYGON::NO_CHECK, 
                                     GEN_POLYGON::DISREGARD_ORIENTATION);
    buffer_parts.append(buffer_part);
  }
// cout << "After loop, size of buffer_parts: " << buffer_parts.size() << endl;


  // now we have to do s.th. for every convex corner

  if (vcopy.size() > 1) 
  { RAT_TYPE abs_distance = abs(distance);

    POINT p = vcopy[ vcopy.last() ];
    POINT q = vcopy[ vcopy.first() ];

    VECTOR u = (q - p);
    double norm = u.to_float().length();
    if (norm == 0) 
    { LEDA_EXCEPTION(1, "create_buffer2: norm of u is zero!");
      return POLYGON();
     }

    u /= RAT_TYPE(norm);

    list_item it;
    forall_items(it, vcopy) 
    { POINT q = vcopy[ it ];
      POINT r = vcopy[ vcopy.cyclic_succ(it) ];

      VECTOR v = (q - r);
      double norm = v.to_float().length();
      if (norm == 0) 
      { LEDA_EXCEPTION(1, "create_buffer3: norm of v is zero!");
        return POLYGON();
       }
      v /= RAT_TYPE(norm);

      RAT_TYPE sin_alpha = u.xcoord() * v.ycoord() - u.ycoord() * v.xcoord();


      if (sign(sin_alpha) * sign(distance) < 0) 
      { // convex corner
        POINT p1 = q + distance * u.rotate90(-1);
        VECTOR w = u + v;
        double norm = w.to_float().length();
        if (norm == 0) 
        { LEDA_EXCEPTION(1, "create_buffer4: norm of w is zero!");
          return POLYGON();
         }

        w /= RAT_TYPE(norm);
		POINT p2, p4;
        POINT p3 = q + distance * v.rotate90(1);

		if (corners == 1)
		{
			LINE l1(p1, u);
			LINE l2(p3, v);
			l1.intersection(l2, p2);

		}
		else // corners == 3
		{
			p2 = q + abs_distance * w;
		}

        list<POINT> vertices;
        vertices.append(p1);
        vertices.append(p2);
        vertices.append(p3);
        vertices.append(q);
        GEN_POLYGON buffer_part(vertices, GEN_POLYGON::NO_CHECK, 
                                          GEN_POLYGON::DISREGARD_ORIENTATION);
        buffer_parts.append(buffer_part);
 // cout << "Size of buffer_part: " << buffer_part.vertices().size() << endl;
 // cout << endl;
      }

      u = -v;
    }
  }

  //GEN_POLYGON P(*this, GEN_POLYGON::NO_CHECK);
  GEN_POLYGON P(vcopy, GEN_POLYGON::NO_CHECK);

  if (distance > 0) 
  { buffer_parts.append(P);
    return GEN_POLYGON::unite_with_bool_ops(buffer_parts);
   }
  else 
   { GEN_POLYGON the_parts = GEN_POLYGON::unite_with_bool_ops(buffer_parts);
     return P.diff(the_parts);
    }

}

GEN_POLYGON GEN_POLYGON::buffer(RAT_TYPE distance, int p) const
{
  if (distance == 0) return *this;

  list<GEN_POLYGON> regions = regional_decomposition();
  list<GEN_POLYGON> buffered_regions;

// cout << "Number of regions: " << regions.size() << endl;

  GEN_POLYGON region;
  forall(region, regions) 
  { list<POLYGON> chains = region.polygons();

    POLYGON exterior = chains.pop_front();
    GEN_POLYGON buffered_region = exterior.buffer(distance, p);

    POLYGON interior;
    forall(interior, chains) 
    { GEN_POLYGON buffer = interior.buffer(distance, p);
      buffered_region = buffered_region.intersection(buffer);
     }

    buffered_regions.append(buffered_region);
  }

  return GEN_POLYGON::unite_with_bool_ops(buffered_regions);
}


gen_polygon GEN_POLYGON::to_gen_polygon() const
{ if ( trivial() ) return gen_polygon( (gen_polygon::KIND) kind() );
  list<polygon> PL;
  POLYGON P;
  forall(P,ptr()->pol_list) PL.append(P.to_polygon());
  return gen_polygon(PL,gen_polygon::NO_CHECK);
} 

gen_polygon GEN_POLYGON::to_float() const { return to_gen_polygon(); }


void GEN_POLYGON::normalize() const
{ POLYGON P;
  forall(P,ptr()->pol_list) P.normalize();
}


ostream& operator<<(ostream& out, const GEN_POLYGON& p) 
{   
  switch ( p.kind() )
  { case GEN_POLYGON_REP::EMPTY: out << 1 << endl; break;
    case GEN_POLYGON_REP::FULL:  out << 2 << endl; break;
    case GEN_POLYGON_REP::NON_TRIVIAL: 
    { 
      out << 3 << endl;
      out << "[" << endl;
      out << p.polygons().size() << endl;
      POLYGON P; 
      forall (P,p.polygons()) out << P << ' ';
      out << "]"; 
      break;
    }
  }
  out << endl;
  return out;
 } 

istream& operator>>(istream& in,  GEN_POLYGON& p) 
{ 
  int knd;
  int sz,i;
  char c;

  in >> knd;
  
  switch (knd)
  {
    case 1: p = GEN_POLYGON(GEN_POLYGON_REP::EMPTY); break;
    case 2: p = GEN_POLYGON(GEN_POLYGON_REP::FULL); break;
    case 3: // non trivial
    {
      in.get(c); // [
      in.get(c);
      list<POLYGON> PL;
      POLYGON pol;
      in >> sz;
      
      for (i=0;i<sz;i++) { 
        while (in.get(c) && is_space(c)) {} 
        if (!in) break; 
        in.putback(c);
        in >> pol; PL.append(pol); //cout << pol << "\n";
      }
      p = GEN_POLYGON(PL,GEN_POLYGON::input_check_type);
      in.get(c);
      in.get(c); // ]
    }
  }
  
  return in;
}

bool GEN_POLYGON::is_simple() const
{ GRAPH<POINT,SEGMENT> G;

  list<SEGMENT> seg_list;
  POLYGON P;
  forall(P,polygons()) 
  { list<SEGMENT> SL = P.segments();
    seg_list.conc(SL);
  }

  SEGMENT_INTERSECTION(seg_list,G,true);

 /* *this is simple iff for every node v in G.outdeg(v) = 2
    since every chain in *this is closed, we have G.outdeg(v) >= 2.
    Summing up all nodes we obtain m = Sum of Outdegs >= 2 * n,
    with equality iff G.outdeg(v) == 2 for all v. 
    Note that this implies that G.number_of_nodes() == seg_list.size()
    ("<" implies that one endpoint is visited more than once => outdeg >= 4,
     ">" implies that there is a proper intersection         => outdeg >= 4.)
  */
  return G.number_of_edges() == 2*G.number_of_nodes()
                  && G.number_of_nodes() == seg_list.length();
/*
  node v;
  forall_nodes(v,G) if ( G.outdeg(v) > 2 ) return false;

  return true;
*/
}


list<POINT>  GEN_POLYGON::vertices() const
{ list<POINT> result;
  if ( trivial() ) return result;
  POLYGON P;
  forall(P,polygons()) 
  { list<POINT> PL = P.vertices();
    result.conc(PL);
  }
  return result;
}


list<SEGMENT> GEN_POLYGON::segments() const
{ list<SEGMENT> result;
  if ( trivial() ) return result;
  POLYGON P;
  forall(P,polygons()) 
  { list<SEGMENT> SL = P.segments();
    result.conc(SL);
  }
  return result;
}



bool GEN_POLYGON::check_representation(const list<POLYGON>& pol_list)
{ GRAPH<POINT,SEGMENT> G;

  list<SEGMENT> seg_list;
  POLYGON P;
  forall(P,pol_list) 
  { list<SEGMENT> SL = P.segments();

  SEGMENT s;
  forall(s,SL) {
    if (s.sqr_length() == 0) 
      False("GEN_POLYGON::check_rep: zero-length segment");
  }

    seg_list.conc(SL);
  }

  SEGMENT_INTERSECTION(seg_list,G,true);

  if ( G.number_of_edges() == 2*G.number_of_nodes() 
                   && G.number_of_nodes() == seg_list.length() ) return true;
    // pol_list is simple (cf. GEN_POLYGON::is_simple)

  if ( G.number_of_edges() != 2*seg_list.length() )
   return False("GEN_POLYGON::check_rep: wrong number of edges");

  // no parallel edges
  node v; edge e;
  forall_edges(e,G)
   if ( target(e) == target(G.cyclic_adj_succ(e)) ) 
     return False("check_rep: parallel edges");

  
  map<SEGMENT,edge> segment_to_edge;

  forall_edges(e,G)
  { SEGMENT s = G[e];
    node v = G.source(e);
    segment_to_edge[s] = ( s.source() == G[v] ? e : G.reversal(e) );
  }

  edge_array<int> label(G);
  int count = 0;
  forall(P,pol_list)
  { list_item it;
    const list<SEGMENT>& seg_list = P.segments();
    forall_items(it,seg_list) 
    { edge e = segment_to_edge[seg_list[it]]; 
      e = G.reversal(e);
      edge f = segment_to_edge[seg_list[seg_list.cyclic_succ(it)]];
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
    if ( !S.empty() ) return False("check_rep: crossing");
  }
    
 
  return true;
}
  
bool GEN_POLYGON::check_representation() const
{ if ( trivial() ) return polygons().empty();
  return GEN_POLYGON::check_representation(polygons()); 
}



GEN_POLYGON::GEN_POLYGON(const POLYGON& P, CHECK_TYPE check, 
                         RESPECT_TYPE respect_orientation)
{ 
  if ( !respect_orientation && P.orientation() < 0)
    PTR = new GEN_POLYGON_REP(P.complement());
  else 
    PTR = new GEN_POLYGON_REP(P);

  if ( check == WEAKLY_SIMPLE && !P.is_weakly_simple() )
    LEDA_EXCEPTION(1,"GEN_POLYGON: POLYGON must be weakly simple");

  if ( check == SIMPLE && !P.is_simple() )
    LEDA_EXCEPTION(1,"GEN_POLYGON: POLYGON must be weakly simple");

}

GEN_POLYGON::GEN_POLYGON(const list<POINT>& pl, CHECK_TYPE check, 
            RESPECT_TYPE respect_orientation)
{ POLYGON P(pl,(POLYGON::CHECK_TYPE) check,
               (POLYGON::RESPECT_TYPE) respect_orientation);
  PTR = new GEN_POLYGON_REP(P);
}


GEN_POLYGON::GEN_POLYGON(const list<POLYGON>& PL, CHECK_TYPE check)
{ 
  GEN_POLYGON_REP* q = new GEN_POLYGON_REP(GEN_POLYGON_REP::EMPTY);
  PTR = q;
  if ( check == CHECK_REP && ! GEN_POLYGON::check_representation(PL) )
    LEDA_EXCEPTION(1,"GEN_POLYGON: representation is invalid.");
  delete q;
  PTR = new GEN_POLYGON_REP(PL);
}


// new (s.n)

GEN_POLYGON GEN_POLYGON::unite_with_bool_ops(const list<GEN_POLYGON>& PL, map<SEGMENT, GenPtr>* ptr_corresp)
{ 
  list<GEN_POLYGON> L = PL;

  while (L.size() > 1)
  { list<GEN_POLYGON> tmp;
    while (L.size() > 1)
    { GEN_POLYGON P = L.pop();
      GEN_POLYGON Q = L.pop();
      tmp.append(P.unite(Q, ptr_corresp));
     }
    L.conc(tmp);
  }

  return L.empty() ? GEN_POLYGON() : L.head();
}

GEN_POLYGON::GEN_POLYGON(const list<GEN_POLYGON>& PL)
{
  GEN_POLYGON result = unite_with_bool_ops(PL);

  if (result.trivial())
    PTR = new GEN_POLYGON_REP(result.kind());
  else
    PTR = new GEN_POLYGON_REP(result.polygons());
}


bool GEN_POLYGON::is_convex() const
{
  if (trivial()) return true;
  if ((ptr()->pol_list).size() != 1) return false;
  return (ptr()->pol_list).head().is_convex();
}

#include <LEDA/core/dictionary.h>

void GEN_POLYGON::canonical_rep()
{ if ( trivial() ) return;

  LEDA_EXCEPTION(1,"not implemented yet");

  list<POLYGON>& PL = ptr()->pol_list;
  list_item it = PL.last();
  while (it)
  { POLYGON P = PL[it];
    list<POINT> pl = P.vertices();
    dictionary<POINT,list_item> D;
    POINT p;
    list_item pl_it;
    list_item prev_occ = nil;
    forall_items(pl_it,pl)
    { POINT p = pl[pl_it];
      dic_item dic_it = D.lookup(p);
      if ( dic_it == nil ) D.insert(p,pl_it);
      else { prev_occ = D.inf(dic_it); break; }
    }
    if ( prev_occ == nil  ) { it = PL.pred(it); continue; }
    // prev_occ and pl_it have the same associated point

    list<POINT> pl1, pl2, pl3;
    pl.split(prev_occ,pl1,pl2,leda::behind);
    pl2.split(pl_it,pl2,pl3,leda::before);
    pl1.conc(pl3);
    
    POLYGON P1(pl1), P2(pl2);
    
    if ( P.orientation() == P1.orientation() )
    { PL[it] = P2;
      PL.insert(P1,it,leda::before);
    }
    else
    { PL[it] = P1;
      PL.insert(P2,it,leda::before);
    }
  }
}



int GEN_POLYGON::side_of(const POINT& p) const
{ if ( empty() ) return -1;
  if ( full() ) return +1;
  
  int orient_last_containing_poly = -polygons().head().orientation(); // == p0.orientation()
  POLYGON P;
  forall(P, polygons()) {
    // Let p_i be the last polygon so far whose bounded region contains p.
    // If p_i and p have the same orientation, then P cannot be nested in 
    // p_i, i.e. they are disjoint, so we do not have to check anything.
    if (orient_last_containing_poly != P.orientation()) {
      region_kind r = P.region_of(p);
      if ( r == ON_REGION ) return 0;
      if ( r == BOUNDED_REGION ) { orient_last_containing_poly *= -1; }
    }
  }
  return orient_last_containing_poly;
}


GEN_POLYGON GEN_POLYGON::translate(RAT_TYPE dx, RAT_TYPE dy) const
{ if ( trivial() ) return *this;
  list<POLYGON> pl;
  POLYGON p;
  forall(p,polygons()) pl.append(p.translate(dx,dy));
  return GEN_POLYGON(pl,NO_CHECK);
}



GEN_POLYGON GEN_POLYGON::translate(const VECTOR& v) const
{ if ( trivial() ) return *this;
  list<POLYGON> pl;
  POLYGON p;
  forall(p,polygons()) pl.append(p.translate(v));
  return GEN_POLYGON(pl,NO_CHECK);
}




GEN_POLYGON GEN_POLYGON::rotate90(const POINT& p, int i) const
{ if ( trivial() ) return *this;
  list<POLYGON> pl;
  POLYGON s;
  forall(s,polygons()) pl.append(s.rotate90(p,i));
  return GEN_POLYGON(pl,NO_CHECK);
}

GEN_POLYGON GEN_POLYGON::reflect(const POINT& p, const POINT& q) const
{ if ( trivial() ) return *this;
  list<POLYGON> pl;
  POLYGON s;
  forall(s,polygons()) pl.append(s.reflect(p,q));
  return GEN_POLYGON(pl,NO_CHECK);
}

GEN_POLYGON GEN_POLYGON::reflect(const POINT& p) const
{ if ( trivial() ) return *this;
  list<POLYGON> pl;
  POLYGON s;
  forall(s,polygons()) pl.append(s.reflect(p));
  return GEN_POLYGON(pl,NO_CHECK);
}

list<POINT> GEN_POLYGON::intersection(const SEGMENT& s) const
{ list<POINT> result;
  if ( trivial() ) return result;
  POLYGON P;
  forall(P,polygons()) 
  { list<POINT> L = P.intersection(s);
    result.conc(L);
  }
  return result;
}

list<POINT> GEN_POLYGON::intersection(const LINE& l) const
{ list<POINT> result;
  if ( trivial() ) return result;
  POLYGON P;
  forall(P,polygons()) 
  { list<POINT> L = P.intersection(l);
    result.conc(L);
  }
  return result;
}


RAT_TYPE GEN_POLYGON::sqr_dist(const POINT& p) const
{ 
  if ( trivial() ) return 0;
  
  POLYGON P = ptr()->pol_list.head();
  RAT_TYPE dmin = P.sqr_dist(p);
  forall(P,ptr()->pol_list) 
  { RAT_TYPE d = P.sqr_dist(p);
    if (d < dmin) dmin = d;
    if (dmin == 0) break;
  }
  return dmin;
}


GEN_POLYGON GEN_POLYGON::make_weakly_simple(bool with_neg_parts, bool strict) const
{
  list<POLYGON> weakly_simple_parts;
  POLYGON p;
  forall_polygons(p, *this) {
    list<POLYGON> weakly_simple_parts_of_p 
      = p.split_into_weakly_simple_parts(strict);
    weakly_simple_parts.conc(weakly_simple_parts_of_p);
  }

  list<GEN_POLYGON> pos_parts_as_gps, neg_parts_as_gps;
  POLYGON part;
  forall(part, weakly_simple_parts) {
    if (part.orientation() > 0)
                pos_parts_as_gps.append(GEN_POLYGON(part, NO_CHECK));
    if (part.orientation() < 0 && with_neg_parts)
                neg_parts_as_gps.append(GEN_POLYGON(part.complement(), NO_CHECK));
  }
  GEN_POLYGON pos_parts(pos_parts_as_gps), neg_parts(neg_parts_as_gps);
  if (pos_parts.empty() && !neg_parts.empty()) pos_parts = GEN_POLYGON(GEN_POLYGON_REP::FULL);
  return pos_parts.diff(neg_parts);
}

GEN_POLYGON GEN_POLYGON::make_weakly_simple(const POLYGON& Q, 
                                                                                        bool with_neg_parts, 
                                                                                        bool strict)
{
  GEN_POLYGON GP(Q, NO_CHECK);
  return GP.make_weakly_simple(with_neg_parts, strict);
}

GEN_POLYGON POLYGON::make_weakly_simple(bool with_neg_parts, bool strict) const
{
  return GEN_POLYGON::make_weakly_simple(*this, with_neg_parts, strict);
}

GEN_POLYGON GEN_POLYGON::complement() const
{ if ( trivial() )
  { return GEN_POLYGON(opposite(kind())); }
  list<POLYGON> R;
  POLYGON P;
  forall(P,polygons()) R.append(P.complement());
  return GEN_POLYGON(R);
}


#ifndef TESTPHASE
static int P0_face     = 1;
static int not_P0_face = 2;
static int P1_face     = 4;
static int not_P1_face = 8;
#endif



static string mes = "gen_polygon: this problem is too difficult for the floating point kernel. Please, switch to rat_gen_polygons. The following error occurred: ";


static void construct_initial_map(const GEN_POLYGON& P0, 
                                  const GEN_POLYGON& P1,
                                  GRAPH<POINT,SEGMENT>& G, 
                                  edge_array<int>& label)
{
  list<SEGMENT> seg_list;
  map<SEGMENT,int> seg_label(0);

  const list<SEGMENT>& L0 = P0.edges();
  const list<SEGMENT>& L1 = P1.edges();

  SEGMENT s;
  forall(s,L0)
  { seg_label[s] = 1;
    seg_list.append(s);
  }

  forall(s,L1)
  { seg_label[s] += 2;  // why "+=" ?
    seg_list.append(s);
  }
    

  SWEEP_SEGMENTS_RB(L0,L1,G,true,true);
  //SEGMENT_INTERSECTION(seg_list,G,true);



#if ( KERNEL == FLOAT_KERNEL )
  if ( Genus(G) != 0 ) LEDA_EXCEPTION(1,mes + "Genus(G) != 0.");
  node v; 
  forall_nodes(v,G) 
  { int deg = G.outdeg(v);
    if (deg % 2 != 0) LEDA_EXCEPTION(1,mes + "odd degree vertex.");
  }
#endif

  
  label.init(G,0);
  edge e0;
  forall_edges(e0,G)
  { if ( label[e0] != 0 ) continue;
    edge e = e0; edge e_rev = G.reversal(e);
    POINT   a = G[source(e)];
    POINT   b = G[target(e)];
    SEGMENT s = G[e];

    if ( (b - a) * (s.target() - s.source()) <= 0 ) 
      leda_swap(e,e_rev);
    // now s and e point into the same direction

    switch ( seg_label[s] )
    { case 1: label[e] = P0_face; 
             label[e_rev] = not_P0_face; 
             break;
      case 2: label[e] = P1_face; 
             label[e_rev] = not_P1_face; 
             break;
      case 3: { edge f = G.cyclic_adj_pred(e);
                if ( target(f) != target(e) ) 
                  f = G.cyclic_adj_succ(e);

                label[e] = P0_face; 
                label[e_rev] = not_P0_face; 
                label[f] = P1_face; 
                label[G.reversal(f)] = not_P1_face;
              }
    }
  }

}



static void extend_labeling(const GEN_POLYGON& P0, 
                            const GEN_POLYGON& P1,
                            const GRAPH<POINT,SEGMENT>& G, 
                            edge e,
                            edge_array<bool>& visited, 
                            edge_array<int>& label)
{ int d = 0; int length = 0;

  edge x = e; 
  do { visited[x] = true;   length++;

       //node v = source(x);
       //if (G.outdeg(v) == 2) v2 = v;

       d |= label[x];

       x = G.face_cycle_succ(x);

  } while (x != e);

  if ( d % 4 == 0 || d < 4 )
    { 
      edge f;
      for ( f = G.cyclic_adj_pred(e); f != e; f = G.cyclic_adj_pred(f) )
      { // bug detected by zhang yun (replace ">" by ">=")           
        //if ((d % 4 == 0 && label[f] % 4 != 0) || (d < 4  && label[f] > 4))
          if ((d % 4 == 0 && label[f] % 4 != 0) || (d < 4  && label[f] >= 4))
             break;
      } 
      if ( f == e )
      { node v = source(e); 
        if ( d % 4 == 0 ) 
            d |= ( P0.side_of(G[v]) == 1 ? P0_face : not_P0_face );
        if ( d < 4 ) 
            d |= ( P1.side_of(G[v]) == 1 ? P1_face : not_P1_face );
      }
      else
      { if ( d % 4 == 0 ) d |= ( label[f] % 4 );
        if ( d < 4 )      d |= ( ( label[f] / 4 ) * 4 );
      }
        }

  x = e;
#if ( KERNEL == FLOAT_KERNEL )
  if ( d % 4 == P0_face + not_P0_face || 
      (d/4)*4 == P1_face + not_P1_face )
    LEDA_EXCEPTION(1,mes + "contradicting edge labels.");
#endif
  
  do { label[x] = d; 
  
       x = G.face_cycle_succ(x);

  } while (x != e);

}


static void construct_labeled_map(const GEN_POLYGON& P0, 
                                  const GEN_POLYGON& P1,
                                  GRAPH<POINT,SEGMENT>& G, 
                                  edge_array<int>& label)
{ construct_initial_map(P0,P1,G,label);

  edge_array<bool> visited(G,false);

  edge e;
  forall_edges(e,G) 
  { if (visited[e]) continue;
    extend_labeling(P0,P1,G,e,visited,label);
  }
}



static bool simplify_graph(GRAPH<POINT,SEGMENT>& G, 
                           edge_array<bool>& relevant)
{ edge e; node v;
  forall_nodes(v,G)
  { list<edge> E = G.out_edges(v);
    forall(e,E)
    { edge f = G.cyclic_adj_succ(e);
      if ( target(e) != target(f) ) continue;
      edge e_rev = G.reversal(e);
      G.del_edge(e); G.del_edge(G.reversal(f));
      G.set_reversal(e_rev,f);
    }
  }

  bool non_empty_result = false;

  forall_nodes(v,G)
  { list<edge> E = G.out_edges(v);
    forall(e,E)
    { if ( relevant[e] || relevant[G.reversal(e)] )
       non_empty_result = true;
  
      if ( relevant[e] == relevant[G.reversal(e)] )
      { G.del_edge(G.reversal(e)); G.del_edge(e); }
    }
  }

  return non_empty_result;
}


template <class POINT, class SEGMENT>
class cmp_for_cycle_tracing : public leda_cmp_base<edge> {

const GRAPH<POINT,SEGMENT>& G;

public:

  cmp_for_cycle_tracing(const GRAPH<POINT,SEGMENT>& g): G(g) {}

  int operator()(const edge& e1, const edge& e2) const
  { node v = G.source(e1);
    node w = G.source(e2);
    if ( v != w ) return compare(G[v],G[w]);
    SEGMENT s1 = G[e1];
    SEGMENT s2 = G[e2];
    return cmp_slopes(s1,s2);
  }
};


static
GEN_POLYGON bool_op_extract_result(GRAPH<POINT, SEGMENT>& G, 
                                                                   const edge_array<bool>& relevant, 
                                                                   bool non_empty,
                                                                   map<SEGMENT, GenPtr>* ptr_corresp)
{ 
  if (G.number_of_edges() == 0 ) 
  { if ( non_empty ) 
      return GEN_POLYGON(GEN_POLYGON_REP::FULL);
    else 
      return GEN_POLYGON(GEN_POLYGON_REP::EMPTY);
  }

  edge_array<bool> visited(G,false);

  list<POLYGON> result;
  
  cmp_for_cycle_tracing<POINT,SEGMENT> cmp(G); 

  list<edge> E = G.all_edges();
  E.sort(cmp);

  if (ptr_corresp == 0)
  {
    edge e0;
    forall(e0,E)
    { edge e = e0;
      if ( visited[e] || visited[G.reversal(e)]) continue;
      int orient;
      if ( relevant[e] ) 
        { orient = +1; }
      else
        { e = G.reversal(e); orient = -1; }

      // collect polygon
      list<SEGMENT> pol;
      edge x = e; 
      do {
         visited[x] = true;
         node v = source(x);
         node w = target(x);

         POINT a = G[v]; 
         POINT b = G[w];  

         pol.append(SEGMENT(a,b));

         x = G.face_cycle_succ(x);  

      } while (x != e);

      POLYGON P(pol,orient);
      result.append(P);
    } 
  }
  else
  { // basically copied from above
    map<SEGMENT, GenPtr>& corresp = *ptr_corresp;
    edge e0;
    forall(e0,E)
    { edge e = e0;
      if ( visited[e] || visited[G.reversal(e)]) continue;
      int orient;
      if ( relevant[e] ) 
        { orient = +1; }
      else
        { e = G.reversal(e); orient = -1; }

      // collect polygon
      list<SEGMENT> pol;
      edge x = e; 
      do {
        visited[x] = true;
        node v = source(x);
        node w = target(x);

        POINT a = G[v]; 
        POINT b = G[w];  
        SEGMENT s = G[x];
         
        if (identical(s.source(),a) && identical(s.target(),b))
          pol.append(s);
        else {
          SEGMENT s_trimmed(a,b);
          pol.append(s_trimmed);
          corresp[s_trimmed] = corresp[s]; // maintain the correspondence
        }

        x = G.face_cycle_succ(x);  

      } while (x != e);

      POLYGON P(pol,orient);
      result.append(P);
    } 
  }

  return GEN_POLYGON(result,GEN_POLYGON::NO_CHECK);     
}


GEN_POLYGON GEN_POLYGON::intersection(const GEN_POLYGON& P1, map<SEGMENT, GenPtr>* ptr_corresp) const
{ // stage I

  if ( empty() || P1.empty() ) 
                        return GEN_POLYGON(GEN_POLYGON_REP::EMPTY);
  if ( full() ) return P1;
  if ( P1.full() ) return *this;

  // stages II and III

  
  GRAPH<POINT,SEGMENT> G;

  const GEN_POLYGON& P0 = *this;

  edge_array<int> label;

  construct_labeled_map(P0,P1,G,label);

  
  // label relevant edges, stage IV

  edge_array<bool> relevant(G,false);

  int d = P0_face + P1_face;
  edge e;
  forall_edges(e,G) if (label[e] == d) relevant[e] = true;

  // stages V and VI

  bool non_empty_result = simplify_graph(G,relevant);

  return bool_op_extract_result(G, relevant, non_empty_result, ptr_corresp);
}

GEN_POLYGON GEN_POLYGON::unite(const GEN_POLYGON& P1, map<SEGMENT, GenPtr>* ptr_corresp) const
{ 
  if ( trivial() || P1.trivial() )
  { if ( full() || P1.full() ) return GEN_POLYGON(GEN_POLYGON_REP::FULL);
    if ( empty() ) return P1;
    if ( P1.empty() ) return *this;
    LEDA_EXCEPTION(1,"unreachable");
  }

// check degeneracy

  if (P1.orientation() == 0)
    return *this;

  if (orientation() == 0)
    return P1;

// check bounding boxes first
// by s.n. (May 2000)

  const list<POLYGON>& p1_list = P1.polygons(); 

  if (p1_list.length() == 1 && p1_list.head().orientation() > 0)
  {
    POLYGON p1 = p1_list.head();
    POINT p1_left, p1_bot, p1_right, p1_top;
    p1.bounding_box(p1_left,p1_bot,p1_right,p1_top);

    const list<POLYGON>& p_list = polygons();
    int count = p_list.length();

    POLYGON pol = p_list.head();

    if (pol.orientation() > 0)
    { forall(pol,p_list)
      { if (pol.orientation() < 0)
        { count--;
          continue;
         }
        POINT pol_left, pol_bot, pol_right, pol_top;
        p1.bounding_box(pol_left,pol_bot,pol_right,pol_top);
        if (POINT::cmp_xy(pol_left ,p1_right) <= 0 &&
            POINT::cmp_xy(pol_right,p1_left)  >= 0 &&
            POINT::cmp_yx(pol_bot,  p1_top)   <= 0 &&
            POINT::cmp_yx(pol_top,  p1_bot)   >= 0) break;
        count--;
       }
     }

    if (count == 0) 
    { list<POLYGON> L = polygons();
      L.append(p1);
      return GEN_POLYGON(L,GEN_POLYGON::NO_CHECK);
     }
  }
  
  GRAPH<POINT,SEGMENT> G;

  const GEN_POLYGON& P0 = *this;

  edge_array<int> label;

  construct_labeled_map(P0,P1,G,label);


  edge_array<bool> relevant(G,false);
  int d = not_P0_face +  not_P1_face;
  edge e;
  forall_edges(e,G) if (label[e] != d) relevant[e] = true;

  
  // stages V and VI

  bool non_empty_result = simplify_graph(G,relevant);

  return bool_op_extract_result(G, relevant, non_empty_result, ptr_corresp);
}

GEN_POLYGON GEN_POLYGON::diff(const GEN_POLYGON& P1, map<SEGMENT, GenPtr>* ptr_corresp) const
{
   if ( trivial() || P1.trivial() )
  { if ( empty() || P1.full() ) 
                return GEN_POLYGON(GEN_POLYGON_REP::EMPTY);
    if ( full() ) return P1.complement();
    if ( P1.empty() ) return *this;
    LEDA_EXCEPTION(1,"unreachable");
  }

  
  GRAPH<POINT,SEGMENT> G;

  const GEN_POLYGON& P0 = *this;

  edge_array<int> label;

  construct_labeled_map(P0,P1,G,label);


  edge_array<bool> relevant(G,false);
  
  int d = P0_face +  not_P1_face;
  edge e;
  forall_edges(e,G) if (label[e] == d) relevant[e] = true;

  
  // stages V and VI

  bool non_empty_result = simplify_graph(G,relevant);

  return bool_op_extract_result(G, relevant, non_empty_result, ptr_corresp);
}


GEN_POLYGON GEN_POLYGON::sym_diff(const GEN_POLYGON& P1, map<SEGMENT, GenPtr>* ptr_corresp) const
{ 
  if ( trivial() || P1.trivial() )
  { if ( empty() ) return P1;
    if ( full() ) return P1.complement();
    if ( P1.empty() ) return *this;
    if ( P1.full() ) return complement();  
    LEDA_EXCEPTION(1,"unreachable");
    return *this;
  }


  GRAPH<POINT,SEGMENT> G;

  const GEN_POLYGON& P0 = *this;

  edge_array<int> label;

  construct_labeled_map(P0,P1,G,label);


  edge_array<bool> relevant(G,false);
  
  int d0 = P0_face +  not_P1_face;
  int d1 = P1_face +  not_P0_face;
  edge e;
  forall_edges(e,G) if (label[e] == d0 || label[e] == d1) relevant[e] = true;

  
  // stages V and VI

  bool non_empty_result = simplify_graph(G,relevant);

  return bool_op_extract_result(G, relevant, non_empty_result, ptr_corresp);
}

GEN_POLYGON 
GEN_POLYGON::unite_parallel(const list<GEN_POLYGON>& PL, int step_size, map<SEGMENT, GenPtr>* ptr_corresp)
{
        if (step_size <= 2) return unite_with_bool_ops(PL, ptr_corresp);

        list<GEN_POLYGON> L = PL;
        list<GEN_POLYGON> tmp;
        while (L.size() > 1) {
                while (L.size() > 1) {
                        list<GEN_POLYGON> to_do;
                        while (!L.empty() && to_do.size() < step_size) {
                                to_do.append( L.pop() );
                        }
                        tmp.append( unite_parallel_step(to_do, ptr_corresp) );
                }
                L.conc(tmp);
        }

        return L.empty() ? GEN_POLYGON() : L.head();
}


inline int abs_i(int i) { return i>=0 ? i : -i; }

GEN_POLYGON GEN_POLYGON::unite_parallel_step(const list<GEN_POLYGON>& PL, map<SEGMENT, GenPtr>* ptr_corresp)
{
        // similar to construct initial map
        list<SEGMENT> all_segs;
        map<SEGMENT, int> seg_id(0);

        int id = 0;
        GEN_POLYGON P;
        forall(P, PL) {
                if (P.full()) return P;

                ++id;
                // check for degeneracy
                if (P.orientation() == 0)
                  continue;
                list<SEGMENT> segs = P.segments();
                SEGMENT s;
                forall(s, segs) {
                        if (seg_id.defined(s)) {
                                s = SEGMENT(s.source(), s.target()); // construct seg with different ID_Number
                        }
                        seg_id[s] = id;
                        all_segs.append(s);
                }
        }

        GRAPH<POINT, SEGMENT> G;
        SEGMENT_INTERSECTION(all_segs, G, true);

#if ( KERNEL == FLOAT_KERNEL )
        if ( Genus(G) != 0 ) LEDA_EXCEPTION(1,mes + "Genus(G) != 0.");
        node v; 
        forall_nodes(v,G) { 
                int deg = G.outdeg(v);
                if (deg % 2 != 0) LEDA_EXCEPTION(1,mes + "odd degree vertex.");
        }
#endif

        // label the edges according to the polygon they originate from
        edge_array<int> label(G, 0);
        edge e0;
        forall_edges(e0, G) {
                if ( label[e0] != 0 ) continue;
                edge e = e0; edge e_rev = G.reversal(e);
                POINT   a = G[source(e)];
                POINT   b = G[target(e)];
                SEGMENT s = G[e];

                if ( (b - a) * (s.target() - s.source()) <= 0 ) 
                leda_swap(e,e_rev);
                // now s and e point into the same direction

                label[e] = seg_id[s]; label[e_rev] = -seg_id[s];
        }

         edge_array<bool> relevant(G, false);
                // an edge is relevant iff its face belongs to at least one polygon

        int_set known_ids(1, PL.size());
                // known_ids[i] == true iff we know whether current face belongs ith polygon

        edge_array<bool> visited(G, false);
        edge e;
        forall_edges(e, G) {
                if (visited[e]) continue;

                known_ids.clear();

                bool relevant_face = false;
                edge f = e;
                do {
                        if (label[f] > 0) {     relevant_face = true; break; }
                        known_ids.insert( abs_i(label[f]) );
                        f = G.face_cycle_succ(f);
                } while (f != e);

                if (! relevant_face) {
                        edge g;
                        for (g = G.cyclic_adj_pred(e); g != e; g = G.cyclic_adj_pred(g)) {
                                int id_g = abs_i(label[g]);
                                if (! known_ids.member(id_g)) {
                                        known_ids.insert(id_g);
                                        if (label[g] > 0) { relevant_face = true; break; }
                                }
                        }

                        if (! relevant_face) {
                                POINT p = G[source(e)];
                                int id = 0;
                                GEN_POLYGON P;
                                forall(P, PL) {
                                        ++id;
                                        if (! known_ids.member(id)) {
                                                // known_ids.insert(id); // unnecessary
                                                if (P.inside(p)) { relevant_face = true; break; }
                                        }
                                }
                        }
                }

                f = e;
                do {
                        visited[f] = true; relevant[f] = relevant_face; f = G.face_cycle_succ(f);
                } while (f != e);
        }


        // stages V and VI of boolean operation

        bool non_empty_result = simplify_graph(G,relevant);

        P = bool_op_extract_result(G, relevant, non_empty_result, ptr_corresp);

        return P;
}


static GEN_POLYGON make_chains_simple(const GEN_POLYGON& P)
{
        list<SEGMENT> all_segs = P.segments();
        GRAPH<POINT,SEGMENT> G;
        SEGMENT_INTERSECTION(all_segs, G, true);

        /* P is simple iff for every node v in G.outdeg(v) = 2
           since every chain in *this is closed, we have G.outdeg(v) >= 2.
           Summing up all nodes we obtain m = Sum of Outdegs >= 2 * n,
           with equality iff G.outdeg(v) == 2 for all v. 
           Note that this implies that G.number_of_nodes() == all_segs.size(),
                 if there are no empty segments.
           ("<" implies that one endpoint appears more than once => outdeg >= 4,
            ">" implies that there is a proper intersection      => outdeg >= 4.)
         */
        if ( G.number_of_edges() == 2*G.number_of_nodes() 
                   && G.number_of_nodes() == all_segs.length() ) {
                return P;
        }


        list<POLYGON> boundary_rep;

        cmp_for_cycle_tracing<POINT, SEGMENT> cmp(G); 

        list<edge> E = G.all_edges();
        E.sort(cmp);

        edge_array<bool>      considered(G, 0);
        node_array<int>       mark(G, 0);
        node_array<list_item> out_edge_item(G, nil);
        int cur_mark = 0;

        // compute face cycles

        edge e;
        forall(e, E) {
                if (considered[e] || considered[G.reversal(e)]) continue;

                POINT p = G[source(e)];
                POINT q = G[target(e)];
                SEGMENT s = G[e];
                if ( (q - p) * (s.target() - s.source()) < 0 )
                {
                        continue; // e and s are point in different directions
                }

                ++cur_mark;

                list<edge> cycle;
                edge c = e;
                do {
                        node u = source(c), v = target(c);
                        list_item in_it_v = out_edge_item[u] = cycle.append(c);
                        if (mark[v] == cur_mark) {
                                // cycle is not simple -> extract simple subcycle from v to v
                                list_item out_it_v = out_edge_item[v];
                                list<edge> subcycle;
                                cycle.extract(out_it_v, in_it_v, subcycle, true);

                                list<SEGMENT> chain;
                                edge f;
                                forall(f, subcycle) {
                                        mark[source(f)] = 0;
                                        chain.append( G[f] );
                                }
                                boundary_rep.append( POLYGON(chain, POLYGON::NO_CHECK) );
                        }

                        mark[v] = cur_mark;
                        considered[c] = true;
                        c = G.face_cycle_succ(c);
                } while (c != e);

                list<SEGMENT> chain;
                edge f;
                forall(f, cycle) {
                        chain.append( G[f] );
                }
                boundary_rep.append( POLYGON(chain, POLYGON::NO_CHECK) );
        }

        return GEN_POLYGON(boundary_rep, GEN_POLYGON::NO_CHECK);
}


#if (KERNEL == FLOAT_KERNEL)

gen_polygon::gen_polygon(const gen_polygon& P, int /*prec*/) : HANDLE_BASE(gen_polygon_rep)(P) {}

double gen_polygon::distance(const point& p) const 
{ return ::sqrt(sqr_dist(p)); }

gen_polygon gen_polygon::translate(double dx, double dy, double dw) const
{  return translate(dx/dw,dy/dw); }

gen_polygon gen_polygon::translate_by_angle(double alpha, double d) const
{ if (trivial() ) return *this;
  list<polygon> pl;
  polygon p;
  forall(p,ptr()->pol_list) pl.append(p.translate_by_angle(alpha,d));
  return gen_polygon(pl, NO_CHECK);
}

gen_polygon gen_polygon::rotate(const point& p, double alpha) const
{ if ( trivial() ) return *this;

  list<polygon> pl;
  polygon s;
  forall(s,ptr()->pol_list) pl.append(s.rotate(p,alpha));
  return gen_polygon(pl, NO_CHECK);
}
  
gen_polygon gen_polygon::rotate(double alpha) const
{ return rotate(point(0,0),alpha); }

#include <LEDA/geo/rat_gen_polygon.h>

rat_gen_polygon gen_polygon::to_rational(int prec) const
{
        if (trivial()) return rat_gen_polygon(rat_gen_polygon_rep::KIND(kind()));

        list<rat_polygon> rat_polys;

        polygon p;
        forall_polygons(p, *this) rat_polys.append(p.to_rational(prec));

        return rat_gen_polygon(rat_polys);
}

#endif

#if (KERNEL == RAT_KERNEL)

rat_gen_polygon::rat_gen_polygon(const gen_polygon& P, int prec)
{ if ( P.trivial() ) 
  { PTR = new rat_gen_polygon_rep((rat_gen_polygon::KIND) P.kind());
     return;
  }

 list<rat_polygon> PL;
 polygon p;
 forall(p,P.polygons()) PL.append(rat_polygon(p,prec));
 
 PTR = new rat_gen_polygon_rep(PL);
}

rat_gen_polygon rat_gen_polygon::translate(integer dx, integer dy, integer dw) const
{  return translate(rat_vector(dx,dy,dw)); }

#endif

#if (KERNEL == REAL_KERNEL)

GEN_POLYGON::GEN_POLYGON(const gen_polygon& P, int prec) 
{
        if (P.trivial()) {
                PTR = new GEN_POLYGON_REP(GEN_POLYGON_REP::KIND(P.kind()));
                return;
        }

        list<POLYGON> polys;
        polygon p;
        forall_polygons(p, P) polys.append(POLYGON(p, prec));

        PTR = new GEN_POLYGON_REP(polys);
}

NUMBER GEN_POLYGON::distance(const POINT& p) const 
{ return sqrt(sqr_dist(p)); }

GEN_POLYGON GEN_POLYGON::translate(NUMBER dx, NUMBER dy, NUMBER dw) const
{  return translate(dx/dw,dy/dw); }

#include <LEDA/geo/rat_gen_polygon.h>

rat_gen_polygon GEN_POLYGON::to_rational(int prec) const
{
        if (trivial()) return rat_gen_polygon(rat_gen_polygon_rep::KIND(kind()));

        list<rat_polygon> rat_polys;

        POLYGON p;
        forall_polygons(p, *this) rat_polys.append(p.to_rational(prec));

        return rat_gen_polygon(rat_polys);
}

#endif

LEDA_END_NAMESPACE 
