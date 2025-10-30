/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _g_dimacs.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph.h>
#include <LEDA/graph/dimacs.h>
#include <LEDA/graph/graph.h>

LEDA_BEGIN_NAMESPACE

string Read_Dimacs_Graph(istream& in, graph& G, list<node>& L,
                                             node_array<string>& n_label,
                                             edge_array<string>& e_label)
{ 
  void* p = &e_label;
  edge_array<line_string>& e_line = *(edge_array<line_string>*)p;
  return read_dimacs_graph0(in,G,L,n_label,e_line,string("")); 
}


void Read_Dimacs_SP(istream& in, graph& G, node& s, edge_array<int>& cost)
{ read_dimacs_sp(in,G,s,cost); }

void Read_Dimacs_SP(istream& in, graph& G, node& s, edge_array<double>& cost)
{ read_dimacs_sp(in,G,s,cost); }



void Read_Dimacs_MF(istream& in, graph& G, node& s, node& t, 
                                                    edge_array<int>& cap)
{ read_dimacs_mf(in,G,s,t,cap); }

void Read_Dimacs_MF(istream& in, graph& G, node& s, node& t,
                                                    edge_array<double>& cap)
{ read_dimacs_mf(in,G,s,t,cap); }




void Read_Dimacs_MCF(istream& in, graph& G, node_array<int>& supply,
                                            edge_array<int>& lcap, 
                                            edge_array<int>& ucap,
                                            edge_array<int>& cost)
{ read_dimacs_mcf(in,G,supply,lcap,ucap,cost); }



void Read_Dimacs_MCF(istream& in, graph& G, node_array<double>& supply,
                                            edge_array<double>& lcap, 
                                            edge_array<double>& ucap,
                                            edge_array<double>& cost)
{ read_dimacs_mcf(in,G,supply,lcap,ucap,cost); }



void Read_Dimacs_MAT(istream& in, graph& G, edge_array<int>& cost)
{ read_dimacs_mat(in,G,cost); }

void Read_Dimacs_MAT(istream& in, graph& G, edge_array<double>& cost)
{ read_dimacs_mat(in,G,cost); }



void Write_Dimacs_SP(ostream& os, const graph& G, node s,
                                  const edge_array<int>& cost)
{ write_dimacs_sp(os,G,s,cost); }


void Write_Dimacs_SP(ostream& os, const graph& G, node s,
                                   const edge_array<double>& cost)
{ write_dimacs_sp(os,G,s,cost); }


void Write_Dimacs_MF(ostream& os, const graph& G, node s, node t,
                                                   const edge_array<int>& cap)
{ write_dimacs_mf(os,G,s,t,cap); }

void Write_Dimacs_MF(ostream& os, const graph& G, node s, node t,
                                                  const edge_array<double>& cap)
{ write_dimacs_mf(os,G,s,t,cap); }




void Write_Dimacs_MCF(ostream& os, const graph& G,
                                                  const node_array<int>& supply,
                                                  const edge_array<int>& lcap, 
                                                  const edge_array<int>& ucap,
                                                  const edge_array<int>& cost)
{ write_dimacs_mcf(os,G,supply,lcap,ucap,cost); }


void Write_Dimacs_MCF(ostream& os, const graph& G,
                                   const node_array<double>& supply,
                                   const edge_array<double>& lcap, 
                                   const edge_array<double>& ucap,
                                   const edge_array<double>& cost)
{ write_dimacs_mcf(os,G,supply,lcap,ucap,cost); }





void Write_Dimacs_MAT(ostream& os, const graph& G, const edge_array<int>& cost)
{ write_dimacs_mat(os,G,cost); }

void Write_Dimacs_MAT(ostream& os, const graph& G, const edge_array<double>& cost)
{ write_dimacs_mat(os,G,cost); }



LEDA_END_NAMESPACE
