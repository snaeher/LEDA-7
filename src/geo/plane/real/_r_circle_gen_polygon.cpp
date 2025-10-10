/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _r_circle_gen_polygon.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_GEN_POLYGON_EXPORT_UNITE_PARALLEL

#include <LEDA/geo/r_circle_gen_polygon.h>
#include <LEDA/geo/rat_gen_polygon.h>

#include <LEDA/core/map.h>
#include <LEDA/core/dictionary.h> // for convert_bridge_representation
#include <LEDA/core/set.h> // for eliminate_cocircular_vertices
#include <LEDA/core/int_set.h> // for unite_parallel
#include <LEDA/graph/face_array.h> // bool-op stage (6)

#if defined(LEDA_DEBUG)
#include <LEDA/system/assert.h>
#define VERIFY(C) C
#else
#undef assert
#define assert(cond) ((void)0)
#define VERIFY(C) ((void)0)
#endif

//------------------------------------------------------------------------------
// r_circle_gen_polygon
//
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE 


r_circle_gen_polygon::CHECK_TYPE r_circle_gen_polygon::input_check_type = 
                                                 WEAKLY_SIMPLE;

r_circle_gen_polygon::r_circle_gen_polygon(KIND k)
{
  if (k == NON_TRIVIAL) 
  { PTR = new rep(rep::KIND(EMPTY));
    LEDA_EXCEPTION(1, "kind cannot be NON_TRIVIAL"); 
   }
  PTR = new rep(rep::KIND(k));
}


r_circle_gen_polygon::r_circle_gen_polygon(const r_circle_polygon& Q, 
                                           CHECK_TYPE check, 
                                           RESPECT_TYPE respect_orient)
{ if (check == SIMPLE && !Q.is_simple()) 
  {  PTR = new rep(rep::KIND(EMPTY));
     LEDA_EXCEPTION(1, "r_circle_gen_polygon: Q not simple");
   }

  if (check == WEAKLY_SIMPLE && !Q.is_weakly_simple()) 
  {  PTR = new rep(rep::KIND(EMPTY));
     LEDA_EXCEPTION(1, "r_circle_gen_polygon: Q not weakly simple");
   }
 
  if (respect_orient == RESPECT_ORIENTATION || Q.is_trivial() 
                                            || Q.orientation() > 0)
      PTR = new rep(Q);
  else
      PTR = new rep(Q.complement());
}


void r_circle_gen_polygon::init(const list<r_circle_polygon>& polys, 
                                CHECK_TYPE check, RESPECT_TYPE respect_orient)
{
  if (! check_representation(polys, check)) 
  { PTR = new rep(rep::KIND(EMPTY));
    LEDA_EXCEPTION(1, "r_circle_gen_polygon::init: polys invalid!");
   }

  if (respect_orient == RESPECT_ORIENTATION || polys.empty() 
                                            || polys.head().orientation()>=0)
     PTR = new rep(polys);
  else 
     { list<r_circle_polygon> reversed_polys;
       r_circle_polygon P;
       forall(P, polys) reversed_polys.append(P.complement());
       PTR = new rep(reversed_polys);
      }
}

r_circle_gen_polygon::r_circle_gen_polygon(const list<r_circle_gen_polygon> & gen_polys)
{
  PTR = new rep; 
  base::operator=(unite(gen_polys));
}

r_circle_gen_polygon::r_circle_gen_polygon(const rat_gen_polygon& Q, 
                                           CHECK_TYPE check, 
                                           RESPECT_TYPE respect_orient)
{ list<r_circle_polygon> chains;
  rat_polygon rp;
  forall_polygons(rp, Q) 
  { r_circle_polygon P(rp, r_circle_polygon::NO_CHECK, 
                           r_circle_polygon::RESPECT_ORIENTATION);
    chains.append(P);
  }
  init(chains, check, respect_orient);
}


r_circle_gen_polygon::r_circle_gen_polygon(const gen_polygon& Q, 
                                           CHECK_TYPE check, 
                                           RESPECT_TYPE respect_orient, 
                                           int prec)
{ list<r_circle_polygon> chains;
  polygon p;
  forall_polygons(p, Q) 
  { r_circle_polygon P(p, r_circle_polygon::NO_CHECK, 
                          r_circle_polygon::RESPECT_ORIENTATION, prec);
    chains.append(P);
  }
  init(chains, check, respect_orient);
}

void
r_circle_gen_polygon::normalize()
{
        r_circle_polygon P;
        forall(P, polygons()) P.normalize();
}

bool 
r_circle_gen_polygon::verify(bool report_errors) const
{
        bool ok = true;

        r_circle_polygon P;
        forall(P, polygons()) ok &= P.verify(report_errors);

        // TODO: check gen_polygon specific invariants
        return ok;
}

bool 
r_circle_gen_polygon::check_representation(const list<r_circle_polygon>& polys,
                                            CHECK_TYPE check)
{
  if (check == NO_CHECK) return true;

  int check_res = r_circle_polygon::check_chains_for_simplicity(polys);

  return check == check_res || 
         (check == WEAKLY_SIMPLE && check_res != NOT_WEAKLY_SIMPLE); 
}


bool 
r_circle_gen_polygon::is_convex() const
{
        if (is_trivial()) return true;
        if (polygons().size() > 1) return false;
        return polygons().head().is_convex();
}

int
r_circle_gen_polygon::size() const
{
        int sz = 0;
        r_circle_polygon P;
        forall(P, polygons()) sz += P.size();
        return sz;
}

list<r_circle_segment> 
r_circle_gen_polygon::segments() const
{
        list<r_circle_segment> all_segs;

        r_circle_polygon P;
        forall(P, polygons()) {
                list<r_circle_segment> segs = P.segments();
                all_segs.conc(segs);
        }

        return all_segs;
}

list<r_circle_point> 
r_circle_gen_polygon::vertices() const
{
        list<r_circle_point> all_verts;

        r_circle_polygon P;
        forall(P, polygons()) {
                list<r_circle_point> verts = P.vertices();
                all_verts.conc(verts);
        }

        return all_verts;
}


list<r_circle_point> 
r_circle_gen_polygon::intersection(const r_circle_segment& s) const
{
        list<r_circle_point> result;
        r_circle_polygon P;
        forall(P, polygons()) {
                list<r_circle_point> inters = P.intersection(s);
                result.conc(inters);
        }
        return result;
}

list<r_circle_point> 
r_circle_gen_polygon::intersection(const rat_line& l) const
{
        list<r_circle_point> result;
        r_circle_polygon P;
        forall(P, polygons()) {
                list<r_circle_point> inters = P.intersection(l);
                result.conc(inters);
        }
        return result;
}


r_circle_gen_polygon 
r_circle_gen_polygon::translate(rational dx, rational dy) const
{
        if (is_trivial()) return *this;

        list<r_circle_polygon> polys;
        r_circle_polygon P;
        forall(P, polygons()) polys.append(P.translate(dx, dy));
        return r_circle_gen_polygon(polys);
}

r_circle_gen_polygon 
r_circle_gen_polygon::rotate90(const rat_point& q, int i) const
{
        if (is_trivial()) return *this;

        list<r_circle_polygon> polys;
        r_circle_polygon P;
        forall(P, polygons()) polys.append(P.rotate90(q, i));
        return r_circle_gen_polygon(polys);
}

r_circle_gen_polygon 
r_circle_gen_polygon::reflect(const rat_point& p, const rat_point& q) const
{
        if (is_trivial()) return *this;

        list<r_circle_polygon> polys;
        r_circle_polygon P;
        forall(P, polygons()) polys.append(P.reflect(p, q));
        return r_circle_gen_polygon(polys);
}

