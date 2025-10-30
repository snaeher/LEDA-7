/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _iv_tree.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


// -------------------------------------------------------------------
//
//  Interval Trees
//
//  Michael Seel   (1990/91)
//
// Implementation as described in
// Kurt Mehlhorn: Data Structures and Algorithms 3, section VIII.5.1.1
//
// -------------------------------------------------------------------

#include <LEDA/geo/iv_tree.h>

LEDA_BEGIN_NAMESPACE 


// -------------------------------------------------------------
// compare-Funktion zu Dictionary
// -------------------------------------------------------------

/*
int compare (const interval_item& p, const interval_item& q)
{ int a = p->cmp(p->koo1,q->koo1);
  return (a) ? a : p->cmp(p->koo2,q->koo2);
 }
*/



// -------------------------------------------------------------
// member functions von iv_tree
// -------------------------------------------------------------

// -------------------------------------------------------------
// lrot() , rrot() , ldrot() , rdrot()
// Rotationen am Knoten p

void iv_tree::lrot(iv_item p, iv_item q)

// p ist der zentrale Knoten um den rotiert wird
// q ist der Vater von p

{ 
  iv_item h = p->son[1];
  p->son[1] = h->son[0];
  h->son[0] = p;
   
  if (!q) root=h;
  else
  {
   if ( p == q->son[0] )   q->son[0]=h;
   else q->son[1]=h;
  }; 
  
  p->gr=p->son[0]->groesse()+p->son[1]->groesse();
  h->gr=p->groesse()+h->son[1]->groesse();

  reorganize_nodelist(h,p);
}

void iv_tree::rrot(iv_item p, iv_item q)

// p ist der zentrale Knoten um den rotiert wird
// q ist der Vater von p

{ 
  iv_item h = p->son[0];
  p->son[0] = h->son[1];
  h->son[1] = p;

  if (!q) root=h;
  else
  {
   if ( p == q->son[0] ) q->son[0] = h;
   else q->son[1] = h; 
  };

  p->gr=p->son[0]->groesse()+p->son[1]->groesse();
  h->gr=p->groesse()+h->son[0]->groesse();

  reorganize_nodelist(h,p);
}

void iv_tree::ldrot(iv_item p, iv_item q)
{ 
  iv_item h = p->son[1];
  //iv_item t = h->son[0];
  rrot(h,p);
  lrot(p,q);
}

void iv_tree::rdrot(iv_item p, iv_item q)
{
  iv_item h = p->son[0];
  //iv_item t = h->son[1];
  lrot(h,p);
  rrot(p,q);
}
 
// ------------------------------------------------------------------
// reorganize_nodelist()
// reorganisiert die nach der Rotation falsch mit Intervallen belegten
// Nodelists entsprechend neu.

