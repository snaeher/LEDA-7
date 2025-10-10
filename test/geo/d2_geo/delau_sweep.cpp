/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  delau_sweep.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#define LEDA_CHECKING_OFF

#include <LEDA/core/sortseq.h>
#include <LEDA/core/p_queue.h>
#include <LEDA/graph/graph.h>
#include <LEDA/geo/rat_kernel.h>
#include <LEDA/geo/rat_kernel_types.h>
#include <LEDA/geo/plane_sweep.h>
#include <LEDA/geo/random_rat_point.h>
#include <math.h>
#include <assert.h>

using namespace leda;

using std::cout;
using std::endl;



#define WAVE_PART rat_wave_part
  
struct WAVE_PART
{
  static POINT INFINITY1;
  static POINT INFINITY2;       
    
  POINT  pred;
  POINT  p;
  POINT  succ;
    
  WAVE_PART(const POINT pp) :  pred(pp), p(pp), succ(pp) {}
  WAVE_PART(const WAVE_PART& wp) :  pred(wp.pred), p(wp.p), succ(wp.succ) {}

  bool describes_circle_event()
  {
    return ( !identical(pred, WAVE_PART::INFINITY1) &&
	     !identical(succ, WAVE_PART::INFINITY2) &&
	     !identical(succ, pred)  && orientation(pred, p, succ) < 0 );
  }

  WAVE_PART* split(WAVE_PART* new_w)
  {
    WAVE_PART* w = new WAVE_PART(p);
    w->succ      = new_w->p;
    w->pred      = pred;
    pred         = new_w->p;
    new_w->succ  = new_w->pred = p;
    return w;
  }
    
  LEDA_MEMORY(WAVE_PART);
};
 
struct del_event_info
{
  POINT p;
  bool  is_site;
  int   refs;
  
  del_event_info() : is_site(false), refs(1) {}
  del_event_info(POINT pp) : p(pp), is_site(true), refs(1) {}
  del_event_info(const del_event_info& ev) 
    : p(ev.p), is_site(ev.is_site), refs(ev.refs) {}  
  
  LEDA_MEMORY(del_event_info);
};

  
inline ostream& operator << (ostream& os, const del_event_info& di) 
{
  if(di.is_site) cout << "site event : " << di.p;
  else           cout << "circle event : ";
  cout << " reference count : " << di.refs << endl;
  return os;
}

int compare(const CIRCLE& c1, const CIRCLE& c2);

typedef sortseq<CIRCLE, del_event_info>  XST;



inline ostream& operator << (ostream& os, const WAVE_PART& w)
{
  os << '[';
  if( identical(w.pred, WAVE_PART::INFINITY1) ) os << " -oo "; 
  else os << w.pred.to_point();
  os << " ---> " << w.p.to_point() << " ---> ";
  if( identical(w.succ, WAVE_PART::INFINITY2) ) os << " +oo "; 
  else  os << w.succ.to_point();
  os  << ']';
  return os;
}

int compare(WAVE_PART*const & w1, WAVE_PART*const & w2);
int compare_at_pos(const CIRCLE& p_sweep,  WAVE_PART*const & w1, 
		   WAVE_PART*const & w2);



typedef sortseq<WAVE_PART*, edge>         YST;
typedef list<POINT>                       LoP;
typedef GRAPH<POINT,int>                  GPI;
typedef plane_sweep<LoP, YST, XST, GPI  > DelaunaySweep;



POINT WAVE_PART::INFINITY1;     
POINT WAVE_PART::INFINITY2;     

int compare(const CIRCLE& c1, const CIRCLE& c2)
{
  if( c1.is_trivial() && c2.is_trivial() ) 
    return compare(c1.point1(), c2.point1());
  
  POINT p1 = c1.center();
  POINT p2 = c2.center();
  COORD s1 = c1.sqr_radius();
  COORD s2 = c2.sqr_radius();
  
  // c1 < c2 <=>  p1.xcoord() + sqrt(s1) < p2.xcoord() + sqrt(s2)
  //         <=>  p1.xcoord() - p2.xcoord() < sqrt(s2) - sqrt(s1)

  int c = compare(p1.xcoord(), p2.xcoord());

  if( c > 0 )                       //  p1.xcoord() - p2.xcoord() > 0
    if ( !(s2 > s1) )   return 1;   //  sqrt(s2) - sqrt(s1)       < 0
    else;
  else
    if( c < 0 )                     //  p1.xcoord() - p2.xcoord() < 0
      if ( !(s1 > s2) )  return -1; //  sqrt(s2) - sqrt(s1)       > 0
      else;
    else                            //  p1.xcoord() - p2.xcoord() = 0
      if ( s2 > s1 )     return -1; //  sqrt(s2) - sqrt(s1)       < 0
      else
        if ( s2 < s1 )   return 1;  //  sqrt(s2) - sqrt(s1)       > 0
        else                        //  sqrt(s2) - sqrt(s1)       = 0
          return compare(p1.ycoord(), p2.ycoord());  

  // sign(p1.xcoord() - p2.xcoord()) = sign(sqrt(s2) - sqrt(s1))
  // 1x square => 
  
  COORD A   = p1.xcoord() - p2.xcoord();
  COORD AA  = A*A;
  COORD B   = s1 + s2;
 
  int cc = compare(AA, B);
  if( cc > 0  )  return c;
  if( cc == 0 )  return compare(p1.ycoord(), p2.ycoord());
  
  // 2x square => 
  
  COORD C  = AA - B;
  COORD CC = C*C;
  COORD D  = 4*s1*s2;
  
  cc = compare( CC, D );
  
  if( cc > 0 )   return -c;
  if( cc == 0 )  return compare(p1.ycoord(), p2.ycoord());
  return c;
}


