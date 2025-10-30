/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _bb1_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//--------------------------------------------------------------------
//  
//  BB[alpha] Trees
//
//  Michael Wenzel   (1989)
//
// Implementation as described in
// Kurt Mehlhorn: Data Structures and Algorithms 1, section III.5.1
//
// -------------------------------------------------------------------
// Aenderungen:
//   - keine virtuellen Funktionen  (M. Wenzel, Nov. 1989)
//   - nicht rekursiv               (M. Wenzel, Nov. 1989)
//   - virtuelle compare-Funktion   (M. Wenzel, Nov. 1989)
//--------------------------------------------------------------------



#undef TEST
#undef DUMP

#include <LEDA/geo/bb1_tree.h>

#ifdef TEST
#define DPRINT(x) cout << string x
#else
#define DPRINT(x)   
#endif

#ifdef DUMP
#define DDUMP(x) cout << string x
#else
#define DDUMP(x)   
#endif



/*
enum children { left = 0 , right = 1 };
enum leaf_or_node { Leaf = 0 , Node = 1 } ;
*/


// -------------------------------------------------------------
// member functions
// -------------------------------------------------------------

LEDA_BEGIN_NAMESPACE

  bb1_tree::bb1_tree(float a)   : st(BB1_STACKSIZE)
  { 
    root = first = iterator = 0;
    anzahl=0;
    if ((a<=0.25) || (a>1-SQRT1_2))
      LEDA_EXCEPTION(3,"alpha not in range");
    alpha=a;
    d = 1/(2-alpha) ;
  }

  bb1_tree::bb1_tree(const bb1_tree& w)  : st(BB1_STACKSIZE)
  { 
    bb1_item p;
    bb1_item l=0;
    anzahl=w.anzahl;
    alpha=w.alpha;
    d=w.d;
    iterator=0;
    if (w.root)
    { if (!w.root->blatt())
      { p=new bb1_node(w.root);
        first=copytree(p,w.root,l); 
        first->sohn[left]=l;
        l->sohn[right]=first; }
      else
      { p=new bb1_node(w.root);
        first=p; }
      root= p;                      }
    else root = 0;
  }

// -------------------------------------------------------------
// locate()
// liefert item mit key(item) >= y und
//              und key(item) <= key(it) fuer alle it
//                                       mit key(it) >= y
//         0, falls nicht existiert

bb1_item bb1_tree::locate(GenPtr y)  const

{ DPRINT(("locate %d in Baum %d\n",int(y),int(this)));
  bb1_item current;
  if (root==0) return 0;  // by s.n
  current=root;
  while (!current->blatt())
   { DDUMP(("current %d\n",int(current->key)));
     if (cmp(y,current->key)<=0) current=current->sohn[left];
     else current=current->sohn[right];   }
  return (cmp(y,current->key)<=0) ? current : 0 ;
}  

// -------------------------------------------------------------
// located()
// liefert item mit key(item) <= y und
//              und key(item) >= key(it) fuer alle it
//                                       mit key(it) <= y

bb1_item bb1_tree::located(GenPtr y)  const

{ bb1_item current;
  if (root==0) return 0;  // by s.n
  current=root;
  while (!current->blatt())
   if (cmp(y,current->key)<=0) current=current->sohn[left];
   else current=current->sohn[right];
  if (cmp(y,current->key)==0) return current;
  current = current->sohn[left];
  return (cmp(y,current->key)>=0) ? current : 0 ;
}  

// -------------------------------------------------------------
// lookup()
// liefert item mit key(item)=y , falls existiert
//                            0 , sonst

bb1_item bb1_tree::lookup(GenPtr y) const

{ bb1_item current = locate(y);
  if (current==0) return 0;
  return (cmp(y,current->key)==0) ? current : 0;
}  

// -------------------------------------------------------------  
// member()
// liefert 1 , falls item existiert mit key(item)=y
//         0 , sonst

bool bb1_tree::member(GenPtr y) const

{ bb1_item current = locate(y);
  if (current==0) return false;
  return ( cmp(y,current->key) == 0 );
}  

// ------------------------------------------------------------
// translate()
// liefert inf(item) , falls item existiert mit item = locate(y) 
//                  0 , sonst

GenPtr bb1_tree::translate(GenPtr y)

{ bb1_item current = locate(y);
  if (current==0) return 0;
  return (cmp(y,current->key)==0) ? current->inf : 0;
}  

// ------------------------------------------------------------
// change_obj()
// liefert item mit key(item) = y und setzt inf(item) auf x
//                                    , falls existiert
//         0                          , sonst