void iv_tree::reorganize_nodelist(iv_item father, iv_item son)
{
  // nach Aendern des Dictionary-Typs in perfekte BB_trees mit Konstruk-
  // tionsprozedur, die aus einer Liste von n geordneten Knoten ein neu-
  // es Dictionary in Zeit O(n) erzeugt, werden von den folgenden 4 Ite-
  // rationen je 2 in ein Misch_Sort Verfahren abgeaendert und die so
  // auf father und son verteilten Intervalle in Linearzeit in die Kno-
  // ten dictionarys eingebunden

  dic_item it;
  nodelist_p fx = new nodelist;
  nodelist_p fy = new nodelist;
  nodelist_p sx = new nodelist;
  nodelist_p sy = new nodelist;

  forall_items(it,*(father->x_nodelist())) // father->x_nodelist
		 			       // liefert Pointer
		 			       // auf x-Dictionary
      {
	 if (split_in_x_interval(father , x_nodelist(father)->key(it)))
	    fx->insert(x_nodelist(father)->key(it),x_nodelist(father)->inf(it));
         else
	    sx->insert(x_nodelist(father)->key(it),x_nodelist(father)->inf(it));
      };
  // die x-Knotenliste des Vaters wird durchsucht und alle passenden
  // Intervalle der Vater-Knotenliste zugeordnet, Rest an den Sohn

  forall_items(it,*(son->x_nodelist())) // son->x_nodelist
		         		    // liefert Pointer
				            // auf x-Dictionary
      {
	 if (split_in_x_interval(father , x_nodelist(son)->key(it))) 
	    fx->insert(x_nodelist(son)->key(it),x_nodelist(son)->inf(it));
         else
	    sx->insert(x_nodelist(son)->key(it),x_nodelist(son)->inf(it));
      };
  // die x-Knotenliste des Sohns wird durchsucht und alle passenden
  // Intervalle der Vater-Knotenliste zugeordnet, Rest an den Sohn


  forall_items(it,*(father->y_nodelist())) // father->y_nodelist
		        	               // liefert Pointer
				               // auf y-Dictionary
      {
	 if (split_in_y_interval(father , y_nodelist(father)->key(it)))
	    fy->insert(y_nodelist(father)->key(it),y_nodelist(father)->inf(it));
         else
	    sy->insert(y_nodelist(father)->key(it),y_nodelist(father)->inf(it));
      };
  // die y-Knotenliste des Vaters wird durchsucht und alle passenden
  // Intervalle der Vater-Knotenliste zugeordnet, Rest an den Sohn

  forall_items(it,*(son->y_nodelist())) // son->y_nodelist
					    // liefert Pointer
					    // auf y-Dictionary
      {
	 if (split_in_y_interval(father, y_nodelist(son)->key(it)))
	    fy->insert(y_nodelist(son)->key(it),y_nodelist(son)->inf(it));
         else
	    sy->insert(y_nodelist(son)->key(it),y_nodelist(son)->inf(it));
      };
  // die y-Knotenliste des Sohns wird durchsucht und alle passenden
  // Intervalle der Vater-Knotenliste zugeordnet, Rest an den Sohn

  delete x_nodelist(father);
  delete y_nodelist(father);
  delete x_nodelist(son);
  delete y_nodelist(son);
  // alte Dictionaries werden geloescht und freigegeben
  father->x_nl = fx;
  father->y_nl = fy;
  son->x_nl = sx;
  son->y_nl = sy;
  // Knotenlisten werden mit neu angeordneten Dictionaries intialisiert
}


// ------------------------------------------------------------
// search()
// nachher: st = ( pk ,..., p1 ) mit 
//          pk = locate(y) , p1 = root 
//          p1 , ... , pk ist Suchpfad nach y
// liefert inneren Knoten k mit key(k) = y , falls existiert
//         0                               , sonst

iv_item iv_tree::search(split_item x)

{ 
  st.clear();
  iv_item p = root;
  iv_item searched = 0;

  if (!root) return 0;         // Baum leer
  while (!p->blatt())
  { 
    if (cmp(x,split_value(p))<=0)
    {
      if (cmp(x,split_value(p))==0) searched = p;
      st.push(p);
      p = p->son[0];
    }
    else
    {
      st.push(p);
      p = p->son[1];
    }
  }
  st.push(p);
  return searched;
}


// ------------------------------------------------------------------
// iv_insert(x_typ,x_typ)
// fuegt ein neues Intervall (x,y) in den Baum ein
// gibt Zeiger auf Knoten, in dessen Knotenliste Intervall
// eingefuegt wurde, zurueck.

iv_item iv_tree::iv_insert(x_typ x, x_typ y)
{
 if (y<x)
 {
  LEDA_EXCEPTION(0,"kein Intervall zum einfuegen");
  return 0;
 }
 else
 {
  interval_item x_iv = new interval(x,y);
  interval_item y_iv = new interval(y,x);
  return ins(x_iv,y_iv,++interval_nb);
 }
}

// ------------------------------------------------------------------
// ins(interval_item,interval_item,int)
// fuegt den entsprechenden Knoten in die Grundstruktur mit dem 
// entsprechenden split_value und fuegt die beiden Intervalle am
// entsprechenden Knoten in die Knotenlisten

