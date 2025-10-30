/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _planar_map.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/planar_map.h>

LEDA_BEGIN_NAMESPACE

planar_map::planar_map(const graph& G) : graph(G)
{ 
  if (!Is_Connected(G))
     LEDA_EXCEPTION(1,"planar_map::planar_map(G): G is not connected"); 

  if (!make_map()) 
     LEDA_EXCEPTION(1,"planar_map::planar_map(G): G is not bidirected"); 

  compute_faces(); 

  // check Euler's formula
  int f = number_of_faces();
  int n = number_of_nodes();
  int m = number_of_edges()/2;  // graph is bidirected !

  if (n-m+f != 2)
     LEDA_EXCEPTION(1,"planar_map::planar_map(G): genus(G) != 0");
}



void planar_map::init_face_entries() const
{ face f;
  forall_faces(f,*this) init_face_entry(f->data(0));
 }

void planar_map::copy_face_entries() const
{ face f;
  forall_faces(f,*this) copy_face_entry(f->data(0));
 }

void planar_map::clear_face_entries() const
{ face f;
  forall_faces(f,*this) clear_face_entry(f->data(0));
 }



node planar_map::new_node(const list<edge>& el)
{
  if (el.length() < 2)
      LEDA_EXCEPTION(1,"planar_map::new_node(el,i):  el.length() < 2."); 

  list_item it = el.first();

  edge e0 = el[it];

  it = el.succ(it);

  face f = adj_face(e0);

  edge e;
  forall(e,el)
  { if (adj_face(e) != f)
      LEDA_EXCEPTION(1,"planar_map::new_node: edges bound different faces."); 
   }

  e = el[it];

  it = el.succ(it);

  GenPtr face_inf = f->data(0);
  copy_face_entry(face_inf);

  edge x = new_edge(e0,e);
  face fx = adj_face(reversal(x));
  clear_face_entry(fx->data(0));
  fx->data(0) = face_inf;

  edge e1 = split_edge(x);

  while(it)
  { copy_face_entry(face_inf);
    e1 = new_edge(e1,el[it]);
    face fx = adj_face(reversal(e1));
    clear_face_entry(fx->data(0));
    fx->data(0) = face_inf;
    it = el.succ(it);
   }

  return source(e1);
}

LEDA_END_NAMESPACE