bb1_item bb1_tree::change_obj(GenPtr y,GenPtr x)

{ bb1_item current = lookup(y);
  if ( current != 0 )
  { current->inf = x ;  
    return current; }
  else return 0;
}  

// ------------------------------------------------------------
// search()
// nachher: st = ( pk ,..., p1 ) mit 
//          pk = locate(y) , p1 = root 
//          p1 , ... , pk ist Suchpfad nach y
// liefert inneren Knoten k mit key(k) = y , falls existiert
//         0                               , sonst

bb1_item bb1_tree::search(GenPtr y) const

{ DPRINT(("search %d in Baum %d\n",int(y),int(this)));
  st.clear();
  bb1_item current = root;
  bb1_item k = 0;

  if (!root) return 0;         // Baum leer
  while (!current->blatt())
  { DDUMP(("current %d\n",int(current->key)));
    int c = cmp(y,current->key);
    st.push(current);
    if (c<=0)
    { if (c==0) k=current;
      current = current->sohn[left];          }
    else
    { current = current->sohn[right];         }
						   }
  st.push(current);
  DDUMP(("Blatt %d\n",int(current->key)));
  return k;
}

// -----------------------------------------------------------
// ord()
// liefert item it mit
//              |{key(it') < key(it) | it' item im Baum}| = k-1
//         0 , falls kein solches item existiert
	
bb1_item bb1_tree::ord(int k) const

{ DPRINT(("ord %d\n",k));
  if (k>anzahl || k<=0) return 0;
  bb1_item cur=root;
  while (!cur->blatt())
  { DDUMP(("ord loop k=%d key=%d\n",k,int(cur->key)));
    int l=cur->sohn[left]->groesse();
    if (k>l)
    { k -= l;
      cur=cur->sohn[right];           }
    else cur=cur->sohn[left]; 
  }
  return cur;
}
    
// ------------------------------------------------------------
// move_iterator()
// bewegt Iterator eine Stelle weiter
// falls am Ende , Iterator = 0

bb1_item bb1_tree::move_iterator() const

  { 
    if (!root) 
    { iterator = 0;
      return 0;      }
    
    if (!iterator) iterator = first;
    else if (iterator->sohn[right]==first) iterator = 0;
	 else iterator = iterator->sohn[right];
    return iterator;
  }
    
// ------------------------------------------------------------
// lrot() , rrot() , ldrot() , rdrot()
// Rotationen am Knoten p

void bb1_tree::lrot(bb1_item p, bb1_item q)
{ DDUMP(("lrot p=%d \n",int(p->key)));
  bb1_item h = p->sohn[right];
  p->sohn[right] = h->sohn[left];
  h->sohn[left] = p;
  if (!q) root=h;
  else
    if (cmp(p->key,q->key)>0) q->sohn[right]=h;
    else q->sohn[left]=h;
  p->gr=p->sohn[left]->groesse()+p->sohn[right]->groesse();
  h->gr=p->groesse()+h->sohn[right]->groesse();
}

void bb1_tree::rrot(bb1_item p, bb1_item q)
{ DDUMP(("rrot p=%d\n",int(p->key)));
  bb1_item h = p->sohn[left];
  p->sohn[left] = h->sohn[right];
  h->sohn[right] = p;
  if (!q) root=h;
  else
  { if (cmp(p->key,q->key)>0) q->sohn[right] = h;
    else q->sohn[left] = h; }
  p->gr=p->sohn[left]->groesse()+p->sohn[right]->groesse();
  h->gr=p->groesse()+h->sohn[left]->groesse();
}

void bb1_tree::ldrot(bb1_item p, bb1_item q)
{ DDUMP(("ldrot p=%d\n",int(p->key)));
  bb1_item h = p->sohn[right];
  bb1_item g = h->sohn[left];
  p->sohn[right] = g->sohn[left];
  h->sohn[left] =  g->sohn[right];
  g->sohn[left] = p;
  g->sohn[right] = h;
  if (!q) root=g;
  else
  { if (cmp(p->key,q->key)>0) q->sohn[right] =g ;
    else q->sohn[left] = g ; }
  p->gr=p->sohn[left]->groesse()+p->sohn[right]->groesse();
  h->gr=h->sohn[left]->groesse()+h->sohn[right]->groesse();
  g->gr=p->groesse()+h->groesse();
}

