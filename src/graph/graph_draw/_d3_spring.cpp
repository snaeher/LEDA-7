/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d3_spring.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>

#include <LEDA/core/array2.h>

LEDA_BEGIN_NAMESPACE


static float log_2(int x)
{ float l = 0;
  while (x)
  { l++;
    x >>= 1;
   }
  return l/2;
}


void D3_SPRING_EMBEDDING(const graph& G, node_array<double>& xpos, 
                                         node_array<double>& ypos,
                                         node_array<double>& zpos,
                                         double xmin, double xmax, 
                                         double ymin, double ymax,
                                         double zmin, double zmax,
                                         int iterations)
{
  if (xmin >= xmax || ymin >= ymax || zmin >= zmax)
      LEDA_EXCEPTION(1,"SPRING_EMBDDING: illegal bounds.");

  double width  = xmax - xmin;
  double height = ymax - ymin; 
  double depth  = zmax - zmin; 

  for (int count = 1; count < iterations; count++)
  {
    double k = ::sqrt(width*height / G.number_of_nodes());

    float l2 = 50*log_2(1+count);

    double tx = width  / l2;
    double ty = height / l2;
    double tz = depth  / l2;

    node_array<double>	xdisp(G,0); 
    node_array<double>	ydisp(G,0); 
    node_array<double>	zdisp(G,0); 

   // repulsive forces

   node v;
   forall_nodes(v,G) 
   { 
     double xv = xpos[v];
     double yv = ypos[v];
     double zv = zpos[v];

     node u;
     forall_nodes(u,G)
     { if (u == v) continue;
       double xdist = xv - xpos[u];
       double ydist = yv - ypos[u];
       double zdist = zv - zpos[u];

       double dist = xdist * xdist + ydist * ydist + zdist * zdist;

       if (dist < 1e-3) dist = 1e-3;

       double frepulse = k*k/dist;

       xdisp[v] += frepulse * xdist;
       ydisp[v] += frepulse * ydist;
       zdisp[v] += frepulse * zdist;
      }
   }

 
   // attractive forces

   edge e;
   forall_edges(e,G)
   { node u = G.source(e);
     node v = G.target(e);

     double xdist=xpos[v]-xpos[u];
     double ydist=ypos[v]-ypos[u];
     double zdist=zpos[v]-zpos[u];

     double dist= ::sqrt(xdist*xdist+ydist*ydist+zdist*zdist);

     float f = float((G.degree(u)+G.degree(v))/16.0);

     dist /= f;

     xdisp[v]-=xdist*dist/k;
     ydisp[v]-=ydist*dist/k;
     zdisp[v]-=zdist*dist/k;
     xdisp[u]+=xdist*dist/k;
     ydisp[u]+=ydist*dist/k;
     zdisp[u]+=zdist*dist/k;
    }


   // preventions

   forall_nodes(v,G)
   { double xd = xdisp[v];
     double yd = ydisp[v];
     double zd = zdisp[v];

     double dist = ::sqrt(xd*xd+yd*yd+zd*zd);

     xd = tx*xd/dist;
     yd = ty*yd/dist;
     zd = tz*zd/dist;

     double xp = xpos[v] + xd;
     double yp = ypos[v] + yd;
     double zp = zpos[v] + zd;
   //if (xp > xmin && xp < xmax)
       xpos[v] = xp;
   //if (yp > ymin && yp < ymax)
       ypos[v] = yp;
   //if (zp > zmin && zp < zmax)
       zpos[v] = zp;

    }
  }

}

LEDA_END_NAMESPACE

