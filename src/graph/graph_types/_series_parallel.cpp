/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _series_parallel.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#include <LEDA/graph/series_parallel.h>

//------------------------------------------------------------------------------
// recognition and decomposition of series-parallel digraph 
// using the algorithm by Valdes, Tarjan, and Lawler,       
// using the canonical decomposition tree                  
//                                                        
// David Alberts, 1997                                   
//
//
// Generators for series-parallel graph
//
// Jens Loewe, 1999
//------------------------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

void spq_node_struct::clear()
{ spq_node s; forall(s,children) s->clear();
  children.clear();
  delete this;
}

ostream& operator<< (ostream& out, spq_node s)
{
  if(!s) return out;

  char t = s->type;
  out << "("; 
  switch(t)
  {
    case 'Q':
    {
      out << index(source(s->e)) << " " << index(target(s->e)) << ")";
      break;
    }
    case 'S':
    case 'P':
    {
      out << t;
      spq_node spq;
      forall(spq,s->children) out << " " << spq;
      out << ")";
      break;
    }
    case 'I':
    {
      out << "I)";
      break;
    }
    default:
    {
      cerr << "malformed spq_node: unknown type\n";
      out << "?)";
      break;
    }
  }
  return out;
}

bool spq_tree::get_st(const graph& G, node& s, node& t)
// compute the single source and the single
// target of G and return true.
// If G does not have exactly one source and
// exactly one target, return false.
{
  s = t = 0;
  bool not_too_many = true;

  node v;
  forall_nodes(v,G)
  {
    if(!G.indeg(v))
    {
      if(!s) s = v;
      else
      {
        not_too_many = false;  // too many sources
        break;
      }
    }
    if(!G.outdeg(v))
    {
      if(!t) t = v;
      else
      {
        not_too_many = false;  // too many targets
        break;
      }
    }
  }

  if(not_too_many && (s && t)) return true;
  else
  {
    s = t = 0;
    return false;
  }
}

void spq_tree::parallel_reduction(edge e, edge f)
// performs a parallel reduction of the edges e and f
{
  spq_node s;
  spq_node e_label = GC[e];
  spq_node f_label = GC[f];
  if((e_label->get_type() == 'P') && (f_label->get_type() == 'P'))
  {
    forall(s,f_label->children) e_label->children.append(s);
  }
  if((e_label->get_type() == 'P') && (f_label->get_type() != 'P'))
  {
    e_label->children.append(f_label);
  }
  if((e_label->get_type() != 'P') && (f_label->get_type() == 'P'))
  {
    f_label->children.push(e_label);
    GC[e] = f_label;
  }
  if((e_label->get_type() != 'P') && (f_label->get_type() != 'P'))
  {
    s = new spq_node_struct(false,e_label,f_label);
    GC[e] = s;
  }
  GC.del_edge(f);
}


edge spq_tree::series_reduction(edge e, edge f, node u, node v, node w)
// performs a series reduction of the edges e=(u,v) and f=(v,w)
{
  edge d;
  spq_node s=0;
  spq_node e_label = GC[e];
  spq_node f_label = GC[f];
  if((e_label->get_type() == 'S') && (f_label->get_type() == 'S'))
  {
    forall(s,f_label->children) e_label->children.append(s);
    s = e_label;
  }
  if((e_label->get_type() == 'S') && (f_label->get_type() != 'S'))
  {
    e_label->children.append(f_label);
    s = e_label;
  }
  if((e_label->get_type() != 'S') && (f_label->get_type() == 'S'))
  {
    f_label->children.push(e_label);
    s = f_label;
  }
  if((e_label->get_type() != 'S') && (f_label->get_type() != 'S'))
  {
    s = new spq_node_struct(true,e_label,f_label);
  }
  d = GC.new_edge(u,w,s);
  GC.del_edge(e);
  GC.del_edge(f);
  GC.del_node(v);
  return d;
}

void spq_tree::build_tree_rec(spq_node s, GRAPH<string,int>& G, node last)
{
  node v;
  if(s->get_type() == 'Q')
  {
    edge e = s->get_edge();
    node src = source(e);
    node tar = target(e);
    v = G.new_node(string("(%d %d)",index(src),index(tar)));
  }
  else
  {
    v = G.new_node(string("%c",s->get_type()));
    spq_node p;
    forall(p,s->children) build_tree_rec(p,G,v);
  }
  G.new_edge(last,v);
}

node spq_tree::build_tree(GRAPH<string,int>& G)
{
  if(!root) return 0;

  node dummy; // dummy node serves as first "last" node in build_tree_rec
  G.clear();
  dummy = G.new_node();
  build_tree_rec(root, G, dummy);
  node G_root = target(G.first_adj_edge(dummy));
  G.del_node(dummy);
  return(G_root);
}

