/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  curve_sweep_templ.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// S. Thiel (2004)

// NOTE: A bug in __HP_aCC requires that all calls to the traits class are 
//       prefixed by "TRAITS::".

#include <LEDA/geo/curve_sweep.h>
#include <LEDA/core/dictionary.h>
#include <LEDA/core/set.h>


#if defined(LEDA_DEBUG)
#include <LEDA/system/assert.h>
#else
#undef assert
#define assert(cond) ((void)0)
#endif


#if defined(LEDA_CURVE_SWEEP_VISUAL_DEBUG) || defined(LEDA_CURVE_SWEEP_COMPACTIFICATION_VISUAL_DEBUG)
#include <LEDA/graphics/real_window.h> // window trace
#include <LEDA/graphics/panel.h> // window trace
#endif

LEDA_BEGIN_NAMESPACE

/*
        The sweepline algorithm follows the implementation in the LEDA book, the main
        differences are the following:
        1) We allow touching curves.
           (Note that they are FUNDAMENTALLY different from overlapping curves, and hence
           must be handled differently.)
    2) We allow curves that intersect properly more than once.
        3) We have a faster embedding algorithm. (Even for rat_segment it is faster.)

        Ideas of the embedding algorithm:
        Consider a curve c that starts in s, has an intersection in p and ends in t, where
        s <_lex p <_lex t.
        We describe how the edges corresponding to c are generated:
        - When the sweep stops in s, we generate a node n_s for s and an edge e1 for the 
          portion between s and p of the curve. Observe that we have not seen p yet, so we 
          do not know the final target of e. Therefore, we add the self-loop edge 
          e1=(n_s,n_s) to the graph G. We call this unfinished edge an "edge stub". We set
          LastEdge[c]:=e1 to store the edge.
          The crucial property of e1 is that it has already the correct position in the 
          adjacency list of n_s (w.r.t. the counter-clockwise ordering).
    - Later the sweep stops in p, and we generate a node n_p corresponding to p.
          When we process the passing curve c, we add the edge e1'=(n_p,n_s) to G. (Note 
          that n_s = source(LastEdge[c]).) Since we know the final target of e1=LastEdge[c] 
          now, we can move its target to n_p (but we keep e1's position in the adj. list of 
          n_s), so that e1=(n_s,n_p). We mark e1 and e1' as reversals of each other.
          As c does not end in p, we add a new edge stub e2=(n_p,n_p), and set 
          LastEdge[c]:=e2.
        - Finally, the sweep reaches t, and we generate a node n_t corresponding to t.
          In an analogous way as above, we move the target of e2 to n_t and we add the 
          reversal e2'=(n_t,n_p).
        What remains to discuss is the question: How do we make sure that the edges are 
        added to the adjacency list of a node in the correct order?
        When the sweep processes an event point p, then it walks down the Y-Structure to 
        handle passing and ending curves, and then it inserts the curves starting in p.
        During this downward walk, we generate the edges that lead from n_p to the left
        (and we move the targets of their reversals to n_p), because the downward walk 
        traverses the incident curves to the left of p in ccw-order.
        AFTER the removal of the ending and the insertion of the starting curves, we 
        walk up the Y-Structure and generate the edge stubs for the edges that lead from
        n_p to the right, because the upward walk traverses the incident curves to right
        of p in ccw-order.
        After that all edges for n_p have been generated in ccw-order, some of them are 
        only stubs, but they have already the correct position in the adj. list of n_p.
*/

/// curve_sweep /////////////////////////////////////////////////////////////////////////////

template <class traits>
curve_sweep<traits>::curve_sweep()
: GraphPtr(nil), PtrToCorrespEdgeArray(nil),
  EmbedGraph(false), CompactifyGraph(true),
  LastNode(nil), LastEdge(nil), 
  YStructure(SweepCompare)
{}

template <class traits>
void
curve_sweep<traits>::run(const list<CURVE>& Cs, GRAPH<CRV_PNT,CURVE>& G)
{
        GraphPtr = &G; PtrToCorrespEdgeArray = 0;
        do_run(Cs);
}

template <class traits>
void
curve_sweep<traits>::run(const list<CURVE>& Cs, GRAPH<CRV_PNT,CURVE>& G, 
                                                 edge_array<edge>& corresp_edge)
{
        // NOTE: The corresp_edge option has not really been tested without EmbedGraph!

        GraphPtr = &G; PtrToCorrespEdgeArray = &corresp_edge;
        do_run(Cs);
}

template <class traits>
void
curve_sweep<traits>::do_run(const list<CURVE>& Cs)
{
        TIME( timer post_t("postprocess-sweep") );
        TIME( timer sweep_t("do-sweep") );
        TIME( timer init_t("init-sweep") );

        TIME( init_t.start() );
        initialize(Cs);
        TIME( init_t.stop() );
        TRACE( trc_begin_sweep(Cs) );
        TIME( sweep_t.start() );
        sweep();
        TIME( sweep_t.stop() );
        TRACE( trc_end_sweep() );
        TIME( post_t.start() );
        post_process();
        TIME( post_t.stop() );
}