int compare_at_pos(const CIRCLE& c_sweep, WAVE_PART*const & w1, 
                   WAVE_PART*const & w2)
{
  POINT p_sweep = c_sweep.point1();
  
  if( w1 == w2 )                          return 0;
  if( identical(w2->succ, WAVE_PART::INFINITY2) )  return -1;
  if( identical(w1->succ, WAVE_PART::INFINITY2) )  return 1;
  
  WAVE_PART w(*w2);
  int c = -1;
  if( identical(p_sweep, w2->p) )
    {
      w=WAVE_PART(*w1);
      c = 1;
    }
  else 
    if ( !identical(w1->p, p_sweep)) 
      error_handler(1, "compare_at_pos: Precondition verletzt!");
  
  POINT p = w.p;
  POINT q = w.succ;
  
  int   lower_p = compare(p, q) < 0 ? 1 : -1;
  
  COORD A = p.xcoord() - p_sweep.xcoord();
  COORD B = q.xcoord() - p_sweep.xcoord();
  COORD C = q.ycoord() - p.ycoord();
  COORD D = p.xcoord() - q.xcoord();
  
  
  COORD x1 = p_sweep.ycoord() - p.ycoord();
  COORD x2;
  
  if ( C == 0 ) 
    if ( lower_p == -1 )
      {
        if ( x1 < 0 );
        else
          { 
            x1 = x1*x1;
            A = p_sweep.xcoord()*p_sweep.xcoord();
            B = p_sweep.xcoord()*(p.xcoord() + q.xcoord());
            C = p.xcoord()*q.xcoord();
            x2 =  A-B+C;
            if ( x1 >= x2 )  return -c;
          }
        return c;
      }
    else
      {
        if ( x1 >= 0 ); 
        else
          {
            x1 = x1*x1;
            A = p_sweep.xcoord()*p_sweep.xcoord();
            B = p_sweep.xcoord()*(p.xcoord() + q.xcoord());
            C = p.xcoord()*q.xcoord();
            x2 = A-B+C;
            if ( x1 > x2 )  return c;
          }
        return -c;
      }
  
  if ( D == 0 )
    if (p_sweep.ycoord() <  (p.ycoord() + q.ycoord())/2 ) return c;
    else                                                  return -c;
  
  x1 = x1 - A*C/D;
  x2 = C/D;
  
  if ( lower_p == -1 )
    if ( x1 < 0 ) return c;
    else
      {
        x1 = x1*x1;
        x2 = x2*x2*(A*(B + D*D/(C*C)*B)) ;
        if ( x2 <= x1 ) return -c;
        else            return  c;
      }
  else
    if ( x1 >= 0 ) return -c; 
    else
      {
        x1 = x1*x1;
        x2 = x2*x2*(A*(B + D*D/(C*C)*B));
        if ( x2 < x1 )    return  c;
        else              return -c;
      }
  
  return c;     
}