r_circle_gen_polygon 
r_circle_gen_polygon::reflect(const rat_point& p) const
{
        if (is_trivial()) return *this;

        list<r_circle_polygon> polys;
        r_circle_polygon P;
        forall(P, polygons()) polys.append(P.reflect(p));
        return r_circle_gen_polygon(polys);
}


real 
r_circle_gen_polygon::sqr_dist(const real_point& p) const
{
        if (is_trivial()) return 0;

        list_item it = polygons().first();
        real min_sd = polygons()[it].sqr_dist(p);
        while ( (it = polygons().succ(it)) ) {
                real sd = polygons()[it].sqr_dist(p);
                if (sd < min_sd) {
                        min_sd = sd;
                        if (sign(min_sd)==0) break;
                }
        }
        return min_sd;
}


r_circle_gen_polygon 
r_circle_gen_polygon::complement() const
{
        if (! is_trivial()) {
                list<r_circle_polygon> compl_polys = polygons();
                list_item it;
                forall_items(it, compl_polys) {
                        compl_polys[it] = compl_polys[it].complement();
                }
                return r_circle_gen_polygon(compl_polys, NO_CHECK);
        }
        else if (is_empty()) return r_circle_gen_polygon(FULL);
        else return r_circle_gen_polygon(EMPTY);
}

r_circle_gen_polygon 
r_circle_gen_polygon::contour() const
{
        if (polygons().size() <= 1) return *this;

        //const list<r_circle_polygon>& boundary_rep = polygons();

        int opposite_orientation = -orientation();
        
        list<r_circle_polygon> contour_polygons;
        list<r_circle_polygon> opposite_orient_polygons;

        r_circle_polygon poly;
        forall(poly, polygons()) {
                // check if poly is nested in a contour polygon
                bool is_nested = false;

                if (poly.orientation() == opposite_orientation) {
                        // if poly has a different orientation as the whole gen. polygon, it must be nested
                        is_nested = true;
                        opposite_orient_polygons.append(poly);
                }
                else {
                        // if poly has the same orientation as the whole gen. polygon and it is nested,
                        // then it must be nested in some polygon with opposite orientation
                        // (unfortunately we may have to check all polygons with opposite orientation)
                        r_circle_polygon outer;
                        forall_rev(outer, opposite_orient_polygons) {
                                region_kind k = UNBOUNDED_REGION;
                                r_circle_point v;
                                forall_vertices(v, poly) {
                                        if ( (k = outer.region_of(v)) != ON_REGION) break;
                                }
                                is_nested = (k == BOUNDED_REGION);
                                if (is_nested) break;
                        }
                }

                if (!is_nested) {
                        contour_polygons.append(poly);
                }
        }

        if (contour_polygons.size() == polygons().size())
                return *this;
        else
                return r_circle_gen_polygon(contour_polygons, NO_CHECK);
}

/*
  Note: We cannot simply each polygon pol by pol.eliminate_cocircular_vertices().
        This might turn a weakly simply gen_polygon into a non-weakly simple 
                one. This is because two polygons P,Q might share a vertex v that is 
                colinear in P but not Q. In that case we have to keep v in P and in Q!
*/
r_circle_gen_polygon 
r_circle_gen_polygon::eliminate_cocircular_vertices() const
{
        if (is_trivial()) return *this;

        set<r_circle_point> vertices_to_keep;

        r_circle_polygon P;
        forall(P, polygons()) {
                P = P.eliminate_cocircular_vertices();
                r_circle_point v;
                forall_vertices(v, P)
                        vertices_to_keep.insert(v);
        }

        list<r_circle_polygon> new_polys;
        forall(P, polygons()) {
                list<r_circle_segment> segs = P.segments();
                list_item it;
                forall_items(it, segs) {
                        list_item it_succ = segs.cyclic_succ(it);
                        r_circle_segment s = segs[it], s_succ = segs[it_succ];
                        if (s.circle() == s_succ.circle() && !vertices_to_keep.member(s.target())) {
                                segs.del_item(it);
                                segs[it_succ] = r_circle_segment(s.source(), s_succ.target(), s.circle());
                        }
                }
                new_polys.append( r_circle_polygon(segs, r_circle_polygon::NO_CHECK) );
        }
        return r_circle_gen_polygon(new_polys, NO_CHECK);
}

r_circle_gen_polygon 
r_circle_gen_polygon::round(int prec) const
{
        if (is_trivial()) return *this;

        list<r_circle_polygon> new_polys;
        r_circle_polygon P;
        forall(P, polygons()) 
                new_polys.append(P.round(prec));
        return r_circle_gen_polygon(new_polys, NO_CHECK);
}

r_circle_polygon 
r_circle_gen_polygon::to_r_circle_polygon() const
{
        if (! is_r_circle_polygon()) {
                LEDA_EXCEPTION(1, "not an r_circle_polygon");
                return r_circle_polygon();
        }

        if (is_trivial()) return r_circle_polygon(r_circle_polygon::KIND(kind()));
        return polygons().head();
}

bool 
r_circle_gen_polygon::is_rat_gen_polygon() const
{
        r_circle_polygon P;
        forall(P, polygons()) 
                if (! P.is_rat_polygon()) return false;
        return true;
}

rat_gen_polygon 
r_circle_gen_polygon::to_rat_gen_polygon() const
{
        if (is_full()) return rat_gen_polygon(rat_gen_polygon_rep::FULL);
        if (is_empty()) return rat_gen_polygon(rat_gen_polygon_rep::EMPTY);

        list<rat_polygon> rat_polys;
        r_circle_polygon P;
        forall(P, polygons()) {
                rat_polys.append(P.to_rat_polygon());
        }
        return rat_gen_polygon(rat_polys);
}

rat_gen_polygon 
r_circle_gen_polygon::approximate_by_rat_gen_polygon(double dist) const
{
        if (is_full()) return rat_gen_polygon(rat_gen_polygon_rep::FULL);
        if (is_empty()) return rat_gen_polygon(rat_gen_polygon_rep::EMPTY);

        list<rat_polygon> rat_polys;
        r_circle_polygon P;
        forall(P, polygons()) {
                rat_polys.append(P.approximate_by_rat_polygon(dist));
        }
        return rat_gen_polygon(rat_polys);
}

gen_polygon 
r_circle_gen_polygon::to_float() const
{
        if (is_full()) return gen_polygon(gen_polygon_rep::FULL);
        if (is_empty()) return gen_polygon(gen_polygon_rep::EMPTY);

        list<polygon> polys;
        r_circle_polygon P;
        forall(P, polygons()) {
                polys.append(P.to_float());
        }
        return gen_polygon(polys);
}

bool 
r_circle_gen_polygon::is_rat_circle() const
{
        return is_r_circle_polygon() && to_r_circle_polygon().is_rat_circle();
}

rat_circle 
r_circle_gen_polygon::to_rat_circle() const
{
        return to_r_circle_polygon().to_rat_circle();
}

void 
r_circle_gen_polygon::bounding_box(real& xmin, real& ymin, real& xmax, real& ymax) const
{
        if (is_trivial()) {
                LEDA_EXCEPTION(1, "cannot compute bounding box of trivial polygon");
                return;
        }

        int orient = orientation();

        list_item it = polygons().first();
        polygons()[it].bounding_box(xmin, ymin, xmax, ymax);
        for ( ; (it = polygons().succ(it)); ) {
                const r_circle_polygon& p = polygons()[it];
                if (p.orientation() != orient) continue; // nested polygon

                real _xmin, _ymin, _xmax, _ymax;
                p.bounding_box(_xmin, _ymin, _xmax, _ymax);
                if (_xmin < xmin) xmin = _xmin;
                if (_ymin < ymin) ymin = _ymin;
                if (_xmax > xmax) xmax = _xmax;
                if (_ymax > ymax) ymax = _ymax;
        }
}