template <class traits>
void 
curve_sweep<traits>::initialize(const list<CURVE>& Cs)
{
        get_graph().clear();
        TRAITS::reset();

        list<CURVE> x_monotonous_parts;

        CURVE crv;
        forall(crv, Cs) {
                TRAITS::update_sentinels(crv);

                x_monotonous_parts.clear();
                TRAITS::add_x_monotonous_parts(crv, x_monotonous_parts);

                CURVE x_mon_crv;
                forall(x_mon_crv, x_monotonous_parts) {
                        // observe x_mon_crv.source() <=_lex x_mon_crv.target()!
                        seq_item it1 = XStructure.insert(x_mon_crv.source(), seq_item(nil));
                        seq_item it2 = XStructure.insert(x_mon_crv.target(), seq_item(nil));

                        if (it1 == it2) {
                                if (!CompactifyGraph) continue;  // ignore zero-length curves
                                x_mon_crv = TRAITS::make_trivial_curve( XStructure.key(it1) ); 
                        }
                        else {
                                CRV_PNT p = XStructure.key(it1), q = XStructure.key(it2);
                                x_mon_crv = TRAITS::make_endpoints_of_curve_identical(x_mon_crv, p, q);
                                assert( identical(x_mon_crv.source(),p) && identical(x_mon_crv.target(),q) );
                        }

                        OriginalCurve[x_mon_crv] = crv; 
                        CurveQueue.insert(x_mon_crv.source(), x_mon_crv);
                }
        }

        CURVE lower_sentinel = TRAITS::get_lower_sentinel();
        CURVE upper_sentinel = TRAITS::get_upper_sentinel();

        SweepPos = lower_sentinel.source();
        SweepCompare.set_position(SweepPos); // needed for the insertions below

        YStructure.insert(upper_sentinel,nil_marker());
        YStructure.insert(lower_sentinel,nil_marker());

        CRV_PNT pstop = upper_sentinel.target();
        CurveQueue.insert(pstop,upper_sentinel);
        NextCurve = CurveQueue.inf(CurveQueue.find_min());
}

template <class traits>
void 
curve_sweep<traits>::sweep()
{
        while ( !XStructure.empty() ) {
                CHECK( check(3) );

                Event = XStructure.min();

                SweepPos = XStructure.key(Event);
                SweepCompare.set_position(SweepPos);
                NodeAtSweepPos = get_graph().new_node(SweepPos);
                TRACE( trc_new_sweep_pos(SweepPos, NodeAtSweepPos) );

                ItemAboveSweepPos = nil; ItemBelowSweepPos = nil;

                handle_ending_and_passing_curves();

                handle_starting_curves();
                                
                XStructure.del_item(Event);
        }
}

template <class traits>
void
curve_sweep<traits>::handle_ending_and_passing_curves()
{
        seq_item sit = XStructure.inf(Event);

        if (sit == nil) sit = YStructure.lookup(TRAITS::make_trivial_curve(SweepPos));
                // NOTE: this (also) handles the case of a trivial curve lying on another curve

        if (sit == nil) return; // only starting segments

        // walk up
        while ( YStructure.inf(sit) == Event || overlaps_succ(sit) )
                sit = YStructure.succ(sit);

        ItemAboveSweepPos = YStructure.succ(sit);

        // walk down 
        bool walk_on_down;
        do { 
                walk_on_down = false;
                CURVE crv = YStructure.key(sit);

                // generate edge(s) in the graph
                if ( !EmbedGraph ) {
                        node u = LastNode[crv]; assert(u != nil);
                        if ( TRAITS::have_same_direction(crv, OriginalCurve[crv]) )
                                get_graph().new_edge(u,NodeAtSweepPos,crv);
                        else            
                                get_graph().new_edge(NodeAtSweepPos,u,crv);
                        LastNode[crv] = NodeAtSweepPos;
                }
                else { // embed (for details see beginning of file)
                        edge e1 = LastEdge[crv]; // get the latest edge stub for crv
                        assert(e1 != nil && target(e1) == source(e1));
                        get_graph().move_edge(e1, e1, NodeAtSweepPos);
                        edge e2 = get_graph().new_edge(NodeAtSweepPos,source(e1),crv);
                        get_graph().set_reversal(e1,e2);
                }

                TRACE( trc_new_edge(get_graph().last_edge()) );

                // handle ending/passing curve
                if ( identical(SweepPos,crv.target()) ) { 
                        // ending curve
                        TRACE( trc_end_curve(crv) );
                        seq_item p_sit = YStructure.pred(sit);
                        if ( overlaps_succ(p_sit) ) {
                                walk_on_down = true;
                                YStructure.change_inf(p_sit, YStructure.inf(sit));
                                if ( touches_succ_at_sweep_pos(sit) ) {
                                        mark_as_touching_at_sweep_pos(p_sit, YStructure.succ(sit));
                                }
                        }
                        else if ( touches_succ_at_sweep_pos(p_sit) 
                                && ( overlaps_succ(sit) || touches_succ_at_sweep_pos(sit)) ) 
                        {
                                mark_as_touching_at_sweep_pos(p_sit, YStructure.succ(sit));
                        }

                        YStructure.del_item(sit);
                        sit = p_sit;
                }
                else {
                        // passing segment
                        TRACE( trc_passing_curve(crv) );
                        sit = YStructure.pred(sit); 
                }
        } while ( YStructure.inf(sit) == Event || walk_on_down || overlaps_succ(sit) );

        ItemBelowSweepPos = sit;
        seq_item sit_first = YStructure.succ(ItemBelowSweepPos);

        // reverse subsequences of curves that overlap or touch in SweepPos (if existing)
        sit = sit_first;
        while ( sit != ItemAboveSweepPos ) { 
                seq_item sub_first = sit;
                seq_item sub_last  = sub_first;

                while ( overlaps_succ(sub_last) || touches_succ_at_sweep_pos(sub_last) )
                        sub_last = YStructure.succ(sub_last);

                if (sub_last != sub_first)
                        YStructure.reverse_items(sub_first, sub_last);

                sit = YStructure.succ(sub_first);
        }

        // reverse the entire bundle
        if ( sit_first != ItemAboveSweepPos )
                YStructure.reverse_items(YStructure.succ(ItemBelowSweepPos), 
                                                                 YStructure.pred(ItemAboveSweepPos));  
}

