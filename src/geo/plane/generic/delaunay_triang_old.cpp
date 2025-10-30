/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  delaunay_triang_old.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



LEDA_BEGIN_NAMESPACE 

void delaunay_triang::save_state(const POINT& p) const
{ if ( !check ) return;
  ofstream out("delaunay_error.aux");
  if (hull_dart != nil)
  { out << pos_source(cur_dart) << endl;
    out << pos_target(cur_dart) << endl;
    out << pos_source(hull_dart) << endl;
    out << pos_target(hull_dart) << endl;
    out << p << endl;
    out.flush();
  }

  write("delaunay_error.graph");
}

void delaunay_triang::save_state(const POINT& p, const edge& answer) const
{ if ( !check ) return;
  ofstream out("delaunay_error.aux");
  if (hull_dart != nil)
  { out << pos_source(cur_dart) << endl;
    out << pos_target(cur_dart) << endl;
    out << pos_source(hull_dart) << endl;
    out << pos_target(hull_dart) << endl;
    out << p << endl;
    out << pos_source(answer) << endl;
    out << pos_target(answer) << endl;
    out.flush();
  }

  write("delaunay_error.graph");
}



bool delaunay_triang::IS_NON_DIAGRAM_DART(edge e) const
{ edge r = reversal(e);
  
  // e1,e2,e3,e4: edges of quadrilateral with diagonal e
  edge e1 = face_cycle_succ(r);
  edge e3 = face_cycle_succ(e);

    // flip test
  POINT a = pos_source(e1);
  POINT b = pos_target(e1);
  POINT c = pos_source(e3);
  POINT d = pos_target(e3);

  if (left_turn(a,b,c) && left_turn(b,c,d) && left_turn(c,d,a)
      && left_turn(d,a,c) && cocircular(a,b,c,d) )
     return true;
  return false;
}

bool delaunay_triang::check_state(const string& location) const
{ 
  if ( !check ) return true;

  write("delaunay_error.graph_after_op");

  if ( !Is_Delaunay_Triangulation((*((GRAPH<POINT,int>*) this)),NEAREST) )
  { 
    cerr << "\n\nCheck_state was called at " << location;
    cerr << "\n\nThe situation before the call of " << location;
    cerr << "\nwas saved into files delaunay_error.graph";
    cerr << "\nand delaunay_error.aux.";
    cerr << "\n\nPlease send these files to ledares@mpi-sb.mpg.de.\n\n";

    return false;
 }

  // check hull_dart and cur_dart

  if ( (hull_dart == nil || cur_dart == nil)
       && (number_of_nodes() >= 2) )
  { cerr << "\nhull_dart or cur_dart contradicts number of nodes\n";
    
    cerr << "\n\nCheck_state was called at " << location;
    cerr << "\n\nThe situation before the call of " << location;
    cerr << "\nwas saved into files delaunay_error.graph";
    cerr << "\nand delaunay_error.aux.";
    cerr << "\n\nPlease send these files to ledares@mpi-sb.mpg.de.\n\n";

    return false;

  }     
  if ( hull_dart == nil ) return true;
  
  // check edge labels
  edge e;
  forall_edges(e,*this)
  { switch ( inf(e)) {

    case HULL_DART:
    { edge next = face_cycle_succ(e);
      int orient = orientation(e,pos_target(next));
      if ( orient > 0 )
      { cerr << "\n\nwrongly labeled hull_dart.";
        
        cerr << "\n\nCheck_state was called at " << location;
        cerr << "\n\nThe situation before the call of " << location;
        cerr << "\nwas saved into files delaunay_error.graph";
        cerr << "\nand delaunay_error.aux.";
        cerr << "\n\nPlease send these files to ledares@mpi-sb.mpg.de.\n\n";

        return false;

      }
      break;
    }
          
    case DIAGRAM_DART:
    { if ( inf(reversal(e)) != HULL_DART &&
                          IS_NON_DIAGRAM_DART(e) )
      { cerr << "\n\nwrongly labeled diagram_dart.";
        
        cerr << "\n\nCheck_state was called at " << location;
        cerr << "\n\nThe situation before the call of " << location;
        cerr << "\nwas saved into files delaunay_error.graph";
        cerr << "\nand delaunay_error.aux.";
        cerr << "\n\nPlease send these files to ledares@mpi-sb.mpg.de.\n\n";

        return false;

      }
      break;
    }
          
    case NON_DIAGRAM_DART:
    { if ( inf(reversal(e)) != NON_DIAGRAM_DART ||
                          !IS_NON_DIAGRAM_DART(e) )
      { cerr << "\n\nwrongly labeled non_diagram_dart.";
        
        cerr << "\n\nCheck_state was called at " << location;
        cerr << "\n\nThe situation before the call of " << location;
        cerr << "\nwas saved into files delaunay_error.graph";
        cerr << "\nand delaunay_error.aux.";
        cerr << "\n\nPlease send these files to ledares@mpi-sb.mpg.de.\n\n";

        return false;

      }
      break;
    }


    default:  cerr << "\n\nillegal edge label.";
            
            cerr << "\n\nCheck_state was called at " << location;
            cerr << "\n\nThe situation before the call of " << location;
            cerr << "\nwas saved into files delaunay_error.graph";
            cerr << "\nand delaunay_error.aux.";
            cerr << "\n\nPlease send these files to ledares@mpi-sb.mpg.de.\n\n";

            return false;

    }
 }
                     
 if ( inf(hull_dart) != HULL_DART )
 { cerr << "\n\nis_hull_dart gives wrong information.";
   
   cerr << "\n\nCheck_state was called at " << location;
   cerr << "\n\nThe situation before the call of " << location;
   cerr << "\nwas saved into files delaunay_error.graph";
   cerr << "\nand delaunay_error.aux.";
   cerr << "\n\nPlease send these files to ledares@mpi-sb.mpg.de.\n\n";

   return false;

 }
 return true;
}