iv_item iv_tree::ins(interval_item x, interval_item y, int lfnr) 
{
 iv_item p;
 iv_item t;
 iv_item father;
 split_pair x_pair(x->koo1,-lfnr);
 split_item x_split = &x_pair; 
 
 if (!root)                                     // neuer Baum
 { 
  root = new iv_node(x_split);
  anzahl=1;
 }
 else     // Baum existiert schon !!
  if (root->blatt())
  {
    if (cmp(x_split,split_value(root))<0)
    {
     iv_item help = new iv_node(x_split);
     root = new iv_node(x_split,is_node,help,root);
     if (x_split->key1==split_value(root->son[1])->key1)
      nodelist_swap(root , root->son[1]);
     anzahl++;
    }
    else
     if (cmp(x_split,split_value(root))>0)
     {
      iv_item help = new iv_node(x_split);
      root = new iv_node(split_value(root),is_node,root,help);
      nodelist_swap(root , root->son[0]);
      anzahl++;
     }
  }
  else // in Knoten von nicht trivialem Baum wird eingefuegt
  {
    search(x_split);
    p = st.pop();
    father = st.top();
  
    if(cmp(x_split,split_value(p))<0)
    {
     iv_item help = new iv_node(x_split);
     p = new iv_node(x_split,is_node,help,p);
     // neuer Knoten in p mit split x, x-Blatt links, p-Blatt rechts
     if (x_split->key1==split_value(p->son[1])->key1)
      nodelist_swap(p , p->son[1]);

     if (cmp(split_value(p),split_value(father))<=0)
       father->son[0] = p;
     else
       father->son[1] = p;
     anzahl++;
    }
    else
     if (cmp(x_split,split_value(p))>0)
     {           
      iv_item help = new iv_node(x_split);
      p = new iv_node(split_value(p),is_node,p,help);
      // neuer Knoten in p mit split p, p-Blatt links, x-Blatt rechts
      nodelist_swap(p , p->son[0]);

      if (cmp(split_value(p),split_value(father))<=0)
        father->son[0] = p;
      else
        father->son[1] = p;
      anzahl++;
     }
  }

 while (!st.empty()) // rebalancieren
 { 
    t=st.pop();
    father = st.empty() ? 0 : st.top();
    t->gr++;  
    float i = t->bal();
    if (i < alpha)
    {
     if (t->son[1]->bal()<=d)
       lrot(t,father);
     else
       ldrot(t,father);
    }
    else
    if (i>1-alpha) 
    {
      if (t->son[0]->bal()>d)
	rrot(t,father);
      else
	rdrot(t,father);
    }
  }
  
  p = sink(root,x,y,lfnr);
  return p;	
}


// ------------------------------------------------------------------
// sink()
// laesst Intervall im Baum bis zu dem Knoten v abwaerts gleiten an dem 
// gilt: 1.Komponente von split_value(v) <in> Intervall <Teilmenge von> x_range(v)

iv_item iv_tree::sink(iv_item v, interval_item x, interval_item y, int lfnr)
{
   if (v)
   {
     if (split_in_x_interval(v,x))
     {
       if (x_nodelist(v)->lookup(x))
       {
         LEDA_EXCEPTION(0,"Interval wurde schon eingefuegt!\n");
	 split_pair h(x->koo1,-lfnr);
	 split_item hi=&h;
	 del(hi);
	 return 0;
       }
       x_nodelist(v)->insert(x,lfnr);
       y_nodelist(v)->insert(y,lfnr);
       return v;
     }
     else
     {
       if (x->cmp(x->koo2,split_value(v)->key1) < 0)
       { 
	 return sink(v->son[0],x,y,lfnr);
       }
       else
       if (x->cmp(split_value(v)->key1,x->koo1) < 0)
       {
	 return sink(v->son[1],x,y,lfnr);
       }
     }
   }

   return 0;
}

// ------------------------------------------------------------------
// iv_delete(x_typ,x_typ)
// loescht ein Intervall (x,y) aus dem Baum
// gibt Zeiger auf geloeschtes intervall zurueck 