template <class traits>
void
curve_sweep<traits>::handle_starting_curves()
{
        seq_item sit;

        while ( identical(SweepPos, NextCurve.source()) ) {
                TRACE( trc_start_curve(NextCurve) );
                if ( !NextCurve.is_trivial() ) {
                        seq_item s_sit = YStructure.locate(NextCurve);
                                
                        if ( ItemAboveSweepPos == nil ) {
                                ItemAboveSweepPos = s_sit; 
                                ItemBelowSweepPos = YStructure.pred(s_sit);
                        }

                        sit = YStructure.insert_at(s_sit, NextCurve, nil_marker());

                        // insert an end event for NextCurve
                        XStructure.insert(NextCurve.target(), sit);

                        // remember the node corresponding to NextCurve.source()
                        // (if EmbedGraph we use LastEdge for the graph)
                        if (!EmbedGraph) LastNode[NextCurve] = NodeAtSweepPos;
                }
                else {
                        // a trivial curve is modelled as a self-loop
                        // (it appears in CurveQueue, only if CompactifyGraph is true)
                        get_graph().new_edge(NodeAtSweepPos, NodeAtSweepPos, NextCurve);
                }

                // delete minimum and assign new minimum to NextCurve                   
                CurveQueue.del_min();
                NextCurve = CurveQueue.inf(CurveQueue.find_min());
        }

        if ( ItemAboveSweepPos == nil ) return; // this can be nil for an isolated node

        // check all curves in the interval [ItemBelowSweepPos;ItemAboveSweepPos[ 
        // for intersections with their successors
        // newly inserted curves (i.e. those with soure() identical to SweepPos) are also
        // checked for overlapping

        TRACE( trc_check_bundle_for_intersections(ItemBelowSweepPos, ItemAboveSweepPos) );

        CRV_PNT inter; // for intersections
        seq_item sit_succ;
        for (sit = ItemBelowSweepPos; sit_succ = YStructure.succ(sit), sit != ItemAboveSweepPos; 
                 sit = sit_succ) 
        {
                if (overlaps_succ(sit))
                        continue; // already detected overlapping

                YStructure.change_inf(sit, nil_marker());

                if (sit == YStructure.min_item() || sit_succ == YStructure.max_item())
                        continue; // skip sentinels
                        
                CURVE c1 = YStructure.key(sit);
                CURVE c2 = YStructure.key(sit_succ);

                if ( (identical(c1.source(),SweepPos)) && sit_succ != ItemAboveSweepPos
                         && TRAITS::A_overlaps_B_given_that_As_source_is_on_B(c1,c2) )
                {
                        YStructure.change_inf(sit, overlapping_marker());
                }
                else if ( (identical(c2.source(),SweepPos)) && sit != ItemBelowSweepPos
                         && TRAITS::A_overlaps_B_given_that_As_source_is_on_B(c2,c1) )
                {
                        YStructure.change_inf(sit, overlapping_marker());
                }
                else // no overlap or touch, but possibly proper intersection
                {
                        TRACE( if (c1.overlaps(c2)) check_report_error(sit, "overlapping not detected") );
                        TRACE( trc_check_for_intersection(sit) );

                        if ( TRAITS::intersect_right_of_pswp_given_A_leq_pswp_leq_B(SweepPos, c1, c2, inter) )
                        {
                                YStructure.change_inf(sit, XStructure.insert(inter, sit));
                                TRACE( trc_found_intersection(sit, inter) );
                        }
                }
        }

        if (EmbedGraph)
                // walk up -> corresponds to ccw-order to the right of SweepPos
                for (sit=YStructure.succ(ItemBelowSweepPos); sit!=ItemAboveSweepPos; sit=YStructure.succ(sit))
                {
                  CURVE crv = YStructure.key(sit);
                  LastEdge[crv] = get_graph().new_edge(NodeAtSweepPos, NodeAtSweepPos, crv);
                  TRACE( trc_new_edge_stub(get_graph().last_edge()) );
                }
}

template <class traits>
void
curve_sweep<traits>::post_process()
{
        // restore the original curves
        edge e;
        forall_edges(e, get_graph()) get_graph()[e] = OriginalCurve[get_graph()[e]];

        // compute corresp. edge if necessary
        if (PtrToCorrespEdgeArray) compute_corresponding_edges(*PtrToCorrespEdgeArray);

        // remove nodes that have been introduced by make_x_monotonous and
        // are no real end point or proper intersection point or original curves
        if (CompactifyGraph) {
                if (EmbedGraph) compactify_embedding();
                else            compactify_simple_graph();

                // delete self-loops (which corresp. to trivial curves)
                edge e;
                forall_edges(e, get_graph()) if (source(e) == target(e)) get_graph().del_edge(e);
        }

        assert(!EmbedGraph || Is_Planar_Map(get_graph()));

        // clear internal data_structures
        OriginalCurve.clear();
        LastNode.clear();
        LastEdge.clear();
        CurveQueue.clear();
        XStructure.clear();
        YStructure.clear();
        SweepPos = CRV_PNT();
        NextCurve = CURVE();
        Event = nil;
        ItemAboveSweepPos = nil; ItemBelowSweepPos = nil;

        TRAITS::finish(); // clear data structures of traits class as well
}

/// corresponding edges ********************

template <class traits>
void 
curve_sweep<traits>::compute_corresponding_edges(edge_array<edge>& corresp_edge)
{
        corresp_edge.init(get_graph(), nil);
        
        if (EmbedGraph) {
                node n;
                forall_nodes(n, get_graph())
                        embedding_corresponding_edges_for(n, corresp_edge);
        }
        else {
                node n;
                forall_nodes(n, get_graph())
                        simple_graph_corresponding_edges_for(n, corresp_edge);
        }

        LastEdge.clear();
}

template <class traits>
void 
curve_sweep<traits>::embedding_corresponding_edges_for(node n, edge_array<edge>& corresp_edge)
// We try to find for every out-edge e of n a corresponding out-edge f of n
// s.th. e and f are labelled with the same curve and have different targets.
// NOTE: This also works if several edges are labeled with identical curves.
{
        LastEdge.clear();

        edge e2;
        forall_out_edges(e2, n) {
                CURVE crv = get_graph()[e2];
                edge e1 = LastEdge[crv];
                if (e1 && target(e1) != target(e2)) {
                        // edges correspond => pop e1 from the front of list
                        LastEdge[crv] = corresp_edge[e1];
                        corresp_edge[e1] = e2; corresp_edge[e2] = e1;
                }
                else {
                        // same target => edges do not correspond => push e2 to the front of the list
                        corresp_edge[e2] = e1; 
                        LastEdge[crv] = e2;
                }
        }

        // those edges that are still in some list have no corresp. edge
        forall(e2, LastEdge) {
                while (e2) {
                        edge e1 = corresp_edge[e2];
                        corresp_edge[e2] = nil;
                        e2 = e1;
                }
        }
}