delaunay_triang::delaunay_triang()
{ nnh = 0;
  neh = 0;
  dnh = 0;
  deh = 0; 
  pre_mvh = 0;
  post_mvh = 0;
  mark_edge_handler = 0;
  cur_dart = nil; 
  hull_dart = nil;
  init_node_marks();
  check = false;
 }

delaunay_triang::delaunay_triang(const list<POINT>& S)
{ nnh = 0;
  neh = 0;
  dnh = 0;
  deh = 0; 
  pre_mvh = 0;
  post_mvh = 0;
  mark_edge_handler = 0;
  cur_dart = nil; 
  hull_dart = nil;
  init_node_marks();
  check = false;
  init(S);
 }

delaunay_triang& delaunay_triang::operator=(const delaunay_triang& T) 
{ nnh = 0;
  neh = 0;
  dnh = 0;
  deh = 0; 
  pre_mvh = 0;
  post_mvh = 0;
  mark_edge_handler = 0;
  GRAPH<POINT,int>::operator=(T); 
  init_hull(); 
  if (number_of_edges() > 0) 
     cur_dart = reversal(hull_dart); 
  else 
     cur_dart = nil;
  init_node_marks();
  check = false;
  return *this; 
} 
  

delaunay_triang::delaunay_triang(const GRAPH<POINT,int>& G) 
                                                   :GRAPH<POINT,int>(G)
{ nnh = 0;
  neh = 0;
  dnh = 0;
  deh = 0; 
  pre_mvh = 0;
  post_mvh = 0;
  mark_edge_handler = 0;
  edge e;
  forall_edges(e,*this) mark_edge(e,DIAGRAM_DART);
  init_hull(); 
  make_delaunay(); 
  if (number_of_edges() > 0) 
     cur_dart = reversal(hull_dart); 
  else 
     cur_dart = nil;
  init_node_marks();
  check = false;
 }

delaunay_triang::delaunay_triang(const delaunay_triang& T):GRAPH<POINT,int>(T)
{ nnh = 0;
  neh = 0;
  dnh = 0;
  deh = 0; 
  pre_mvh = 0;
  post_mvh = 0;
  mark_edge_handler = 0;
  init_hull(); 
  if (number_of_edges() > 0) 
     cur_dart = reversal(hull_dart); 
  else 
     cur_dart = nil;
  init_node_marks();
  check = false;
}



/*

void delaunay_triang::init(const list<POINT>& L) 
{ clear();
  hull_dart = cur_dart = nil;
  if (!L.empty())
  { DELAUNAY_TRIANG(L,*this);
    init_hull();
    cur_dart = reversal(hull_dart);
   }
  init_node_marks();
}

*/

