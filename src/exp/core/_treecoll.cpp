/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _treecoll.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



// dyna_trees.c

#include <LEDA/core/tree_collection.h>

#define BIG MAXDOUBLE

LEDA_BEGIN_NAMESPACE

void dyna_trees::splay(d_vertex x) {
     d_vertex y, z, zz, a, b, c, d;
     double   mincost_a,
	      mincost_b,
	      mincost_c,
	      mincost_d,
	      mincost_x,
	      mincost_y,
	      mincost_z,
	      
	      cost_x,
	      cost_y,
	      cost_z;

     
     while (x->parent) {
	  // splay-step : 

	  y=x->parent; // Vater von x
	  z=y->parent; // Grossvater von x (oder 0)
	  zz = (z==0) ? 0 : z->parent; // Urgrossvater von z (oder 0)

	  // 1.Fall: x hat Vater, aber keinen Grossvater

	  if (z==0){ 
		if (y->left==x) { 
		    // x wurzel des linken unterbaums  (1)
		    a=x->left;
		    b=x->right;
		    c=y->right;
		    x->parent=0;
		    x->right=y;
		    y->parent=x;
		    y->left=b;
		    if (b) b->parent=y;

		    x->successor=y->successor;
		    y->successor=0;

		    mincost_a = (a) ? a->dmin + x->dmin + y->dmin : BIG;
		    mincost_b = (b) ? b->dmin + x->dmin + y->dmin : BIG;
		    mincost_c = (c) ? c->dmin + y->dmin : BIG;
		    mincost_x = x->dmin + y->dmin;
		    mincost_y = y->dmin;
		    cost_x	= x->dcost + mincost_x;
		    cost_y	= y->dcost + mincost_y;
		    
		    mincost_x = mincost_y;
		    mincost_y = (mincost_b <= mincost_c) ? mincost_b : mincost_c;
		    if (cost_y <= mincost_y) mincost_y = cost_y;
		    x->dcost = cost_x - mincost_x;
		    y->dcost = cost_y - mincost_y;
		    x->dmin  = mincost_x;
		    y->dmin  = mincost_y - mincost_x;
		    if (a) a->dmin  = mincost_a - mincost_x;
		    if (b) b->dmin  = mincost_b - mincost_y;
		    if (c) c->dmin  = mincost_c - mincost_y;
		}
		else {
		    // x wurzel des rechten unterbaums (2)
		    a=y->left;
		    b=x->left;
		    c=x->right;
		    x->parent=0;
		    x->left=y;
		    y->parent=x;
		    y->right=b;
		    if (b) b->parent=y;
		    
		    x->successor=y->successor;
		    y->successor=0;
		    
		    mincost_a = (a) ? a->dmin + y->dmin : BIG;
		    mincost_b = (b) ? b->dmin + x->dmin + y->dmin : BIG;
		    mincost_c = (c) ? c->dmin + x->dmin + y->dmin : BIG;
		    mincost_x = x->dmin + y->dmin;
		    mincost_y = y->dmin;
		    cost_x	= x->dcost + mincost_x;
		    cost_y	= y->dcost + mincost_y;
		    
		    mincost_x = mincost_y;
		    mincost_y = (mincost_a <= mincost_b) ? mincost_a : mincost_b;
		    if (cost_y <= mincost_y) mincost_y = cost_y;
		    x->dcost = cost_x - mincost_x;
		    y->dcost = cost_y - mincost_y;
		    x->dmin  = mincost_x;
		    y->dmin  = mincost_y - mincost_x;
		    if (a) a->dmin  = mincost_a - mincost_y;
		    if (b) b->dmin  = mincost_b - mincost_y;
		    if (c) c->dmin  = mincost_c - mincost_x;	       
		}
		continue;
	  }

	  
	  // 2.Fall: x hat also Grossvater, x und parent(x) linke (rechte)
	  //		   Soehne.
	  // Linke Soehne:

	  if ((z->left==y)&&(y->left==x)){   //  (3)
		a=x->left;
		b=x->right;
		c=y->right;
		d=z->right;
		y->left=b;
		if (b) b->parent=y;
		z->left=c;
		if (c) c->parent=z;
		x->right=y;
		y->parent=x;
		y->right=z;
		z->parent=y;
		if (zz) {
		    if (zz->left==z){
			 zz->left=x;
		    }
		    else{
			 zz->right=x;
		    }
		}
		else {	 //  z is solid-tree-root
		    x->successor=z->successor;	
		    z->successor=0;
		} 
		x->parent=zz;
		
		mincost_a = (a) ? a->dmin + x->dmin + y->dmin + z->dmin : BIG;
		mincost_b = (b) ? b->dmin + x->dmin + y->dmin + z->dmin : BIG;
		mincost_c = (c) ? c->dmin + y->dmin + z->dmin : BIG;
		mincost_d = (d) ? d->dmin + z->dmin : BIG;
		mincost_x =	 x->dmin + y->dmin + z->dmin;
		mincost_y =	 y->dmin + z->dmin;
		mincost_z =	 z->dmin;
		cost_x	 = mincost_x + x->dcost;
		cost_y	 = mincost_y + y->dcost;
		cost_z	 = mincost_z + z->dcost;
		
		mincost_x = mincost_z;
		mincost_z = (mincost_c <= mincost_d) ? mincost_c : mincost_d;
		if (cost_z <= mincost_z) mincost_z = cost_z;
		mincost_y = (mincost_b <= mincost_z) ? mincost_b : mincost_z;
		if (cost_y <= mincost_y) mincost_y = cost_y;
		x->dcost = cost_x - mincost_x;
		y->dcost = cost_y - mincost_y;
		z->dcost = cost_z - mincost_z;
		x->dmin = mincost_x;
		y->dmin = mincost_y - mincost_x;
		z->dmin = mincost_z - mincost_y;
		if (a) a->dmin	= mincost_a - mincost_x;
		if (b) b->dmin	= mincost_b - mincost_y;
		if (c) c->dmin	= mincost_c - mincost_z;
		if (d) d->dmin	= mincost_d - mincost_z;

		continue;
	  }

	  
	  // Rechte Soehne:   (4)
	  
	  if ((z->right==y)&&(y->right==x)){
		a=z->left;
		b=y->left;
		c=x->left;
		d=x->right;
		z->right=b;
		if (b) b->parent=z;
		z->parent=y;
		y->left=z;
		y->right=c;
		if (c) c->parent=y;
		y->parent=x;
		x->left=y;
		if (zz) {
		    if (zz->left==z){
			 zz->left=x;
		    }
		    else{
			 zz->right=x;
		    }
		}
		else { 
		    x->successor=z->successor;
		    z->successor=0;
		}
		x->parent=zz;
		
		mincost_a = (a) ? a->dmin + z->dmin : BIG;
		mincost_b = (b) ? b->dmin + y->dmin + z->dmin : BIG;
		mincost_c = (c) ? c->dmin + x->dmin + y->dmin + z->dmin : BIG;
		mincost_d = (d) ? d->dmin + x->dmin + y->dmin + z->dmin : BIG;
		mincost_x =	 x->dmin + y->dmin + z->dmin;
		mincost_y =	 y->dmin + z->dmin;
		mincost_z =	 z->dmin;
		cost_x	 = mincost_x + x->dcost;
		cost_y	 = mincost_y + y->dcost;
		cost_z	 = mincost_z + z->dcost;
		
		mincost_x = mincost_z;
		mincost_z = (mincost_a <= mincost_b) ? mincost_a : mincost_b;
		if (cost_z <= mincost_z) mincost_z = cost_z;
		mincost_y = (mincost_c <= mincost_z) ? mincost_c : mincost_z;
		if (cost_y <= mincost_y) mincost_y = cost_y;
		x->dcost = cost_x - mincost_x;
		y->dcost = cost_y - mincost_y;
		z->dcost = cost_z - mincost_z;
		x->dmin = mincost_x;
		y->dmin = mincost_y - mincost_x;
		z->dmin = mincost_z - mincost_y;
		if (a) a->dmin	= mincost_a - mincost_z;
		if (b) b->dmin	= mincost_b - mincost_z;
		if (c) c->dmin	= mincost_c - mincost_y;
		if (d) d->dmin	= mincost_d - mincost_x;

		continue;
	  }
	  

	  // 3.Fall: x linkes, p(x) rechtes Kind (oder umgekehrt)
	  // Zuerst x links, p(x) rechts:
	  
	  if ((z->right==y)&&(y->left==x)){   // (5)
		a=z->left;
		b=x->left;
		c=x->right;
		d=y->right;
		z->right=b;
		if (b) b->parent=z;
		z->parent=x;
		x->left=z;
		y->left=c;
		if (c) c->parent=y;
		y->parent=x;
		x->right=y;
		if (zz) {
		    if (zz->left==z){
			 zz->left=x;
		    }
		    else{
			 zz->right=x;
		    }
		} 
		else {
		    x->successor=z->successor;
		    z->successor=0;
		}
		x->parent=zz;
		
		mincost_a = (a) ? a->dmin + z->dmin : BIG;
		mincost_b = (b) ? b->dmin + x->dmin + y->dmin + z->dmin : BIG;
		mincost_c = (c) ? c->dmin + x->dmin + y->dmin + z->dmin : BIG;
		mincost_d = (d) ? d->dmin + y->dmin + z->dmin : BIG;
		mincost_x =	 x->dmin + y->dmin + z->dmin;
		mincost_y =	 y->dmin + z->dmin;
		mincost_z =	 z->dmin;
		cost_x	 = mincost_x + x->dcost;
		cost_y	 = mincost_y + y->dcost;
		cost_z	 = mincost_z + z->dcost;
		
		mincost_x = mincost_z;
		mincost_z = (mincost_a <= mincost_b) ? mincost_a : mincost_b;
		if (cost_z <= mincost_z) mincost_z = cost_z;
		mincost_y = (mincost_c <= mincost_d) ? mincost_c : mincost_d;
		if (cost_y <= mincost_y) mincost_y = cost_y;
		x->dcost = cost_x - mincost_x;
		y->dcost = cost_y - mincost_y;
		z->dcost = cost_z - mincost_z;
		x->dmin = mincost_x;
		y->dmin = mincost_y - mincost_x;
		z->dmin = mincost_z - mincost_x;
		if (a) a->dmin	= mincost_a - mincost_z;
		if (b) b->dmin	= mincost_b - mincost_z;
		if (c) c->dmin	= mincost_c - mincost_y;
		if (d) d->dmin	= mincost_d - mincost_y;

		continue;
	  }

	  
	  // Nun x rechts, p(x) links:
	  
	  if ((z->left==y)&&(y->right==x)){	// (6)
		a=y->left;
		b=x->left;
		c=x->right;
		d=z->right;
		y->right=b;
		if (b) b->parent=y;
		y->parent=x;
		x->left=y;
		z->left=c;
		if (c) c->parent=z;
		z->parent=x;
		x->right=z;
		if (zz) {
		    if (zz->left==z){
			 zz->left=x;
		    }
		    else{
			 zz->right=x;
		    }
		}
		else {
		    x->successor=z->successor;
		    z->successor=0;
		}
		x->parent=zz;
		
		mincost_a = (a) ? a->dmin + y->dmin + z->dmin : BIG;
		mincost_b = (b) ? b->dmin + x->dmin + y->dmin + z->dmin : BIG;
		mincost_c = (c) ? c->dmin + x->dmin + y->dmin + z->dmin : BIG;
		mincost_d = (d) ? d->dmin + z->dmin : BIG;
		mincost_x =	 x->dmin + y->dmin + z->dmin;
		mincost_y =	 y->dmin + z->dmin;
		mincost_z =	 z->dmin;
		cost_x	 = mincost_x + x->dcost;
		cost_y	 = mincost_y + y->dcost;
		cost_z	 = mincost_z + z->dcost;
		
		mincost_x = mincost_z;
		mincost_z = (mincost_c <= mincost_d) ? mincost_c : mincost_d;
		if (cost_z <= mincost_z) mincost_z = cost_z;
		mincost_y = (mincost_a <= mincost_b) ? mincost_a : mincost_b;
		if (cost_y <= mincost_y) mincost_y = cost_y;
		x->dcost = cost_x - mincost_x;
		y->dcost = cost_y - mincost_y;
		z->dcost = cost_z - mincost_z;
		x->dmin = mincost_x;
		y->dmin = mincost_y - mincost_x;
		z->dmin = mincost_z - mincost_x;
		if (a) a->dmin	= mincost_a - mincost_y;
		if (b) b->dmin	= mincost_b - mincost_y;
		if (c) c->dmin	= mincost_c - mincost_z;
		if (d) d->dmin	= mincost_d - mincost_z;

	  }
     }
}


