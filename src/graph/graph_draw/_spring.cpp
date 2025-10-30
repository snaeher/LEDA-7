/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _spring.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graph/graph_alg.h>
#include <LEDA/graph/graph_draw.h>

#include <LEDA/core/array2.h>
#include <LEDA/system/assert.h>


#define FREPULSE(d) ((k2 > d) ? kk/d : 0)

LEDA_BEGIN_NAMESPACE

static float log_2(int x)
{ float l = 0;
  while (x)
  { l++;
    x >>= 1;
   }
  return l/2;
}


void SPRING_EMBEDDING(const graph& G, node_array<double>& xpos, 
                                      node_array<double>& ypos,
                                      double xleft, double xright, 
                                      double ybottom, double ytop,
                                      int iterations)
{ list<node> L;
  SPRING_EMBEDDING(G,L,xpos,ypos,xleft,xright,ybottom,ytop,iterations); }


void SPRING_EMBEDDING(const graph& G, const list<node>& fixed_nodes,
                                      node_array<double>& xpos, 
                                      node_array<double>& ypos,
                                      node_array<double>& xrad,
                                      node_array<double>& yrad,
                                      double xleft, double xright, 
                                      double ybottom, double ytop,
                                      int iterations)
{


  if (xleft >= xright || ybottom >= ytop)
      LEDA_EXCEPTION(1,"SPRING_EMBDDING: illegal bounds.");

  if (G.number_of_nodes() < 2) return;

  node_array<list_item>	lit(G); 
  node_array<bool>  fixed(G,false);

  node u = 0;
  edge e;

  node v;
  forall(v,fixed_nodes) fixed[v] = true;

  int c_f = 1;

  double width  = xright - xleft;
  double height = ytop - ybottom; 

  double tx_null = width/50;
  double ty_null = height/50;
  double tx = tx_null;
  double ty = ty_null;

  double k = ::sqrt(width*height / G.number_of_nodes()) / 2;
  double k2 = 2*k;
  double kk = k*k;

  int ki = int(k);

  if (ki == 0) ki = 1;

  //build  matrix of node lists

  int xA = int(width / ki + 1);
  int yA = int(height / ki + 1);

  array2<list<node> > A(-1,xA,-1,yA);

  forall_nodes(v,G)
  { int i = int((xpos[v] - xleft)   / ki);
    int j = int((ypos[v] - ybottom) / ki);

    if (i >= xA || i < 0 || j >= yA || j < 0) 
      LEDA_EXCEPTION(1,string("spring embedder: node out of range (%f,%f,%f,%f)",
                      xleft,ybottom,xright,ytop));

    lit[v] = A(i,j).push(v);
  }


  while (c_f < iterations)
  {
    node_array<double>	xdisp(G,0); 
    node_array<double>	ydisp(G,0); 

   // repulsive forces

   forall_nodes(v,G) 
   { int i = int((xpos[v] - xleft)   / ki);
     int j = int((ypos[v] - ybottom) / ki);

     double xpv = xpos[v];
     double ypv = ypos[v];
     double xrv = xrad[v];
     double yrv = yrad[v];

     for(int m = -1; m <= 1; m++)
      for(int n = -1; n <= 1; n++)
       forall(u,A(i+m,j+n))
       { if(u == v) continue;
         double xdist = xpv - xpos[u];
         double ydist = ypv - ypos[u];

         double xd = fabs(xdist) - (xrv + xrad[u]);
         double yd = fabs(ydist) - (yrv + yrad[u]);

         if (xd < 0) xd = 0;
         if (yd < 0) yd = 0;

         double dist = ::sqrt(xd*xd + yd*yd);
         if (dist < 1e-3) dist = 1e-3;

         xdisp[v] += FREPULSE(dist) * xdist / dist;
         ydisp[v] += FREPULSE(dist) * ydist / dist;
        }

     if (c_f < iterations/2)
     { xdisp[v] *=  (double(rand_int(750,1250))/1000.0);
       ydisp[v] *=  (double(rand_int(750,1250))/1000.0);
      }
    }

 

   // attractive forces

   forall_edges(e,G)
   { node u = G.source(e);
     node v = G.target(e);
     double xdist=xpos[v]-xpos[u];
     double ydist=ypos[v]-ypos[u];
     double dist= ::sqrt(xdist*xdist+ydist*ydist);

     double f = (G.degree(u)+G.degree(v))/16.0;

     dist /= f;

     xdisp[v]-=xdist*dist/k;
     ydisp[v]-=ydist*dist/k;
     xdisp[u]+=xdist*dist/k;
     ydisp[u]+=ydist*dist/k;
    }


   // preventions

   forall_nodes(v,G)
   { 
     if (fixed[v]) continue;

     int i0 = int((xpos[v] - xleft)/ki);
     int j0 = int((ypos[v] - ybottom)/ki);

     double xd= xdisp[v];
     double yd= ydisp[v];
     double dist = ::sqrt(xd*xd+yd*yd);

     if (dist < 1) dist = 1;

     xd = tx*xd/dist;
     yd = ty*yd/dist;

     double xp = xpos[v] + xd;
     double yp = ypos[v] + yd;

     double xr = 2*xrad[v];
     double yr = 2*yrad[v];

     int i = i0;
     int j = j0;
     
     if (xp > xleft+xr && xp < xright-xr) 
     { xpos[v] = xp;
       i = int((xp - xleft)/ki);
      }

     if (yp > ybottom+yr && yp < ytop-yr) 
     { ypos[v] = yp;
       j = int((yp - ybottom)/ki);
      }
 
     if (i != i0 || j != j0)
     { assert(lit[v] != 0);
       A(i0,j0).del_item(lit[v]);
       lit[v] = A(i,j).push(v);
      }
 
    }

   tx = tx_null / log_2(c_f); 
   ty = ty_null / log_2(c_f);

   c_f++;
  }

}

void SPRING_EMBEDDING(const graph& G, const list<node>& fixed_nodes,
                                      node_array<double>& xpos, 
                                      node_array<double>& ypos,
                                      double xleft, double xright, 
                                      double ybottom, double ytop,
                                      int iterations)
{ node_array<double> xrad(G,0);
  node_array<double> yrad(G,0);
  SPRING_EMBEDDING(G,fixed_nodes,xpos,ypos,xrad,yrad,xleft,xright, 
                                                     ybottom,ytop,iterations);
 }

void SPRING_EMBEDDING(const graph& G, node_array<double>& xpos,
                                      node_array<double>& ypos,
                                      node_array<double>& xrad,
                                      node_array<double>& yrad,
                                      int iterations)
{
  int n = G.number_of_nodes();

  double node_area = 0;

  node v;
  forall_nodes(v,G) 
      node_area += 4.0*xrad[v]*yrad[v]*(1 + float(G.degree(v))/n);

  double total_area = 9*node_area;

  double d = ::sqrt(total_area);

  forall_nodes(v,G)
  { xpos[v] = rand_int(int(0.4*d),int(0.6*d));
    ypos[v] = rand_int(int(0.4*d),int(0.6*d));
   }

  list<node> fixed;
  SPRING_EMBEDDING(G,fixed,xpos,ypos,xrad,yrad,0,d,0,d,iterations);

 }


LEDA_END_NAMESPACE