void bb1_tree::rdrot(bb1_item p, bb1_item q)
{ DDUMP(("rdrot p=%d\n",int(p->key)));
  bb1_item h = p->sohn[left];
  bb1_item g = h->sohn[right];
  p->sohn[left] = g->sohn[right];
  h->sohn[right] =  g->sohn[left];
  g->sohn[right] = p;
  g->sohn[left] = h;
  if (!q) root=g;
  else
  { if (cmp(p->key,q->key)>0) q->sohn[right] =g ;
    else q->sohn[left] = g ; }
  p->gr=p->sohn[left]->groesse()+p->sohn[right]->groesse();
  h->gr=h->sohn[left]->groesse()+h->sohn[right]->groesse();
  g->gr=p->groesse()+h->groesse();
}
 
// ------------------------------------------------------------------
// sinsert()
// fuegt ein neues Item (y,x) in den Baum ein
//                 , falls noch kein Item it vorhanden mit key(it)=y
// change_obj(y,x) ,sonst
// fuellt Keller mit zu rebalancierenden Knoten 
// gibt eingefuegtes Blatt zurueck

      bb1_item bb1_tree::sinsert(GenPtr y,GenPtr x)
      { DPRINT(("sinsert %d [%d] in Baum %d\n",int(y),int(x),int(this)));
	bb1_item inserted;
	bb1_item help;

	if (!alpha) LEDA_EXCEPTION(5,"alpha nicht gesetzt");
	if (iterator)  LEDA_EXCEPTION(6,"insert while tree listed");

	st.clear();                           // loesche Suchpfad

	if (!root) { DDUMP(("Baum war leer\n"));
		     bb1_item p=new  bb1_node(y,x);
		     p->sohn[left]=p;
		     p->sohn[right]=p;
		     root=p; 
		     first=p;
		     first->sohn[left]=first;
		     first->sohn[right]=first;
		     anzahl=1; 
		     inserted = p;
		   }
	else
	  if (root->blatt())
	    if (cmp(y,root->key)<0)
	    { DDUMP(("links von Wurzel\n"));
	      bb1_item p=new bb1_node(y,x,Leaf,root,root);
	      DDUMP(("Blatt[%d] %d %d %d %d\n",p,p->key,p->inf,p->sohn[left],p->sohn[right]));
	      root->sohn[left]=p;
	      root->sohn[right]=p;
	      bb1_item s=new bb1_node(y,x,Node,p,root);
	      DDUMP(("Knoten[%d] %d %d %d %d\n",s,s->key,s->inf,s->sohn[left],s->sohn[right]));
	      first=p;
	      root=s;
	      st.push(s);
	      anzahl++; 
	      inserted = p;
	     }
	    else if (cmp(y,root->key)>0)         // hier rechts einfuegen
		 { DDUMP(("rechts von Wurzel\n"));
		   bb1_item p=new bb1_node(y,x,Leaf,root,root);
		   root->sohn[left]=p;
		   root->sohn[right]=p;
		   bb1_item s=new bb1_node(root->key,root->inf,Node,root,p);
		   root=s;
		   st.push(s);
		   anzahl++;
		   inserted = p;
		  }
		 else { DPRINT(("gleicher Schluessel vorhanden\n"));
			root->inf = x;
			inserted = root;
		       }
	  else                           // root ist innerer Knoten
	  { bb1_item father;
	    search(y);                   // fuelle Suchstack 
	    bb1_item t=st.pop();
	    father = st.top();
					 // einfuegen
	    if (cmp(y,t->key)<0)
	    { DDUMP(("insert links von Blatt\n"));
	      help = t->sohn[left];
	      bb1_item  p = new bb1_node(y,x,Leaf,help,t);
	      t->sohn[left]=p;
	      if (first==t) first=p;
	      help->sohn[right]=p;
	      bb1_item s=new bb1_node(y,x,Node,p,t);
	      if (cmp(s->key,father->key)<=0) father->sohn[left]=s;
	      else father->sohn[right]=s;
	      anzahl++; 
	      inserted = p;
	      st.push(s);
	    }
	    else if (cmp(y,t->key)>0)     
		 { DDUMP(("insert rechts von Blatt -> neues Maximum\n"));
		   help=t->sohn[right];
		   bb1_item p=new bb1_node(y,x,Leaf,t,help);
		   t->sohn[right]=p;
		   help->sohn[left]=p;
		   bb1_item s=new bb1_node(t->key,t->inf,Node,t,p);

		   father->sohn[right] = s;
		   anzahl++;
		   inserted = p;
		   st.push(s);
		 }
		 else { DPRINT(("gleicher Schluessel vorhanden\n"));
			t->inf = x;
			st.clear();     // keine Rebalancierung notwendig
			inserted = t;
		      }
	  }
       return inserted;
     }


