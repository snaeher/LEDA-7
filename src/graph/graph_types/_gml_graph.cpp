/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _gml_graph.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------- //
// class gml_graph, parser for graph in GML format                   //
//                                                                    //
// by David Alberts (1997)                                            //
//------------------------------------------------------------------- //

#include<LEDA/graph/gml_graph.h>

LEDA_BEGIN_NAMESPACE

bool gml_graph::parse(const char* filename) 
{ return gml_parser::parse(filename); }

bool gml_graph::parse(istream& ins)   { return gml_parser::parse(ins); }

bool gml_graph::parse_string(string s)   { return gml_parser::parse_string(s); }


void gml_graph::init_rules()
{
  // graph rule
  append("graph");
  add_rule(new_graph,gml_list);

  // directed graph (default) or not?
  append("directed");
  add_rule(directed,gml_int);
  goback();

  // type of node parameter (optional)
  append("nodeType");
  add_rule(nodeType,gml_string);
  goback();

  // type of edge parameter (optional)
  append("edgeType");
  add_rule(edgeType,gml_string);
  goback();

  // new node
  append("node");
  add_rule(new_node,gml_list);

  // node index
  append("id");
  add_rule(node_index,gml_int);
  goback();

  // node parameter
  append("parameter");
  add_rule(node_param,gml_string);
  goback();

  goback();

  // new edge
  append("edge");
  add_rule(new_edge,gml_list);

  // edge source
  append("source");
  add_rule(edge_source,gml_int);
  goback();

  // edge target
  append("target");
  add_rule(edge_target,gml_int);
  goback();

  // edge parameter
  append("parameter");
  add_rule(edge_param,gml_string);

  reset_path();
}

void gml_graph::add_graph_rule(gml_graph_rule f, gml_value_type t, char* key)
{
  if(key)
  {
    reset_path();
    append("graph");
    append(key);
  }

  graph_rules[next_rule] = f;
  add_rule(next_rule,t);
  next_rule++;

  if(key) reset_path();
}

void gml_graph::add_node_rule(gml_node_rule f, gml_value_type t, char* key)
{
  if(key)
  {
    reset_path();
    append("graph");
    append("node");
    append(key);
  }

  node_rules[next_rule] = f;
  add_rule(next_rule,t);
  next_rule++;

  if(key) reset_path();
}

void gml_graph::add_edge_rule(gml_edge_rule f, gml_value_type t, char* key)
{
  if(key)
  {
    reset_path();
    append("graph");
    append("edge");
    append(key);
  }

  edge_rules[next_rule] = f;
  add_rule(next_rule,t);
  next_rule++;

  if(key) reset_path();
}


bool gml_graph::interpret(gml_rule r, const gml_object* gobj)
{
  bool ok = true;

  switch(r)
  {
    case new_graph:
    {
      ok = graph_intro(gobj);
      break;
    }
    case directed:
    {
      if(gobj->get_int()) the_graph->make_directed();
      else                the_graph->make_undirected();
      break;
    }
    case nodeType:
    {
      right_node_type = !string::cmp(the_graph->node_type(),gobj->get_string());
      break;
    }
    case edgeType:
    {
      right_edge_type = !string::cmp(the_graph->edge_type(),gobj->get_string());
      break;
    }
    case new_node:
    { node_count++;
      switch (node_count) {
        case 1: current_node = dummy1;
                break;
        case 2: current_node = dummy2;
                break;
        default:
                current_node = the_graph->new_node();
                break;
       }

      has_id = false;
      gml_node_rule f;
      forall(f,new_node_rules) ok = ok && (*f)(gobj,the_graph,current_node);
      break;
    }
    case node_index:
    {
      (*(node_by_id))[gobj->get_int()] = current_node;
      has_id = true;
      break;
    }
    case node_param:
    {
      the_graph->set_node_entry(current_node,string(gobj->get_string()));
      break;
    }
    case new_edge:
    {
      edge e = the_graph->new_edge(dummy1,dummy2);
      (*(edge_s))[e] = -1;
      (*(edge_t))[e] = -1;
      current_edge = e;
      gml_edge_rule f;
      forall(f,new_edge_rules) ok = ok && (*f)(gobj,the_graph,current_edge);
      break;
    }
    case edge_source:
    {
      (*(edge_s))[current_edge] = gobj->get_int();
      break;
    }
    case edge_target:
    {
      (*(edge_t))[current_edge] = gobj->get_int();
      break;
    }
    case edge_param:
    {
      the_graph->set_edge_entry(current_edge,string(gobj->get_string()));
      break;
    }
    default:
    {
      if(node_rules.defined(r))
      {
        ok = (*(node_rules[r]))(gobj,the_graph,current_node);
        break;
      }
      if(edge_rules.defined(r))
      {
        ok = (*(edge_rules[r]))(gobj,the_graph,current_edge);
        break;
      }
      if(graph_rules.defined(r))
      {
        ok = (*(graph_rules[r]))(gobj,the_graph);
        break;
      }
      break;
    }
  }

  return ok;
}