void iv_tree::iv_delete(x_typ x, x_typ y)
{
  if (y<x) 
  {
   LEDA_EXCEPTION(0,"kein Intervall!\n");
   return;
  }
  else
  if (!root) LEDA_EXCEPTION(0,"Baum ist leer!\n");
  else
  {
   iv_item v = root;
   interval xi(x,y);
   interval yi(y,x);
   interval_item x_iv = &xi;
   interval_item y_iv = &yi; 
   while ( !split_in_x_interval(v,x_iv) && !v->blatt() )
   {
    if (split_value(v)->cmp(y,split_value(v)->key1) < 0)
    {
      v = v->son[0];
    }
    if (split_value(v)->cmp(split_value(v)->key1,x) < 0) 
    {
      v = v->son[1];
    }
   }
   // nun ist v der Knoten an dem das Intervall [x,y] abgespeichert sein muesste
   dic_item help1 = x_nodelist(v)->lookup(x_iv);
   dic_item help2 = y_nodelist(v)->lookup(y_iv);
   if (!(help1 && help2))
   {
    LEDA_EXCEPTION(0,"Intervall nicht vorhanden!\n");
    return;
   }
   else
   {
    split_pair s(x_nodelist(v)->key(help1)->koo1,-x_nodelist(v)->inf(help1));
    split_item search_split = &s; 
    x_nodelist(v)->del_item(help1);
    y_nodelist(v)->del_item(help2);
    if (!del(search_split)) 
      LEDA_EXCEPTION(1,"Intervall geloescht, Grundstruktur nicht in Ordnung\n");
   } 
  }
}


// ------------------------------------------------------------------
// del()
// loescht Item it im Baum mit split(it)=y , falls existiert
// gibt 1 zurueck, falls existent und geloescht
// gibt 0 zurueck, falls nicht existent