template <class traits>
void 
curve_sweep<traits>::simple_graph_corresponding_edges_for(node n, edge_array<edge>& corresp_out_edge)
// For every edge e into a node n we try to compute a corresponding edge f out of n
// which is labelled with the same curve as e.
// NOTE: This also works if several edges are labeled with identical curves.
// NOTE: Every self-loop has itself as corresp_out_edge (required by compactify_simple_graph).
{
        LastEdge.clear();

        edge e2;
        forall_in_edges(e2, n) {
                CURVE crv = get_graph()[e2];
                edge e1 = LastEdge[crv];
                // push e2 to the front of the list for crv
                corresp_out_edge[e2] = e1; 
                LastEdge[crv] = e2;
        }

        forall_out_edges(e2, n) {
                CURVE crv = get_graph()[e2];
                edge e1 = LastEdge[crv]; // pick an edge into n labelled with crv
                if (e1) {
                        // edges correspond => pop e1 from front of the list
                        LastEdge[crv] = corresp_out_edge[e1];
                        corresp_out_edge[e1] = e2; // make e2 the out-edge corresp. to the in-edge e1
                }
                // else out-edge e2 has no corresp. in-edge => nothing to do
        }

        // those edges that are still in some list have no corresp. edge
        forall(e2, LastEdge) {
                while (e2) {
                        edge e1 = corresp_out_edge[e2];
                        corresp_out_edge[e2] = nil;
                        e2 = e1;
                }
        }
}

/// compactify *****************************

#ifdef LEDA_CURVE_SWEEP_COMPACTIFICATION_VISUAL_DEBUG
window* TraceWinForCompatification = 0;
#endif

template <class traits>
void 
curve_sweep<traits>::compactify_simple_graph()
{
        list<node> trivial_curve_nodes = Delete_Loops(get_graph()); // -> no self loops!
        set<node> nodes_corresp_to_trivial_curve;
        node n;
        forall(n, trivial_curve_nodes) nodes_corresp_to_trivial_curve.insert(n);

        edge_array<edge> corresp_out_edge(get_graph(), nil);

        forall_nodes(n, get_graph()) {
                if ( get_graph().outdeg(n) != get_graph().indeg(n) 
                         || nodes_corresp_to_trivial_curve.member(n) ) continue;

                bool compactify_n = true;

                // check that 
                // - there are at most two different nodes adjacent to n
                node n1 = nil, n2 = nil;
                edge e;
                forall_out_edges(e, n) {
                        node tgt = target(e);
                        if (n1 == tgt || n2 == tgt) continue;

                        if (n1 == nil) n1 = tgt;
                        else if (n2 == nil) n2 = tgt;
                        else { compactify_n = false; break; }
                }

                assert(n1); // since G.outdeg(n) != 0
                if (!compactify_n) continue;

                // check that 
                // - there are exactly two different nodes adjacent to n and
                // - every in-edge has a corresponding out-edge and
                // - all curves overlap (the scale_crv)
                simple_graph_corresponding_edges_for(n, corresp_out_edge);

                edge e_first_in = get_graph().first_in_edge(n);
                CURVE scale_crv = get_graph()[e_first_in];
                if (n2 == nil) n2 = source(e_first_in);
                        // if n2 is nil (i.e. all out-edges end in n1), 
                        // then ALL in-edges of n must start in n2

                forall_in_edges(e, n) {
                        node src = source(e);

                        if ( (n1 != src && n2 != src)
                                || !corresp_out_edge[e]
                                || !TRAITS::A_overlaps_B_given_they_share_three_points(get_graph()[e], scale_crv) )
                        {
                                compactify_n = false; break; 
                        }                       
                }

                assert(n1 != n2); // since all edges into n1 have a corresp. edge
                if (!compactify_n) continue;

#ifdef LEDA_CURVE_SWEEP_COMPACTIFICATION_VISUAL_DEBUG
                point pos = get_graph()[n].to_float();
                cout << "compacting " << index(n) << " " << pos << " " << endl;
                if (TraceWinForCompatification) {
                        TraceWinForCompatification->draw_node(pos, blue);
                        //TraceWinForCompatification->draw_vline(pos.xcoord());
                }
#endif

                edge e_in;
                forall_in_edges(e_in, n) {
                        edge e_out = corresp_out_edge[e_in]; // the corresp. out edge
                        get_graph().move_edge(e_in, e_in, target(e_out));
                }
                get_graph().del_node(n);
        }
}

