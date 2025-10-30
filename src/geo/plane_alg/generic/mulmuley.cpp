/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  mulmuley.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/list.h>
#include <LEDA/graph/ugraph.h>
#include <LEDA/core/map.h>


LEDA_BEGIN_NAMESPACE

//----------------------------------------------------------------------------
// Segment Intersection
//
// Ulrike Bartuschka  (1996)
//----------------------------------------------------------------------------


static edge locate_seg(const GRAPH<POINT,SEGMENT>& G, const SEGMENT& seg, 
		       node v, bool& collinear)
{
  // Precondition : incident edges of v are ordered as their associated 
  // subsegments  appear in counterclockwise direction.
  // The first edge (|v|,|w|) is either an edge whose associated subsegment 
  // is directed from right to left and has the minimal slope or (if no such
  // edge exist) an edge whose associated  subsegment is directed vertically
  // downwards (at least a "frame edge" exist!)
  
  // case 1 : Only segments, that are not inserted at this time, start or end
  //          at node |v| (There is only a "frame edge" incident to v and we 
  //          have to follow this edge) 
  if( G.degree(v) == 1 )  return G.first_adj_edge(v);
  

  POINT   p = G[v];
  edge    e = G.last_adj_edge(v);
  node    w = G.opposite(v,e); 
  int     o = orientation( seg, G[w] ); 
  
  // case 2 : The first incident edge of |v| is collinear to |seg|
  if (!o) 
    { 
      if( compare(G[w], p) > 0 )  collinear =  true;
      return e; 
    }
  
  // case 3 : The last incident edge of |v| lies below |seg|
  if ( o < 0 ) return e;
  
  // case 4 : Find the minimal incident edge of |v| that is collinear to |seg|
  //          or lies below |seg| and return this edge
  while( (o = orientation(seg, G[G.opposite(v, G.adj_pred(e,v))])) > 0 )
    e = G.adj_pred(e,v);
  
  e = G.adj_pred(e,v);
  
  if ( !o && compare(G[opposite(v,e)], p) > 0  )  collinear = true;
  
  return e;
}