void 
r_circle_gen_polygon::bounding_box(double& xmin, double& ymin, 
                                                               double& xmax, double& ymax) const
{
        real xmin_r, ymin_r, xmax_r, ymax_r;

        bounding_box(xmin_r, ymin_r, xmax_r, ymax_r);

        xmin = xmin_r.get_double_lower_bound();
        ymin = ymin_r.get_double_lower_bound();
        xmax = xmax_r.get_double_upper_bound();
        ymax = ymax_r.get_double_upper_bound();
}


int 
r_circle_gen_polygon::side_of(const r_circle_point& pnt) const
{
        if (is_empty()) return -1;
        if (is_full())  return +1;

        int orient_last_containing_poly = -polygons().head().orientation(); // == p0.orientation()
        r_circle_polygon p;
        forall(p, polygons()) {
                // Let p_i be the last polygon so far whose bounded region contains pnt.
                // If p_i and p have the same orientation, then p cannot be nested in 
                // p_i, i.e. they are disjoint, so we do not have to check anything.
                if (orient_last_containing_poly != p.orientation()) {
                        region_kind r = p.region_of(pnt);
                        if (r == ON_REGION) return 0;
                        if (r == BOUNDED_REGION) { orient_last_containing_poly *= -1; }
                }
        }
        return orient_last_containing_poly;
}

region_kind 
r_circle_gen_polygon::region_of(const r_circle_point& p) const
{
        if (is_empty()) return UNBOUNDED_REGION;
        if (is_full())  return BOUNDED_REGION;

        int side = side_of(p);
        if ( side == 0 ) return ON_REGION;
        return side * polygons().head().orientation() > 0 ?     BOUNDED_REGION : UNBOUNDED_REGION; 
}

double 
r_circle_gen_polygon::approximate_area() const
{
        if (is_full()) LEDA_EXCEPTION(1,"area: cannot compute area of full polygon");

        double area = 0;
        r_circle_polygon P;
        forall(P,polygons()) area += P.approximate_area();
        return area;
}

bool 
r_circle_gen_polygon::operator==(const r_circle_gen_polygon& Q) const
{
        if (identical(*this, Q)) return true;
        if (kind() != Q.kind()) return false;
        if (is_trivial()) return true;
        if (orientation() != Q.orientation()) return false;

        return sym_diff(Q).is_empty();
}

/*
class reporter {
public:
        reporter(const string& msg) : Msg(msg)
        { cout << Msg << " >>>>" << endl; }
        ~reporter()
        { cout << Msg << " <<<<" << endl; }
private:
        string Msg;
};
*/

r_circle_gen_polygon::gp_type
r_circle_gen_polygon::bool_operation(BOOL_OP op, const gp_type& gp1, const gp_type& gp2)
{
        // stage (1)
        if (gp1.is_trivial() || gp2.is_trivial()) return bool_op_trivial_case(op, gp1, gp2);

        // stage (2) and (3)
        GRAPH<r_circle_point, r_circle_segment> G;
        edge_array<unsigned> label;
        bool_op_construct_map(gp1, gp2, G, label);

        // stage (4)
        edge_array<bool> relevant;
        bool_op_mark_relevant_edges(op, G, label, relevant);

        // stage (5)
        bool non_empty = bool_op_simplify_graph(G, relevant);

        // stage (6)
        return bool_op_extract_result(G, relevant, non_empty);
}

r_circle_gen_polygon::gp_type
r_circle_gen_polygon::bool_op_trivial_case(BOOL_OP op, const gp_type& gp1, const gp_type& gp2)
{
        // stage (1)
        assert(gp1.is_trivial() || gp2.is_trivial());

        switch (op) {
        case UNITE:             if (gp1.is_full() || gp2.is_full()) return gp1;
                                        return gp1.is_empty() ? gp2 : gp1;
        case INTERSECT: if (gp1.is_empty() || gp2.is_empty()) return gp1;
                                        return gp1.is_full() ? gp2 : gp1;
        case DIFF:              if (gp1.is_empty() || gp2.is_empty()) return gp1;
                                        return gp1.is_full() ? gp2.complement() : r_circle_gen_polygon(EMPTY);
        case SYMDIFF:   if (gp1.is_empty()) return gp2;
                                        if (gp2.is_empty()) return gp1;
                                        return gp1.is_full() ? gp2.complement() : gp1.complement();
        }

        return gp_type();
}

static const unsigned unlabelled   = 0;
static const unsigned gp1_face     = 1;
static const unsigned non_gp1_face = 2;
static const unsigned gp1_labels   = gp1_face | non_gp1_face;
static const unsigned gp2_face     = 4;
static const unsigned non_gp2_face = 8;
static const unsigned gp2_labels   = gp2_face | non_gp2_face;
static const unsigned gp1_and_gp2_face = gp1_face | gp2_face;
static const unsigned non_gp1_and_gp2_face = non_gp1_face | non_gp2_face;

void 
r_circle_gen_polygon::bool_op_construct_map(const gp_type& gp1, const gp_type& gp2, 
                                                                                        GRAPH<r_circle_point, r_circle_segment>& G, 
                                                                                        edge_array<unsigned>& label)
{
        // stage (2)
        list<r_circle_segment> all_segs;
        map<r_circle_segment, unsigned long> instance_of(0);

        r_circle_segment s;
        list<r_circle_segment> segs;
        
        segs = gp1.segments();
        forall(s, segs) instance_of[s] = 1;
        all_segs.conc(segs);
        
        segs = gp2.segments();
        forall(s, segs) instance_of[s] += 2;
        all_segs.conc(segs);

        edge_array<edge> corresp_edge;
        SWEEP_SEGMENTS_MC(all_segs, instance_of, G, corresp_edge);

        label.init(G, unlabelled);
        edge e;
        forall_edges(e, G) {
                if (label[e] != unlabelled || G[e].source() != G[source(e)]) continue;
                assert(corresp_edge[e] == nil);

                // e has the same direction as G[e] and it is the first edge that corresp. to G[e]

                do {
                        edge e_rev = G.reversal(e);

                        switch (instance_of[G[e]]) {
                        case 1: label[e] = gp1_face;         label[e_rev] = non_gp1_face; break;
                        case 2: label[e] = gp2_face;         label[e_rev] = non_gp2_face; break;
                        case 3: label[e] = gp1_and_gp2_face; label[e_rev] = non_gp1_and_gp2_face; break;
                                        // NOTE: G[e] appears twice in all_segs => two edges of G are labelled with it.
                        }

                        e = corresp_edge[e_rev];
                } while (e);
        }


        VERIFY( forall_edges(e, G) assert(label[e] != unlabelled) );

        // stage (3)
        edge_array<bool> visited(G, false);
        forall_edges(e, G) {
                if (visited[e]) continue;

                unsigned face_label = 0;
                edge f = e;
                do {
                        face_label |= label[f]; f = G.face_cycle_succ(f);
                } while (f != e);

                if ( (face_label & gp1_labels) == 0 || (face_label & gp2_labels) == 0 ) {
                        unsigned look_for_labels = (face_label & gp1_labels) == 0 ? gp1_labels : gp2_labels;
                        edge g;
                        for (g = G.cyclic_adj_pred(e); g != e; g = G.cyclic_adj_pred(g)) {
                                if ( (label[g] & look_for_labels) != 0 ) break;
                        }

                        if (g == e) {
                                // => no edge of the look_for_labels polygon is incident to src,
                                //    i.e. src cannot lie on the boundary of the look_for_labels polygon
                                node src = source(e);
                                if (look_for_labels == gp1_labels)
                                        face_label |= (gp1.inside(G[src])) ? gp1_face : non_gp1_face;
                                else
                                        face_label |= (gp2.inside(G[src])) ? gp2_face : non_gp2_face;
                        }
                        else {
                                        face_label |= label[g] & look_for_labels;
                        }
                }

                f = e;
                do {
                        visited[f] = true; label[f] = face_label; f = G.face_cycle_succ(f);
                } while (f != e);
        }

/*
        forall_edges(e, G) { // XXX
                cout << G[source(e)].to_point() << " -> " << G[target(e)].to_point() << " ";
                cout << label[e] << " ";
                cout << G[e].to_rat_segment().to_float() << endl;
        }
*/
}

