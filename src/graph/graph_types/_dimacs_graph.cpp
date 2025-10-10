/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _dimacs_graph.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/dimacs_graph.h>
#include <LEDA/system/assert.h>


LEDA_BEGIN_NAMESPACE

dimacs_graph::dimacs_graph(int ns, int as) : 
                            n_slots(ns), a_slots(as), n_sz(ns+2), a_sz(as+2) 
{ int i;
  for(i=0; i<max_n_slot; i++)
  { max_node_data[i] = 0;
    min_node_data[i] = MAXINT;
   }
  for(i=0; i<max_a_slot; i++)
  { max_edge_data[i] = 0;
    min_edge_data[i] = MAXINT;
   }
}


dimacs_graph::~dimacs_graph() 
{ delete[] A;
  delete[] V; 
 }


void dimacs_graph::parse_line(const char* line, int& cur_arc)
{
    switch (line[0])
    {
      case 'p' : { sscanf(line+1,"%s %d %d", problem, &n,&m);
                   V = new int[(n+1)*n_sz];
                   A = new int[m*a_sz];
                   for(int i=0; i<=n; i++) 
                   { vertex* v = V_get(i);
                     v->first_adj = v->last_adj = -1;
                    }
                   cur_arc = 0;
                   break;
                 }
      
      case 'n' : { int i=-1, x=0;

                   switch (n_slots) {
                   case 0: sscanf(line+1,"%d",&i);
                           break;
                   case 1: sscanf(line+1,"%d %d",&i,&x);
                           if (i>=0 && i<=n) V_get(i)->data[0] = x;
                           if (x > max_node_data[0]) max_node_data[0] = x;
                           if (x < min_node_data[0]) min_node_data[0] = x;
                           break;
                   }

                   if (i < 0 || i > n)
                     LEDA_EXCEPTION(1,
                        string("dimacs: illegal index %d in node line.",i));

                   if (i < min_i) min_i = i;
                   if (i > max_i) max_i = i;

                   vlist.append(i);
                   break;
                 }                   

      case 'e' :
      case 'a' : { int i,j,x,y,z;

                   switch (a_slots) {
                   case 0: { int n = sscanf(line+1,"%d %d",&i,&j);
                             if (n < 2) 
                               LEDA_EXCEPTION(1,
                               string("dimacs: missing data for arc %d.",cur_arc));
                             break;
                            }

                   case 1: { int n = sscanf(line+1,"%d %d %d",&i,&j,&x);
                             if (n < 3) 
                               LEDA_EXCEPTION(1,
                               string("dimacs: missing data for arc %d.",cur_arc));
                             if (x > max_node_data[0]) max_node_data[0] = x;
                             if (x < min_node_data[0]) min_node_data[0] = x;
                             break;
                            }

                   case 2: { int n = sscanf(line+1,"%d %d %d %d",&i,&j,&x,&y);
                             if (n < 4) 
                               LEDA_EXCEPTION(1,
                               string("dimacs: missing data for arc %d.",cur_arc));
                             if (x > max_edge_data[0]) max_edge_data[0] = x;
                             if (x < min_edge_data[0]) min_edge_data[0] = x;
                             if (y > max_edge_data[1]) max_edge_data[1] = y;
                             if (y < min_edge_data[1]) min_edge_data[1] = y;
                             break;
                            }

                   case 3: { int n = sscanf(line+1,"%d %d %d %d %d",&i,&j,&x,&y,&z);
                             if (n < 5) 
                               LEDA_EXCEPTION(1,
                               string("dimacs: missing data for arc %d.",cur_arc));
                             if (x > max_edge_data[0]) max_edge_data[0] = x;
                             if (x < min_edge_data[0]) min_edge_data[0] = x;
                             if (y > max_edge_data[1]) max_edge_data[1] = y;
                             if (y < min_edge_data[1]) min_edge_data[1] = y;
                             if (z > max_edge_data[2]) max_edge_data[2] = z;
                             if (z < min_edge_data[2]) min_edge_data[2] = z;
                             break;
                            }
                   }

/*
cout << "x = " << x << endl;
*/

                   if (i < 0 || i > n || j < 0 || j > n)
                     LEDA_EXCEPTION(1,"dimacs: illegal index in arc line.");

                   if (cur_arc >= m)
                     LEDA_EXCEPTION(1,"dimacs: too many arc lines");

                   if (i < min_i) min_i = i;
                   if (j < min_i) min_i = j;
                   if (i > max_i) max_i = i;
                   if (j > max_i) max_i = j;

                   vertex* v = V_get(i);
                   arc* e = new (A_get(cur_arc)) arc(j);

                   if (v->first_adj == -1) 
                     v->first_adj = cur_arc;
                   else
                     A_get(v->last_adj)->next_adj = cur_arc;

                   v->last_adj = cur_arc++;

                   switch (a_slots) {
                   case 3: e->data[2] = z;
                   case 2: e->data[1] = y;
                   case 1: e->data[0] = x;
                   }

                   break;
                 }       

      default  : 
                 break;
    }
}



