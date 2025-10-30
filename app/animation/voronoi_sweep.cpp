/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  delaunay_sweep.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



#define LEDA_CHECKING_OFF

#include <LEDA/core/p_queue.h>
#include <LEDA/graph/graph.h>
#include <LEDA/geo/rat_kernel.h>
#include <LEDA/geo/rat_kernel_types.h>
#include <LEDA/graphics/geowin.h>
#include <math.h>

LEDA_BEGIN_NAMESPACE
COMPARE_DECL_PREFIX
int compare(const CIRCLE& c1, const CIRCLE& c2);
LEDA_END_NAMESPACE

#include <LEDA/core/sortseq.h>


#define WAVE_PART rat_wave_part

struct WAVE_PART;

LEDA_BEGIN_NAMESPACE
int compare_at_pos(const CIRCLE& p_sweep,  WAVE_PART*const & w1, 
		   WAVE_PART*const & w2);
LEDA_END_NAMESPACE

#include <LEDA/geo/plane_sweep.h>

using namespace leda;

#if defined(LEDA_STD_IO_HEADERS)
using std::ostream;
using std::istream;
using std::cout;
using std::cerr;
using std::endl;
#endif


static bool interactive = true;


static list<CIRCLE> circles;
  
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


LEDA_BEGIN_NAMESPACE
int compare(WAVE_PART*const & w1, WAVE_PART*const & w2);
LEDA_END_NAMESPACE




typedef sortseq<WAVE_PART*, edge>         YST;
typedef list<POINT>                       LoP;
typedef GRAPH<POINT,int>                  GPI;
typedef plane_sweep<LoP, YST, XST, GPI  > DelaunaySweep;



POINT WAVE_PART::INFINITY1;     
POINT WAVE_PART::INFINITY2;     

LEDA_BEGIN_NAMESPACE
COMPARE_DECL_PREFIX
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
LEDA_END_NAMESPACE

LEDA_BEGIN_NAMESPACE
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
  { if ( lower_p == -1 )
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
   }
  
  if ( D == 0 )
     return (p_sweep.ycoord() <  (p.ycoord() + q.ycoord())/2) ? c : -c;
  
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
LEDA_END_NAMESPACE


bool initialize(DelaunaySweep& DSweep, const list<POINT>& L)
{
  if( L.empty() ) return true;
  
  DelaunaySweep::output& G = DSweep.get_output();
  
  
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
    DSweep.insert(w1, e2);
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
          
          DSweep.insert(w2, e2);
          
          w1 = w2;
          v1 = v2;
        }

  
    for( ; j < n; j++ ) DSweep.new_event(CIRCLE(A[j]), del_event_info(A[j]));
    



  return true;
}
     