bool initialize(DelaunaySweep& DS, const list<POINT>& L)
{
  if( L.empty() ) return true;
  
  DelaunaySweep::output& G = DS.get_output();
  
  
    G.clear();

    list<POINT> L1  = L;
    L1.sort();

    array<POINT> A(L.length());
    int n = 0;
    A[0] = L1.pop();
    POINT p;
    forall(p, L1) if (p != A[n]) A[++n] = p;
    n++;

    node dummy_node = G.new_node(A[0]);

    node v1 = G.new_node(A[0]);
    edge e1 = G.new_edge(dummy_node, v1, HULL_EDGE); 
    edge e2 = G.new_edge(v1, dummy_node, HULL_EDGE); 
    G.set_reversal(e1, e2);
    if ( n == 1 )  return true;   // |L| == 1

    node v2;
    
    if( n == 2 )
      {
        v2 = G.new_node(A[1]);
        e1 = G.new_edge(v1, v2, HULL_EDGE);
        e2 = G.new_edge(v2, v1, HULL_EDGE);
        G.set_reversal(e1, e2);
        return true;              // |L| == 2
      }

  
    int i = 2;
    while( i < n && !orientation(A[0], A[1], A[i]) ) i++;
    
    if( i == n )
      {
        i = 1;
        do
          {
            v2 = G.new_node(A[i]);
            e1 = G.new_edge(v1, v2, HULL_EDGE);
            e2 = G.new_edge(v2, v1, HULL_EDGE);
            G.set_reversal(e1, e2);
            i++;  
            v1 = v2;
          }
        while ( i < n );
        return true;                // L contains collinear points only
      }
    
    WAVE_PART* w1 = new WAVE_PART(A[0]);
    w1->pred = WAVE_PART::INFINITY1;
    w1->succ = WAVE_PART::INFINITY2;
    DS.insert(w1, e2);
    WAVE_PART* w2;
    int j = 1;
    
    if ( !compare(A[0].xcoord(), A[i-1].xcoord() ) )
      for(; j < i; j++ )
        {
          w2 = new WAVE_PART(A[j]);
          v2 = G.new_node(A[j]);
          w2->pred = w1->p;
          w2->succ = WAVE_PART::INFINITY2;
          w1->succ = w2->p;
          
          e1 = G.new_edge(v1, v2, HULL_EDGE);
          e2 = G.new_edge(v2, v1, HULL_EDGE);
          G.set_reversal(e1, e2);
          
          DS.insert(w2, e2);
          
          w1 = w2;
          v1 = v2;
        }

  
    for( ; j < n; j++ ) DS.new_event(CIRCLE(A[j]), del_event_info(A[j]));
    
  return true;
}
     

