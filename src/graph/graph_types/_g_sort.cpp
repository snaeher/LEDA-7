/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _g_sort.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/numbers/integer.h>
#include <LEDA/numbers/rational.h>
#include <LEDA/numbers/real.h>

//--------------------------------------------------------------------------
// sorting
//
// by S. Naeher  (1995)
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

static const graph_map<graph>*  NA;

static graph* GGG;


/*
static int array_cmp_nodes(const node& x, const node& y) 
{ return NA->cmp_entry(NA->array_read(x),NA->array_read(y)); }

static int array_cmp_edges(const edge& x, const edge& y) 
{ return NA->cmp_entry(NA->array_read(x),NA->array_read(y)); }
*/

static int int_array_cmp_nodes(const node& x, const node& y) 
{ return LEDA_COMPARE(int,NA->array_read(x),NA->array_read(y)); }

static int int_array_cmp_edges(const edge& x, const edge& y) 
{ return LEDA_COMPARE(int,NA->array_read(x),NA->array_read(y)); }

static int float_array_cmp_nodes(const node& x, const node& y) 
{ return LEDA_COMPARE(float,NA->array_read(x),NA->array_read(y)); }

static int float_array_cmp_edges(const edge& x, const edge& y) 
{ return LEDA_COMPARE(float,NA->array_read(x),NA->array_read(y)); }

static int double_array_cmp_nodes(const node& x, const node& y) 
{ return LEDA_COMPARE(double,NA->array_read(x),NA->array_read(y)); }

static int double_array_cmp_edges(const edge& x, const edge& y) 
{ return LEDA_COMPARE(double,NA->array_read(x),NA->array_read(y)); }

static int integer_array_cmp_nodes(const node& x, const node& y) 
{ return LEDA_COMPARE(integer,NA->array_read(x),NA->array_read(y)); }

static int integer_array_cmp_edges(const edge& x, const edge& y) 
{ return LEDA_COMPARE(integer,NA->array_read(x),NA->array_read(y)); }


static int rational_array_cmp_nodes(const node& x, const node& y) 
{ return LEDA_COMPARE(rational,NA->array_read(x),NA->array_read(y)); }

static int rational_array_cmp_edges(const edge& x, const edge& y) 
{ return LEDA_COMPARE(rational,NA->array_read(x),NA->array_read(y)); }


static int real_array_cmp_nodes(const node& x, const node& y) 
{ return LEDA_COMPARE(real,NA->array_read(x),NA->array_read(y)); }

static int real_array_cmp_edges(const edge& x, const edge& y) 
{ return LEDA_COMPARE(real,NA->array_read(x),NA->array_read(y)); }



static int graph_cmp_nodes(const node& x, const node& y)
{ return GGG->cmp_node_entry(x,y); }

static int graph_cmp_edges(const edge& x, const edge& y)
{ return GGG->cmp_edge_entry(x,y); }


void graph::sort_nodes(const list<node>& vl)
{ 
  if (vl.length() != number_of_nodes())
       LEDA_EXCEPTION(1,"graph::sort_nodes(list<node>): illegal node list");

  v_list.clear();

  node v;
  forall(v,vl) 
  { if (graph_of(v) != this)
       LEDA_EXCEPTION(1,"graph::sort_nodes(list<node>): illegal node list");
    v_list.append(v);
   }
 }



void graph::sort_nodes(int (*f)(const node&, const node&))
{ list<node> vl = all_nodes();
  vl.sort(f); 
  sort_nodes(vl);
}

void graph::sort_nodes(const leda_cmp_base<node>& cmp)
{ list<node> vl = all_nodes();
  vl.sort(cmp); 
  sort_nodes(vl);
}

void graph::sort_edges(const list<edge>& el)
{ 
  node v;
  edge e;

  if (el.length() != number_of_edges())
      LEDA_EXCEPTION(1,"graph::sort_edges(list<edge>): illegal edge list");


  // clear all adjacency lists
  forall_nodes(v,*this) v->clear_adj_lists();

#if GRAPH_REP != 2
  e_list.clear();
#endif

//forall(e,el)
  forall_rev(e,el)
  { 
    node v = source(e);
    node w = target(e);

    if (graph_of(v) != this)
        LEDA_EXCEPTION(1,"graph::sort_edges(list<edge>): edge not in graph");

#if GRAPH_REP != 2
  //e_list.append(e);
    e_list.push(e);
#endif

  //v->append_adj_edge(e,0,0);
    v->push_adj_edge(e,0,0);

    if (undirected)
     //w->append_adj_edge(e,0,1);
       w->push_adj_edge(e,0,1);
    else
     //w->append_adj_edge(e,1,1);
       w->push_adj_edge(e,1,1);
   }
 }