bool gml_graph::list_end(gml_rule r, const gml_object* gobj)
{
  bool ok = true;

  switch(r)
  {
    case new_graph:
    {
      ok = graph_end(gobj);
      break;
    }
    case new_node:
    {
      if(!has_id)
      {
        print_error(*gobj,"missing node id");
        ok = false;
      }

      gml_node_rule f;
      forall(f,node_done_rules) ok = ok && (*f)(gobj,the_graph,current_node);

      current_node = nil;

      break;
    }
    case new_edge:
    {
      ok = edge_end(gobj);
      break;
    }
    default:
    {
      break;
    }
  }

  return ok;
}


bool gml_graph::graph_intro(const gml_object* gobj)
{
  node_by_id = new map<int,node>;
  edge_s = new map<edge,int>;
  edge_t = new map<edge,int>;

  the_graph->clear();
  node_count = 0;
  dummy1 = the_graph->new_node();
  dummy2 = the_graph->new_node();

  right_node_type = true;
  right_edge_type = true;

  // call new graph rules
  bool ok = true;
  gml_graph_rule f;
  forall(f,new_graph_rules) ok = ok && (*f)(gobj,the_graph);

  return ok;
}


bool gml_graph::graph_end(const gml_object* gobj)
{
  bool ok = true;

  if(!right_node_type || !right_edge_type)
  {
    if(!right_node_type)
      print_error(*gobj,"wrong node type");
    if(!right_edge_type)
      print_error(*gobj,"wrong edge type");
    ok = false;
  }

  edge e;
  node s,t;
  // settle edges
  forall_edges(e,*the_graph)
  {
    s = t = 0;
    if((*(edge_s))[e] != -1)
      s = (*(node_by_id))[(*(edge_s))[e]];
    if((*(edge_t))[e] != -1)
      t = (*(node_by_id))[(*(edge_t))[e]];
    if(s && t) the_graph->move_edge(e,s,t);
  }

  if (node_count < 2) the_graph->del_node(dummy2);
  if (node_count < 1) the_graph->del_node(dummy1);

  // call graph done rules
  gml_graph_rule f;
  forall(f,graph_done_rules) ok = ok && (*f)(gobj,the_graph);

  if(node_by_id)
  {
    delete node_by_id;
    delete edge_s;
    delete edge_t;

    node_by_id = 0;
    edge_s = 0;
    edge_t = 0;
  }

  return ok;
}


bool gml_graph::edge_end(const gml_object* gobj)
{
  bool ok = true;

  if((*(edge_s))[current_edge] == -1)
  {
    print_error(*gobj,"edge without source");
    ok = false;
  }
  if((*(edge_t))[current_edge] == -1)
  {
    print_error(*gobj,"edge without target");
    ok = false;
  }

  gml_edge_rule f;
  forall(f,edge_done_rules) ok = ok && (*f)(gobj,the_graph,current_edge);

  current_edge = nil;

  return ok;
}

LEDA_END_NAMESPACE