void
r_circle_gen_polygon::bool_op_mark_relevant_edges(BOOL_OP op,
                             const GRAPH<r_circle_point, r_circle_segment>& G, 
                             const edge_array<unsigned>& label,
                             edge_array<bool>& relevant)
{
        // stage (4)
        relevant.init(G, false);

        edge e;
        switch (op) {
        case UNITE:             forall_edges(e, G) 
                                                if (label[e] != (non_gp1_face + non_gp2_face)) relevant[e] = true;
                                        break;

        case INTERSECT: forall_edges(e, G) 
                                                if (label[e] == (gp1_face + gp2_face)) relevant[e] = true;
                                        break;

        case DIFF:              forall_edges(e, G) 
                                                if (label[e] == (gp1_face + non_gp2_face)) relevant[e] = true;
                                        break;

        case SYMDIFF:   forall_edges(e, G) 
                                                if (   label[e] == (gp1_face + non_gp2_face) 
                                                        || label[e] == (gp2_face + non_gp1_face) ) relevant[e] = true;
                                        break;
        }
}

bool
r_circle_gen_polygon::bool_op_simplify_graph(GRAPH<r_circle_point, r_circle_segment>& G, 
                                                                                         const edge_array<bool>& relevant)
{
        // stage (5)
        edge e; node v;
        forall_nodes(v,G) { 
                list<edge> E = G.out_edges(v);
                forall(e,E) {
                        edge f = G.cyclic_adj_succ(e);
                        if ( target(e) != target(f) ) continue;
                        
                        // see if the associated curves overlap
                        // (given that both pass through source(e) and target(e))
                        r_circle_point common1 = G[source(e)], common2 = G[target(e)];
                        if (! G[e].overlaps(G[f], common1, common2)) continue;

                        edge e_rev = G.reversal(e), f_rev = G.reversal(f);
                        G.set_reversal(e_rev,f); // NOTE: maybe e == f
                        G.del_edge(e); G.del_edge(f_rev);
                }
        }

        bool non_empty_result = false;

        forall_nodes(v,G) {
                list<edge> E = G.out_edges(v);
                forall(e,E) {
                        if ( relevant[e] || relevant[G.reversal(e)] )
                        non_empty_result = true;

                        if ( relevant[e] == relevant[G.reversal(e)] )
                        { G.del_edge(G.reversal(e)); G.del_edge(e); }
                }
        }

        return non_empty_result;
}


class face_cmp_for_extraction : public leda_cmp_base<face> {
        const GRAPH<r_circle_point, r_circle_segment>& G;
    const face_array<r_circle_point>&              lex_smallest_point;
        const face_array<edge>&                        edge_with_lex_smallest_point;

public:
        face_cmp_for_extraction(const GRAPH<r_circle_point, r_circle_segment>& g,
                                                        const face_array<r_circle_point>& lsp,
                                                        const face_array<edge>& elsp)
        : G(g), lex_smallest_point(lsp), edge_with_lex_smallest_point(elsp)     {}

        int operator() (const face& f1, const face& f2) const
        {
                if (f1 == f2) return 0;

                int cmp = compare(lex_smallest_point[f1], lex_smallest_point[f2]);
                if (cmp) return cmp;

                edge e1 = edge_with_lex_smallest_point[f1], e2 = edge_with_lex_smallest_point[f2];
                assert( source(e1) == source(e2) );
                return compare_tangent_slopes(G[e1], G[e2], G[source(e1)]);
        }
};


r_circle_gen_polygon::gp_type
r_circle_gen_polygon::bool_op_extract_result(GRAPH<r_circle_point, r_circle_segment>& G, 
                                                                                         const edge_array<bool>& relevant, 
                                                                                         bool non_empty)
{
        // stage (6)
/*
        We differ from the LEDA book, because Lemma 75 (pg. 785) is not valid for curved edges.
*/

        if (G.number_of_edges() == 0) {
                return non_empty ? r_circle_gen_polygon(FULL) : r_circle_gen_polygon(EMPTY);
        }

        // first we trim the relevant edges
        edge e;
        forall_edges(e, G) {
                if (relevant[e]) G[e] = G[e].trim(G[source(e)], G[target(e)]);
        }

        G.compute_faces();
        face_array<r_circle_point> lex_smallest_point(G);
        face_array<edge>           edge_with_lex_smallest_point(G, nil);

        list<face> F = G.all_faces();
        list_item it;
        forall_items(it, F) {
                face f = F[it];
                edge e = G.first_face_edge(f);
                if (! relevant[e]) { F.del_item(it); continue; }

                edge edge_with_lex_smallest = e;
                r_circle_point lex_smallest = G[e].source();
                
                forall_face_edges(e, f) {
                        r_circle_segment s = G[e];
                        r_circle_point lex_smallest_on_s = s.lex_smallest_point();
                        if (lex_smallest_on_s == s.target()) continue; // ignore targets (for speed-up)
                        if (compare(lex_smallest_on_s, lex_smallest) < 0) {
                                lex_smallest = lex_smallest_on_s; edge_with_lex_smallest = e;
                        }
                }

                lex_smallest_point[f] = lex_smallest;
                edge_with_lex_smallest_point[f] = edge_with_lex_smallest;
        }

        face_cmp_for_extraction cmp(G, lex_smallest_point, edge_with_lex_smallest_point);
        F.sort(cmp);

        list<r_circle_polygon> result;

        face f;
        forall(f, F) {
                list<r_circle_segment> segs;
                edge e;
                forall_face_edges(e, f) {
                        assert( relevant[e] );
                        segs.append( G[e] );
                }
                
                r_circle_point lex_smallest = lex_smallest_point[f];
                e = edge_with_lex_smallest_point[f];
                edge e_pred = G.face_cycle_pred(e);
                int orient = r_circle_polygon::orientation_at_lex_smallest_bend(G[e_pred], lex_smallest, G[e]);
				// in rare cases (intel example) the computation comes up with the wrong result. therefore:
				// code fragment removed again by C.U. on July 14 due to problem reported by Metalix
				/*
				if (orient == -1)
				{
					r_circle_polygon rcp;
					bool inside = false;
					forall(rcp, result)
						if((rcp.orientation() == 1) && rcp.inside(lex_smallest))
						{
							inside = true;
							break;
						}
					if(!inside)
						orient = 1;
				}
				*/
                r_circle_polygon p(segs, orient);
                result.append(p);
        }

        return r_circle_gen_polygon(result, NO_CHECK, RESPECT_ORIENTATION);
}