void graph::sort_edges(int (*f)(const edge&, const edge&))
{ list<edge> el = all_edges();
  el.sort(f);
  sort_edges(el);
 }

void graph::sort_edges(const leda_cmp_base<edge>& cmp)
{ list<edge> el = all_edges();
  el.sort(cmp);
  sort_edges(el);
 }


void graph::sort_nodes(const graph_map<graph>& A) 
{ NA = &A; 
  switch (A.elem_type_id()) {

  case INT_TYPE_ID: sort_nodes(int_array_cmp_nodes); 
                    break;
  case FLOAT_TYPE_ID:  
                    sort_nodes(float_array_cmp_nodes); 
                    break;
  case DOUBLE_TYPE_ID:  
                    sort_nodes(double_array_cmp_nodes); 
                    break;
  case INTEGER_TYPE_ID:  
                    sort_nodes(integer_array_cmp_nodes); 
                    break;
  case RATIONAL_TYPE_ID:  
                    sort_nodes(rational_array_cmp_nodes); 
                    break;
  case REAL_TYPE_ID:  
                    sort_nodes(real_array_cmp_nodes); 
                    break;
  default:
          LEDA_EXCEPTION(1,"G.sort_nodes(node_array<T>): T must be numerical.");
  }
 }


void graph::sort_edges(const graph_map<graph>& A) 
{ NA = &A; 
  switch (A.elem_type_id()) {

  case INT_TYPE_ID: sort_edges(int_array_cmp_edges); 
                    break;
  case FLOAT_TYPE_ID:  
                    sort_edges(float_array_cmp_edges); 
                    break;
  case DOUBLE_TYPE_ID:  
                    sort_edges(double_array_cmp_edges); 
                    break;
  case INTEGER_TYPE_ID:  
                    sort_edges(integer_array_cmp_edges); 
                    break;
  case RATIONAL_TYPE_ID:  
                    sort_edges(rational_array_cmp_edges); 
                    break;
  case REAL_TYPE_ID:  
                    sort_edges(real_array_cmp_edges); 
                    break;
  default:
          LEDA_EXCEPTION(1,"G.sort_edges(node_array<T>): T must be numerical.");
  }
 }


void graph::sort_nodes() 
{ GGG = this; 
  sort_nodes(graph_cmp_nodes); 
 }

void graph::sort_edges() 
{ GGG = this; 
  sort_edges(graph_cmp_edges); 
 }




// bucket sort

static int array_ord_node(const node& x) 
{ return LEDA_CONST_ACCESS(int,NA->array_read(x)); }

static int array_ord_edge(const edge& x) 
{ return LEDA_CONST_ACCESS(int,NA->array_read(x)); }


void graph::bucket_sort_nodes(int l, int h, int (*ord)(const node&))
{ list<node> vl = all_nodes();
  vl.bucket_sort(l,h,ord);
  sort_nodes(vl);
}

void graph::bucket_sort_edges(int l, int h, int (*ord)(const edge&))
{ list<edge> el = all_edges();
  el.bucket_sort(l,h,ord);
  sort_edges(el);
 }

void graph::bucket_sort_nodes(int (*ord)(const node&))
{ list<node> vl = all_nodes();
  vl.bucket_sort(ord);
  sort_nodes(vl);
}

void graph::bucket_sort_edges(int (*ord)(const edge&))
{ list<edge> el = all_edges();
  el.bucket_sort(ord);
  sort_edges(el);
 }


void graph::bucket_sort_nodes(const graph_map<graph>& A) 
{ NA = &A; 
  switch (A.elem_type_id()) {

  case INT_TYPE_ID: bucket_sort_nodes(array_ord_node); 
                    break;
  default:
    LEDA_EXCEPTION(1,"G.bucket_sort_nodes(node_array<T>): T must be integer.");
  }
 }


void graph::bucket_sort_edges(const graph_map<graph>& A) 
{ NA = &A; 
  switch (A.elem_type_id()) {

  case INT_TYPE_ID: bucket_sort_edges(array_ord_edge); 
                    break;
  default:
    LEDA_EXCEPTION(1,"G.bucket_sort_edges(edge_array<T>): T must be integer.");
  }
 }

LEDA_END_NAMESPACE