void delaunay_triang::init(const list<POINT>& L0) 
{ 
  // construct a triangulation for the points in L0

  clear();

  if (L0.empty()) return;

  list<POINT> L = L0;

  L.sort();  

  // initialize graph with a single edge starting at the first point

  POINT last_p = L.pop();          // last visited point
  node  last_v = new_node(last_p); // last inserted node

  while (!L.empty() && last_p == L.head()) L.pop();

  if (L.empty()) return;

  last_p = L.pop();
  node v = new_node(last_p);
  edge x = new_edge(last_v,v);
  edge y = new_edge(v,last_v);
  set_reversal(x,y);
  last_v = v;


  // scan remaining points

  POINT p;
  forall(p,L) 
  { if (p == last_p) continue; 

    edge e =  last_adj_edge(last_v);

    last_v = new_node(p);
    last_p = p;

    // walk up to upper tangent
    do e = face_cycle_pred(e); while (orientation(e,p) > 0);

    // walk down to lower tangent and triangulate
    do { e = face_cycle_succ(e);
         edge x = new_edge(e,last_v);
         edge y = new_edge(last_v,source(e));
         set_reversal(x,y);
       } while (orientation(e,p) > 0);
   }

  // mark edges of convex hull

  hull_dart = last_edge();
  
  cur_dart = reversal(hull_dart);
 
  edge e0 = hull_dart;
  edge e  = e0;
  do { mark_edge(e,HULL_DART);
       e = face_cycle_succ(e);
     } while (e != e0); 

  make_delaunay();

  init_node_marks();
}



void delaunay_triang::init_hull()
{ 
  hull_dart = nil;

  edge e;
  forall_edges(e,*this)
  { if ( orientation(e,pos_target(face_cycle_succ(e))) <= 0 ) 
    { hull_dart = e;
      break;
    }
  } 

  if (hull_dart)
  { edge e = hull_dart;
    do { mark_edge(e,HULL_DART);
         e = face_cycle_succ(e);
       } 
    while (e != hull_dart);
  }
}


void delaunay_triang::make_delaunay()
{ list<edge> S = all_edges();
  make_delaunay(S);
}


void delaunay_triang::make_delaunay(list<edge>& S)
{
  // Transforms graph into a Delaunay delaunay_triang by flipping edges.
  // Diagonals of co-circular convex quadrilaterals are marked as 
  // NON_DIAGRAM_DART
  // We maintain a stack $S$ of edges containing diagonals which might
  // have to be flipped. 

  if (number_of_nodes() <= 3) return;

  while ( !S.empty() )
  { edge e = S.pop();
    edge r = reversal(e);

    if (is_hull_dart(e) || is_hull_dart(r)) continue;

    mark_edge(e,DIAGRAM_DART);
    mark_edge(r,DIAGRAM_DART);

    // e1,e2,e3,e4: edges of quadrilateral with diagonal e
    edge e1 = face_cycle_succ(r);
    edge e3 = face_cycle_succ(e);

    // flip test
    POINT a = pos_source(e1);
    POINT b = pos_target(e1);
    POINT c = pos_source(e3);
    POINT d = pos_target(e3);

    if (left_turn(b,d,a) && right_turn(b,d,c))
    { // the quadrilateral is convex
      int soc = side_of_circle(a,b,c,d);

      if (soc == 0) // co-circular quadrilateral(a,b,c,d) 
      { mark_edge(e,NON_DIAGRAM_DART);
        mark_edge(r,NON_DIAGRAM_DART);
       }
      if (soc > 0) // flip
      { edge e2 = face_cycle_succ(e1);
        edge e4 = face_cycle_succ(e3);
  
        S.push(e1); 
        S.push(e2); 
        S.push(e3); 
        S.push(e4); 
  
        // flip diagonal
        move_edge(e,e2,source(e4));
        move_edge(r,e4,source(e2));
      }
    }
  }

}


list<POINT> delaunay_triang::points() const
{ list <POINT> L;
  node v;
  forall_nodes(v,*this) L.append(pos(v));
  return L;
}

edge delaunay_triang::d_face_cycle_succ(edge e) const
{ e = reversal(e);
  do e = cyclic_adj_pred(e); 
     while (!is_diagram_dart(e));
  return e;
}

edge delaunay_triang::d_face_cycle_pred(edge e) const
{ do e = cyclic_adj_succ(e); 
     while (!is_diagram_dart(e));
  return reversal(e);
}

#define IN_HALFSPACE(e,p)\
((pos_target(e) - pos_source(e)) * (p - pos_source(e)) >= 0 )