r_circle_gen_polygon 
r_circle_gen_polygon::unite_with_bool_ops(const list<r_circle_gen_polygon>& PL)
{
        list<r_circle_gen_polygon> L = PL;

        while (L.size() > 1) {
                list<r_circle_gen_polygon> tmp;
                while (L.size() > 1) {
                        r_circle_gen_polygon P = L.pop();
                        r_circle_gen_polygon Q = L.pop();
                        tmp.append(P.unite(Q));
                }
                L.conc(tmp);
        }

        return L.empty() ? r_circle_gen_polygon() : L.head();
}

r_circle_gen_polygon 
r_circle_gen_polygon::unite_parallel(const list<r_circle_gen_polygon>& PL, int step_size)
{
		step_size = 2; // work_around for Metalix
        if (step_size <= 2) return unite_with_bool_ops(PL);

        list<r_circle_gen_polygon> L = PL;
        list<r_circle_gen_polygon> tmp;
        while (L.size() > 1) {
                while (L.size() > 1) {
                        list<r_circle_gen_polygon> to_do;
                        while (!L.empty() && to_do.size() < step_size) {
                                to_do.append( L.pop() );
                        }
                        tmp.append( unite_parallel_step(to_do) );
                }
                L.conc(tmp);
        }

        return L.empty() ? r_circle_gen_polygon() : L.head();
}

inline int abs_i(int i) { return i>=0 ? i : -i; }

r_circle_gen_polygon 
r_circle_gen_polygon::unite_parallel_step(const list<r_circle_gen_polygon>& L)
{
        // stage (1) and (2)
        list<r_circle_segment> all_segs;
        map<r_circle_segment, int> colour_map(0);

        int colour = 0;
        r_circle_gen_polygon gp;
        forall(gp, L) {         
                if (gp.is_full()) return gp;
                // NOTE: gp.is_empty() is not a special case!

                ++colour;
                list<r_circle_segment> segs = gp.segments();
                r_circle_segment s;
                forall(s, segs) {
                        if (colour_map.defined(s)) {
                                s = s.clone(); // construct seg with different ID_Number
                        }
                        colour_map[s] = colour;
                        all_segs.append(s);
                }
        }

        GRAPH<r_circle_point, r_circle_segment> G;
        edge_array<edge> corresp_edge(G);
        SWEEP_SEGMENTS_SC(all_segs, colour_map, G, corresp_edge);

        // label the edges according to the polygon they originate from
        edge_array<int> label(G, 0);

        edge e;
        forall_edges(e, G) {
                if (label[e] != 0 || G[e].source() != G[source(e)]) continue;
                assert(corresp_edge[e] == nil);

                // e has the same direction as G[e] and it is the first edge that corresp. to G[e]
                do {
                        edge e_rev = G.reversal(e);
                        int col = colour_map[G[e]];
                        label[e] = col; label[e_rev] = -col;

                        e = corresp_edge[e_rev];
                } while (e);
        }

        VERIFY( forall_edges(e, G) assert(label[e] != 0) );

        // stage (3) and (4)
        edge_array<bool> relevant(G, false); // for stage (4)
                // an edge is relevant iff its face belongs to at least one polygon

        int_set known_colours(1, L.size());
                // known_colours[i] == true iff we know whether current face belongs ith polygon

        edge_array<bool> visited(G, false);
        forall_edges(e, G) {
                if (visited[e]) continue;

                known_colours.clear();

                bool relevant_face = false;
                edge f = e;
                edge best_scan = e; // source(best_scan) has max. outdeg of all edges on face (avoid point locs)
                edge best_ploc = nil; // source(best_ploc) is a rat_point (if possible)
                do {
                        if (label[f] > 0) {     relevant_face = true; break; }
                        known_colours.insert( abs_i(label[f]) );
                        node src_of_f = source(f);
                        if (G.outdeg(src_of_f) > G.outdeg(source(best_scan))) best_scan = f;
                        if (G[src_of_f].is_rat_point()) best_ploc = f;
                        f = G.face_cycle_succ(f);
                } while (f != e);

                if (best_ploc == nil) best_ploc = best_scan;
                else if (G.outdeg(source(best_ploc)) > 2) best_scan = best_ploc;
                        // if source(best_ploc) lies on the boundary of two polygons we have to scan it!

                if (! relevant_face) {
                        edge g;
                        for (g = G.cyclic_adj_pred(best_scan); g != best_scan; g = G.cyclic_adj_pred(g)) {
                                int id_g = abs_i(label[g]);
                                if (! known_colours.member(id_g)) {
                                        known_colours.insert(id_g);
                                        if (label[g] > 0) { relevant_face = true; break; }
                                }
                        }

                        if (! relevant_face) {
                                r_circle_point p = G[source(best_ploc)];
                                        // for all unknown colours we have that p is NOT on the boundary!
                                int col = 0;
                                r_circle_gen_polygon gp;
                                forall(gp, L) {
                                        ++col;
                                        if (! known_colours.member(col)) {
                                                // known_colours.insert(col); // unnecessary
                                                if (gp.inside(p)) { relevant_face = true; break; }
                                        }
                                }
                        }
                }

                f = e;
                do {
                        visited[f] = true; relevant[f] = relevant_face; f = G.face_cycle_succ(f);
                } while (f != e);
        }

        // stage (5)
        bool non_empty = bool_op_simplify_graph(G, relevant);

        // stage (6)
        return bool_op_extract_result(G, relevant, non_empty);
}


rat_gen_polygon 
r_circle_gen_polygon::convert_curved_gp_to_straight_gp
        (const r_circle_gen_polygon& curved_gp, double dist, 
         list<r_circle_segment>& all_orig_segs, map<rat_segment, GenPtr>& orig_seg)
{
        if (curved_gp.trivial()) {
                return curved_gp.empty() ? rat_gen_polygon(rat_gen_polygon_rep::EMPTY) 
                                         : rat_gen_polygon(rat_gen_polygon_rep::FULL);
        }

        list<rat_polygon> straight_polys;

        r_circle_polygon poly;
        forall_polygons(poly, curved_gp) {
                list<rat_segment> straight_segs;

                r_circle_segment seg;
                forall_segments(seg, poly) {
                        list_item it =  all_orig_segs.append(seg);
                        r_circle_segment* ptr_seg = &all_orig_segs[it];

                        list<rat_segment> approx_segs = seg.approximate_by_rat_segments(dist);
                        rat_segment straight_seg;
                        forall(straight_seg, approx_segs) {
                                orig_seg[straight_seg] = ptr_seg;
                        }
                        straight_segs.conc(approx_segs);
                }

                straight_polys.append( rat_polygon(straight_segs, poly.orientation()) );
        }

        return rat_gen_polygon(straight_polys, rat_gen_polygon::NO_CHECK);
}


r_circle_gen_polygon 
r_circle_gen_polygon::convert_straight_gp_to_curved_gp
        (const rat_gen_polygon& straight_gp, map<rat_segment, GenPtr>& orig_seg)
{
        if (straight_gp.trivial()) {
                return straight_gp.empty() ? r_circle_gen_polygon(r_circle_gen_polygon::EMPTY) 
                                           : r_circle_gen_polygon(r_circle_gen_polygon::FULL);
        }

        list<r_circle_polygon> result;
        rat_polygon straight_poly;
        forall_polygons(straight_poly, straight_gp) {
                list<r_circle_segment> segs;

                //rat_segment straight_seg;
                //forall_segments(straight_seg, straight_poly) {
                //      r_circle_segment* ptr_seg = (r_circle_segment*) orig_seg[straight_seg];
                //      segs.append( ptr_seg->trim_approximate(straight_seg.source(), straight_seg.target()) );
                //}

                list<rat_segment> straight_segs = straight_poly.segments();
                if (straight_segs.empty()) continue; // should not happen, but just in case ...

                list_item start = straight_segs.first();
                GenPtr ptr_start = orig_seg[straight_segs[start]];
                list_item it = start;
                do {
                        it = straight_segs.cyclic_pred(it);
                } while (it != start && orig_seg[straight_segs[it]] == ptr_start);
                start = straight_segs.cyclic_succ(it);

                it = start;
                do {
                        rat_segment straight_seg = straight_segs[it];
                        rat_point src = straight_seg.source();
                        r_circle_segment* ptr_seg = (r_circle_segment*) orig_seg[straight_seg];

                        do {
                                it = straight_segs.cyclic_succ(it);
                        } while (it != start && orig_seg[straight_segs[it]] == ptr_seg);

                        rat_point tgt = straight_segs[it].source();

                        segs.append( ptr_seg->trim_approximate(src, tgt) );

                } while (it != start);

                result.append( r_circle_polygon(segs, straight_poly.orientation()) );
        }

        return r_circle_gen_polygon(result, NO_CHECK, RESPECT_ORIENTATION);
}