void MULMULEY_SEGMENTS( const list<SEGMENT>& L, GRAPH<POINT,SEGMENT>& G, 
			bool embedded)
{
  list<POINT>        pl;
  SEGMENT            s;
  COORD              MINY(-1);
  
  G.clear();
  if (L.empty()) return;
  
  bool directed = G.is_directed();
  
  G.make_undirected();
  
  // *************************************************************************
  // initialize list |pl| with all endpoints of L, sort endpoints 
  // lexicographically and compute an lower bound for the y-coordinates of the
  // input segments
  
  forall(s,L) 
    { 
      if( s.start().ycoord() <= MINY ) MINY = s.start().ycoord() - 1;
      if( s.end().ycoord() <= MINY )   MINY = s.end().ycoord() - 1;
      pl.append(s.start()); pl.append(s.end()); 
    }
  pl.sort();
  
  
  // *************************************************************************
  // build frame for point set |pl|

  // all edges of the frame are labeled with a special frame segment
  // we can use it to identify attachements lateron
  SEGMENT    frame_seg;
  
  // map |V| associates each endpoint of the input segments with a node of |G|
  map<POINT, node>   V; 
  
  
  POINT       p = POINT( pl.head().xcoord(), MINY ); 
  node last_bot = G.new_node(p);      // last bottom frame node
  node last_v   = last_bot;           // last node
  edge e1, e2, e3, e4;
  
  forall(p,pl)
    {
      // Points with equal coordinates are associated with the same node
      if ( !compare( p, G[last_v]) ) 
	{ 
	  V[p] = last_v; 
	  continue; 
	}
      
      s = SEGMENT(p, G[last_v]);      
      if ( !s.is_vertical() )
	{ 
	  // insert a new "frame node" in to |G|
	  last_v = G.new_node(POINT( p.xcoord(), MINY ));
	  
	  // insert a horizontal 
	  G.new_edge(last_bot, G.last_adj_edge(last_bot), last_v, nil, 
		     frame_seg, leda::before, leda::behind );
	  
	  last_bot = last_v;
	}

      // create a node for point |p| and update map |V|
      node v = G.new_node(p);
      V[p] = v; 

      // insert a vertical
      G.new_edge(last_v, G.last_adj_edge(last_v), v, nil, frame_seg, 
		 leda::behind, leda::behind);
      
      last_v = v;
    }
  
  // *************************************************************************  // traverse intersected faces for each segment 
     

  forall(s, L) 
    {
      // |s| represents the input segment and |seg| is a copy of the input 
      // segment but directed from the lexicographically smaller endpoint to
      // the bigger one
      SEGMENT seg;
      
      if( compare( s.start(), s.end() ) <= 0 ) seg = s;
      else   seg = SEGMENT(s.end(), s.start());

      // |last_node| refers to the last inserted node associated with |s| 
      node last_node = V[seg.start()];

      // node associated with the "end" of |seg| 
      node end_node  = V[seg.end()];

      bool collinear, need_loc  = true;                         
      edge cur_edge=0, entry_edge=0;                   
      node cur_node=0, next_node=0;

      // travel trough G until |end_node| is reached
      while (last_node != end_node)     
	{ 
	  // At the beginning or if |seg| is crossing a previously inserted 
	  // node (point) we need to locate the next face that is intersected
	  // by |seg|
	  if (need_loc)                 
	    { 
	      need_loc = false; 
	      collinear = false;

	      // |loacte_seg| will locate the next intersected face f.
	      // It returns the edge that is incident to |last_node| and
	      // whose associated subsegment bounds f below |seg| 
              // (|collinear| = false )
	      // or
	      // it is collinear to |seg| (|collinear| = true)	      

	      entry_edge = locate_seg(G,seg,last_node, collinear);
	      
	      if ( !collinear )
		{ 
		  cur_node  = G.opposite(last_node, entry_edge);
		  cur_edge  = G.cyclic_adj_pred(entry_edge, cur_node);
		}
	      else
		{
		  cur_node = last_node;
		  cur_edge = entry_edge;
		}
	    }
	  
	  next_node = G.opposite(cur_node,cur_edge);
	  
	  int orient = 0; 
	  
	  // walk along the lower boundary of the current face until a side
	  // intersects |seg|
	  if (cur_node != last_node)  // not collinear !
	    while ((orient = orientation( seg, G[next_node])) < 0 ||
		   (!orient && seg.is_vertical() &&
		    compare(G[next_node], G[last_node]) < 0 ))
	      { 
		cur_edge = G.cyclic_adj_pred(cur_edge,next_node);
		cur_node = next_node;
		next_node = G.opposite(cur_node,cur_edge);
	      }
	  // *****************************************************************
	  // case 1 : |seg| intersects the current face in the point
	  //          associated with |next_node|
	  if (orient == 0) 
	    { 
	      // insert a new edge for |s| from |last_node| to |next_node|
              if (last_node == next_node)
              { string msg = "Error in Mulmuley: constructing selfloop.";
                #if KERNEL == FLOAT_KERNEL
                msg += "\\n Please use the rational geometry kernel.";
                #endif
                LEDA_EXCEPTION(999,msg);
               }
	      G.new_edge(last_node, entry_edge, next_node, cur_edge,
			 s, leda::behind, leda::before);
	      
	      if( identical(G[cur_edge],frame_seg) && seg.is_vertical() )
		G.del_edge(cur_edge);
	      
	      last_node = next_node;
	      need_loc  = true;  // location necessary in next step
	    }
	  // *****************************************************************
	  // case 2 : proper intersection
	  else 
	    { 
	      edge    next_edge = G.cyclic_adj_pred(cur_edge,cur_node);
	      edge    prev_edge;
	      SEGMENT cur_seg = G[cur_edge];
	      POINT   inter; 
	      bool    is_attachment = identical(cur_seg,frame_seg);
	      
	      // compute point of intersection :
	      if (is_attachment)
		{ 
		  POINT   tarc = G[G.target(cur_edge)];
		  SEGMENT att_seg = SEGMENT( POINT(tarc.xcoord(), MINY), tarc);
		  seg.intersection_of_lines(att_seg,inter);
		}
	      else seg.intersection_of_lines(cur_seg,inter);
	     
	      // insert new node for this intersection point in G :
	      node new_v = G.new_node(inter);
	      
	      // *************************************************************
	      // case 2.a : |seg| intersects a vertical segment of the frame
	      // 
	      if ( is_attachment )
		{
		  // insert a new edge for |s| from |last_node| to the new
		  // created node of this intersection point 
		  entry_edge =
		    G.new_edge(last_node,entry_edge,new_v,nil,s,leda::behind,
			       leda::before);
		  
		  // build an new frame segment from intersection point 
		  // vertical upwards to |G[next_node]|
		  G.new_edge(new_v,entry_edge,next_node,cur_edge,cur_seg,
			     leda::behind,leda::behind);
		  
		  last_node = new_v;
		  
		  // delete the old frame segment (|cur_edge|) from G
		  // if |cur_node| is a node of the frame delete it
		  if ( !V.defined(G[cur_node]) )
		    {
		      e3 = nil;
		      node prev_node = cur_node;
		      while ( G.degree(cur_node) > 2 &&
			      identical(
				  G[G.cyclic_adj_succ(cur_edge, cur_node)],
				  G[G.cyclic_adj_pred(cur_edge, cur_node)]) )
			{ 
                          // |cur_node| refers neither to a segment endpoint
			  // nor to an intersection point between segments
			  // in |L| 
			  e1  = G.cyclic_adj_pred(cur_edge, cur_node);
			  e2  = G.cyclic_adj_succ(cur_edge, cur_node);
			  next_node = G.opposite(cur_node, e1);
			  prev_node = G.opposite(cur_node, e2);
			  
			  next_edge = 
			    G.new_edge(prev_node, e2, next_node, e1, G[e1],
				       leda::before, leda::before);
			  if (!e3) e3 = next_edge;
			  G.del_edge(e1); G.del_edge(e2);
			}
		      if( e3 ) next_edge = e3; 
		      G.del_edge(cur_edge);
		      if ( !G.degree(cur_node) ) G.del_node(cur_node);
		      cur_node = prev_node; 
		    }
		  else // cur_node is associated with a segment endpoint
		    G.del_edge(cur_edge);
		}
	      // *************************************************************
	      // case 2.b : |seg| intersects a input segment
	      //
	      else  
		{
		  // intersection splits |cur_edge| into two parts
		  // insert a new edge for each part(-> |prev_edge| and |e1|)
		  // and delete |cur_edge|
		  
		  int c; // = compare( G[next_node], G[cur_node]);
		  
		  if ( next_node == G.target(cur_edge) ) c = 1;
		  else                                   c = 0;
		  if( c )
		    { 
		      // insert a new edge for |s| from |last_node| to the new
		      // created node of this intersection point 
		      G.new_edge(last_node,entry_edge,new_v,nil,s,
				 leda::behind, leda::behind);

		      e1 = G.new_edge(cur_node, cur_edge, new_v,nil, cur_seg,
				      leda::before, leda::behind);  
		      
		      prev_edge = G.new_edge(new_v, nil, next_node, cur_edge, 
					     cur_seg,
					     leda::behind, leda::before);
		    }
		  else
		    {
		      prev_edge = G.new_edge(next_node, cur_edge, new_v, nil, 
					     cur_seg, 
					     leda::before, leda::behind);
		      
		      // insert a new edge for |s| from |last_node| to the new
		      // created node of this intersection point 
		      G.new_edge(last_node, entry_edge, new_v, nil, s,
				 leda::behind, leda::behind);
		      
		      e1 = G.new_edge(new_v, nil, cur_node, cur_edge, cur_seg,
				      leda::behind, leda::before);
		    }
		  
		  entry_edge = e1;
		  last_node = new_v;
		  G.del_edge(cur_edge);
		  
		  // It might be the insected segment has collinears.
		  // All parallel edges of |cur_edge| must be splitted too.
		  while ( G.opposite(cur_node, next_edge) == next_node )
		    {
		      cur_edge  = next_edge;
		      cur_seg   = G[cur_edge];
		      next_edge = G.cyclic_adj_pred(next_edge, cur_node);
		      
		      prev_edge = G.new_edge( G.source(prev_edge), prev_edge,
					      G.target(prev_edge), prev_edge,
					      cur_seg,
					      c ? leda::before : leda::behind,
					      c ? leda::behind : leda::before);
		      
		      e1 = G.new_edge( G.source(e1), e1, G.target(e1), e1,
				       cur_seg, 
				       c ?  leda::before : leda::behind,
				       c ? leda::behind : leda::before);
		      
		      entry_edge = e1;
		      G.del_edge(cur_edge);
		    }
		}
	      
              // move current edge to next edge
	      cur_edge   = next_edge;
	    } // if (orient == 0) (else-part)
	  
	} // while (last_node != end_node)
      
    } // forall(s, L) 
  
  
  // delete rest of frame
  forall(p,pl)
    {
      node n1 = V[p];
      
      e1 = G.first_adj_edge(n1);
      
      while( e1 )
	{
	  e2 = G.adj_succ(e1, n1);
	  if ( identical( G[e1], frame_seg) )
	    {
	      node n2 = G.opposite(n1, e1);
	      
	      if( V.defined(G[n2]) ) G.del_edge(e1);
	      else  // intersection point or frame node
		if ( identical(G[G.cyclic_adj_succ(e1, n2)], frame_seg) )
		  G.del_node(n2);
		else 
		  {
		    while ( G.degree(n2) > 2 &&
			    identical(G[G.cyclic_adj_pred(e1, n2)],
				      G[G.cyclic_adj_succ(e1, n2)]) )
		      { 
		        e3 = G.cyclic_adj_pred(e1, n2);
			e4 = G.cyclic_adj_succ(e1, n2);
			node n3 = G.opposite(n2, e3);
			node n4 = G.opposite(n2, e4);
			
			G.new_edge(n4, e4, n3, e3, G[e3], 
				   leda::behind, leda::behind);
			G.del_edge(e3);
			G.del_edge(e4); 
		      }
		    G.del_edge(e1);
		    if ( !G.degree(n2) ) G.del_node(n2); 
		    break;
		  }   
	    } 
	  e1 = e2;
	}
    }
    
  if (directed)
    { 
      edge e, e1;
      list<edge> EL = G.all_edges();

      if (embedded)
	// computes a bidirected graph 
	// edges are sorted as before	  
	forall(e, EL) 
	{
	  e1 = G.new_edge( G.target(e), e, G.source(e), e, G[e],
			   leda::before );
	  G.set_reversal( e, e1 );
	}
      else
	// computes a directed graph 
	// edges are directed according to the associated input segment
	forall(e, EL)
	{ 
	  SEGMENT s = G[e];
	  if (compare(s.source(),s.target()) > 0) G.rev_edge(e);
	}
      G.make_directed();
    }
}




LEDA_END_NAMESPACE




