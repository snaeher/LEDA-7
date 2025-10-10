#include <LEDA/geo/geo_alg.h>

using namespace leda;

using std::cout;
using std::flush;
using std::endl;


bool check_hull(const list<rat_point>& H)
{ int err = 0;
  list_item i;
  forall_items(i,H)
  { list_item j = H.cyclic_succ(i);
    list_item k = H.cyclic_succ(j);
    rat_point a = H[i];
    rat_point b = H[j];
    rat_point c = H[k];
    if (orientation(a,b,c) <= 0) err++;
  }
  return err == 0;
}


class ch_edge {

public:

rat_point   source;
rat_point   target;
ch_edge* succ;
ch_edge* pred;
ch_edge* link;
bool     outside;

ch_edge(const rat_point& a, const rat_point& b, ch_edge*& p) : source(a), target(b) 
{ outside = true; 
  link = p;
  p = this;
}

~ch_edge() {}

LEDA_MEMORY(ch_edge)

};



list<rat_point>  CONVEX_HULL_FILT(const list<rat_point>& L)
{ 
  if (L.length() < 2) return L;

  list<rat_point> CH;

  rat_point a = L.head();
  rat_point b = L.tail();
  rat_point p;


/*
  forall(p,L)
  { if (rat_point::cmp_xy(p,a) < 0) a = p;
    if (rat_point::cmp_xy(p,b) > 0) b = p;
   }
*/

  if ( a == b )
  { forall(p,L)
      if (p != a) { b = p; break; }
    if ( a == b )
    { // all points are equal
      CH.append(a);
      return CH; 
    } 
  }


  rat_point c = a;

  forall(p,L)
    if (orientation(a,b,p) != 0 ) { c = p; break; }

  if (c == a )
  { // all points are collinear 
    forall(p,L)
    { if ( compare(p,a) < 0 ) a = p;
      if ( compare(p,b) > 0 ) b = p;
    }
    CH.append(a); CH.append(b);
    return CH;
  }
  // a, b, and c are not collinear

  if (orientation(a,b,c) < 0 ) leda_swap(b,c);

  ch_edge* last_edge = NULL;


  ch_edge* T[3]; // initial triangle

  T[0] = new ch_edge(a,b,last_edge);
  T[1] = new ch_edge(b,c,last_edge);
  T[2] = new ch_edge(c,a,last_edge);

  int i;
  for(i = 0; i < 2; i++)  T[i]->succ = T[i+1];
  T[2]->succ = T[0];

  for(i = 1; i < 3; i++)  T[i]->pred = T[i-1];
  T[0]->pred = T[2];

  forall(p,L)
  { int i = 0;
    while (i < 3 && !right_turn(T[i]->source,T[i]->target,p) ) i++;

    if (i == 3)  continue; // p inside initial triangle

    ch_edge* e = T[i];


rat_point::float_computation_only = 1;

    while (! e->outside)
    { ch_edge* r0 = e->pred;
      if (right_turn(r0->source,r0->target,p)) 
      { e = r0; 
        continue;
       }
      ch_edge* r1 = e->succ;
      if (right_turn(r1->source,r1->target,p)) 
      { e = r1;
        continue;
       }
      break; 
    }

    rat_point::float_computation_only = 0;

    if (!e->outside) continue;  // p inside current hull (maybe wrong !)

    // confirm that p lies outside
    if (!right_turn(e->source,e->target,p)) continue;

    
    // compute "upper" tangent (p,high->source)

    ch_edge* high = e->succ;
    while (orientation(high->source,high->target,p) <= 0) high = high->succ;

    // compute "lower" tangent (p,low->target)

    ch_edge* low = e->pred;
    while (orientation(low->source,low->target,p) <= 0) low = low->pred;

    e = low->succ;

    // add new tangents between low and high

    ch_edge* e_l = new ch_edge(low->target,p,last_edge);
    ch_edge* e_h = new ch_edge(p,high->source,last_edge);

    e_h->succ = high;
    e_l->pred = low;
    high->pred = e_l->succ = e_h;
    low->succ  = e_h->pred = e_l;

    // mark edges between low and high as "inside" 
    // and define refinements

    while (e != high)
    { ch_edge* q = e->succ;
      e->pred = e_l;
      e->succ = e_h;
      e->outside = false;
      e = q;
    }

  }

  ch_edge* l_edge = last_edge;

  CH.append(l_edge->source);
  for(ch_edge* e = l_edge->succ; e != l_edge; e = e->succ) 
     CH.append(e->source);

  // clean up 
  while (l_edge)
  { ch_edge* e = l_edge;
    l_edge = l_edge->link;
    delete e;
  }

  return CH;
}






int main()
{
  int N = read_int("N = ");

  list<rat_point>  L;

  string gen;

  while (gen == "")
  { gen  = read_string("generator d(isk)/s(quare)/c(ircle) : ");
    if (gen[0] == 'd')
       random_points_in_disc(N,10000,L);
    else if (gen[0] == 's')
       random_points_in_square(N,10000,L);
    else if (gen[0] == 'c')
       random_points_on_circle(N,10000,L);
    else gen = "";
  }

  L.permute();



{ cout << "CONVEX_HULL_IC              " << flush;
  float T = used_time();
  list<rat_point> C = CONVEX_HULL_IC(L);
  cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  assert(check_hull(C));
 }

  rat_point::use_static_filter = false;

{ cout << "CONVEX_HULL_IC(dyn_filter)  " << flush;
  float T = used_time();
  list<rat_point> C = CONVEX_HULL_IC(L);
  cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  assert(check_hull(C));
 }

  rat_point::use_static_filter = true;

 rat_point::float_computation_only = 1;

{ cout << "CONVEX_HULL_IC(float_only)  " << flush;
  float T = used_time();
  list<rat_point> C = CONVEX_HULL_IC(L);
  cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  //assert(check_hull(C));
 }

  rat_point::float_computation_only = 0;


  cout << endl;

{ cout << "CONVEX_HULL_FILT            " << flush;
  float T = used_time();
  list<rat_point> C = CONVEX_HULL_FILT(L);
  cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  assert(check_hull(C));
 }

  rat_point::use_static_filter = false;

{ cout << "CONVEX_HULL_FILT(dyn_filt)  " << flush;
  float T = used_time();
  list<rat_point> C = CONVEX_HULL_FILT(L);
  cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  assert(check_hull(C));
 }

  rat_point::use_static_filter = true;


  cout << endl;

{ cout << "CONVEX_HULL_SWEEP           " << flush;
  float T = used_time();
  list<rat_point> C = CONVEX_HULL_S(L);
  cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  assert(check_hull(C));
 }

{ cout << "UPPER_CONVEX_HULL           " << flush;
  float T = used_time();
  list<rat_point> C = UPPER_CONVEX_HULL(L);
  cout << string("|C| = %d   time = %.2f",C.length(),used_time(T)) << endl;
  //assert(check_hull(C));
 }

  cout << endl;

  rat_point::print_statistics();
  cout << endl;

  return 0;
}
