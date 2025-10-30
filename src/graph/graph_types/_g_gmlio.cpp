/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _g_gmlio.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// ---------------------------------------------------------------- //
// writing LEDA graph in GML format                                //
// reading LEDA graph in GML format                                //
//                                                                  //
// David Alberts (1996)                                             //
// David Alberts (1997) new version, accepts unknown lists, etc.    //
//                      no more lex/yacc                            //
// ---------------------------------------------------------------- //

#include<LEDA/graph/graph.h>
#include<LEDA/system/stream.h>
#include<LEDA/graph/gml_graph.h>

LEDA_BEGIN_NAMESPACE

bool graph::write_gml(string outfile,
                      void (*node_cb)(ostream&,const graph*, const node),
                      void (*edge_cb)(ostream&,const graph*, const edge)) const
// writes a graph description in GML format to outfile.
// If an error occurs, false is returned.
{
  file_ostream out(outfile);

  if(out.fail()) return false;
  else           return write_gml(out,node_cb,edge_cb);
}

bool graph::write_gml(ostream& out,
                      void (*node_cb)(ostream&,const graph*, const node),
                      void (*edge_cb)(ostream&,const graph*, const edge)) const
// writes a graph description in GML format to outfile.
// If an error occurs, false is returned.
{
  if(out.fail()) return false;

  string void_str("void");

  out << "Creator " << '"' << "LEDA write_gml" << '"' << "\n\n";
  out << "graph [\n\n";
  out << "  directed " << int(is_directed()) << "\n";
  out << "\n";

  node v;
  if((string(node_type()) != void_str) || node_cb)
  {
    forall_nodes(v,*this)
    {
      out << "  node [\n";
      out << "    id " << index(v) << "\n";
      if(string(node_type()) != void_str)
      {
        out << "    parameter " << '"' << get_node_entry_string(v);
        out << '"' << "\n";
      }
      if(node_cb) (*node_cb)(out,this,v);
      out << "  ]\n";
    }
  }
  else
    forall_nodes(v,*this) out << "  node [ id " << index(v) << " ]\n";

  out << "\n";

  edge e;
  if((string(edge_type()) != void_str) || edge_cb)
  {
    forall_edges(e,*this)
    {
      out << "  edge [\n";
      out << "    source " << index(source(e)) << "\n";
      out << "    target " << index(target(e)) << "\n";
      if(string(edge_type()) != void_str)
      {
        out << "    parameter " << '"' << get_edge_entry_string(e);
        out << '"' << "\n";
      }
      if(edge_cb) (*edge_cb)(out,this,e);
      out << "  ]\n";
    }
  }
  else
  {
    forall_edges(e,*this)
    {
      out << "  edge [ source " << index(source(e)) << " ";
      out << "target " << index(target(e)) << " ]\n";
    }
  }
  out << "\n]\n";

  return true;
}


bool graph::read_gml(string s)
{
  gml_graph* parser = new gml_graph(*this,s.cstring());
  bool ok = !parser->errors();
  delete parser;
  return ok;
}

bool graph::read_gml(istream& in)
{
  gml_graph* parser = new gml_graph(*this,in);
  bool ok = !parser->errors();
  delete parser;
  return ok;
}

LEDA_END_NAMESPACE