d_vertex dyna_trees::assemble(d_vertex u, d_vertex v, d_vertex w)
{
     double mincost_u,
	    mincost_v,
	    mincost_w,
	    cost_v;

     v->left=u;
     v->right=w;
     if (u) u->parent=v;
     if (w) w->parent=v;
     
     mincost_u = (u) ? u->dmin : BIG;
     mincost_v =       v->dmin;
     mincost_w = (w) ? w->dmin : BIG;
     cost_v = mincost_v + v->dcost;
     
     mincost_v = (mincost_u <= mincost_w) ? mincost_u : mincost_w;
     if (cost_v < mincost_v) mincost_v = cost_v;
     v->dcost = cost_v - mincost_v;
     v->dmin = mincost_v;
     if (u) u->dmin = mincost_u - mincost_v;
     if (w) w->dmin = mincost_w - mincost_v;

     return v;
}


void   dyna_trees::disassemble(d_vertex v, d_vertex& v1, d_vertex& v2) {
     double mincost_v1,
	    mincost_v2;


     v1=v->left;
     v2=v->right;
     if (v1) v1->parent=0;
     if (v2) v2->parent=0;
     v->left=0;
     v->right=0;

     mincost_v1 = (v1) ? v1->dmin + v->dmin : BIG;
     mincost_v2 = (v2) ? v2->dmin + v->dmin : BIG;

     if (v1) v1->dmin = mincost_v1;
     if (v2) v2->dmin = mincost_v2;
     
     v->dmin += v->dcost;
     v->dcost = 0;
}


