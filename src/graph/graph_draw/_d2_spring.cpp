/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d2_spring.c
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



void D2_SPRING_EMBEDDING(const graph& G, node_array<double>& xpos, 
                                         node_array<double>& ypos,
                                         double xleft, double xright, 
                                         double ybottom, double ytop,
                                         int iterations)
{ 
  if (G.number_of_nodes() < 2) return;

  double width  = xright - xleft;
  double height = ytop - ybottom; 

  for (int count = 1; count < iterations; count++)
  {
    double k = ::sqrt(width*height / G.number_of_nodes()) / 2;

    //float l2 = 50*log_2(1+count);

    float l2 = 25*log_2(1+count);

    double tx = width  / l2;
    double ty = height / l2;

    node_array<double>	xdisp(G,0); 
    node_array<double>	ydisp(G,0); 

   // repulsive forces

   node v;
   forall_nodes(v,G) 
   { 
     double xv = xpos[v];
     double yv = ypos[v];

     node u;
     forall_nodes(u,G)
     { if (u == v) continue;
       double xdist = xv - xpos[u];
       double ydist = yv - ypos[u];

       double dist = xdist * xdist + ydist * ydist;

       if (dist < 1e-3) dist = 1e-3;

       double frepulse = k*k/dist;

       xdisp[v] += frepulse * xdist;
       ydisp[v] += frepulse * ydist;
      }

     //xdisp[v] *=  (double(rand_int(750,1250))/1000.0);
     //ydisp[v] *=  (double(rand_int(750,1250))/1000.0);
   }

 
   // attractive forces

   edge e;
   forall_edges(e,G)
   { node u = G.source(e);
     node v = G.target(e);

     double xdist=xpos[v]-xpos[u];
     double ydist=ypos[v]-ypos[u];

     double dist=::sqrt(xdist*xdist+ydist*ydist);

     float f = float((G.degree(u)+G.degree(v))/16.0);

     dist /= f;

     xdisp[v]-=xdist*dist/k;
     ydisp[v]-=ydist*dist/k;
     xdisp[u]+=xdist*dist/k;
     ydisp[u]+=ydist*dist/k;
    }


   // preventions

   forall_nodes(v,G)
   { double xd = xdisp[v];
     double yd = ydisp[v];

     double dist = ::sqrt(xd*xd+yd*yd);

     xd = tx*xd/dist;
     yd = ty*yd/dist;

     double xp = xpos[v] + xd;
     double yp = ypos[v] + yd;

     //if (xp > xleft && xp < xright) 
         xpos[v] = xp;
     //if (yp > ybottom && yp < ytop) 
         ypos[v] = yp;
    }
  }
}



void D2_SPRING_EMBEDDING1(const graph& G, node_array<double>& xpos, 
                                          node_array<double>& ypos,
                                          double xleft, double xright, 
                                          double ybottom, double ytop,
                                          int iterations)
{
  if (G.number_of_nodes() < 2) return;

  double width  = xright - xleft;
  double height = ytop - ybottom; 

  for (int count = 1; count < iterations; count++)
  {
    double k = ::sqrt(width*height / G.number_of_nodes()) / 2;

    //float l2 = 50*log_2(1+count);

    float l2 = 25*log_2(1+count);

    double tx = width  / l2;
    double ty = height / l2;

    node_array<double>	xdisp(G,0); 
    node_array<double>	ydisp(G,0); 

   // repulsive forces

   node v;
   forall_nodes(v,G) 
   { double xv = xpos[v];
     double yv = ypos[v];
     node u;
     forall_nodes(u,G)
     { if(u == v) continue;
       double xdist = xv - xpos[u];
       double ydist = yv - ypos[u];
       double dist = xdist * xdist + ydist * ydist;
       if (dist < 1e-3) dist = 1e-3;
       double frepulse = k*k/dist;
       xdisp[v] += frepulse * xdist;
       ydisp[v] += frepulse * ydist;
      }

     edge e;
     forall_edges(e,G)
     { node a = source(e);
       node b = target(e);
       if (a == v || b == v) continue;
       double xdist = xv - (xpos[a]+xpos[b])/2;
       double ydist = yv - (ypos[a]+ypos[b])/2;
       double dist = xdist * xdist + ydist * ydist;
       if (dist < 1e-3) dist = 1e-3;
       double frepulse = k*k/dist;
       xdisp[v] += frepulse * xdist;
       ydisp[v] += frepulse * ydist;
      }
   }


 
   // attractive forces

   edge e;
   forall_edges(e,G)
   { node u = G.source(e);
     node v = G.target(e);

     double xdist=xpos[v]-xpos[u];
     double ydist=ypos[v]-ypos[u];

     double dist= ::sqrt(xdist*xdist+ydist*ydist);

     float f = float((G.degree(u)+G.degree(v))/16.0);

     dist /= f;

     xdisp[v]-=xdist*dist/k;
     ydisp[v]-=ydist*dist/k;
     xdisp[u]+=xdist*dist/k;
     ydisp[u]+=ydist*dist/k;
    }


   // preventions

   forall_nodes(v,G)
   { double xd = xdisp[v];
     double yd = ydisp[v];

     double dist = ::sqrt(xd*xd+yd*yd);

     xd = tx*xd/dist;
     yd = ty*yd/dist;

     double xp = xpos[v] + xd;
     double yp = ypos[v] + yd;

     //if (xp > xleft && xp < xright) 
         xpos[v] = xp;
     //if (yp > ybottom && yp < ytop) 
         ypos[v] = yp;
    }
  }
}


LEDA_END_NAMESPACE