bool transit(DelaunaySweep& DSweep)
{ 
  DelaunaySweep::output& G          = DSweep.get_output();
  DelaunaySweep::ystructure& Y      = DSweep.get_ystructure();
  DelaunaySweep::xstructure& X      = DSweep.get_xstructure();
  DelaunaySweep::event   event_item = DSweep.current_event();
  DelaunaySweep::xkey    p_sweep    = DSweep.event_key(event_item);
  DelaunaySweep::xinf    event_inf  = DSweep.event_inf(event_item);
  
  node n, n1 = nil;
  edge e, e1 = nil, e2 = nil, e1_succ = nil, e2_pred = nil;
  CIRCLE ci;
  
  // If the event_item is linked with an item in the [[Y-structure]],
  // at event point a circle event occurs. An input point may be placed
  // at this point too.
  // In the other case the current event is a site event only.
  DelaunaySweep::yitem   yit = DSweep.get_yref(event_item);
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
      wloc = DSweep.key(yit);
      yit_last  = yit;
      yit_first = DSweep.pred(yit_last);
      ci = CIRCLE( wloc->p, wloc->pred, event_inf.p);
      
      yit = Y.insert_at(yit_last, w, (edge)0);      
      w = wloc->split(w);
      yit_first = Y.insert_at(yit, w, DSweep.inf(yit_last));
      
      if (!ci.is_degenerate() &&  ci.center().ycoord() == event_inf.p.ycoord())
        {
          DSweep.set_xref(yit_first, event_item);
          DSweep.set_yref(event_item, yit_first);
        }
      
      e = DSweep.inf(yit_last);
      e1 = G.new_edge( n1, G.source(e), HULL_EDGE );
      e2 = G.new_edge( e, n1, HULL_EDGE, leda::after );
      G.set_reversal(e1, e2);
      DSweep.inf(yit_last) = e2;
      DSweep.inf(yit)      = e1;
      yit = yit_first;
    }
  
  if ( DSweep.get_yref(event_item) )  
    {
      // 2. determine topmost item of subsequence of disappearing wave parts
      while( DSweep.get_xref(yit) == event_item ) yit = DSweep.succ(yit);       
      
      // 3. + 4. insert edges and remove wave parts
      yit_succ = yit;
      yit_last = yit_last ? yit_last : yit;
      yit = DSweep.pred(yit_succ);
      n1 = G.source(DSweep.inf(yit_succ));
      e2_pred = e =  DSweep.inf(yit_succ);
      G[e] = DIAGRAM_EDGE;
      n = G.source(DSweep.inf(yit));
      e = DSweep.inf(yit);
      
      do
        {
          yit_first = DSweep.pred(yit);
          w = DSweep.key(yit);
          DSweep.del_item(yit);
          delete w;
          yit = yit_first;
          G[e] = DIAGRAM_EDGE;
          n = G.source(DSweep.inf(yit));
          e1_succ = G.reversal(e);
          e = DSweep.inf(yit);
        }
      while( DSweep.get_xref(yit) == event_item );
      
      // 5. close the face
      DSweep.key(yit_first)->succ = DSweep.key(yit_succ)->p;
      DSweep.key(yit_succ)->pred  = DSweep.key(yit_first)->p;
      
      e1 = G.new_edge( e1_succ, n1, DIAGRAM_EDGE,  leda::before );  
      e2 = G.new_edge( e2_pred, n , HULL_EDGE,  leda::after );  
      G.set_reversal(e1, e2);
      
      DSweep.inf(yit_succ) = e2;
    }
  
  // 6. Delete non relevant circle events
  DelaunaySweep::event xit1 = DSweep.get_xref(yit_first);
  DelaunaySweep::event xit2 = DSweep.get_xref(yit_last);
  
  if ( xit1 )
  { if ( DSweep.event_inf(xit1).refs == 1 ) DSweep.del_event(xit1);
    else  { DSweep.set_yref(xit1, nil); DSweep.event_inf(xit1).refs--; }
   }
  
  if ( xit2 )
  { if ( DSweep.event_inf(xit2).refs == 1 ) DSweep.del_event(xit2);
    else { DSweep.set_yref(xit2, nil); DSweep.event_inf(xit2).refs--; }
   }
    
  xit1 = nil, xit2= nil;
  
  // 7. Insert circle events for wave parts that become adjacent
  w = DSweep.key(yit_first);
  if( w->describes_circle_event() )
    {
      ci = CIRCLE(w->p, w->pred, w->succ);
      xit1 = X.lookup(ci);
      if(!xit1) xit1 = DSweep.new_event(ci, del_event_info());
      else DSweep.event_inf(xit1).refs++;
      DSweep.set_yref(xit1, yit_first);
    }
  
  w = DSweep.key(yit_last);
  if( w->describes_circle_event() )
    {
      ci = CIRCLE(w->p, w->pred, w->succ);
      xit2 = X.lookup(ci);
      if(!xit2) xit2 = DSweep.new_event(ci, del_event_info());
      else DSweep.event_inf(xit2).refs++;
      DSweep.set_yref(xit2, yit_last);
    }
  
  DSweep.set_xref(yit_first, xit1);
  DSweep.set_xref(yit_last,  xit2);

//s.n.
  if (xit1) DSweep.set_yref(xit1,yit_first);
  if (xit2) DSweep.set_yref(xit2,yit_last);
  
  return true;
  
}



bool finish(DelaunaySweep& DSweep, const list<POINT>&)
{
  DelaunaySweep::ystructure& Y = DSweep.get_ystructure(); 
  DelaunaySweep::output& G = DSweep.get_output();
  DelaunaySweep::yitem       yit;
  DelaunaySweep::ykey        w;
 
  if( !G.first_node() )  return true;

  G.del_node(G.first_node());

  while( !Y.empty() )
    {
      yit = Y.max();
      w = DSweep.key( yit );
      DSweep.del_item(yit);
      delete w;
    }
 
  return true; 
}