r_circle_gen_polygon 
r_circle_gen_polygon::unite_approximate(const r_circle_gen_polygon& Q, double dist) const
{
        // approximate each r_circle_gen_polygon by a rat_gen_polygon 
        // and maintain the correspondence between the respective segments
        list<r_circle_segment> all_orig_segs;
        map<rat_segment, GenPtr> orig_seg;

        rat_gen_polygon P_straight = convert_curved_gp_to_straight_gp(*this, dist, all_orig_segs, orig_seg);
        rat_gen_polygon Q_straight = convert_curved_gp_to_straight_gp(Q,     dist, all_orig_segs, orig_seg);

        // compute the union of the approximations
        rat_gen_polygon straight_result = P_straight.unite(Q_straight, &orig_seg);

        // convert the result back into a curved gen_polygon
        return convert_straight_gp_to_curved_gp(straight_result, orig_seg);
}

r_circle_gen_polygon 
r_circle_gen_polygon::intersection_approximate(const r_circle_gen_polygon& Q, double dist) const
{
        // approximate each r_circle_gen_polygon by a rat_gen_polygon 
        // and maintain the correspondence between the respective segments
        list<r_circle_segment> all_orig_segs;
        map<rat_segment, GenPtr> orig_seg;

        rat_gen_polygon P_straight = convert_curved_gp_to_straight_gp(*this, dist, all_orig_segs, orig_seg);
        rat_gen_polygon Q_straight = convert_curved_gp_to_straight_gp(Q,     dist, all_orig_segs, orig_seg);

        // compute the intersection of the approximations
        rat_gen_polygon straight_result = P_straight.intersection(Q_straight, &orig_seg);

        // convert the result back into a curved gen_polygon
        return convert_straight_gp_to_curved_gp(straight_result, orig_seg);
}

r_circle_gen_polygon 
r_circle_gen_polygon::diff_approximate(const r_circle_gen_polygon& Q, double dist) const
{
        // approximate each r_circle_gen_polygon by a rat_gen_polygon 
        // and maintain the correspondence between the respective segments
        list<r_circle_segment> all_orig_segs;
        map<rat_segment, GenPtr> orig_seg;

        rat_gen_polygon P_straight = convert_curved_gp_to_straight_gp(*this, dist, all_orig_segs, orig_seg);
        rat_gen_polygon Q_straight = convert_curved_gp_to_straight_gp(Q,     dist, all_orig_segs, orig_seg);

        // compute the diff of the approximations
        rat_gen_polygon straight_result = P_straight.diff(Q_straight, &orig_seg);

        // convert the result back into a curved gen_polygon
        return convert_straight_gp_to_curved_gp(straight_result, orig_seg);
}

r_circle_gen_polygon 
r_circle_gen_polygon::sym_diff_approximate(const r_circle_gen_polygon& Q, double dist) const
{
        // approximate each r_circle_gen_polygon by a rat_gen_polygon 
        // and maintain the correspondence between the respective segments
        list<r_circle_segment> all_orig_segs;
        map<rat_segment, GenPtr> orig_seg;

        rat_gen_polygon P_straight = convert_curved_gp_to_straight_gp(*this, dist, all_orig_segs, orig_seg);
        rat_gen_polygon Q_straight = convert_curved_gp_to_straight_gp(Q,     dist, all_orig_segs, orig_seg);

        // compute the sym_diff of the approximations
        rat_gen_polygon straight_result = P_straight.sym_diff(Q_straight, &orig_seg);

        // convert the result back into a curved gen_polygon
        return convert_straight_gp_to_curved_gp(straight_result, orig_seg);
}


r_circle_gen_polygon 
r_circle_gen_polygon::unite_approximate(const list<r_circle_gen_polygon>& PL, 
                                                                                double dist, int step_size)
{
        // approximate every r_circle_gen_polygon by a rat_gen_polygon 
        // and maintain the correspondence between the respective segments
        list<r_circle_segment> all_orig_segs;
        map<rat_segment, GenPtr> orig_seg;

        list<rat_gen_polygon> straight_gps;
        r_circle_gen_polygon gp;
        forall(gp, PL) {
                rat_gen_polygon straight_gp = convert_curved_gp_to_straight_gp(gp, dist, all_orig_segs, orig_seg);
                straight_gps.append(straight_gp);
        }

        // compute the union of the approximations
        rat_gen_polygon straight_result = rat_gen_polygon::unite_parallel(straight_gps, step_size, &orig_seg);

        // convert the result back into a curved gen_polygon
        return convert_straight_gp_to_curved_gp(straight_result, orig_seg);
}


r_circle_gen_polygon 
r_circle_gen_polygon::make_weakly_simple(const list<r_circle_segment>& chains,
                                         bool del_bridges)
{
		// get rid of full circles first
	
		r_circle_segment cs;
		list<r_circle_segment> newchains;

		bool contains_circle(false);
		forall(cs, chains)
		{
			if (!cs.is_full_circle())
				contains_circle = true;
		}

		if (contains_circle)
		{
			forall(cs, chains)
			{
				if (!cs.is_full_circle())
					newchains.append(cs);
				else // mach zwei Halbkreise und haenge die an
				{
				  r_circle_segment s1(cs.source(), cs.middle(), cs.circle());
				  r_circle_segment s2(cs.middle(), cs.source(), cs.circle());
				  newchains.append(s1);
				  newchains.append(s2);
				}
			}
		}
		
        // stage (2) (adapted from bool_op_construct_map)
        GRAPH<r_circle_point,r_circle_segment> G;
        edge_array<edge> corresp_edge;
		if(contains_circle)
			SWEEP_SEGMENTS(newchains, G, corresp_edge); // here chains
		else
			SWEEP_SEGMENTS(chains, G, corresp_edge); // here chains

        edge_array<unsigned> label(G, unlabelled);
        edge e;
        forall_edges(e, G) {
                if (label[e] != unlabelled || G[e].source() != G[source(e)]) continue;
                assert(corresp_edge[e] == nil);

                // e has the same direction as G[e] and it is the first edge that corresp. to G[e]
                do {
                        edge e_rev = G.reversal(e);
                        label[e] = gp1_face; label[e_rev] = non_gp1_face;
                        e = corresp_edge[e_rev];
                } while (e);
        }

        // stage (2*) new: delete bridges = pairs of overlapping edges
        if (del_bridges) {
                node v;
                forall_nodes(v,G) {
                        edge e = G.first_adj_edge(v);
                        while (e) {
                                edge f = G.adj_succ(e);
                                if (!f) break;

                                if ( target(e) == target(f) && G[e].overlaps(G[f], G[source(e)], G[target(e)]) ) 
                                {
                                        edge g = G.adj_succ(f);
                                        edge e_rev = G.reversal(e), f_rev = G.reversal(f);

                                        bool g_overlaps = ( g != nil && target(e) == target(g) && G[e].overlaps(G[g], G[source(e)], G[target(e)]) );
                                        if (g_overlaps) {
                                                edge g_rev = G.reversal(g);
                                                label[g]     |= label[e]     | label[f];
                                                label[g_rev] |= label[e_rev] | label[f_rev];
                                        }

                                        G.del_edge(e); G.del_edge(e_rev);
                                        G.del_edge(f); G.del_edge(f_rev);
                                        e = g;
                                }
                                else e = f;
                        }
                }
        }

        // stage (3) (adapted from bool_op_construct_map) and stage (4)
        edge_array<bool> relevant(G, false); // for stage (4)
        edge_array<bool> visited(G, false);
        forall_edges(e, G) {
                if (visited[e]) continue;

                //unsigned face_label = 0;
                unsigned num_gp1_edges = 0, num_non_gp1_edges = 0;
                edge f = e;
                do {
                        // NOTE: (gp1_edge | non_gp1_edge) is counted as gp1_edge!
                        if (label[f] == non_gp1_face) ++num_non_gp1_edges;
                        else                          ++num_gp1_edges;
                        f = G.face_cycle_succ(f);
                } while (f != e);

                // take the majority vote 
                bool relevant_face = (num_gp1_edges >= num_non_gp1_edges);

                f = e;
                do {
                        visited[f] = true; relevant[f] = relevant_face; f = G.face_cycle_succ(f);
                } while (f != e);
        }

        // stage (5)
        bool non_empty = bool_op_simplify_graph(G, relevant);

        // stage (6)
        return bool_op_extract_result(G, relevant, non_empty);
}


