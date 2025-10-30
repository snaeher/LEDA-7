/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _g_inout.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// graph i/o
//
// S. Naeher  (last modified: December 1996)
//------------------------------------------------------------------------------


#include <LEDA/graph/graph.h>
#include <LEDA/system/stream.h>

LEDA_BEGIN_NAMESPACE

const char delim = '|';

void graph::write(string file_name) const
{ ofstream out(file_name);
  if (out.fail()) 
    LEDA_EXCEPTION(1,string("graph::write() cannot open file %s",~file_name));
  write(out);
}


void graph::write(ostream& out) const
{
  list<edge> E;

  node_array<int> A(*this,0);
  edge_array<int> B(*this,0);

  int n_count = 0;
  int e_count = 0;

  node v;
  forall_nodes(v,*this) 
  { A[v] = ++n_count;
    edge e;
    forall_adj_edges(e,v) 
    { if (source(e) != v) continue;  // ugraph
      B[e] = ++e_count;
      E.append(e);
     }
   }

  out << "LEDA.GRAPH" << endl;
  out << node_type()  << endl;
  out << edge_type()  << endl;
  
  if (!undirected) 
     out << -1 << endl;
  else 
     out << -2 << endl;
      
  out << n_count << endl;

  forall_nodes(v,*this)
  { out << delim << '{';
    write_node_entry(out,v->data(0));
    out << '}' << delim << endl;
   }

  out << e_count << endl;

  edge e;
  forall(e,E)
  { int s = A[source(e)];
    int t = A[target(e)];
    int r = (e->reversal()) ? B[e->reversal()] : 0;
    out << s << " " << t << " " << r << " " << delim << '{';
    write_edge_entry(out,e->data(0));
    out << '}' << delim << endl;
   }

 write_maps(out,io_map_list[0],io_map_list[1]);
}




int graph::read(string file_name)
{ ifstream in(file_name);
  if (in.fail())  return 1;
  return read(in);
}


static string read_data_string(istream& in)
{ 
  char  c;

  // for compatibility with old graph format

  do in.get(c); while (isspace(c) && c != '\n');
  if (c != delim)
  { in.putback(c);
    return read_line(in);
   }

  in.get(c);
  if (c != '{')  
    LEDA_EXCEPTION(1,"graph::read(): error in graph format.");

  const int buf_sz = 1024;
  char  buf[buf_sz];
  char* buf_p    = buf;
  char* buf_last = buf + buf_sz - 1;

  buf[0] = ' '; // dummy blank

  string data_str;
  int    level = 1;

  while (level > 0)
  { in.get(c);

    if (c == delim && *buf_p == '}') 
       level--;
    else
       if (c == '{' && *buf_p == delim) 
          level++;

    if (level == 0 || buf_p == buf_last)
    { if (level > 0) buf_p++;
      *buf_p = '\0';
      data_str += string(buf+1);
      buf_p = buf;
     }

    *++buf_p = c;
   }

  //in.get(c); // eat eol-character
  read_line(in);

  return data_str;
}


static int read_reversal(istream& in)
{ // returns index of reversal edge if present
  // 0 if not present 
  int r = 0;
  char  c;
  do in.get(c); while (isspace(c) && c != '\n');
  in.putback(c);
  if (isdigit(c)) in >> r;
  return r;
}


int graph::read(istream& in)
{ 
  clear();
  undirected = false;

  int result = 0;
  int n,i,v,w;

  string this_n_type = node_type();
  string this_e_type = edge_type();

  string d_type,n_type,e_type;

  in >> d_type;

  if (d_type != "LEDA.GRAPH") return 3;

  in >> n_type; 
  in >> e_type;

  in >> n;

  if (n < 0)
  { if (n <  -2) return 3;
    if (n == -2) undirected = true;
    in >> n;
  }

  read_line(in);

  node* A = new node[n+1];

  set_node_bound(n);

  if (this_n_type == "void" || n_type != this_n_type)  // do not read node info
    { for (i=1; i<=n; i++)
      { A[i] = new_node();
        read_data_string(in);
       }
      if (n_type != this_n_type) result = 2;   // incompatible node types
     }
  else
    if (this_n_type == "string")
      for (i=1; i<=n; i++)
      { A[i] = new_node(GenPtr(0));
        A[i]->data(0) = leda_copy(read_data_string(in));
       }
    else
      for (i=1; i<=n; i++)
      { A[i] = new_node(GenPtr(0));
        string data_str = read_data_string(in);
        //istringstream str_in(data_str.cstring(),data_str.length());
        string_istream str_in(data_str);
        read_node_entry(str_in,A[i]->data(0));
       }


  in >> n;       // number of edges

  edge* B = new edge[n+1];

  set_edge_bound(n);

  if (this_e_type == "void" || e_type != this_e_type) // do not read edge info
    { if (e_type != this_e_type) result = 2;   // incompatible edge types
      for (i=1; i<=n; i++)
      { in >> v >> w;
        edge e = new_edge(A[v],A[w]);
        int r = read_reversal(in);
        read_data_string(in);
        B[i] = e;
        if (r > 0 && r < i) set_reversal(e,B[r]);
      }
     }
  else
    if (this_e_type == "string")
      for (i=1; i<=n; i++)
      { in >> v >> w;
        edge e = new_edge(A[v],A[w],GenPtr(0));
        int r = read_reversal(in);
        e->data(0) = leda_copy(read_data_string(in));
        B[i] = e;
        if (r > 0 && r < i) set_reversal(e,B[r]);
       }
    else
      for (i=1; i<=n; i++)
      { in >> v >> w;
        edge e = new_edge(A[v],A[w],GenPtr(0));
        int r = read_reversal(in);
        string data_str = read_data_string(in);
        //istringstream str_in(data_str.cstring(),data_str.length());
        string_istream str_in(data_str);
        read_edge_entry(str_in,e->data(0));
        B[i] = e;
        if (r > 0 && r < i) set_reversal(e,B[r]);
       }

  delete[] A;
  delete[] B;

  read_maps(in,io_map_list[0],io_map_list[1]);

  return result;
}