d_vertex dyna_trees::makepath(void* i)
{
     if (first==0) {
	  first=last=new d_node(i);
     }
     else {
	  last->next=new d_node(i);
	  last=last->next;
     }
     return last;
}


d_vertex dyna_trees::findpath(d_vertex v)
{
     splay(v);
     return v;
}



d_vertex dyna_trees::findpathcost(d_path p, double& d)
{
     d_vertex w;
     w=p;
     
     while( !( (w->dcost==0) && ( (w->right==0)||(w->right->dmin>0) ) ) ) {
	  if (w->right) {
		if (w->right->dmin == 0) {
		     w=w->right;
		} else {
		     if (w->dcost>0) w=w->left;
		}  
	  }
	  else {
		if (w->dcost>0) w=w->left;
	  }
     }
     splay(w);
     d=w->dmin;
     return w;
}



d_vertex dyna_trees::findtail(d_path p)
{
     d_vertex w;
     w=p;
     
     while(w->right) {
	  w=w->right;
     }
     splay(w);
     return w;
}



void dyna_trees::addpathcost(d_path p, double x)
{
     p->dmin += x;
}



d_vertex dyna_trees::join(d_path p, d_path v, d_path q)
{
     return assemble(p, v, q);
}



void dyna_trees::split(d_vertex v, d_vertex& v1, d_vertex& v2)
{
     splay(v);
     disassemble(v, v1, v2);
}