/*
void DELAUNAY_FORTUNE(const list<POINT>& S, GRAPH<POINT, int>& G)
{  
  G.clear();
  DelaunaySweep DSweep(&S, &G);
  
  DSweep.set_init_handler(initialize);
  DSweep.set_event_handler(transit);
  DSweep.set_finish_handler(finish);
  
  if( DSweep.init(S) )
    if ( DSweep.sweep() ) DSweep.finish(S);
    else  cout << "Fehler beim Sweepen !" << endl;
  else cout << "Initialisierung fehlgeschlagen !" << endl;
} 
*/



GeoBaseScene<list<rat_point> >* shore_sc = 0;
GeoBaseScene<list<rat_point> >* delau_sc = 0;


static GeoWin* Gwp = 0;
static window* Wp = 0;
DelaunaySweep* Dsweep =0;



class WaveFrontRep
{
  mutable  list<point> poly;

  public :
  
  point                        p_sweep;
  DelaunaySweep::ystructure*   yst;
  
  WaveFrontRep() : yst(0) {}
  WaveFrontRep(const WaveFrontRep& w) :  p_sweep(w.p_sweep), yst(w.yst) {}
  
  double yparab( double y, point p ) const;
  void xparab( double x, point p, double& y1, double& y2 ) const; 
  void computeX(WAVE_PART* w, double& x0) const;
  void computeY(WAVE_PART* w, double& y0, double& y1) const;
  void plotter(window& w, point p, double y0, double y1, color col) const;
  void show_ystructure() const;
};

unsigned long ID_Number(const WaveFrontRep& x)
{ return ID_Number(x.p_sweep); }

const char* leda_tname(WaveFrontRep* w) {  return "WaveFrontRep"; }

bool identical(const WaveFrontRep& w1, const WaveFrontRep& w2)
{ return &w1 == &w2; }




double WaveFrontRep::yparab( double y, point p ) const
{
  if ( p_sweep.xcoord() - p.xcoord()  == 0 ) return Wp->xmin();
  
  double A = p_sweep.xcoord()*p_sweep.xcoord();
  double B = (y - p.ycoord())*(y - p.ycoord());
  double C = p.xcoord()*p.xcoord();
  double res = (A - B - C)/(2*(p_sweep.xcoord() - p.xcoord()));
  return res;
}

void WaveFrontRep::xparab( double x, point p, double& y1, double& y2 ) const
{
  double A = p_sweep.xcoord()*p_sweep.xcoord();
  double B = 2*x*(p.xcoord() - p_sweep.xcoord());
  double C = p.xcoord()*p.xcoord();
  double D = sqrt(B + A - C);
  
  y1 = p.ycoord() - D;
  y2 = p.ycoord() + D;
}

void  WaveFrontRep::computeX(WAVE_PART* w, double& x0) const
{
  double x;
  point  p;
  
  x0 = Wp->xmin();
  if ( !identical(w->pred, WAVE_PART::INFINITY1) ) 
    {
      p = w->pred.to_point();
      x = yparab(w->p.to_point().ycoord(), p);
      if ( x > x0 ) x0 = x;
    }
  if ( !identical(w->succ, WAVE_PART::INFINITY2 ) )
    {
      p = w->succ.to_point();
      x = yparab(w->p.to_point().ycoord(), p);
      if ( x > x0 ) x0 = x;
    }
}