template <class traits>
void 
curve_sweep<traits>::compactify_embedding()
{
        bool compute_corresp_edge_array = false;
        if (PtrToCorrespEdgeArray == 0) {
                PtrToCorrespEdgeArray = new edge_array<edge>(get_graph(), nil);
                compute_corresp_edge_array = true;
        }

        edge_array<edge>& corresp_edge = *PtrToCorrespEdgeArray;

        node n;
        forall_nodes(n, get_graph()) {
                if (get_graph().outdeg(n) == 0 || get_graph().outdeg(n) % 2 != 0) continue;

                bool compactify_n = true;

                // check that there are exactly two different nodes adjacent to n
                // (by the same number of edges)
                node n1 = nil, n2 = nil;
                int num_edges_to_n1 = 0, num_edges_to_n2 = 0;
                edge e;
                forall_out_edges(e, n) {
                        node tgt = target(e);
                        if (n1 == nil) n1 = tgt;
                        if (n1 == tgt) {
                                ++num_edges_to_n1;
                                continue;
                        }

                        if (n2 == nil) n2 = tgt;
                        if (n2 == tgt) {
                                ++num_edges_to_n2;
                                continue;
                        }

                        // found a third target
                        compactify_n = false; break;
                }

                if (num_edges_to_n1 != num_edges_to_n2 || !compactify_n) continue;

                if (compute_corresp_edge_array) embedding_corresponding_edges_for(n, corresp_edge);

                // check that
                // - every edge (n,n2) has a corresponding edge
                // - that all curves overlap
                CURVE scale_crv = get_graph()[get_graph().first_adj_edge(n)];

                forall_out_edges(e, n) {
                        if ( target(e) == n1 ) continue;
                        if (   !corresp_edge[e]
                                || !TRAITS::A_overlaps_B_given_they_share_three_points(get_graph()[e], scale_crv) )
                        {
                                compactify_n = false; break;
                        }
                }

                if (!compactify_n) continue;

                // do the compactification: move the in_edges of n and then del n
                // PROBLEM: overlapping edges are ordered in the adj. lists according
                //   to the ids of their corresp. x-montonous parts (not the ids of the
                //   original curves)!
                //   Assume we have two overlapping segments s_e and s_f, which are both
                //   split in two x-mon. parts: s_e1,s_e2 and s_f1,s_f2. Suppose further
                //   that s_ei/s_fi starts in G[n] and ends in G[ni]. It may be the case
                //   that the corresp. edges cross at n, i.e. the adj. list of n looks 
                //   as follows: e1, f1, e2, f2. 
                //   This implies the list of n1: ..., f1_rev, e1_rev, ...
                //   and for n2:                  ..., f2_rev, e2_rev, ...
                //   So we have to change the relative order of fi_rev and ei_rev for 
                //   either n1 or n2 (see pg. 783 of the LEDA book).
                edge e1_rev_succ = nil, e2_rev_pred = nil;
                edge e1, e2;
                forall_out_edges(e2, n) {
                        if (target(e2) != n2) continue;
                        e1 = corresp_edge[e2];
                        assert( e1!=nil && target(e1)==n1 && identical(get_graph()[e1], get_graph()[e2]) );
                        edge e1_rev = get_graph().reversal(e1); assert(e1_rev);
                        edge e2_rev = get_graph().reversal(e2); assert(e2_rev);
                        if (e1_rev_succ == nil) { e1_rev_succ = e1_rev; e2_rev_pred = e2_rev; }
                        get_graph().move_edge(e1_rev, e1_rev_succ, n2, leda::before);
                        get_graph().move_edge(e2_rev, e2_rev_pred, n1, leda::behind);
                        get_graph().set_reversal(e1_rev, e2_rev);
                        e1_rev_succ = e1_rev; e2_rev_pred = e2_rev;
                }
                get_graph().del_node(n);
        }

        if (compute_corresp_edge_array) {
                delete PtrToCorrespEdgeArray; PtrToCorrespEdgeArray = 0;
        }
}

/// brute-force ****************************

template <class traits>
void 
curve_sweep<traits>::run_brute_force(const list<CURVE>& Cs, GRAPH<CRV_PNT,CURVE>& G)
{
        GraphPtr = &G; PtrToCorrespEdgeArray = 0;

        get_graph().clear();
        TRAITS::reset();

        dictionary<CRV_PNT, node> node_of;

        CURVE crv;
        list<CURVE> all_curves;

        if (CompactifyGraph) {
                all_curves = Cs;
        }
        else {
                // create x-monotonous parts
                forall(crv, Cs) {
                        TRAITS::add_x_monotonous_parts(crv, all_curves);
                }
        }

        TIME( cout << "all_curves.size(): " << all_curves.size() << endl );

        // add nodes for the sources and targets
        forall(crv, all_curves) {
                CRV_PNT src = crv.source();
                if (! node_of.lookup(src)) node_of.insert(src, G.new_node(src));

                CRV_PNT tgt = crv.target();
                if (! node_of.lookup(tgt)) node_of.insert(tgt, G.new_node(tgt));
        }

        // test every pair of curves for intersections
        CURVE crv1, crv2;
        list_item it1, it2;
        forall_items(it1, all_curves) {
                crv1 = all_curves[it1];
                for (it2 = all_curves.succ(it1); it2; it2 = all_curves.succ(it2)) {
                        crv2 = all_curves[it2];
                        list<CRV_PNT> inters = TRAITS::intersections(crv1, crv2);
                        CRV_PNT inter;
                        forall(inter, inters) {
                                if (! node_of.lookup(inter)) node_of.insert(inter, G.new_node(inter));
                        }
                }
        }

        bool save_embed = get_embedding_flag(); set_embedding_flag(false);
        bool save_compactify = get_compactification_flag(); set_compactification_flag(false);
        post_process();
        set_embedding_flag(save_embed);
        set_compactification_flag(save_compactify);
}

/// CHECK ****************************

#if defined(LEDA_CURVE_SWEEP_CHECK) || defined(LEDA_CURVE_SWEEP_TRACE)

template <class traits>
bool
curve_sweep<traits>::check(int accuracy) const
{
        bool ok = check_invariants(accuracy);
        return ok;
}