void delaunay_triang::check_locate(edge answer, const POINT& p) const
{ 
  if (answer == nil && dim() < 1) return;

  if ( seg(answer).contains(p) && ( is_hull_dart(answer)
       || (!is_hull_dart(answer) && !is_hull_dart(reversal(answer)) ))) 
  return;

  if (orientation(answer,p) < 0) 
  { cerr << "\norientation(" << seg(answer) << "," << p << ") < 0.";
    goto error_in_locate;
  }

  if ( is_hull_dart(answer) && orientation(answer,p) > 0 ) return;

  if (dim() == 1)
  { // orientation = 0 and answer does not contain p; so beyond extreme point
    edge e = face_cycle_succ(answer);
    if ( e == reversal(answer) && !IN_HALFSPACE(e ,p) )
       return;
    else 
    { cerr << "\n\ndim = 1 error.";
      goto error_in_locate;
    }
  }

  // dim == 2: answer must not be a hull edge and triangle must contain p 
  if ( orientation(answer,p) > 0 &&
       orientation(face_cycle_succ(answer), p) > 0 &&
       orientation(face_cycle_pred(answer), p) > 0 )
     return;
  else
    { cerr << "\n\ndim = 2 error";
      goto error_in_locate;
    }

error_in_locate: 

  cerr << "\nAn error occurred in delaunay_triang::locate(point).";
  cerr << "\nI save the situation into the two files ";
  cerr << "\ndelaunay_error.graph and delaunay_error.aux.";
  cerr << "\n\nPlease send these files to leda@mpi-sb.mpg.de.\n\n";

  save_state(p,answer);
  exit(1);
}
 

edge delaunay_triang::locate(POINT p) const
{ 
  if (number_of_edges() == 0) return nil;

  if (dim() == 1)
  { 
    edge e = hull_dart;
    int orient = orientation(e,p);
    if (orient != 0)
    { if (orient < 0) e = reversal(e);
      if (check) check_locate(e ,p);
      return e;
    }

    // p is collinear with the points in S. We walk 
           
    if ( !IN_HALFSPACE(e,p) ) e = reversal(e);

    // in the direction of e. We know IN_HALFSPACE(e,p) 
         
    edge e1 = face_cycle_succ(e);
    while ( e1 != reversal(e) && IN_HALFSPACE(e1,p) ) 
    { e = e1;  
      e1 = face_cycle_succ(e); 
    }
         
    if (check) check_locate(e ,p);

    return e;
 }

   
  edge e = is_hull_dart(cur_dart) ? reversal(cur_dart) : cur_dart;

  if (p == pos_source(e) ) return reversal(e);
    
  int orient = orientation(e,p);

  if (orient == 0) 
  { e = face_cycle_pred(e);
    orient = orientation(e,p);
  }

  if (orient < 0) e = reversal(e);
   
  SEGMENT s(pos_source(e),p);

  while ( true )     
  { 
    if (is_hull_dart(e)) break;

    
    edge e1 = face_cycle_succ(e);
    edge e2 = face_cycle_pred(e);
    int d = ::orientation(s,pos_target(e1));
    edge e_next = reversal( (d < 0) ? e2 : e1 );
    int orient = orientation(e_next,p);
    if ( orient > 0 )  { e = e_next; continue; }
    if ( orient == 0 ) { e = e_next; break; }
    if ( d == 0 && orient < 0 && orientation(e2,p) == 0 ) 
      e = reversal(e2);
    break;

  }

  if (check) check_locate(e ,p);

  ((edge&)cur_dart) = e;
  return e;

}


node delaunay_triang::lookup(POINT p) const
{ if (number_of_nodes() == 1)
  { node v = first_node();
    return (pos(v) == p) ? v : nil;
  }
  edge e = locate(p);
  if (pos(source(e)) == p) return source(e);
  if (pos(target(e)) == p) return target(e);
  return nil;
}


