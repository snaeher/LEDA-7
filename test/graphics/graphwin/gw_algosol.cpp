/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  gw_algosol.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

#include <LEDA/pixmaps/algosol.xpm>
#include <LEDA/pixmaps/algosol/algosol_news.xpm>
#include <LEDA/pixmaps/algosol/algosol_prod.xpm>
#include <LEDA/pixmaps/algosol/algosol_serv.xpm>
#include <LEDA/pixmaps/algosol/algosol_res.xpm>
#include <LEDA/pixmaps/algosol/algosol_comp.xpm>

using namespace leda;

main()
{
  GraphWin gw;

  gw.display();

  for(;;)
  { gw.set_bg_xpm(algosol_news_xpm);
    leda_wait(0.75);
    gw.set_bg_xpm(algosol_comp_xpm);
    leda_wait(0.75);
    gw.set_bg_xpm(algosol_prod_xpm);
    leda_wait(0.75);
    gw.set_bg_xpm(algosol_serv_xpm);
    leda_wait(0.75);
    gw.set_bg_xpm(algosol_res_xpm);
    leda_wait(0.75);
  }

  return 0;
}