template <class traits>
bool 
curve_sweep<traits>::check_invariants(int accuracy) const
{
        if (YStructure.size() == 2) return true; // only the two sentinels

        CURVE p_sweep_as_crv = TRAITS::make_trivial_curve(SweepPos);
        seq_item yit = YStructure.locate_succ(p_sweep_as_crv); // yit >= SweepPos
        if (yit == nil) {
                check_report_error(0, "could not locate SweepPos in YStructure");
                return false;
        }

        // find segment immediately below SweepPos
        seq_item yit_below = yit;
        do {
                yit_below = YStructure.pred(yit_below);
        } while (yit_below && TRAITS::compare_intersections_with_sweepline_at
                                                          (SweepPos, p_sweep_as_crv, YStructure.key(yit_below)) == 0);

        if (yit_below == nil) {
                check_report_error(0, "walked down below sentinel");
                return false;
        }

        // find segment immediately above SweepPos
        seq_item yit_above = yit;
        while (yit_above && TRAITS::compare_intersections_with_sweepline_at
                                                        (SweepPos, p_sweep_as_crv, YStructure.key(yit_above)) == 0)
        {
                yit_above = YStructure.pred(yit_above);
        }
        if (yit_above == nil) {
                check_report_error(0, "walked up above sentinel");
                return false;
        }

        seq_item yit_start = yit_below, yit_stop = yit_above;
        if (accuracy > 0) { yit_start = YStructure.min_item(); yit_stop = YStructure.max_item(); }

        // check: order in YStruct, overlap, YStruct.inf entries (intersections later)
        for(yit = yit_start; yit != yit_stop; yit = YStructure.succ(yit)) {
                seq_item yit_succ = YStructure.succ(yit);

                CURVE c1 = YStructure.key(yit);
                CURVE c2 = YStructure.key(yit_succ);

                // check order in YStructure:
                int cmp = TRAITS::compare_intersections_with_vertical_line_through_sweeppos(SweepPos, c1, c2);
                if (cmp > 0) {
                        check_report_error(yit, "order in YStructure must be reversed");
                        return false;
                }

                // Notation: <c1> denotes the item of c1, i.e. yit
                //           <c2> denotes the item of c2, i.e. yit_succ

                // Inv.: c1,c2 overlap => <c1> should be marked as overlapping
                if (c1.overlaps(c2)) {
                        if (!overlaps_succ(yit)) {
                                check_report_error(yit, "should be marked as overlapping");
                                return false;
                        }
                        else continue;
                }
                else if (overlaps_succ(yit)) {
                        check_report_error(yit, "should NOT be marked as overlapping");
                        return false;
                }

                // Inv.: yit is marked with an event xit := inf(<c1>)
                //       => xit belongs to XStructure

                seq_item xit = YStructure.inf(yit);
                if (is_marked_with_event(yit)) {
                        if (XStructure.my_sortseq(xit) != &XStructure) {
                                check_report_error(yit, string("%x is no item in XStructure", xit));
                                return false;
                        }
                        
                        // Inv.: xit=<p> => p is contained in c1
                        CRV_PNT p = XStructure.key(xit);
                        if (! c1.contains(p)) {
                                check_report_error(yit, string("item %x is not contained in c1", xit));
                                return false;
                        }

                        if (compare(p, SweepPos) < 0) {
                                check_report_error(yit, string("item %x refers to point before SweepPos", xit));
                                return false;
                        }
                }
        }

        // now we check the bundle [yit_below,yit_above] for intersections
        for(yit = yit_below; yit != yit_above; yit = YStructure.succ(yit)) {
                if (overlaps_succ(yit)) continue; // we checked this already ...

                CURVE c1 = YStructure.key(yit);
                CURVE c2 = YStructure.key(YStructure.succ(yit));

                CRV_PNT i;
                bool do_intersect = false;
                if (accuracy > 1) {
                        list<CRV_PNT> inters = TRAITS::intersections(c1, c2);
                        while (!inters.empty() && compare(inters.front(),SweepPos) <= 0)
                                inters.pop_front();
                        if (!inters.empty()) {
                                do_intersect = true; i = inters.front();
                        }
                }
                else
                        do_intersect = TRAITS::intersect_right_of_pswp_given_A_leq_pswp_leq_B(SweepPos, c1, c2, i);

                // Inv.: c1,c2 intersect in i => xit = <i>
                if (do_intersect) {
                        seq_item xit = YStructure.inf(yit);
                        if (! is_marked_with_event(xit)) {
                                check_report_error(yit, "intersecting curve is not marked");
                                return false;
                        }
                        
                        // Observe: The tests above ensure that xit belongs to the XStrucutre
                        if (XStructure.key(xit) != i) {
                                CRV_PNT key = XStructure.key(xit);
                                if (!c2.contains(key)) {
                                        check_report_error(yit, "xit of c1 is not contained in c2");
                                        return false;
                                }
                                if (compare(key, SweepPos) <= 0) {
                                        check_report_error(yit, "xit of c1 <=_lex SweepPos");
                                        return false;
                                }
                                if (compare(key, i) > 0) {
                                        check_report_error(yit, "xit of c1 is a later intersection");
                                        return false;
                                }
                        }
                }
        }

        return true;
}

template <class traits>
void 
curve_sweep<traits>::check_report_error(seq_item yit, string msg) const
{
        if (yit) {
                seq_item yit_succ = YStructure.succ(yit);
                CURVE c1 = YStructure.key(yit), c2 = YStructure.key(yit_succ);
                cout << endl << "ERROR in YStructure/XStructure"
                         << " (Pos="; TRAITS::print_point(cout, SweepPos); cout << ")" << endl;
                cout << "  " << yit_succ << " "; TRAITS::print_curve(cout, c2);
                cout << " / id:" << trc_crv_id_to_str(c2) << endl;
                cout << "  " << yit << " "; TRAITS::print_curve(cout, c1); 
                cout << " / id:" << trc_crv_id_to_str(c1);
                cout << " / inf:" << trc_yitem_inf_to_str(yit) << endl;
                cout << "  " << msg << endl << endl;
        }
        else {
                cout << "ERROR: " << msg << endl;
        }

        if (YStructure.size() < 15 || confirm("Error: Dump YStructure? "))
                dump_YStructure("ERROR");

        if (!confirm("Continue? ")) exit(1);
}

template <class traits>
void 
curve_sweep<traits>::dump_YStructure(const string& msg, ostream& out) const
{
        cout << endl;
        if (msg == "") cout << "YStructure:";
        else           cout << "YStructure (" << msg << "):" << endl;
        cout << endl;

        seq_item sit;
        forall_rev_items(sit, YStructure) {
                out << sit << " ";
                CURVE crv = YStructure.key(sit);
                if (crv == TRAITS::get_lower_sentinel()) out << "lower sentinel";
                else if (crv == TRAITS::get_upper_sentinel()) out << "upper sentinel";
                else {
                        TRAITS::print_curve(cout, crv);
                }
                cout << " id:" << trc_crv_id_to_str(crv) << " / " 
                         << trc_yitem_inf_to_str(sit) << endl;
        }
        cout << endl;
}