// ------------------------------------------------------------------
// insert()
// fuegt ein neues Item (y,x) in den Baum ein
//       mittels sinsert
// balanciert Baum mit ueblichen Rotationen
// gibt eingefuegtes Blatt zurueck

bb1_item bb1_tree::insert(GenPtr y, GenPtr x)
{ 
  bb1_item inserted;
  bb1_item t,father;

/*
  copy_key(y);
  copy_inf(x);
*/

  inserted = sinsert(y,x);
  if (!st.empty())
    st.pop();                       // pop father of leaf
  
				    // rebalancieren
  while (!st.empty())
  { t=st.pop();
    father = st.empty() ? 0 : st.top();
    t->gr++;  
    float i = t->bal();
    DDUMP(("rebal cur=%d groesse=%d bal=%f\n",int(t->key),t->groesse(),i));
    if (i < alpha)
      if (t->sohn[right]->bal()<=d) lrot(t,father);
      else ldrot(t,father);
    else if (i>1-alpha) 
         { if (t->sohn[left]->bal() > d ) rrot(t,father);
	   else rdrot(t,father);
          }
  }
  DDUMP(("eingefuegtes Blatt hat key %d und inf %d\n",int(inserted->key),int(inserted->inf)));
  return inserted;
}

// ------------------------------------------------------------------
// sdel()
// loescht Item it im Baum mit key(it)=y , falls existiert
//         und gibt Zeiger auf it zurueck
// 0 , sonst
// fuellt Keller mit zu rebalancierenden Knoten

bb1_item bb1_tree::sdel(GenPtr y)
{ DPRINT(("delete %d aus Baum %d\n",int(y),int(this)));

  if (!alpha) LEDA_EXCEPTION(5,"alpha nicht gesetzt");
  if (iterator)  LEDA_EXCEPTION(6,"Baum gelistet beim Loeschen");

  st.clear();
  if (root==0) return 0;                         // s.n.

  if (root->blatt())                             // Wurzel loeschen
    if (cmp(y,root->key)==0)
      { DDUMP(("Wurzel loeschen\n"));
        bb1_item p = root;
        first=iterator=0;
        anzahl=0; 
        root=0; 
        return p; 
       }
    else 
      { DPRINT(("Element nicht im Baum\n"));  
        return 0; }
  else 
  { bb1_item p,father;
    bb1_item pp=search(y);

    if (st.size()==2)                            // Sohn der Wurzel
    { DDUMP(("Sohn der Wurzel loeschen\n"));
      p=st.pop();
      father=st.pop();

      int v1 = cmp(y,father->key);
      if (cmp(y,p->key)!=0) { DPRINT(("Element nicht im Baum\n"));
                                return 0; }
      anzahl--;

      if (v1<=0)
        root=root->sohn[right];
      else
        root=root->sohn[left];

      if (!root->blatt())
       { if (first==p) first=p->sohn[right];
         first->sohn[left]=p->sohn[left];
         (first->sohn[left])->sohn[right]=first;
        }
      else
       { first=root;
         root->sohn[left]=root;
         root->sohn[right]=root ;
        }

      st.push(father);
      return p; 
    }
    else                                // Blatt mit Tiefe >= 2     
    { bb1_item q=st.pop();

      if (cmp(y,q->key)!=0)
      { DDUMP(("Schluessel nicht vorhanden\n"));
	return 0;   }

      bb1_item p = st.pop();
      father=st.top();
      DDUMP(("Blatt %d mit Vater %d , Grossvater %d\n",int(q->key),int(p->key),int(father->key)));
      int v2 = cmp(y,p->key);
      int v1 = cmp(y,father->key);
      anzahl--;
      if (v1<=0)
        if (v2<=0)
        { father->sohn[left]=p->sohn[right];
          if (first==q) { first=first->sohn[right];
			  q->sohn[right]->sohn[left]=first; }
	}
        else father->sohn[left]=p->sohn[left];
      else if (v2<=0) father->sohn[right]=p->sohn[right];
           else  father->sohn[right]=p->sohn[left];
      q->sohn[right]->sohn[left]=q->sohn[left];
      q->sohn[left]->sohn[right]=q->sohn[right];
      if ( pp && (p!=pp) && p->sohn[left] )
      { pp->key = q->sohn[left]->key ;
        DPRINT(("inneren Knoten mit %d ueberschrieben und Info %d bleibt\n",pp->key,pp->inf));
      }
      st.push(p);
      return q;
    }
  }
#if defined(__GNUG__)
  return 0; // never reached ?
#endif
}