node delaunay_triang::insert(POINT p)
{ if ( check ) save_state(p);

  node v; 

  
  if (number_of_nodes() == 0)  
  { v = new_node(p); 
                     if ( check && !check_state("delaunay_triang::insert") )   
                     { cerr << "The point inserted was " << p; 
                       exit(1);
                     }
                     return v;
 }

  if (number_of_nodes() == 1)
  { node w = first_node();
    if (p == pos(w)) 
    { assign(w,p); 
      v = w; 
      
      if ( check && !check_state("delaunay_triang::insert") )   
      { cerr << "The point inserted was " << p; 
        exit(1);
      }
      return v;

    }
    else
    { v = new_node(p);
      edge x = new_edge(v,w);
      edge y = new_edge(w,v);
      mark_edge(x,HULL_DART);
      mark_edge(y,HULL_DART);
      set_reversal(x,y);
      hull_dart = x;
      cur_dart = x;
      
      if ( check && !check_state("delaunay_triang::insert") )   
      { cerr << "The point inserted was " << p; 
        exit(1);
      }
      return v;

    }
  }


  edge e = locate(p);
  if (p == pos_source(e)) 
    { assign(source(e),p); return source(e); }
  if (p == pos_target(e)) 
    { assign(target(e),p); return target(e); } 

  bool p_on_e = seg(e).contains(p);


  if ( dim() == 1 && orientation(e,p) == 0 )
  { 
    v = new_node(p);
    edge x = new_edge(v,target(e));
    edge y = new_edge(target(e),v);
    mark_edge(x,HULL_DART);
    mark_edge(y,HULL_DART);
    set_reversal(x,y);

    if (p_on_e)
    { x = new_edge(v,source(e));  
      y = new_edge(source(e),v);
      mark_edge(x,HULL_DART);
      mark_edge(y,HULL_DART);
      set_reversal(x,y);
      hull_dart = cur_dart = x;

      del_edge(reversal(e));
      del_edge(e);
    }

    
    if ( check && !check_state("delaunay_triang::insert") )   
    { cerr << "The point inserted was " << p; 
      exit(1);
    }
    return v;

 }

  
  v  = new_node(p);
  edge e1 = e;
  edge e2 = e;
  list<edge> E;
  bool outer_face = is_hull_dart(e);

  if (outer_face) //  move e1/e2 to compute upper/lower tangents
  { do e1 = face_cycle_pred(e1); while (orientation(e1,p) > 0);
    do e2 = face_cycle_succ(e2); while (orientation(e2,p) > 0);
  }

  // insert edges between v and target(e1) ... source(e2)
  e = e1;
  do { e = face_cycle_succ(e);
       edge x = new_edge(e,v);
       edge y = new_edge(v,source(e));
       set_reversal(x,y);
       mark_edge(e,DIAGRAM_DART);
       E.append(e);
       E.append(x);
     } while (e != e2);

  if (outer_face) // mark last visited and new edges as hull edges
  { mark_edge(face_cycle_succ(e1),HULL_DART);
    mark_edge(face_cycle_pred(e2),HULL_DART);
    mark_edge(e2,HULL_DART);
    hull_dart = e2;
  }

  make_delaunay(E); // restores Delaunay property
  
  if ( check && !check_state("delaunay_triang::insert") )   
  { cerr << "The point inserted was " << p; 
    exit(1);
  }
  return v;


}


void delaunay_triang::del(node v)
{  
  if (v == nil) 
     LEDA_EXCEPTION(1,"delaunay_triang::del: nil argument.");

  if (number_of_nodes() == 0) 
     LEDA_EXCEPTION(1,"delaunay_triang::del: graph is empty.");

  if (check) save_state(inf(v));

  if ( dim() < 2 )
  { 
    if ( outdeg(v) == 2)
    { node s = target(first_adj_edge(v));
      node t = target(last_adj_edge(v));
      edge x = new_edge(s,t);  
      edge y = new_edge(t,s);
      mark_edge(x,HULL_DART);
      mark_edge(y,HULL_DART);
      set_reversal(x,y);
    }

    del_node(v);
    cur_dart = hull_dart = first_edge();    
  
    
    if ( check && !check_state("delaunay_triang::del(node v)") )
    { cerr << "deleted the node with position " << pos(v);
      exit(1);
    }
    return;

 }

 
 list<edge> E;

 int min_deg = 3;

 edge e;
 forall_adj_edges(e,v) 
 { E.append(face_cycle_succ(e));
   if (is_hull_dart(e)) min_deg = 2;
 }

 int count = 0;
 e = first_adj_edge(v);

 while ( outdeg(v) > min_deg && count < outdeg(v) )
 { edge e_pred = cyclic_adj_pred(e);
   edge e_succ = cyclic_adj_succ(e);
   POINT a = pos_target(e_pred); POINT c = pos_target(e_succ);

   if ( !right_turn(a,c,pos(v)) && right_turn(a,c,pos_target(e)) )
   { // e is flipable
     edge r = reversal(e);

     move_edge(e,reversal(e_succ),target(e_pred));
     move_edge(r,reversal(e_pred),target(e_succ),leda::before);

     mark_edge(e,DIAGRAM_DART);
     mark_edge(r,DIAGRAM_DART);
     E.append(e);

     e = e_pred;
     count = count - 2;    
     if ( count < 0 ) count = 0;
   }
   else
   { e = e_succ;
     count++;
   }
 }

 if ( min_deg == 2 )
 {   
   edge e,x;
   forall_adj_edges(e,v)
   { x = face_cycle_succ(e);
     mark_edge(x,HULL_DART);
     if ( !is_hull_dart(reversal(x)) ) 
       mark_edge(reversal(x),DIAGRAM_DART);
   }
   hull_dart = x;
 }

 cur_dart = E.head();
    
 del_node(v);
 make_delaunay(E);


 
 if ( check && !check_state("delaunay_triang::del(node v)") )
 { cerr << "deleted the node with position " << pos(v);
   exit(1);
 }
 return;

}