template <class traits>
string 
curve_sweep<traits>::trc_yitem_inf_to_str(seq_item yit) const
{
        if (has_nil_marker(yit)) return string("nil");
        else if (overlaps_succ(yit)) return string("ovl");
        else return string("xit");
}

template <class traits>
string 
curve_sweep<traits>::trc_crv_id_to_str(const CURVE& crv) const
{
        return string("%03x", ID_Number(crv) & 0xFFF);
}

/// TRACE ****************************

template <class traits>
void 
curve_sweep<traits>::trc_new_sweep_pos(const CRV_PNT& new_pos, node node_at_new_pos) const
{
        cout << endl;
        cout << "TRC: new sweep pos: "; TRAITS::print_point(cout, new_pos);
        cout << " / corresp. node: " << index(node_at_new_pos) << endl;
}

template <class traits>
void 
curve_sweep<traits>::trc_start_curve(const CURVE& crv) const
{
        cout << "TRC:   start curve: "; TRAITS::print_curve(cout, crv);
        cout << " / id: " << trc_crv_id_to_str(crv) << endl;
        if (! identical(get_sweep_position(), crv.source()))
                check_report_error(0, "curve does not start here!");
}

template <class traits>
void 
curve_sweep<traits>::trc_end_curve(const CURVE& crv) const
{
        cout << "TRC:   end curve: "; TRAITS::print_curve(cout, crv);
        cout << " / id: " << trc_crv_id_to_str(crv) << endl;
        if (! identical(get_sweep_position(), crv.target()))
                check_report_error(0, "curve does not end here!");
}

template <class traits>
void 
curve_sweep<traits>::trc_passing_curve(const CURVE& crv) const
{
        cout << "TRC:   passing curve: "; TRAITS::print_curve(cout, crv);
        cout << " / id: " << trc_crv_id_to_str(crv) << endl;
        if (! crv.contains(get_sweep_position()))
                check_report_error(0, "curve does not pass here!");
}

template <class traits>
void 
curve_sweep<traits>::trc_new_edge(edge e) const
{
        CURVE crv = get_graph()[e];
        cout << "TRC:   new edge: (" << index(source(e)) << "," << index(target(e)) << ")";
        cout << " / curve id: " << trc_crv_id_to_str(crv) << endl;
}

template <class traits>
void 
curve_sweep<traits>::trc_new_edge_stub(edge e) const
{
        CURVE crv = get_graph()[e];
        cout << "TRC:   new edge stub: (" << index(source(e)) << "," << index(target(e)) << ")";
        cout << " / curve id: " << trc_crv_id_to_str(crv) << endl;
        if (target(e) != source(e))
                check_report_error(0, "edge stub is no self-loop");
}

template <class traits>
void 
curve_sweep<traits>::trc_check_bundle_for_intersections(seq_item below, seq_item above) const
{
#ifdef LEDA_CURVE_SWEEP_TRACE_BUNDLE
        CURVE crv1 = get_YStructure().key(below), crv2 = get_YStructure().key(above);
        cout << "TRC:   check bundle for intersections:" <<endl;
        cout << "       below: "; TRAITS::print_curve(cout, crv1);
        cout << " / id: " << trc_crv_id_to_str(crv1) << endl;
        cout << "       above: "; TRAITS::print_curve(cout, crv2);
        cout << " / id: " << trc_crv_id_to_str(crv2) << endl;
#endif
}

template <class traits>
void 
curve_sweep<traits>::trc_check_for_intersection(seq_item sit) const
{
#ifdef LEDA_CURVE_SWEEP_TRACE_INTERSECTION
        seq_item sit_succ = get_YStructure().succ(sit);
        CURVE crv1 = get_YStructure().key(sit), crv2 = get_YStructure().key(sit_succ);
        cout << "TRC:     checking for intersection: " << endl;
        cout << "         crv1: "; TRAITS::print_curve(cout, crv1);
        cout << " / id: " << trc_crv_id_to_str(crv1) << endl;
        cout << "         crv2: "; TRAITS::print_curve(cout, crv2);
        cout << " / id: " << trc_crv_id_to_str(crv2) << endl;
#endif
}

template <class traits>
void 
curve_sweep<traits>::trc_found_intersection(seq_item sit, const CRV_PNT& inter) const
{
#ifdef LEDA_CURVE_SWEEP_TRACE_INTERSECTION
        seq_item sit_succ = get_YStructure().succ(sit);
        CURVE crv1 = get_YStructure().key(sit), crv2 = get_YStructure().key(sit_succ);
        cout << "TRC:     -> found intersection: "; TRAITS::print_point(cout, inter); cout << endl;
#endif
}

#endif // #if defined(LEDA_CURVE_SWEEP_CHECK) || defined(LEDA_CURVE_SWEEP_TRACE)

/// curve_sweep_trc_win /////////////////////////////////////////////////////////////////////

#if defined(LEDA_CURVE_SWEEP_VISUAL_DEBUG)

template <class traits>
curve_sweep_trc_win<traits>::curve_sweep_trc_win(window* trc_win)
 : TraceWin(trc_win) 
{
        MessagePanel = new panel(250, 150, "Sweep Messages");

        if (win()) {
                int x = win()->xpos()-mpanel()->width()-10;
                mpanel()->display(x, window::center);
        }
        else {
                mpanel()->display(window::center, window::center);
        }
}

template <class traits>
curve_sweep_trc_win<traits>::~curve_sweep_trc_win()
{
        delete MessagePanel; MessagePanel = 0;
}