void spq_tree::init(const graph& G)
{
  node v,w;
  edge e;
  spq_node s;

  the_is_series_parallel = false;
  if(root) root->clear();

  // determine source and target of input graph
  node so,to;
  if(!get_st(G,so,to)) return;

  // --- produce a copy of the input graph --- //
  GC.clear();
  node_array<node> nodes(G);
  forall_nodes(v,G) nodes[v] = GC.new_node(v);
  forall_edges(e,G)
  {
    s = new spq_node_struct(e);
    v = nodes[source(e)];
    w = nodes[target(e)];
    GC.new_edge(v,w,s);
  }

  node sc = nodes[so];                   // source of copied graph
  node tc = nodes[to];                   // target of copied graph

  stack<node> unsatisfied;               // "list" of nodes to process

  forall_nodes(v,GC)
    if((v != sc) && (v != tc)) unsatisfied.push(v);

  sp_reduce(sc,tc,unsatisfied);
}



void spq_tree::sp_reduce(node s, node t, stack<node>& unsatisfied)
{
  node u,v,w;
  edge d,e,f;

  node_array<bool> on_the_list(GC,true); // unsatisfied nodes
  on_the_list[s] = on_the_list[t] = false;

  while(!unsatisfied.empty())                     // general step
  {
    v = unsatisfied.pop();                        // (a)
    on_the_list[v] = false;

    if(GC.indeg(v) > 1)                           // (b) incoming edges
    {
      e = GC.first_in_edge(v);
      u = source(e);
      for(f=GC.in_succ(e), d=0; f && (source(f) == u); f=d)
      {
        d = GC.in_succ(f);
        parallel_reduction(e,f);
      }
    }

    if(GC.outdeg(v) > 1)                          // (c) outgoing edges
    {
      e = GC.first_adj_edge(v);
      w = target(e);
      for(f=GC.adj_succ(e), d=0; f && (target(f) == w); f=d)
      {
        d = GC.adj_succ(f);
        parallel_reduction(e,f);
      }
    }

    if((GC.indeg(v) == 1) && (GC.outdeg(v) == 1)) // (d) 1 in 1 out?
    {
      e = GC.first_in_edge(v);
      u = source(e);
      f = GC.first_adj_edge(v);
      w = target(f);

      d = series_reduction(e,f,u,v,w);         // (i)

      if((!on_the_list[u]) && (u != s))           // (ii)
      {
        unsatisfied.push(u);
        on_the_list[u] = true;
      }

      if((!on_the_list[w]) && (w != t))           // (iii)
      {
        unsatisfied.push(w);
        on_the_list[w] = true;
      }
    }

  } // end of general step

  if(GC.number_of_nodes() != 2)  // <-- change for more general setup
  {
    the_is_series_parallel = false;
  }
  else
  {
    e = GC.first_adj_edge(s);
    if(!e)
    {
      LEDA_EXCEPTION(1,"ARRRGH! There should have been an edge !");
    }
    for(f=GC.adj_succ(e), d=0; f; f=d)          // look for parallel edges
    {
      d = GC.adj_succ(f);
      parallel_reduction(e,f);
    }
    the_is_series_parallel = true;
    root = GC[GC.first_adj_edge(s)];
  }
}



//------------------------------------------------------------------------------
// Generators for series-parallel graph
//------------------------------------------------------------------------------


void g_add_spqnode(graph& G, spq_node n, node s, node t)
{


  switch(n->type)
    {
    case 'I':
    case 'Q':
      n->e = G.new_edge(s, t);
      n->type = 'Q';
      break;
      
    case 'P':
     { spq_node child;
       forall(child, n->children) g_add_spqnode(G, child, s, t);
       break;
      }

    case 'S':
      { spq_node last = n->children.back();
        spq_node child;
        forall(child, n->children)
	{
	  if(child != last)
	    {
	      node tmp = G.new_node();
	      g_add_spqnode(G,child, s, tmp);
	      s = tmp;
	    }
	  else g_add_spqnode(G, child, s, t);
	}
        break;
      }
    }
}


static spq_node_struct* create_random_tree(int num_s, int num_p)
{
  random_source rnd;

  if(num_s == 0 && num_p == 0) return new spq_node_struct;
  else
    {
      char s_or_p = rnd(1, num_s + num_p) <= num_s ? 'S' : 'P';
      if(s_or_p == 'S' && num_s == 0) s_or_p = 'P';
      if(s_or_p == 'P' && num_p == 0) s_or_p = 'S';

      if(s_or_p == 'S') num_s--;
      else num_p--;

      int left_s = num_s / 2;
      int right_s = left_s;
      int left_p = num_p / 2;
      int right_p = left_p;

      if(left_s + right_s < num_s)
      { if(rnd(1, 100) < 50) left_s++;
	else right_s++;
       }
    
      if(left_p + right_p < num_p)
      { if(rnd(1, 100) < 50) left_p++;
	else right_p++;
       }

      return new spq_node_struct(s_or_p == 'S',
				 create_random_tree(left_s, left_p),
				 create_random_tree(right_s, right_p));
    }
}

   
 
bool random_sp_graph(graph& G, spq_tree& T, int nodes, int edges)
{
  if(nodes < 2 || nodes > edges + 1) return false;

  spq_tree t;

  int num_p = edges - nodes + 1; // number of parallel compositions
  int num_s = nodes - 2; // number of series compositions

  t.root = create_random_tree(num_s, num_p);
  t.the_is_series_parallel = true;

  g_add_spqnode(G, t.root, G.new_node(), G.new_node());

  T = t;
  return true;
}

LEDA_END_NAMESPACE