void delaunay_triang::del(POINT p)
{ node v = lookup(p);
  if ( v != nil ) del(v);
}



#include <LEDA/core/p_queue.h>

list<node> delaunay_triang::nearest_neighbors(POINT p, 
                                              int k) const
{ list<node> result;
  int n = number_of_nodes();

  if ( k <= 0 ) return result;
  if ( n <= k ) return all_nodes();

  // insert p and search neighbors graph starting at p

  node v = lookup(p);  
  bool old_node = true;
  if ( v == nil ) 
  { v = ((delaunay_triang*)this)->insert(p);
    old_node = false;
  } 

  unmark_all_nodes();
  
  p_queue<RAT_TYPE,node> PQ;

  PQ.insert(0,v); mark(v);

  while ( k > 0 )
  { pq_item it = PQ.find_min();
    node w = PQ.inf(it); PQ.del_item(it);
    if ( old_node || w != v) { result.append(w); k--; }
    node z;
    forall_adj_nodes(z,w)
    { if ( !is_marked(z) )
      { PQ.insert(p.sqr_dist(pos(z)),z);
        mark(z);
      }
    }
  }
    
  if ( !old_node ) ((delaunay_triang*)this)->del(v);

  return result;
} 


node delaunay_triang::nearest_neighbor(POINT p) const
{ 
  if (number_of_nodes() == 0) return nil;
  if (number_of_nodes() == 1) return first_node();

  edge e = locate(p);

  if ( is_hull_dart(e) ) 
  { while ( !IN_HALFSPACE(e,p) )          e = face_cycle_pred(e);
    while ( !IN_HALFSPACE(reverse(e),p) ) e = face_cycle_succ(e);
  }

  unmark_all_nodes(); 

  node min_v = source(e);
  RAT_TYPE min_d = p.sqr_dist(pos(min_v));

  list<node> L;
  L.append(source(e)); 
  L.append(target(e)); 
  mark_node(source(e));
  mark_node(target(e));

  while ( !L.empty() )
  { node v = L.pop();

    if ( p.sqr_dist(pos(v)) < min_d )
    { min_v = v;
      min_d = p.sqr_dist(pos(v));
    }

    forall_adj_edges(e,v)
    { node w = target(e);
      if ( !is_marked(target(e)) && 
           IN_HALFSPACE(e,p) && IN_HALFSPACE(reversal(e),p) && 
           supporting_line(e).sqr_dist(p) < min_d )
      { L.append(w); 
        mark_node(w);
      }
    }
  }

  return min_v;

}


void delaunay_triang::dfs(node s, const CIRCLE& C, list<node>& L) const
{ L.append(s);
  mark_node(s);
  node u;
  forall_adj_nodes(u,s)
      if (!is_marked(u) && ! C.outside(pos(u))) dfs(u,C,L);
}
 
 
list<node> delaunay_triang::range_search(const CIRCLE& C) const
{ 
  list<node> L;

  int orient = C.orientation();
  if (orient == 0)
      LEDA_EXCEPTION(1,"delaunay_triang::range_search: circle must be proper");

  if (number_of_nodes() == 0) return L;
  if ( number_of_nodes() == 1 && C.side_of(pos(first_node())) * orient >= 0 ) 
  { L.append(first_node());
    return L;
  }

  POINT p = C.center();
  node v = lookup(p);  
  bool new_v = false;

  if ( v == nil )
  { new_v = true;
    v = ((delaunay_triang*)this) -> insert(p); 
  }

  unmark_all_nodes(); 

  dfs(v,C,L);

  if (new_v)
  { L.pop();   
   ((delaunay_triang*)this) -> del(v);
  }
  return L;
}
 