// ------------------------------------------------------------------
// del()
// loescht Item it im Baum mit key(it)=y , falls existiert
//         und gibt Zeiger auf it zurueck
// 0 , sonst
// mittels sdel                                     
// rebalanciert Baum danach

bb1_item bb1_tree::del(GenPtr y)
{
  bb1_item p,father;
  bb1_item deleted = sdel(y);
  if (!deleted)
    return 0;
  if (!st.empty())
    delete(st.pop());

					 // rebalancieren
  while (!st.empty())
  { p = st.pop();
    father = st.empty() ? 0 : st.top() ;
    p->gr--;              
    float i=p->bal();
    DDUMP(("rebal cur=%d groesse=%d bal=%f\n",int(p->key),p->groesse(),i));
    if (i<alpha)
      if (p->sohn[right]->bal() <= d) lrot(p,father);
      else ldrot(p,father);
    else if (i>1-alpha)
         { if(p->sohn[left]->bal() > d) rrot(p,father);
           else rdrot(p,father);
          }
  }
  return deleted;
} 

// -----------------------------------------------------------------
// Gleichheitsoperator
// weist this Kopie der Baumes w zu

bb1_tree& bb1_tree::operator=(const bb1_tree& w)
{ DDUMP(("operator = wurzel%d\n",w.root->key));
  bb1_item p;
  bb1_item l=0;
  if (anzahl!=0) deltree(root); 
  st.clear();
  anzahl=w.anzahl;
  alpha=w.alpha;
  d=w.d;
  iterator=0;
  if (w.root)
  { if (!w.root->blatt())
    { p=new bb1_node(w.root);
      first=copytree(p,w.root,l);
      first->sohn[left]=l ;
      l->sohn[right]=first ; }
    else
    { p=new bb1_node(w.root);
      first=p; }
    root= p;                       }
  else root = 0;
  DDUMP(("root=%d, first=%d\n",int(root->key),int(first->key)));
  return *this;
}

bb1_item bb1_tree::copytree(bb1_item p, bb1_item q,bb1_item& ll) 
{ DDUMP(("copytree %d\n",p->key));
  bb1_item a;
  bb1_item r;
  bb1_item s;
  if (p->blatt())
  { if (ll==0) p->sohn[left]=0;
    else
    { p->sohn[left]=ll;
      ll->sohn[right]=p; }
    p->sohn[right]=0;
    a=p;
    ll=p; 
    DDUMP(("ll gesetzt %d\n",int(ll->key))); }
  else {
    r=new bb1_node(q->sohn[left]);
    p->sohn[left]=r;
    a=copytree(p->sohn[left],q->sohn[left],ll);
    s=new bb1_node(q->sohn[right]);
    p->sohn[right]=s;
    copytree(p->sohn[right],q->sohn[right],ll); }
  return a; 
}

// ------------------------------------------------------------------
// Destruktoren

void bb1_tree::clear()
{ if (root)
  { DPRINT(("clear %d\n",int(root->key)));
    deltree(root);
   }
  else DPRINT(("clear\n"));
  root=0;
  anzahl=0;
  first=0;
}

void bb1_tree::deltree(bb1_item p)
{ if (p)
  { DDUMP(("deltree : current=%d\n",int(p->key)));
    if (!p->blatt())
    {  deltree(p->sohn[left]);
       deltree(p->sohn[right]); 
     }
    delete(p);
  }
}


void bb1_tree::draw(DRAW_BB_NODE_FCT draw_node,
                   DRAW_BB_EDGE_FCT draw_edge, 
                   bb1_node* r, 
                   double x1, double x2, double y, 
                   double ydist, double last_x)
{ 
 double x = (x1+x2)/2;

 if (r==nil) return;

 if (last_x != 0) draw_edge(last_x,y+ydist,x,y);

 draw_node(x,y,r->key);

 if (!r->blatt()) 
 { draw(draw_node,draw_edge,r->sohn[0],x1,x,y-ydist,ydist,x);
   draw(draw_node,draw_edge,r->sohn[1],x,x2,y-ydist,ydist,x);
  }
}


void bb1_tree::draw(DRAW_BB_NODE_FCT draw_node,
                   DRAW_BB_EDGE_FCT draw_edge, 
                   double x1, double x2, double y, double ydist)
{ draw(draw_node,draw_edge,root,x1,x2,y,ydist,0); }

LEDA_END_NAMESPACE