d_path dyna_trees::expose(d_vertex v)
{
     d_path   p,
	      q,
	      r;
     d_vertex w;
     
     p=0;
     while (v) {
	  w=findpath(v)->successor;
	  split(v,q,r);
	  if (q) q->successor=v;
	  p=join(p, v, r);
	  v=w;
     }
     p->successor=0;
     return p;
}


d_vertex dyna_trees::maketree(void* i)
{
     copy_inf(i);

     d_vertex v;
     v=makepath(i);
     v->successor=0;
     return v;
}


d_vertex dyna_trees::findroot(d_vertex v)
{
     return findtail(expose(v));
}



d_vertex dyna_trees::findcost(d_vertex v, double& d)
{
     return findpathcost(expose(v),d);
}



void dyna_trees::addcost(d_vertex v, double x)
{
     addpathcost(expose(v),x);
}



void dyna_trees::link(d_vertex v, d_vertex w)
{
     join( (d_vertex) 0, expose(v), expose(w) )->successor=0;
}



void dyna_trees::cut(d_vertex v) 
{
     d_path p,q;
     
     expose(v);
     split(v,p,q);
     v->successor=q->successor=0;
}



dyna_trees::~dyna_trees()
{
     d_node *x, 
	    *y;
	  
     x=first;
     while(x) {
	  y=x->next;
	  delete x;
	  x=y;
     }
}
 
LEDA_END_NAMESPACE