bool transit(DelaunaySweep& DS)
{ 
  DelaunaySweep::output& G          = DS.get_output();
  DelaunaySweep::ystructure& Y      = DS.get_ystructure();
  DelaunaySweep::xstructure& X      = DS.get_xstructure();
  DelaunaySweep::event   event_item = DS.current_event();
  DelaunaySweep::xkey    p_sweep    = DS.event_key(event_item);
  DelaunaySweep::xinf    event_inf  = DS.event_inf(event_item);
  
  node n, n1 = nil;
  edge e, e1 = nil, e2 = nil, e1_succ = nil, e2_pred = nil;
  CIRCLE ci;
  
  // If the event_item is linked with an item in the [[Y-structure]],
  // at event point a circle event occurs. An input point may be placed
  // at this point too.
  // In the other case the current event is a site event only.
  DelaunaySweep::yitem   yit = DS.get_yref(event_item);
  DelaunaySweep::ykey    w;
  
  DelaunaySweep::yitem   yit_first = nil;
  DelaunaySweep::yitem   yit_last  = nil;
  DelaunaySweep::yitem   yit_succ  = nil;
  
  
  if( event_inf.is_site )   // 1. site event
    {
      w   = new WAVE_PART(event_inf.p);
      n1  = G.new_node(event_inf.p);
      
      WAVE_PART* wloc;
      yit  = Y.locate(w);
      wloc = DS.key(yit);
      yit_last  = yit;
      yit_first = DS.pred(yit_last);
      ci = CIRCLE( wloc->p, wloc->pred, event_inf.p);
      
      yit = Y.insert_at(yit_last, w, 0);      
      w = wloc->split(w);
      yit_first = Y.insert_at(yit, w, DS.inf(yit_last));

      if (!ci.is_degenerate() &&  ci.center().ycoord() == event_inf.p.ycoord())
        {
          DS.set_xref(yit_first, event_item);
          DS.set_yref(event_item, yit_first);
        }
      
      e = DS.inf(yit_last);
      e1 = G.new_edge( n1, G.source(e), HULL_EDGE );
      e2 = G.new_edge( e, n1, HULL_EDGE, leda::after );
      G.set_reversal(e1, e2);
      DS.inf(yit_last) = e2;
      DS.inf(yit)      = e1;
      yit = yit_first;
    }
  
  if ( DS.get_yref(event_item) )  
    {
      // 2. determine upperst item of subsequence of disappearing wave parts
      while( DS.get_xref(yit) == event_item ) yit = DS.succ(yit);       
      
      // 3. + 4. insert edges and remove wave parts
      yit_succ = yit;
      yit_last = yit_last ? yit_last : yit;
      yit = DS.pred(yit_succ);
      n1 = G.source(DS.inf(yit_succ));
      e2_pred = e =  DS.inf(yit_succ);
      G[e] = DIAGRAM_EDGE;
      n = G.source(DS.inf(yit));
      e = DS.inf(yit);
      
      do
        {
          yit_first = DS.pred(yit);
          w = DS.key(yit);
          DS.del_item(yit);
          delete w;
          yit = yit_first;
          G[e] = DIAGRAM_EDGE;
          n = G.source(DS.inf(yit));
          e1_succ = G.reversal(e);
          e = DS.inf(yit);
        }
      while( DS.get_xref(yit) == event_item );
      
      // 5. close the face
      DS.key(yit_first)->succ = DS.key(yit_succ)->p;
      DS.key(yit_succ)->pred  = DS.key(yit_first)->p;
      
      e1 = G.new_edge( e1_succ, n1, DIAGRAM_EDGE,  leda::before );  
      e2 = G.new_edge( e2_pred, n , HULL_EDGE,  leda::after );  
      G.set_reversal(e1, e2);
      
      DS.inf(yit_succ) = e2;
    }

/*
  { seq_item sit;
    int count = 0;
    forall_items(sit,DS.Y_structure)
        if (sit == yit_first) count++;
    if (count == 0) 
       cout << "not in Y: yit_first = " << yit_first << endl;
    assert(count > 0);
   }
*/
  
  // 6. Delete non relevant circle events
  DelaunaySweep::event xit1 = DS.get_xref(yit_first);
  DelaunaySweep::event xit2 = DS.get_xref(yit_last);
  
  if ( xit1 )
  { 
/*
    seq_item sit;
    int count = 0;
    forall_items(sit,DS.X_structure)
        if (sit == xit1) count++;
    if (count == 0) 
    { cout << "yit_first = " << yit_first << endl;
      cout << "xit1      = " << xit1 << endl;
     }
    assert(count > 0);
*/
    DelaunaySweep::xinf& xi = DS.event_inf(xit1);
    if ( xi.refs == 1 ) DS.del_event(xit1);
    else  { DS.set_yref(xit1, nil); xi.refs--; }
   }
  
  if ( xit2 )
  { DelaunaySweep::xinf& xi = DS.event_inf(xit2);
    if ( xi.refs == 1 ) DS.del_event(xit2);
    else { DS.set_yref(xit2, nil); xi.refs--; }
   }
    
  xit1 = nil, xit2= nil;
  
  // 7. Insert circle events for wave parts that become adjacent
  w = DS.key(yit_first);
  if( w->describes_circle_event() )
    {
      ci = CIRCLE(w->p, w->pred, w->succ);
      if( !(xit1 = X.lookup(ci)) ) xit1 = DS.new_event(ci, del_event_info());
      else                         DS.event_inf(xit1).refs++;
      DS.set_yref(xit1, yit_first);
    }
  
  w = DS.key(yit_last);
  if( w->describes_circle_event() )
    {
      ci = CIRCLE(w->p, w->pred, w->succ);
      if( !(xit2 = X.lookup(ci)) ) xit2 = DS.new_event(ci, del_event_info());
      else                         DS.event_inf(xit2).refs++;
      DS.set_yref(xit2, yit_last);
    }
  
  DS.set_xref(yit_first, xit1);
  DS.set_xref(yit_last,  xit2);

//s.n.
  if (xit1) DS.set_yref(xit1,yit_first);
  if (xit2) DS.set_yref(xit2,yit_last);
  
  return true;
  
}

bool finish(DelaunaySweep& DS, const list<POINT>&)
{
  DelaunaySweep::ystructure& Y = DS.get_ystructure(); 
  DelaunaySweep::output& G = DS.get_output();
  DelaunaySweep::yitem       yit;
  DelaunaySweep::ykey        w;
 
  if( !G.first_node() )  return true;

  G.del_node(G.first_node());

  while( !Y.empty() )
    {
      yit = Y.max();
      w = DS.key( yit );
      DS.del_item(yit);
      delete w;
    }
 
  return true; 
}

void DELAUNAY_FORTUNE(const list<POINT>& S, GRAPH<POINT, int>& G)
{  
  G.clear();
  DelaunaySweep DS(&S, &G);
  
  DS.set_init_handler(initialize);
  DS.set_event_handler(transit);
  DS.set_finish_handler(finish);
  
  if( DS.init(S) )
    if ( DS.sweep() ) DS.finish(S);
    else  cout << "Fehler beim Sweepen !" << endl;
  else cout << "Initialisierung fehlgeschlagen !" << endl;
} 



void run(int N)
{ cout << N << endl;
  list<rat_point> L;
  GRAPH<POINT,int> G;
  random_points_in_square(N,10000,L);
  DELAUNAY_FORTUNE(L,G); 
}

int main()
{ int N = read_int("N = ");
  rand_int.set_seed(12345*N);
  while (N) run(N--);
  return 0;
}


