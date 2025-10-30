/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _curve_sweep.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// S. Thiel (2004)

#include <LEDA/geo/curve_sweep_traits.h>
#include "curve_sweep_templ.cpp"

// The functions implemented in this file are declared at the end of r_circle_segment.h!

LEDA_BEGIN_NAMESPACE

/// curve_sweep (r_circle_segment) //////////////////////////////////////////////////////////

typedef curve_sweep<curve_sweep_traits_r_circle_segment> r_circle_segment_sweeper;

__exportF 
void SWEEP_SEGMENTS(const list<r_circle_segment>& S, 
					GRAPH<r_circle_point,r_circle_segment>& G, 
					bool embed)
{
	r_circle_segment_sweeper sweeper;
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(true);
    sweeper.run(S, G);
}

__exportF 
void SWEEP_SEGMENTS(const list<r_circle_segment>& S, 
					GRAPH<r_circle_point,r_circle_segment>& G, 
					edge_array<edge>& corresp_edge,
					bool embed)
{
	r_circle_segment_sweeper sweeper;
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(true);
    sweeper.run(S, G, corresp_edge);
}

__exportF 
void INTERSECT_SEGMENTS_BRUTE_FORCE(const list<r_circle_segment>& S, 
									GRAPH<r_circle_point,r_circle_segment>& G)
{
	r_circle_segment_sweeper sweeper;
    sweeper.run_brute_force(S, G);
}

/// curve_sweep (r_circle_segment, coloured, mc) ////////////////////////////////////////////

typedef curve_sweep<colour_sweep_traits_mc_r_circle_segment> mc_r_circle_segment_sweeper;

__exportF 
void SWEEP_SEGMENTS_MC(const list<r_circle_segment>& S, 
					   const map<r_circle_segment, unsigned long>& colour_map, 
					   GRAPH<r_circle_point,r_circle_segment>& G, 
					   bool embed)
{
	mc_r_circle_segment_sweeper sweeper;
	sweeper.set_colours(colour_map);
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(true);
    sweeper.run(S, G);
}

__exportF 
void SWEEP_SEGMENTS_MC(const list<r_circle_segment>& S, 
					   const map<r_circle_segment, unsigned long>& colour_map, 
					   GRAPH<r_circle_point,r_circle_segment>& G, 
					   edge_array<edge>& corresp_edge,
					   bool embed)
{
	mc_r_circle_segment_sweeper sweeper;
	sweeper.set_colours(colour_map);
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(true);
    sweeper.run(S, G, corresp_edge);
}

/// curve_sweep (r_circle_segment, coloured, sc) ////////////////////////////////////////////

typedef curve_sweep<colour_sweep_traits_sc_r_circle_segment> sc_r_circle_segment_sweeper;

__exportF 
void SWEEP_SEGMENTS_SC(const list<r_circle_segment>& S, 
					   const map<r_circle_segment, int>& colour_map, 
					   GRAPH<r_circle_point,r_circle_segment>& G, 
					   bool embed)
{
	sc_r_circle_segment_sweeper sweeper;
	sweeper.set_colours(colour_map);
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(true);
    sweeper.run(S, G);
}

__exportF 
void SWEEP_SEGMENTS_SC(const list<r_circle_segment>& S, 
					   const map<r_circle_segment, int>& colour_map, 
					   GRAPH<r_circle_point,r_circle_segment>& G, 
					   edge_array<edge>& corresp_edge,
					   bool embed)
{
	sc_r_circle_segment_sweeper sweeper;
	sweeper.set_colours(colour_map);
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(true);
    sweeper.run(S, G, corresp_edge);
}

/// curve_sweep (rat_segment) ///////////////////////////////////////////////////////////////

typedef curve_sweep<curve_sweep_traits_rat_segment> rat_segment_sweeper;

__exportF 
void CURVE_SWEEP_SEGMENTS(const list<rat_segment>& S, 
						  GRAPH<rat_point,rat_segment>& G, 
						  bool embed)
{
	rat_segment_sweeper sweeper;
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(false);
    sweeper.run(S, G);
}

__exportF 
void CURVE_SWEEP_SEGMENTS(const list<rat_segment>& S, 
						  GRAPH<rat_point,rat_segment>& G, 
						  edge_array<edge>& corresp_edge,
						  bool embed)
{
	rat_segment_sweeper sweeper;
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(false);
    sweeper.run(S, G, corresp_edge);
}

__exportF 
void INTERSECT_SEGMENTS_BRUTE_FORCE(const list<rat_segment>& S, 
									GRAPH<rat_point,rat_segment>& G)
{
	rat_segment_sweeper sweeper;
    sweeper.run_brute_force(S, G);
}


/// curve_sweep (VISUAL DEBUGGING) //////////////////////////////////////////////////////////

#if defined(LEDA_CURVE_SWEEP_VISUAL_DEBUG)

typedef curve_sweep_trc_win<curve_sweep_traits_r_circle_segment> r_circle_segment_sweeper_trc_win;

__exportF 
void WIN_SWEEP_SEGMENTS(const list<r_circle_segment>& S, 
						GRAPH<r_circle_point,r_circle_segment>& G, 
						bool embed, window* w)
{
	r_circle_segment_sweeper_trc_win sweeper(w);
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(true);
	sweeper.run(S, G);
}

typedef curve_sweep_trc_win<curve_sweep_traits_rat_segment> rat_segment_sweeper_trc_win;

__exportF 
void WIN_SWEEP_SEGMENTS(const list<rat_segment>& S, 
						GRAPH<rat_point,rat_segment>& G, 
						bool embed, window* w)
{
	rat_segment_sweeper_trc_win sweeper(w);
	sweeper.set_embedding_flag(embed);
	sweeper.set_compactification_flag(false);
	sweeper.run(S, G);
}

#endif // #if defined(LEDA_CURVE_SWEEP_VISUAL_DEBUG)

LEDA_END_NAMESPACE 