// from _r_circle_polygon.cpp
list<r_circle_polygon> r_circle_polygon::split_into_weakly_simple_parts() const
{
        list<r_circle_polygon> result;

        r_circle_gen_polygon gp = r_circle_gen_polygon::make_weakly_simple(*this);

        r_circle_polygon p;
        forall(p, gp.polygons()) {
                if (p.orientation() > 0)
                        result.push(p);
                else
                        result.append(p);
        }

        return result;
}

// from _r_circle_polygon.cpp
r_circle_gen_polygon r_circle_polygon::make_weakly_simple() const
{
        return r_circle_gen_polygon::make_weakly_simple(*this);
}


r_circle_gen_polygon 
r_circle_gen_polygon::convert_bridge_representation(const r_circle_polygon& p, int input_tolerance)
{
        if (p.is_trivial()) return r_circle_gen_polygon( r_circle_gen_polygon::KIND(p.kind()) );
        if (p.is_rat_circle()) return r_circle_gen_polygon( p, NO_CHECK, DISREGARD_ORIENTATION );

        list<r_circle_segment> segs = p.segments();

        // find the segment containing the lex smallest point of p
        // (in case of a tie choose the segment with smallest slope)
        list_item lex_smallest_it = segs.first();
        r_circle_point lex_smallest = segs[lex_smallest_it].source();

        list_item it;
        forall_items(it, segs) {
                r_circle_segment cur = segs[it];
                r_circle_point lex_smallest_cur = cur.lex_smallest_point();
                if (lex_smallest_cur == cur.target()) continue; // ignore targets (for speed-up)
                int cmp = compare(lex_smallest_cur, lex_smallest);
                if (   (cmp < 0)
                        || (cmp == 0
                            // we have two segments that contain lex_smallest, so we compare slopes
                                // we assume that two segments share no point except for common endpoints possibly
                            && (compare_tangent_slopes(cur, segs[lex_smallest_it], lex_smallest) < 0)) )
                {
                        lex_smallest = lex_smallest_cur; lex_smallest_it = it;
                }
        }

        // change segs s.th. it starts with segment containing lex_smallest_point
        list<r_circle_segment> segs2;
        segs.extract(nil, lex_smallest_it, segs2, false);
        segs.conc(segs2);
        lex_smallest_it = segs.first();

        // we extract one pos. (outer) polygon and zero or more neg. holes
        r_circle_polygon pos_poly; list<r_circle_polygon> neg_polys;

        dictionary<r_circle_point, list_item> previous_occ;
        forall_items(it, segs) {
                r_circle_segment& s = segs[it];
                previous_occ.insert(s.source(), it);

                dic_item dit = previous_occ.lookup(s.target());
                if (dit != nil) {
                        list_item prev_it = previous_occ.inf(dit);
                        list<r_circle_segment> chain;
                        segs.extract(prev_it, it, chain, true);
                        r_circle_segment ss;
                        forall(ss, chain) previous_occ.del(ss.source());
                        r_circle_segment first = chain.head(), last = chain.tail();
                        if (!first.is_straight_segment() || !last.is_straight_segment() || !first.overlaps(last))
                        {
                                r_circle_polygon poly(chain, r_circle_polygon::NO_CHECK, r_circle_polygon::DISREGARD_ORIENTATION);
                                if (prev_it == lex_smallest_it) {
                                        pos_poly = poly;
                                }
                                else {
                                        neg_polys.append(poly);
                                }
                        }
                }
        }

        switch (input_tolerance) {
        case 0: // every poly weakly simple, neg. polys are pairwise disjoint and completely contained in pos. poly
                {
                        list<r_circle_polygon> boundary_rep;
                        boundary_rep.append(pos_poly);

                        r_circle_polygon neg_poly;
                        forall(neg_poly, neg_polys) {
                                boundary_rep.append( neg_poly.complement() );
                        }
                        return r_circle_gen_polygon(boundary_rep, NO_CHECK, RESPECT_ORIENTATION);
                }
        case 1: // every poly weakly simple, neg. polys are pairwise disjoint, but may intersect the pos. poly
                {
                        r_circle_gen_polygon pos = r_circle_gen_polygon(pos_poly, NO_CHECK, RESPECT_ORIENTATION);
                        r_circle_gen_polygon neg = r_circle_gen_polygon(neg_polys, NO_CHECK, RESPECT_ORIENTATION);
                        return pos.diff(neg);
                }
        case 2: // every poly weakly simple, neg. polys may intersect the pos. poly and themselves
                {
                        r_circle_gen_polygon pos = r_circle_gen_polygon(pos_poly, NO_CHECK, RESPECT_ORIENTATION);

                        list<r_circle_gen_polygon> neg_gen_polys;
                        r_circle_polygon neg_poly;
                        forall(neg_poly, neg_polys) {
                                neg_gen_polys.append( r_circle_gen_polygon(neg_poly, NO_CHECK, RESPECT_ORIENTATION) );
                        }
                        r_circle_gen_polygon neg = r_circle_gen_polygon::unite(neg_gen_polys);

                        return pos.diff(neg);
                }
        default:
                {
                        list<r_circle_polygon> pos_polys; pos_polys.append(pos_poly);
                        return assemble(pos_polys, neg_polys);
                }
        }
}