void WaveFrontRep::computeY(WAVE_PART* w, double& y0, double& y1) const
{
  y0 = Wp->ymin();
  y1 = Wp->ymax();
  
  double A, B, C, D, E, F;
  point p, q;
  int   tol;
  
  xparab( Wp->xmin(), w->p.to_point(), E, F );
  
  if( E > y0 ) y0 = E;
  if( F < y1 ) y1 = F;
  
  if( !identical(w->succ, WAVE_PART::INFINITY2 ) )
    {
      p = w->p.to_point();
      q = w->succ.to_point();
      
      A = p.xcoord() - p_sweep.xcoord();
      B = q.xcoord() - p_sweep.xcoord();
      C = q.ycoord() - p.ycoord();
      D = p.xcoord() - q.xcoord();
      
      if ( C == 0 ) 
	{
	  A = p_sweep.xcoord()*p_sweep.xcoord();
	  B = p_sweep.xcoord()*(p.xcoord() + q.xcoord());
	  C = p.xcoord()*q.xcoord();
	  tol = compare(p, q) < 0 ? -1 : 1;
	  F =  p.ycoord() + tol*sqrt(A-B+C); 
	}
      else
	if ( D == 0 )
	  if ( A != 0 && C > 0) F = (q.ycoord() + p.ycoord())/2;
          else; 
	else
	  {
	    if ( C > 0 )
	      F = (A*C)/D + C/D*sqrt(A*(B+D*D/(C*C)*B));
	    else
	      F = (A*C)/D - C/D*sqrt(A*(B+D*D/(C*C)*B));
	    
	    F = F + p.ycoord();
	  }
    }
  
  
  if( !identical(w->pred, WAVE_PART::INFINITY1) )
    {
      p = w->p.to_point();
      q = w->pred.to_point();
      
      A = p.xcoord() - p_sweep.xcoord();
      B = q.xcoord() - p_sweep.xcoord();
      C = q.ycoord() - p.ycoord();
      D = p.xcoord() - q.xcoord();
      
      if ( C == 0 ) 
	{
	  A = p_sweep.xcoord()*p_sweep.xcoord();
	  B = p_sweep.xcoord()*(p.xcoord() + q.xcoord());
	  C = p.xcoord()*q.xcoord();
	  tol = compare(p, q) < 0 ? -1 : 1;
	  E =  p.ycoord() - tol*sqrt(A-B+C); 
	}
      else
	if ( D == 0 ) 
	  if ( A != 0 && C < 0) E = (q.ycoord() + p.ycoord())/2;
	  else;
	else
	  {
	    if ( C > 0 )
	      E = (A*C)/D - C/D*sqrt(A*(B+D*D/(C*C)*B));
	    else
	      E = (A*C)/D + C/D*sqrt(A*(B+D*D/(C*C)*B));
	    
	    E = E + p.ycoord();
	  }
    }
  
  if( E > y0 ) y0 = E;
  if( F < y1 ) y1 = F;
}


void WaveFrontRep::plotter(window& w, point p, double y0, double y1, 
			   color col) const
{
  if (y1 <= y0) return;
  
  double dy = 4/w.scale();

  poly.clear();
  
  for(double y=y0; y<y1; y+=dy)
  { double x = yparab(y,p);
    poly.append(point(x,y));
   }
  
  double x1 = yparab(y1,p);
  poly.append(point(x1,y1));

  int lw = w.set_line_width(2);
  w.draw_spline(poly, 4, col);
  w.set_line_width(lw);  

  //w.set_node_width(2);
  w.set_node_width(1);
  w.draw_filled_node(poly.back(),col);

}


void WaveFrontRep::show_ystructure() const
{
  if (!yst) return;

  if (!shore_sc->is_visible()) return;

  color clr = black;

  circles.clear();

  poly.clear();

  DelaunaySweep::yitem   dit;
  forall_items(dit, *yst)
  { WAVE_PART* w = yst->key(dit);
     double y0,y1;
     computeY(w,y0,y1);
     plotter(*Wp,w->p.to_point(),y0,y1,clr);
/*
     if( w->describes_circle_event() )
     { CIRCLE ci(w->p, w->pred, w->succ);
       circles.append(ci);
      }
*/
  }

/*
  poly.push(point(Wp->xmin(),Wp->ymin()));
  poly.append(point(Wp->xmin(),Wp->ymax()));
  Wp->draw_filled_polygon(poly,ivory);
*/

/*
  int lw = Wp->set_line_width(2);
  Wp->draw_spline(poly,4,clr);
  Wp->set_line_width(lw);
*/
}



// //////////////////////////////////////////////////////////////////////
// routines for using the class in GeoWin

// ps_file output

ps_file& operator<<(ps_file& ps,  const WaveFrontRep& c1)
{
 return ps;
}

// Presentation of the Sweepline :
window& operator<<(window& w, const WaveFrontRep& c1)
{ 
  point r1 = c1.p_sweep;
  point r2 = r1.translate(0, 1);
  int lw = w.set_line_width(2);
  if (interactive)
    w.draw_line(r1, r2, green2);
  else
    w.draw_line(r1, r2, red);
  w.set_line_width(lw);
  c1.show_ystructure();
  return w;
}

window& operator >> (window& w, WaveFrontRep& p)
{
  return w;
}