template <class traits>
void 
curve_sweep_trc_win<traits>::wait(string msg) const
{
        msg += string("\\newline NO = dump YStructure");
        if (mpanel()) {
                if (! mpanel()->confirm(msg)) dump_YStructure();
        }
}

template <class traits>
void 
curve_sweep_trc_win<traits>::draw_curve(const CURVE& crv, int col, bool use_xor_mode) const
{
        if (! win()) return;

        drawing_mode m_sav = win()->set_mode(use_xor_mode ? xor_mode : src_mode);
        color c_sav = win()->set_fg_color(color(col));
        draw(*win(), crv); //(*win()) << crv;
        win()->set_fg_color(c_sav);
        win()->set_mode(m_sav); 
}

template <class traits>
void 
curve_sweep_trc_win<traits>::draw_pos(const CRV_PNT& p, int col, bool use_xor_mode) const
{
        if (! win()) return;

        drawing_mode m_sav = win()->set_mode(use_xor_mode ? xor_mode : src_mode);
        color c_sav = win()->set_fg_color(color(col));
        win()->draw_filled_node(p.to_point(), col);
        win()->set_fg_color(c_sav);
        win()->set_mode(m_sav); 
}

template <class traits>
void 
curve_sweep_trc_win<traits>::highlight(const CURVE& crv, string msg, int col) const
{
        draw_curve(crv, col, true);
        wait(msg);
        draw_curve(crv, col, true);
}


template <class traits>
void 
curve_sweep_trc_win<traits>::check_report_error(seq_item yit, string msg) const
{
        if (!win() || !mpanel()) {
                base::check_report_error(yit, msg); return;
        }

        if (yit) {
                seq_item yit_succ = get_YStructure().succ(yit);
                CURVE c1 = get_YStructure().key(yit), c2 = get_YStructure().key(yit_succ);

                int col = pink;
                draw_curve(c1, col, true); draw_curve(c2, col+1, true);
                wait("ERROR: " + msg);
                draw_curve(c1, col, true); draw_curve(c2, col+1, true);
        }
        else wait("ERROR: " + msg);
}

template <class traits>
void 
curve_sweep_trc_win<traits>::dump_YStructure(const string& msg, ostream& out) const
{
        int col = pink;

        base::dump_YStructure(msg, out);
        if (!win() || !mpanel()) return;

        if (msg != "") mpanel()->set_tmp_label(msg);

        const sortseq<CURVE,seq_item>& YStructure = get_YStructure();

        for (int run = 1; run <= 2; ++run) {
                seq_item sit;
                forall_items(sit, YStructure) {
                        CURVE crv = YStructure.key(sit);
                        if (sit == YStructure.min_item()) {
                                if (crv == TRAITS::get_lower_sentinel()) continue;
                                if (run == 1) mpanel()->acknowledge("min_item != lower_sentinel");
                        }
                        if (sit == YStructure.max_item()) {
                                if (crv == TRAITS::get_upper_sentinel()) continue;
                                if (run == 1) mpanel()->acknowledge("max_item != upper_sentinel");
                        }

                        draw_curve(crv, col, true);
                        if (is_marked_with_event(sit)) {
                                seq_item xit = YStructure.inf(sit);
                                draw_pos(get_XStructure().key(xit), blue, true);
                        }

                        if (run == 1) {
                                string msg = string("dump crv id: ") + trc_crv_id_to_str(crv);
                                msg += string(" / inf: ") + trc_yitem_inf_to_str(sit);
                                mpanel()->acknowledge(msg);
                        }
                }

                if (run == 1) mpanel()->acknowledge("dump Y finished");
        }

        if (msg != "") mpanel()->reset_frame_label();
}


template <class traits>
void 
curve_sweep_trc_win<traits>::trc_begin_sweep(const list<CURVE>& orig_curves) const
{
        base::trc_begin_sweep(orig_curves);
        if (!win()) return;

        win()->clear();
        CURVE crv;
        forall(crv, orig_curves) draw_curve(crv, black);
}

template <class traits>
void 
curve_sweep_trc_win<traits>::trc_new_sweep_pos(const CRV_PNT& new_pos, node node_at_new_pos) const
{
        base::trc_new_sweep_pos(new_pos, node_at_new_pos);
        if (!win()) return;

        draw_pos(new_pos, red);
        win()->draw_vline(new_pos.to_point().xcoord());
        wait("new sweep pos");
}

template <class traits>
void 
curve_sweep_trc_win<traits>::trc_start_curve(const CURVE& crv) const
{
        base::trc_start_curve(crv);
        if (!win()) return;

        draw_curve(crv, yellow);
        wait("start curve");
}

template <class traits>
void 
curve_sweep_trc_win<traits>::trc_end_curve(const CURVE& crv) const
{
        base::trc_end_curve(crv);
        if (!win()) return;

        draw_curve(crv, green);
        wait("end curve");
}

template <class traits>
void 
curve_sweep_trc_win<traits>::trc_passing_curve(const CURVE& crv) const
{
        base::trc_passing_curve(crv);
        if (!win()) return;
        highlight(crv, "passing curve");
}

template <class traits>
void 
curve_sweep_trc_win<traits>::trc_new_edge(edge e) const
{
        base::trc_new_edge(e);
        if (!win()) return;
}

template <class traits>
void 
curve_sweep_trc_win<traits>::trc_check_bundle_for_intersections(seq_item below, seq_item above) const
{
        base::trc_check_bundle_for_intersections(below, above);
        if (!win()) return;

#ifdef LEDA_CURVE_SWEEP_TRACE_BUNDLE
        CURVE crv1 = get_YStructure().key(below), crv2 = get_YStructure().key(above);

        draw_curve(crv1, green, true);
        draw_curve(crv2, green, true);
        wait("check bundle for inter");
        draw_curve(crv2, green, true);
        draw_curve(crv1, green, true);
#endif
}

#endif // #if defined(LEDA_CURVE_SWEEP_VISUAL_DEBUG)

LEDA_END_NAMESPACE