r_circle_gen_polygon
r_circle_gen_polygon::assemble(const list<r_circle_polygon>& pos_parts, const list<r_circle_polygon>& neg_parts)
// precond: (a) We assume that both the pos. and the neg. polys are positively oriented.
//          (b) Every neg. polygon N is contained in a pos. polygon P or intersects it.
//              If N intersects P, then the intersection contains an open region, i.e. 
//              it is not allowed that N just "touches" P.
//              => N cannot contain any pos. polygon.
//          (c) A pos. polygon P is not contained in another pos. polygon Q.
{
        list<r_circle_segment> all_segs;
        map<r_circle_segment, bool> belongs_to_pos_poly(false);

        r_circle_polygon poly;
        forall(poly, pos_parts) {
                r_circle_segment s;
                forall_segments(s, poly) {
                        all_segs.append(s);
                        belongs_to_pos_poly[s] = true;
                }
        }

        forall(poly, neg_parts) {
                list<r_circle_segment> segs = poly.segments();
                all_segs.conc(segs);
        }

        // stage (2) (adapted from bool_op_construct_map)
        GRAPH<r_circle_point,r_circle_segment> G;
        edge_array<edge> corresp_edge;
        SWEEP_SEGMENTS(all_segs, G, corresp_edge);

        // try a short-cut: check if the input is simple
        if (G.number_of_edges() == 2*all_segs.size() && G.number_of_nodes() == all_segs.size()) {
                // NOTE: If we assume G.outdeg(v)>=2 for all v, this implies G.outdeg(v)==2 for all v
                bool is_simple = true;
                node v;
                forall_nodes(v, G) {
                        assert( G.outdeg(v) == 2 );
                        if (target(G.first_out_edge(v)) == target(G.last_out_edge(v))) {
                                // maybe the edges overlap => maybe not simple
                                is_simple = false; break;
                        }
                }
                if (is_simple) {
                        list<r_circle_polygon> boundary_rep = pos_parts;
                        r_circle_polygon neg_poly;
                        forall(neg_poly, neg_parts) {
                                boundary_rep.append( neg_poly.complement() );
                        }
                        return r_circle_gen_polygon(boundary_rep, NO_CHECK, RESPECT_ORIENTATION);
                }
        }

        edge_array<unsigned> label(G, unlabelled);
        edge e;
        forall_edges(e, G) {
                if (label[e] != unlabelled || G[e].source() != G[source(e)]) continue;
                assert(corresp_edge[e] == nil);

                // e has the same direction as G[e] and it is the first edge that corresp. to G[e]
                do {
                        edge e_rev = G.reversal(e);
                        if (belongs_to_pos_poly.defined(G[e])) {
                                // segment from pos. poly
                                label[e] = gp1_face; label[e_rev] = non_gp1_face;
                        }
                        else {
                                // segment from neg. poly
                                label[e] = gp2_face; label[e_rev] = non_gp2_face;
                        }
                        e = corresp_edge[e_rev];
                } while (e);
        }

        // stage (3) (adapted from bool_op_construct_map)
        edge_array<bool> visited(G, false);
        forall_edges(e, G) {
                if (visited[e]) continue;

                unsigned face_label = 0;
                edge f = e;
                do {
                        face_label |= label[f]; f = G.face_cycle_succ(f);
                } while (f != e);

                // If the face consists only of gp1-edges it must be a non-gp2-face 
                // because of precond (b).
                // Note that some gp2-chains might touch the face but that does not 
                // matter.
                if ( (face_label & gp2_labels) == 0 ) face_label |= non_gp2_face;

                // If the face consists only of gp2-edges it must be a gp1-face 
                // because of precond (b).
                // Note that the face cannot properly intersect a gp1-chain, hence it
                // must be contained in pos. polygon.
                if ( (face_label & gp1_labels) == 0 ) face_label |= gp1_face;

                f = e;
                do {
                        visited[f] = true; label[f] = face_label; f = G.face_cycle_succ(f);
                } while (f != e);
        }

        // stage (4)
        edge_array<bool> relevant;
        bool_op_mark_relevant_edges(DIFF, G, label, relevant);

        // stage (5)
        bool non_empty = bool_op_simplify_graph(G, relevant);

        // stage (6)
        return bool_op_extract_result(G, relevant, non_empty);
}

void 
r_circle_gen_polygon::write(ostream& out, const r_circle_gen_polygon& p)
{
        out << p.kind();
        if (! p.is_trivial()) {
                out << " [" << endl;
                out << p.polygons().size() << endl;
                r_circle_polygon pol; 
                forall(pol, p.polygons()) out << pol << ' ';
                out << "]";
        }
        out << endl; 
}

void 
r_circle_gen_polygon::read(istream& in, r_circle_gen_polygon& p, 
                           CHECK_TYPE chk, RESPECT_TYPE resp_or)
{
        int _k;
        in >> _k;

        r_circle_gen_polygon::KIND k = r_circle_gen_polygon::KIND(_k);

        if (k != r_circle_gen_polygon::NON_TRIVIAL) {
                p = r_circle_gen_polygon(k);
        }
        else {
                char c;
                while (in.get(c) && is_space(c));
                if (c != '[') { in.putback(c); return; }
                while (in.get(c) && is_space(c));
                in.putback(c);

                int sz; in >> sz; 

                if (!in) return;

                list<r_circle_polygon> PL;
                for(int i = 0; i < sz; ++i) { 
                        while (in.get(c) && is_space(c));
                        if (!in) break;
                        in.putback(c);
                        r_circle_polygon pol;
                        r_circle_polygon::read(in, pol, r_circle_polygon::NO_CHECK, 
                                                   r_circle_polygon::RESPECT_ORIENTATION); 
                        PL.append(pol); 
                }
                p = r_circle_gen_polygon(PL, chk, resp_or);
                while (in.get(c) && is_space(c));
                // skip ]
        }
}

ostream& operator<<(ostream& out, const r_circle_gen_polygon& p) 
{ r_circle_gen_polygon::write(out, p); 
  return out;
} 

istream& operator>>(istream& in, r_circle_gen_polygon& p) 
{ r_circle_gen_polygon::read(in, p, r_circle_gen_polygon::NO_CHECK); 
  r_circle_gen_polygon::CHECK_TYPE ct = r_circle_gen_polygon::input_check_type;
  if (!r_circle_gen_polygon::check_representation(p.polygons(),ct))
  { string msg = "unknown error";
    if (ct == r_circle_gen_polygon::WEAKLY_SIMPLE) msg = "not weakly simple";
    if (ct == r_circle_gen_polygon::SIMPLE) msg = "not simple"; 
    msg += "\n\t\t(use input_check_type = NO_CHECK to disable checking)";
    LEDA_EXCEPTION(1, string("r_circle_gen_polygon::operator>>: ") + msg);
   }
  return in;
}

//------------------------------------------------------------------------------
// buffering
//------------------------------------------------------------------------------


r_circle_gen_polygon r_circle_gen_polygon::buffer(double delta) const
{
  const r_circle_gen_polygon& P = *this;

  list<r_circle_polygon> L = P.polygons();

  if (L.empty()) return P;

  list<r_circle_gen_polygon> boundaries;
  list<r_circle_gen_polygon> holes;

  r_circle_polygon poly;
  forall(poly,L)
  {
    double area = poly.approximate_area();

    if (fabs(area) < delta) continue;


    if (area > 0) 
        boundaries.append(poly.buffer(delta));
    else
      { r_circle_gen_polygon h = poly.complement().buffer(-delta);
        if (!h.empty()) holes.append(h);
       }
  }


  r_circle_gen_polygon B = r_circle_gen_polygon::unite(boundaries);
  r_circle_gen_polygon H = r_circle_gen_polygon::unite(holes);

  B = B.diff(H);

#if defined(__DMC__)
  double eps = sqrt(fabs(B.approximate_area()))/1e6;
#else
  double eps = std::sqrt(fabs(B.approximate_area()))/1e6;
#endif

  r_circle_polygon rcp;
  list<r_circle_polygon> result;
  forall(rcp,B.polygons()) 
  { double A = rcp.approximate_area();
    if (fabs(A) < eps) continue;
    result.append(rcp);
   }

  return r_circle_gen_polygon(result);
}



LEDA_END_NAMESPACE 