int iv_tree::del(split_item y)
{
  st.clear();
  if (root==0) return 0;                         // s.n.

  if (root->blatt())                             // Wurzel loeschen
    if (cmp(y,split_value(root))==0)
    { 
      if (!x_nodelist(root)->empty()) 
	 LEDA_EXCEPTION(1,"noch Intervalle in zu loeschender Wurzel!\n");
      anzahl=0; 
      delete root;
      root=0; 
      return 1; 
    }
    else
    {
     LEDA_EXCEPTION(0,"Element nicht im Baum\n");  
     return 0;
    }
  else // Baum nicht trivial
  {
    iv_item p,father;

    //iv_item pp=search(y);

    if (st.size()==2)                            // Sohn der Wurzel
    { 
      p=st.pop();
      father=st.pop();

      int v1 = cmp(y,split_value(father));
      if (cmp(y,split_value(p))!=0)
      {
        return 0;
      }
      anzahl--;
      if (v1<=0)
      {
        root=root->son[1];
	if(!x_nodelist(father)->empty())
	  nodelist_swap(father,root);
      }
      else
      {
        root=root->son[0];
	if(!x_nodelist(father)->empty())
	  {
	    if (!root->son[1]) 
	       nodelist_swap(father,root);
            else
	    if ((root->son[1])&&(x_nodelist(father)->size()==2))
            {
	       nodelist_swap(father,root->son[1]);
	       reorganize_nodelist(root,root->son[1]);
            }
            else
	       nodelist_swap(father,root->son[1]);
          } 
      }
      if (!x_nodelist(father)->empty() || !x_nodelist(p)->empty())
	LEDA_EXCEPTION(1,"Knotenlisten beim Loeschen nicht leer\n");
      delete father;
      delete p;
    }
    else                                // Blatt mit Tiefe >= 2     
    {
      iv_item p=st.pop();
      if (cmp(y,split_value(p))!=0)
      { 
	return 0;
      }
      iv_item q = st.pop();
      father = st.top();
      int v1 = cmp(y,split_value(father));
      int v2 = cmp(y,split_value(q));
      anzahl--;
      if (v1<=0)
        if (v2<=0)
	{
	  father->son[0]=q->son[1];
	  if(!x_nodelist(q)->empty())
	    nodelist_swap(q,father->son[1]);
        }
        else
	{
	  father->son[0]=q->son[0];
	  if(!x_nodelist(q)->empty())
	  {
	    if (!father->son[0]->son[1])
	       nodelist_swap(q,father->son[0]);
            else
	    if ((father->son[0]->son[1])&&(x_nodelist(q)->size()==2))
            {
	       nodelist_swap(q,father->son[0]->son[1]);
	       reorganize_nodelist(father->son[0],father->son[0]->son[1]);
            }
            else
	       nodelist_swap(q,father->son[0]->son[1]);
          } 
        }
      else
	if (v2<=0)
	{
	  father->son[1]=q->son[1];
	  if(!x_nodelist(q)->empty())
	    nodelist_swap(q,father->son[1]);
        }
        else
	{
	  father->son[1]=q->son[0];
	  if(!x_nodelist(q)->empty())
	  {
	    if (!father->son[1]->son[1])
	       nodelist_swap(q,father->son[1]);
            else
	    if ((father->son[1]->son[1])&&(x_nodelist(q)->size()==2))
            {
	       nodelist_swap(q,father->son[1]->son[1]);
	       reorganize_nodelist(father->son[1],father->son[1]->son[1]);
            }
            else
	       nodelist_swap(q,father->son[1]->son[1]);
          } 
        }
      if (!x_nodelist(p)->empty())
	LEDA_EXCEPTION(1,"Knotenlisten von p beim Loeschen nicht leer");
      delete p;
      delete q;
    }
  }
  
  // REBALANCIEREN
  iv_item p;
  iv_item father;
  while (!st.empty())
  { p = st.pop();
    father = st.empty() ? 0 : st.top() ;
    p->gr--;              
    float i=p->bal();
    if (i<alpha)
      if (p->son[1]->bal() <= d)
       lrot(p,father);
      else
       ldrot(p,father);
    else
    if (i>1-alpha)
    { if(p->son[0]->bal() > d)
        rrot(p,father);
      else
        rdrot(p,father);
     }
  }
  return 1;
}


// ------------------------------------------------------------------
// iv_query(x_typ,x_typ)
// gibt alle Intervalle in einer Liste zurueck, die das mit 
// den Parametern uebergebene Intervall schneiden.

interval_list iv_tree::iv_query(x_typ x, x_typ y)
{
 interval_list query_list;
 if (!root)
 {
   LEDA_EXCEPTION(0,"Baum ist leer");
   return query_list;
 } 
 if (y<x)
 { 
  LEDA_EXCEPTION(0,"kein Intervall\n");
  return query_list;
 }
 split_pair xp(x,-MAXINT);
 split_pair yp(y,MAXINT);
 split_item xi = &xp;
 split_item yi = &yp;
 iv_item v = root;
 int done=0;
 // Bestimmung der P-Menge mit Hilfe von search-Schleife nach x,
 // unterwegs Abzweig zu search nach y
 // alle zwischen den Pfaden liegenden Knoten werden mit get_all
 // abgearbeitet
 // jeder Knoten wird mit Hilfe von check_iv() nach den in den 
 // Knotenmengen vorhandenen Intervallen, die die Schnittbegingung
 // erfuellen abgearbeitet.
 while (!v->blatt())
 {
   check_nodelist(query_list,v,x,y);
   if (cmp(xi,split_value(v))<=0) // xi <= split(v)
   {
     if ((cmp(yi,split_value(v))>0) && !done)
     {
       y_search(query_list,v->son[1],yi,x,y);
       done=1;
     }
     else
     {
       if (done)  // klappert den gesamten Unterbaum von v der C-Menge ab
         get_all_in_tree(query_list,v->son[1]);
     }
     v=v->son[0];
   }
   else  // xi > split(v)
   {
     v=v->son[1];
   }
 }
 check_nodelist(query_list,v,x,y);
 return query_list;
}

