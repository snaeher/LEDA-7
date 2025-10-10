/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _dimacs_graph0.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/dimacs_graph0.h>
#include <LEDA/system/assert.h>

LEDA_BEGIN_NAMESPACE

void dimacs_base_graph::clear_all_node_entries()
{ int i;
  forall(i,vlist) clear_node_data(V[i].data); 
}

void dimacs_base_graph::clear_all_arc_entries()
{ for(int j=0; j<m; j++) clear_arc_data(A[j].data); }


dimacs_base_graph::~dimacs_base_graph() 
{ delete[] A;
  delete[] V_vec; 
 }


inline bool next_line(istream& is, char* line, int sz)
{ if (&is == &cin)
    return (fgets(line,sz,stdin) != NULL);
  else
  { //return (bool(is.getline(line,sz)));
    is.getline(line,sz);
    return is.good();
  }
}
  

void dimacs_base_graph::read(istream& is)
{
  int cur_arc = 0;

  char line[256];
  int min_i = MAXINT;
  int max_i = 0;
  int i,j;

  while (next_line(is,line,sizeof(line)))
  {
    char c = line[0];

    istringstream in(line+1);
  
    switch ( c )
    {
      case 'p' : { in >> problem >> n >> m; 
                   //sscanf(line+1,"%s %d %d", problem, &n,&m);
                   V_vec = new vertex[n+1];
                   A = new arc[m];
                   A_stop = A+m;
                   cur_arc = 0;
                   for(i=0; i<=n; i++) V_vec[i].first_adj = -1;
                   V = V_vec;
                   break;
                 }
      
      case 'n' : { in >> i;
                   //sscanf(line+1,"%d",&i);
                   if (i < 0 || i > n)
                     LEDA_EXCEPTION(1,"dimacs: illegal index in node line.");

                   if (i < min_i) min_i = i;
                   if (i > max_i) max_i = i;

                   vlist.append(i);
                   vertex* v = V+i;
                   read_node_data(in,v->data);
                   break;
                 }                   

      case 'e' :
      case 'a' : { in >> i >> j;
                   //sscanf(line+1,"%d %d",&i,&j);

                   if (i < 0 || i > n || j < 0 || j > n)
                     LEDA_EXCEPTION(1,"dimacs: illegal index in arc line.");

                   if (cur_arc >= m)
                     LEDA_EXCEPTION(1,"dimacs: too many arc lines");

                   if (i < min_i) min_i = i;
                   if (j < min_i) min_i = j;
                   if (i > max_i) max_i = i;
                   if (j > max_i) max_i = j;

                   vertex* v = V+i;
                   arc* e = new (A+cur_arc) dimacs_arc(j,v->first_adj);
                   v->first_adj = cur_arc++;
                   read_arc_data(in,e->data);
                   break;
                 }       

      default  : 
                 break;
    }
  }

  if (cur_arc != m)
    LEDA_EXCEPTION(1,string("dimacs: less than m = %d arc lines",m));

  assert(max_i-min_i < n);

  if (min_i > 0) 
  { V = V+1;
    for(int j=0; j<m; j++) A[j].target--;
    list_item it;
    forall_items(it,vlist) vlist[it]--;
   }
}



void dimacs_base_graph::print(ostream& out) const
{ for(int i=0; i<n; i++)
  { out << string("%3d:",i); 
    int j = first_adj_edge(i);
    while (j != -1)
    { out << " " << target(j) << " (";
      out << node_inf(j);
      out << ")";
      j = next_adj_edge(j);
     }
    out << endl;
  }
  out << endl;
  vlist.print(out);
  out << endl;
}

LEDA_END_NAMESPACE
