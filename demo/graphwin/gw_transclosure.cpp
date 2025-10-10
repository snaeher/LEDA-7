/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_transclosure.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

using namespace leda;

int main()
{
  GraphWin gw;
  gw.display();

  bool compute_closure = true;

  bool go_on = true;
  do {
    if (compute_closure)
      gw.message("Enter graph and its transistive closure will be computed");
    else
      gw.message("Enter graph and its transistive reduction will be computed");

    go_on = gw.edit();
    if (! go_on) break;

    //graph& G0 = gw.get_graph();
    if (compute_closure)
      //MAKE_TRANSITIVELY_CLOSED(G0);
      gw_make_transitively_closed(gw);
    else
      //MAKE_TRANSITIVELY_REDUCED(G0);
      gw_make_transitively_reduced(gw);
    gw.update_graph();

    /*
    graph G0 = gw.get_graph();
    graph G = TRANSITIVE_CLOSURE(G0);
    gw.set_graph(G);
    */

    gw.message("");
    gw.redraw();
    go_on = gw.edit();
    
    compute_closure = ! compute_closure;

    //gw.set_graph(G);
  } while (go_on);

  return 0;
}