ostream& operator << (ostream& os, const WaveFrontRep& w) { return os; }
istream& operator >> (istream& is, WaveFrontRep& w)      {  return is; }


bool geowin_intersects(const WaveFrontRep& c1, double x1, double y1, 
		       double x2, double y2, bool)
{
  double xc = c1.p_sweep.xcoord();
  return (xc >= x1 && xc <= x2);
}

void geowin_fit(const WaveFrontRep& obj, double& x1, double& x2,
		double& y1, double& y2)
{
}

void geowin_move_view(WaveFrontRep& obj, double dx, double dy)
{
  obj.p_sweep = obj.p_sweep.translate(dx,dy);
/*
  point p = obj.p_sweep.translate(dx, dy);
  WaveFrontRep w;
  w.p_sweep = p; 
  w.yst = obj.yst;
  return w;
*/
}


bool sweepline_pre_move_handler(GeoWin&, const WaveFrontRep&, 
                                  double dx, double dy)
{ 
  return dx >0; 
}



//s.n.
//void update_delaunay(const list<WaveFrontRep>& L, GRAPH<rat_point, int>& G)
static void update_delaunay(const list<WaveFrontRep>& L, list<rat_point>&)
{
  circles.clear();

  if( !L.empty() )
  { const WaveFrontRep& wfr = L.head();  

    DelaunaySweep::yitem   dit;
    forall_items(dit, *(wfr.yst))
    { WAVE_PART* w = wfr.yst->key(dit);
      if( w->describes_circle_event() )
        circles.append(CIRCLE(w->p, w->pred, w->succ));
    }

    point pp = wfr.p_sweep;
    rat_point ppp = rat_point(pp);
    rat_circle stop = rat_circle(ppp, ppp, ppp);
    Dsweep->animate(stop, 0);
  }
}


extern bool initialize(DelaunaySweep& DSweep, const list<rat_point>& L);
extern bool transit(DelaunaySweep& DSweep );
extern bool finish(DelaunaySweep& DSweep, const list<rat_point>& L);





void finish_sweep(GeoWin& gw)
{
  if (Dsweep == 0) return;

  geo_scene sline = gw.get_scene_with_name("Sweepline");
  geo_scene out = gw.get_scene_with_name("Sweep Output");

  circles.clear();
  Dsweep->sweep();
  Dsweep->finish(Dsweep->get_input());
/*
  gw.set_visible(sline,false);
  gw.edit(out);
  gw.activate(sline);
*/
  delete Dsweep;
  gw.destroy(sline);
  Dsweep = 0;
  gw.reset_actions();
}

void break_sweep(GeoWin& gw)
{
  if (Dsweep == 0) return; 

  geo_scene sline = gw.get_scene_with_name("Sweepline");
  geo_scene out = gw.get_scene_with_name("Sweep Output");
  
  Dsweep->finish(Dsweep->get_input());
  gw.edit(out);
  gw.activate(sline);
  delete Dsweep;
  gw.destroy(sline);
  Dsweep = 0;
  gw.reset_actions();

}


// s.n.
static GRAPH<rat_point,int> Gtmp;


class redraw_graph : public geowin_redraw
{
public:
 virtual bool operator()(window& W,color col1, color col2, double,double,double,double)
 { 
  if (!delau_sc->is_visible()) return false; 

  edge_array<bool> drawn(Gtmp,false);
  edge e;
  forall_edges(e,Gtmp) 
  { edge r = Gtmp.reversal(e);
    if (r)
    { if (drawn[e]) continue;
       drawn[r] = drawn[e] = true;
     }
    node v = source(e);
    node w = target(e);
    //W.draw_segment(Gtmp[v].to_point(),Gtmp[w].to_point(),col1);
    W.set_node_width(2);
    W.draw_edge(Gtmp[v].to_point(),Gtmp[w].to_point(),col1);
  }  
  return false;
 }
};

redraw_graph RDG;

geowin_update<list<WaveFrontRep>, list<rat_point> > sweep_update(update_delaunay);