list<node> delaunay_triang::range_search(const POINT& a, 
                                          const POINT& b,
                                          const POINT& c) const
{ int orient = ::orientation(a,b,c);
  CIRCLE C(a,b,c);
  list<node> L = range_search(C);
  list_item it = L.first_item();
  while (it != nil)
  { POINT p = pos(L[it]);
    list_item next_it = L.succ(it);
    if ( ::orientation(a,b,p) == - orient ||
         ::orientation(b,c,p) == - orient ||
         ::orientation(c,a,p) == - orient )      
       L.del_item(it);
    it = next_it;
  }
  return L;
}



list<node> delaunay_triang::range_search(const POINT& a, const POINT& c) const
{
  POINT b(c.xcoord(),a.ycoord());
  POINT d(a.xcoord(),c.ycoord());

  if (::orientation(a,b,c) < 0) 
  { POINT tmp = b;
    b = d;
    d = tmp;
   }

  CIRCLE C(a,b,c);

  list<node> L = range_search(C);
  list_item it = L.first_item();
  while (it != nil)
  { POINT p = pos(L[it]);
    list_item next_it = L.succ(it);
    if ( ::orientation(a,b,p) < 0 || ::orientation(b,c,p) < 0 ||
         ::orientation(c,d,p) < 0 || ::orientation(d,a,p) < 0 )
       L.del_item(it);
    it = next_it;
  }
  return L;
}
    

// from graph_alg.h
extern list<edge> MIN_SPANNING_TREE(const graph&, 
                                    int(*cmp)(const edge&,const edge&));
static const delaunay_triang* T_tmp;

static int cmp_edge_length(const edge& e1, const edge& e2)
{ RAT_TYPE l1 = T_tmp->pos_source(e1).sqr_dist(T_tmp->pos_target(e1)); 
  RAT_TYPE l2 = T_tmp->pos_source(e2).sqr_dist(T_tmp->pos_target(e2)); 
  return compare(l1,l2);
 }
  
list<edge> delaunay_triang::minimum_spanning_tree() const 
{ T_tmp = this; 
  return MIN_SPANNING_TREE(*this,cmp_edge_length);
}



void delaunay_triang::compute_voronoi(GRAPH<CIRCLE,POINT>& VD)
{ 
  VD.clear();

  if (number_of_nodes() < 2) return;

  // create Voronoi nodes

  edge_array<node> vnode(*this,nil);

  // for outer face

  edge  e = hull_dart;
  POINT a = pos_source(e);
  edge  x = e;
  do { POINT b = pos_target(x);
       vnode[x] =  VD.new_node(CIRCLE(a,midpoint(a,b),b));
       a = b;
       x = face_cycle_succ(x);
     } while ( x != e);

  // for all other faces

  forall_edges(e,*this)
  { 
    if (vnode[e] || !is_diagram_dart(e)) continue;

    edge  x = face_cycle_succ(e);
    POINT a = pos_source(e);
    POINT b = pos_target(e);
    POINT c = pos_target(x);
    node  v = VD.new_node(CIRCLE(a,b,c));

    x = e;
    do { vnode[x] = v;
         x = d_face_cycle_succ(x);
        } while( x != e);
   }

  // construct Voronoi edges

  // for outer face

  e = hull_dart;
  x = e;
  do { edge r = reversal(x); 
       POINT p = pos_target(x);
       VD.new_edge(vnode[x],vnode[r],p);
       x = cyclic_adj_pred(r);
     } while ( x != e);

  // for all other faces

  forall_edges(e,*this)
  { node v = vnode[e]; 

    if (!is_diagram_dart(e) || VD.outdeg(v) > 0) continue;

    POINT p = pos_target(e);
    edge x = e;
    do { edge r = reversal(x); 
         VD.new_edge(v,vnode[r],p);
         x = d_face_cycle_succ(x);
       } while (x != e);
   }
   // add a check for correctness
}


void delaunay_triang::draw_nodes(void (*draw_node)(const POINT&)) 
{ node v;
  forall_nodes(v,*this) draw_node(pos(v)); 
 }


void delaunay_triang::draw_edge(edge e,
                      void (*draw_diagram_edge)(const POINT&,const POINT&),
                      void (*draw_triang_edge) (const POINT&,const POINT&),
                      void (*draw_hull_dart)   (const POINT&,const POINT&))
{ 
    node v = source(e);
    node w = target(e);

    if (index(v) > index(w))
    { node tmp = v;
      v = w;
      w = tmp;
     }

    POINT p = pos(v);
    POINT q = pos(w);
    if (is_hull_dart(e) || is_hull_dart(reversal(e)))
       draw_hull_dart(p,q);
    else
       if (is_diagram_dart(e))
          draw_diagram_edge(p,q);
       else
          draw_triang_edge(p,q);
}



