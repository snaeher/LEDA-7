/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  gw_cluster.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/graphwin.h>

using namespace leda;

int main(int argc, char** argv) 
{
  GraphWin gw;

  gw.new_shape( 50, 50,150,150,"cluster1",blue);
  gw.new_shape(200,200,400,400,"cluster2",red,ivory,blue,1,dashed);
  gw.new_shape( 50,400,150,200,"cluster3",green2,invisible,red,2,solid);
  //gw.new_shape(200, 50,400,150,"cluster4",black,orange,black);

  list<point> L;
  L.append(point(200,50));
  L.append(point(300,20));
  L.append(point(400,50));
  L.append(point(430,100));
  L.append(point(400,150));
  L.append(point(300,180));
  L.append(point(200,150));
  L.append(point(170,100));

  gw.new_shape(polygon(L),"cluster4",black,yellow,black);

  circle C(200,250,100);
  gw.new_shape(C,"cluster5",green2,invisible,green2);


  gw.display();
  gw.edit();

  return 0;
}