static list<WaveFrontRep> wave_front_list; 

 
void start_sweep( GeoWin& gw)
{
  // is a sweep running ?
  if( Dsweep ) return;
  
  // mouse actions:
  gw.clear_actions();
  gw.set_action(A_LEFT| A_DRAG | A_OBJECT, geo_object_dragging);
  gw.del_pin_point();
  
  // get input
  list<rat_point> L;
  //gw.get_objects(L);
  geowin_get_objects(gw, gw.get_active_scene(), L);
   

  // make a scene for the sweepline

  wave_front_list.clear();

  GeoEditScene< list<WaveFrontRep> >*
  SweeplineScene = (GeoEditScene< list<WaveFrontRep> >*)

  geowin_new_scene(gw, wave_front_list); 

  gw.set_name(SweeplineScene, "Sweepline");

  gw.set_description(SweeplineScene, 
        "This scene contains the (vertical) sweep line. \
         Drag the line with the mouse over the input to perform the sweep.");
  
  geowin_set_pre_move_handler(SweeplineScene, sweepline_pre_move_handler);
  SweeplineScene->set_box_intersection_fcn(geowin_intersects);
  SweeplineScene->set_get_bounding_box_fcn(geowin_fit);
  SweeplineScene->set_move_fcn(geowin_move_view);
  
  // scene for graph :
  // link it to the  SweeplineScene 
  // s.n.
  GeoBaseScene<list<rat_point> >* tmp;
  
  tmp = geowin_new_scene(gw, sweep_update, SweeplineScene, "Sweep Output");  

  gw.set_visible(tmp, true);
  gw.set_color(tmp, blue);
  gw.set_color2(tmp, blue);

  gw.set_redraw_pt((geo_scene)tmp, &RDG);
  
  // create a DelaunaySweepObjekt
  // s.n.
  //Dsweep = new DelaunaySweep(&L, &(tmp->get_objref()));   
  Dsweep = new DelaunaySweep(&L, &Gtmp);

  Dsweep->set_init_handler(initialize);
  Dsweep->set_event_handler(transit);
  Dsweep->set_finish_handler(finish);
  
  if( !Dsweep->init(L) )
  { gw.message("Initialization failed !");
    finish_sweep(gw);
    return;
  }
  
  DelaunaySweep::xkey swp = Dsweep->sweep_position();
  list<WaveFrontRep> LL;
  WaveFrontRep startfront;
  circle ccc = swp.to_circle();
  point ppp  = ccc.center();
  startfront.p_sweep = ppp.translate(ccc.radius(), 0);
  startfront.yst =&(Dsweep->get_ystructure());
  LL.append(startfront);
  
  SweeplineScene->set_objects(LL);
  gw.set_visible(SweeplineScene, true); 
  gw.activate(SweeplineScene);
  gw.redraw();
}


#include <LEDA/geo/rat_geo_alg.h>

geowin_graph_update<list<rat_point>,GRAPH<rat_circle,rat_point> > voro(VORONOI);