// ------------------------------------------------------------------
// y_search()
// verzweigt vom Knoten v aus, in einen neuen Suchpfad nach dem y-
// Wert des query-Intervalls und fuehrt auch dort die P-checks durch

void iv_tree::y_search(interval_list& il, iv_item v, split_item ys,
		       x_typ x, x_typ y)
{
 while (!v->blatt())
 {
   check_nodelist(il,v,x,y);
   if (cmp(ys,split_value(v))<=0) // ys <= split(v)
     v=v->son[0];
   else  // ys > split(v)
   {
     get_all_in_tree(il,v->son[0]);
     // klappert den gesamten Unterbaum von v der C-Menge ab
     v=v->son[1];
   }
 }
 check_nodelist(il,v,x,y);
}
  
// ------------------------------------------------------------------
// check_nodelist()
// ueberprueft entsprechend der in MEHLHORN III dargestellten Fall-
// unterscheidung fuer P-Knoten die jeweilige x- oder y-Knotenliste
// oder uebernimmt alle Eintraege.

void iv_tree::check_nodelist(interval_list& il, iv_item v, x_typ x, x_typ y)
{
  if ((split_value(v)->cmp(x,split_value(v)->key1)<=0)
  && (split_value(v)->cmp(split_value(v)->key1,y)<=0))
  // dann ist split(v) im query-Intervall, das damit alle Intervalle
  // der Knotenliste schneidet.
    take_all_iv(il,v);
  else
  if (split_value(v)->cmp(x,split_value(v)->key1)>0)
  // Intervall oberhalb des split(v)
    check_y_iv(il,v,x);
  else
  // Intervall unterhalb des split(v)
    check_x_iv(il,v,y);
}


// ------------------------------------------------------------------
// get_all_in_tree()
// uebernimmt alle im Unterbaum des Knotens v abgespeicherten Inter-
// valle in die uebergebene Liste
// entspricht einer Teilmenge der C-Menge

void iv_tree::get_all_in_tree(interval_list& il, iv_item v)
{
  take_all_iv(il,v);
  if (!v->blatt())
  { 
    get_all_in_tree(il,v->son[0]);
    get_all_in_tree(il,v->son[1]);
  }
}

// ------------------------------------------------------------------
// take_all_iv()
// uebernimmt an einem Knoten alle abgespeicherten Intervalle
// in die mituebergebene Liste.

void iv_tree::take_all_iv(interval_list& il, iv_item v)
{
  dic_item it;
  forall_items(it,*(v->x_nodelist()))
  {
    interval_item help = x_nodelist(v)->key(it);
    interval_item ii = new interval(help->koo1, help->koo2);
    il.append(ii);
  }
}


// ------------------------------------------------------------------
// check_y_iv()
// ueberprueft an einem Knoten alle Intervalle der y-Knotenliste
// auf Schnittbedingung mit dem uebergebenen x-Wert des query-
// Intervalls und haengt bei erfuellter Schnittbedingung das In-
// tervall an die mituebergebene Liste an.

  void iv_tree::check_y_iv(interval_list& il, iv_item v, x_typ x)
  { 
      if (!y_nodelist(v)->empty())
      {
	//dic_item maxit = y_nodelist(v)->max();
	dic_item maxit = y_nodelist(v)->last_item();
	if (split_value(v)->cmp(y_nodelist(v)->key(maxit)->koo1,x)>=0) 
	{
	  dic_item it = maxit;
	  int intersect = 1;
	  while(it && intersect) 
	  {
	    if (split_value(v)->cmp(y_nodelist(v)->key(it)->koo1,x)>=0) 
	    {
	     interval_item help = y_nodelist(v)->key(it); 
	     interval_item ii = new interval(help->koo2, help->koo1);
	     // die Intervalle der y-Knotenliste haben 1. Komponente
	     // y und 2. Komponente x, zur Rueckgabe ist aber die
	     // umgekehrte Reihenfolge verlangt.
	     il.append(ii);
	    }
	    else
	     intersect = 0;
	    it = y_nodelist(v)->pred_item(it);
          }
        }
      }
  }