static int id_off = 0;

void graph::print_node(node v,ostream& o) const
{ if (super() != 0)
  { super()->print_node(node(graph::inf(v)),o);
    return;
   }
  o << "[" << (v->id()+id_off) << "]";
  if (string::cmp(node_type(),"void"))
  { o << "(";
    write_node_entry(o,v->data(0));
    o << ")";
   }
}

void graph::print_edge(edge e,ostream& o) const
{ if (super() != 0)
  { super()->print_edge(edge(graph::inf(e)),o);
    return;
   }
  o <<   "[" << (source(e)->id()+id_off) << "]";
  o <<  ((undirected) ? "==" : "--");
  if (string::cmp(edge_type(),"void"))
  { o << "(";
    write_edge_entry(o,e->data(0));
    o << ")";
   }
  o <<  ((undirected) ? "==" : "-->");
  o << "[" << (target(e)->id()+id_off) << "]";
}


void graph::print(string s, ostream& out) const
{ node v;
  edge e;
  out << s << endl;
  forall_nodes(v,*this)
  { print_node(v,out);
    out << " : ";
    forall_adj_edges(e,v) print_edge(e,out);
    out << endl;
   }
  out << endl;
}


void graph::print_node1(node v,ostream& o) const
{ id_off = 1;
  print_node(v,o);
  id_off = 0;
}

void graph::print_edge1(edge e,ostream& o) const
{ id_off = 1;
  print_edge(e,o);
  id_off = 0;
}

void graph::print1(string s, ostream& out) const
{ id_off = 1;
  print(s,out);
  id_off = 0;
}



// convert node and edge entries into a string and vice versa

string graph::get_node_entry_string(node v) const
{ string_ostream out;
  write_node_entry(out,v->data(0));
  out << ends;
  //char* p = out.str();
  //string s(p);
  //delete[] p;
  return out.str();
 }

string graph::get_edge_entry_string(edge e) const
{ string_ostream out;
  write_edge_entry(out,e->data(0));
  out << ends;
  //char* p = out.str();
  //string s(p);
  //delete[] p;
  return out.str();
 }


void graph::set_node_entry(node v, string s)
{ clear_node_entry(v->data(0));
  if (string::cmp(node_type(),"string") == 0)
     v->data(0) = leda_copy(s);
  else
    { //istringstream in(s.cstring());
      string_istream in(s);
      read_node_entry(in,v->data(0));
     }
 }

void graph::set_edge_entry(edge e, string s)
{ clear_edge_entry(e->data(0));
  if (string::cmp(edge_type(),"string") == 0)
     e->data(0) = leda_copy(s);
  else
    { //istringstream in(s.cstring());
      string_istream in(s);
      read_edge_entry(in,e->data(0));
     }
 }




// map I/O


void graph::add_io_map(graph_map<graph>& m) 
{ io_map_list[m.get_kind()].append(&m); }

void graph::clear_io_maps() 
{ io_map_list[0].clear();  
  io_map_list[1].clear();  
  io_map_list[2].clear(); 
}


void graph::write_maps(ostream& out, const list<graph_map<graph>*>& nmap, 
                                     const list<graph_map<graph>*>& emap) const
{ 
  graph_map<graph>* p;

  forall(p,nmap)
  { node v;
    forall_nodes(v,*this) 
    { p->write_entry(out,p->array_access(v));
      out << endl; 
     }
   }

  forall(p,emap)
  { node v;
    forall_nodes(v,*this) 
    { edge e;
      forall_adj_edges(e,v) 
      { if (source(e) != v) continue;
        p->write_entry(out,p->array_access(e));
        out << endl;
       }
     }
   }
}


void graph::read_maps(istream& in, const list<graph_map<graph>*>& nmap, 
                                   const list<graph_map<graph>*>& emap) const
{ 
  int n = number_of_nodes();
  int m = number_of_edges();

  graph_map<graph>* p;

  forall(p,nmap)
  { p->init(this,n,0);
    node v;
    forall_nodes(v,*this) p->read_entry(in,p->array_access(v));
   }

  forall(p,emap)
  { p->init(this,m,1);
    edge e;
    forall_edges(e,*this) p->read_entry(in,p->array_access(e));
   }
}

LEDA_END_NAMESPACE