void delaunay_triang::draw_edges(
                      void (*draw_diagram_edge)(const POINT&,const POINT&),
                      void (*draw_triang_edge) (const POINT&,const POINT&),
                      void (*draw_hull_dart)   (const POINT&,const POINT&))
{ edge e;
  forall_edges(e,*this) 
    if (index(source(e)) < index(target(e)))
       draw_edge(e,draw_diagram_edge,draw_triang_edge,draw_hull_dart);
}

void delaunay_triang::draw_edges(const list<edge>& L,
                      void (*draw_edge)(const POINT&,const POINT&) )
{ edge e;
  forall(e,L) 
    draw_edge(pos_source(e),pos_target(e));
}


void delaunay_triang::draw_hull(void (*draw_poly)(const list<POINT>&))
{ list<POINT> CH;
  edge e = get_hull_dart();
  if (e != nil)
  { edge x = e;
    do { POINT p = pos(source(x));
         CH.append(p);
         x = face_cycle_succ(x);
       } while (x != e);
    draw_poly(CH);
   }
 }


void  delaunay_triang::draw_voro_edge(const CIRCLE& c1, const CIRCLE& c2,
                                 void (*draw_edge)(const POINT&,const POINT&),
                                 void (*draw_ray) (const POINT&,const POINT&))
{
  int orient1 = c1.orientation();
  int orient2 = c2.orientation();

  if (orient1 == 0 && orient2 == 0)
    { POINT a = c1.point1();
      POINT b = c1.point3();
      POINT c = midpoint(a,b);
      draw_ray(c,b.rotate90(c));
      draw_ray(c,a.rotate90(c));
     }
  else
    if (orient1 == 0)
       { POINT cw = c2.center();
         POINT p1 = c1.point1();
         POINT p3 = c1.point3();
         VECTOR vec = p3 - p1;
         POINT cv  = cw + vec.rotate90();
         draw_ray(cw,cv);
        }
    else
       if (orient2 == 0)
         { POINT cv = c1.center();
           POINT p1 = c2.point1();
           POINT p3 = c2.point3();
           VECTOR vec = p3 - p1;
           POINT cw  = cv + vec.rotate90();
           draw_ray(cv,cw);
          }
       else
         draw_edge(c1.center(),c2.center());
}


void  delaunay_triang::draw_voro_edges(
                       void (*draw_edge)(const POINT&,const POINT&),
                       void (*draw_ray) (const POINT&,const POINT&))
{ 
  if (number_of_edges() == 0) return;

  CIRCLE C_NULL;

  edge_array<CIRCLE> vnode(*this,C_NULL);

  // for outer face

  edge  e = hull_dart;

  POINT a = pos_source(e);
  edge  x = e;
  do { POINT b = pos_target(x);
       vnode[x] =  CIRCLE(a,midpoint(a,b),b);
       a = b;
       x = face_cycle_succ(x);
     } while ( x != e);


  forall_edges(e,*this)
  { 
    if (!identical(vnode[e],C_NULL) || !is_diagram_dart(e)) continue;

    edge  x = face_cycle_succ(e);
    POINT a = pos_source(e);
    POINT b = pos_target(e);
    POINT c = pos_target(x);

    CIRCLE C(a,b,c);

    x = e;
    do { vnode[x] = C;
         x = d_face_cycle_succ(x);
        } while( x != e);
   }

  forall_edges(e,*this) 
  {  if (identical(vnode[e],C_NULL)) continue;
     edge r = reversal(e);
     draw_voro_edge(vnode[e],vnode[r],draw_edge,draw_ray);
     vnode[r] = C_NULL;
   }
}



void  delaunay_triang::draw_voro(const GRAPH<CIRCLE,POINT>& VD,
                                 void (*draw_site)   (const POINT&), 
                                 void (*draw_edge)(const POINT&,const POINT&),
                                 void (*draw_ray) (const POINT&,const POINT&))
{ 
  edge_array<bool> visited(VD,false); 

  node v;
  forall_nodes(v,*this) draw_site(pos(v));

  edge e;
  forall_edges(e,VD)
  { if (visited[e]) continue;
    visited[e] = visited[VD.reversal(e)] = true;
    node v = source(e);
    node w = target(e);
    draw_voro_edge(VD[v],VD[w],draw_edge,draw_ray);
  }
}


LEDA_END_NAMESPACE 