void dimacs_graph::read(string fname)
{
  FILE* in = stdin;

  if (fname != "") in = fopen(fname,"r");

  char line[256];

  int cur_arc = 0;

  min_i = MAXINT;
  max_i = 0;

  while (fgets(line,sizeof(line),in) != NULL) parse_line(line,cur_arc);

  if (cur_arc != m)
    LEDA_EXCEPTION(1,string("dimacs: less than m = %d arc lines",m));

  assert(max_i-min_i < n);
}



void dimacs_graph::read(istream& is)
{
  if (&is == &cin) {
     read();
     return;
  }

  int cur_arc = 0;

  char line[256];

  min_i = MAXINT;
  max_i = 0;

  while (is.getline(line,sizeof(line))) parse_line(line,cur_arc);

  if (cur_arc != m)
    LEDA_EXCEPTION(1,string("dimacs: less than m = %d arc lines",m));

  assert(max_i-min_i < n);

}

void dimacs_graph::read(http& ht)
{
  int cur_arc = 0;

  min_i = MAXINT;
  max_i = 0;

  string line;
  while (ht.get_line(line)) parse_line(line,cur_arc);

  if (cur_arc != m)
    LEDA_EXCEPTION(1,string("dimacs: less than m = %d arc lines",m));

  assert(max_i-min_i < n);

}



void dimacs_graph::print(ostream& out) const
{ for(int i=0; i<n; i++)
  { out << string("%3d:",i); 
    int j = first_adj_edge(i);
    while (j != -1)
    { out << " " << target(j) << " (";
      for(int s=0; s < n_slots; s++) out << V_get(j)->data[s];
      out << ")";
      j = next_adj_edge(j);
     }
    out << endl;
  }
  out << endl;
  vlist.print(out);
  out << endl;
}

void dimacs_graph::print_statistics(ostream& out) const
{ int i;

  out << string("|V| = %d",n) << endl;
  out << string("|E| = %d",m) << endl;

  int max_outdeg = 0;
  int min_outdeg = m;

  int* outdeg = new int[m];
  for(i=0; i<n; i++) outdeg[i] = 0;

  int* tcount = new int[n_slots];
  for(i=0; i<n_slots; i++) tcount[i] = 0;

  for(i=0; i<n; i++)
  { int d = 0;
    int j = first_adj_edge(i);
    while (j != -1)
    { d++;
      j = next_adj_edge(j);
     }

    outdeg[d]++;

    if (d > max_outdeg) max_outdeg = d;
    if (d < min_outdeg) min_outdeg = d;

    for(int j=0; j<n_slots; j++)
      if (V_get(i)->data[j] == 0) tcount[j]++;
   }


/*
  for(i=min_outdeg; i<=max_outdeg; i++)
  { if (outdeg[i] == 0) continue;
    out << string("outdeg %2d: %d", i, outdeg[i]) << endl;
   }
  out << endl;
*/

  for(i=0; i<n_slots; i++)
  { int x = max_node_data[i];
    int bits = 0;
    while (x) { bits++; x /= 2; }

    out << string("n_data[%d]:  min = %8d  max = %8d (%2d)  zero = %d  (%5.2f %%)",
                   i,min_node_data[i],max_node_data[i],bits,tcount[i],
                   double(100*tcount[i])/n) << endl;
   }

  for(i=0; i<a_slots; i++)
  { int x = max_edge_data[i];
    int bits = 0;
    while (x) { bits++; x /= 2; }
    out << string("e_data[%d]:  min = %8d  max = %8d (%2d)",
                   i,min_edge_data[i],max_edge_data[i],bits) << endl;
   }

  delete[] outdeg;
}


LEDA_END_NAMESPACE