int main()
{
  if (getenv("LEDA_OPEN_MAXIMIZED")) interactive = false;

  rand_int.set_seed(1234567);

  //GeoWin GW("Voronoi Sweep Animation");
  GeoWin GW("GeoWin: Fortune Sweep");
  Gwp = &GW;
  

  list<rat_point> L;

/*
  for(int i=0;i<10; i++) {
    window& W = GW.get_window();
    int xmin = W.xmin();
    int xmax = W.xmax();
    int ymin = W.ymin();
    int ymax = W.ymax();
    int dx = (xmax-xmin)/10;
    int dy = (ymax-ymin)/6;
    int x = rand_int(xmin+dx,xmax-dx);
    int y = rand_int(ymin+dy,ymax-dy);
    L.append(rat_point(x,y,1));
  }
*/

  geo_scene sc = geowin_new_scene(GW,L);
  GW.set_point_style(sc,circle_point);
  GW.set_visible(sc,true);
  GW.set_name(sc,"sweep_points");
  GW.set_description(sc, "We want to construct the Delaunay triangulation of the points \
  contained in this scene by running a line sweep algorithm."); 

  // make a (dummy) scene for the shore line 
  // s.n.
  list<rat_point> dummy_L;
  shore_sc = geowin_new_scene(GW,dummy_L);  
  GW.set_name(shore_sc,"shoreline");  

  GW.set_visible(shore_sc, true);

  GW.set_description(shore_sc, "This scene contains the wavefront separating the part of the \
  plane with smaller distance to one of the already reached points in the sweep than to the \
  sweep line.");

  // and another one for the delaunay triangulation
  // s.n.
  list<rat_point> dummy_L1;
  delau_sc = geowin_new_scene(GW,dummy_L1);  
  GW.set_name(delau_sc,"delaunay");  

  GW.set_visible(delau_sc, false);
/*
  if (interactive)
    GW.set_visible(delau_sc, false);
  else
    GW.set_visible(delau_sc, true);
*/

  GW.set_description(delau_sc, "This scene contains the Delaunay triangulation of the input. \
  While running the sweep the scene shows the already constructed part of the Delaunay triangulation.");

  geo_scene circle_sc = geowin_new_scene(GW,circles);
  GW.set_color(circle_sc,red);
  GW.set_fill_color(circle_sc,invisible);
  GW.set_name(circle_sc,"Circle Events");

  geo_scene sc_voro = geowin_new_scene(GW,voro,sc, "Voronoi Diagram");

  if (interactive)
    GW.set_color(sc_voro,brown);
  else
    GW.set_color(sc_voro,blue2);

  GW.set_visible(sc_voro,true);
  GW.set_description(sc_voro, "This scene contains the Voronoi diagram of the input points. \
  Note that the Voronoi diagram is the dual graph of the Delaunay triangulation.");

  GW.add_help_text("delaunay_sweep");
  GW.hide_buttons();

  Wp = &GW.get_window();

  if (interactive)
    GW.display(window::center,window::center,true);
  else
  { // frameless
    GW.set_show_status(false);
    GW.display(window::center,window::center,false);
   }


  if (interactive) Wp->display_help_text("delaunay_sweep");

  string msg1 = "";
  msg1 += "Define some points using the left mouse button ";
  msg1 += "- the Voronoi Diagram will be shown.\\n ";
  msg1 += "\\blue Press 'done' to start the animation.";

  string msg2 = "";
  msg2 += "Edit the input set by adding, deleting, or moving points around.";
  msg2 += "\\n ";
  msg2 += "\\blue Press 'done' to start a new animation.";

  string msg3 = "";
  msg3 += "Click and drag the sweep line to the right.\\n ";
  msg3 += "\\blue Press 'done' to start an animated run of the sweep.";

  if (interactive) GW.message(msg1);

  for(;;)
  { 
    if (interactive)
    { GW.set_fill_color(sc,yellow);
      GW.edit(sc);
      GW.message(msg3);
     }
    else
    { L.clear();
      GW.set_visible(sc_voro,false);
      int n = 16;
      for(int i=0;i<n; i++) {
        window& W = GW.get_window();
        int xmin = (int)W.xmin();
        int xmax = (int)W.xmax();
        int ymin = (int)W.ymin();
        int ymax = (int)W.ymax();
        int d = (ymax-ymin)/n;
        int x = rand_int(xmin+d,xmax-d);
        int y = rand_int(ymin+d,ymax-d);
        x = 10*(x/10)+1;
        y = 10*(y/10)+1;
        L.append(rat_point(x,y,1));
      }

      L.sort();
      L.unique();
      sc->update_and_redraw();
      sleep(0.5);

      GW.set_visible(sc_voro,true);
      sc_voro->update_and_redraw();
      sleep(0.5);
    }

    GW.set_fill_color(sc,ivory);
    start_sweep(GW);

    if (interactive) 
    { GW.edit(GW.get_scene_with_name("Sweepline"));
      GW.message("");
      }

    double dx = 0.7;
    if (interactive) dx = 1.0;
/*
    double dx = 1.0;
    if (window::display_type() == "xx") dx *= 2;
*/

    double sweep_max = Wp->xmax();

    list_item it = L.max();
    if (it) sweep_max += 1.5*(Wp->xmax() - L[it].xcoordD());

    for(int i=0; i<1000; i++)
    { list_item it = wave_front_list.first();
      point p = wave_front_list[it].p_sweep;
      p = p.translate(dx,0); 
      wave_front_list[it].p_sweep = p;
      GW.get_scene_with_name("Sweepline")->update_and_redraw();
      if (p.xcoord() > sweep_max) break;
      // allow interaction (e.g. closing display)
      Wp->get_mouse();
    }

    finish_sweep(GW);

    if (interactive) {
/*
      GW.message("\\blue Press 'done' to finish.");
      GW.edit(GW.get_scene_with_name("Sweep Output"));
*/
      GW.message(msg2);
    }
    else
      Wp->read_mouse();

   }

  break_sweep(GW);

 
  return 0;
}