// ------------------------------------------------------------------
// check_x_iv()
// ueberprueft an einem Knoten alle Intervalle der x-Knotenliste
// auf Schnittbedingung mit dem uebergebenen y-Wert des query-
// Intervalls und haengt bei erfuellter Schnittbedingung das In-
// tervall an die mituebergebene Liste an.

  void iv_tree::check_x_iv(interval_list& il, iv_item v, x_typ y)
  { 
      if (!x_nodelist(v)->empty())
      {
	//dic_item minit = x_nodelist(v)->min();
	dic_item minit = x_nodelist(v)->first_item();
	if (split_value(v)->cmp(x_nodelist(v)->key(minit)->koo1,y)<=0) 
	{
	  dic_item it = minit; 
	  int intersect = 1;
	  while(it && intersect) 
	  {
	    if (split_value(v)->cmp(x_nodelist(v)->key(it)->koo1,y)<=0) 
	    {
	     interval_item help = x_nodelist(v)->key(it); 
	     interval_item ii = new interval(help->koo1, help->koo2);
	     // die Intervalle der x-Knotenliste haben 1. Komponente
	     // x und 2. Komponente y.
	     il.append(ii);
	    }
	    else
	     intersect = 0;
	    it = x_nodelist(v)->next_item(it);
          }
        }
      }
  }


//-----------------------------------------------------------------------
// void print_split(iv_item) 
// gibt split_value von p aus

  void iv_tree::print_split(iv_item it)   
  { 
    if (it)
      { it->split_value()->print();
        if (it->blatt()) text(" (blatt)\n");
        else text("(Knoten)\n");
       }
    else
      cout << " Knoten leer!\n";
  }




//-----------------------------------------------------------------------
// void pr_iv_tree(iv_item p;int blancs)
// gibt den Baum mit Wurzel p aus

void iv_tree::pr_iv_tree(iv_item p,int blancs)
{
 if (p==0)
 {
  for (int j=1;j<=blancs;j++) cout << " ";
  cout << "NIL\n";
  return;
 }
 else
 {
  pr_iv_tree(p->son[0],blancs+10); 
  for (int j=1;j<=blancs;j++) cout << " ";
  
  cout << "(" << p->split_value()->key1 << "," << p->split_value()->key2;
  cout << ") ";
  pr_iv_list(p);
  pr_iv_tree(p->son[1],blancs+10); 
 }
}
 
//-----------------------------------------------------------------------
// void pr_iv_list(iv_item p)
// gibt die Intervalliste eines Knotens p aus

void iv_tree::pr_iv_list(iv_item p)
{
  dic_item it;
  forall_items(it,*(p->x_nodelist()))
  { cout << "[" << x_nodelist(p)->key(it)->koo1;
    cout << "," << x_nodelist(p)->key(it)->koo2 << "]"; 
    cout << "#" << x_nodelist(p)->inf(it) << ";";
   }
  cout << "  *  ";
  forall_items(it,*(p->y_nodelist()))
  { cout << "[" << y_nodelist(p)->key(it)->koo1;
    cout << "," << y_nodelist(p)->key(it)->koo2<< "]"; 
    cout << "#" << y_nodelist(p)->inf(it) << ";";
   }
  cout << "\n";
}

//-----------------------------------------------------------------------
// void pr_list()
// gibt die zurueckgegebene Intervalliste der query aus

void iv_tree::pr_list(interval_list& il)
{
  cout << "Liste: \n";
  if(il.empty()) cout << " leer\n";
  list_item it;
  forall_items(it,il)
    il.contents(it)->print(); 
}
    

//-----------------------------------------------------------------------
// Funktion fuer Destruktor
//-----------------------------------------------------------------------
  
void iv_tree::deltree(iv_item p)
{
 if (p)
 {
  if (!p->blatt())
  {
   deltree(p->son[0]);
   deltree(p->son[1]);
  }
  delete(p);
 }
}


LEDA_END_NAMESPACE 
