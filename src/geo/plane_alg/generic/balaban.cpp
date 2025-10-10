/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  balaban.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/list.h>
#include <LEDA/core/slist.h>
#include <LEDA/core/sortseq.h>
#include <LEDA/core/map.h>

LEDA_BEGIN_NAMESPACE

#if (KERNEL == RAT_KERNEL)
#define STAIRCASE RAT_STAIRCASE
#endif

// Problem :
// Find all $K$ intersecting pairs in a set $S$ consisting of $n$ straight-line 
// segments in the plane.

// Algorithm :
// Literature : Ivan J. Balanban ???? S.211 - 219

// The following implementation should make no assumptions about the input :
// -  segments may have length zero                              
// -  segments may be vertical                                   
// -  end  and intersection points may have the same abscissae   
// -  end  and intersection points may have the same coordinates 

typedef void (*rep_int_func) (const SEGMENT&, const SEGMENT&);



static int cmp_at_x(const SEGMENT& s1, const SEGMENT& s2, const POINT& x)
{   
  // Precondition : s1.start <= s1.end s2.start <= s2.end

  if(s1.is_vertical())
  {  if (s2.is_vertical()) 
        return 0;
     else               
        return (orientation(s2,x) >  0) ? 1 : -1;
   }

  if (s2.is_vertical()) return (orientation(s1, x) <= 0) ? 1 : -1;

  int c = cmp_segments_at_xcoord(s1, s2, x);
  if (c) return c;

  int cs = cmp_slopes(s1,s2);
  return (orientation(s1,x) > 0) ? cs : -cs;
}




// |stairs| : pointer to an array that implements the "staircase recursion
//            stack"
//            $O(n)$ space

static SEGMENT*  stairs = 0;

// |StairCount| : integer that stores the number of stairs that currently 
// stored in |stairs|

static int      StairCount = 0;



// Classes :

class STAIRCASE
{  
  int   bcount;
  int   ecount;
  
  public :
  
  int   b;   
  int   e;

  STAIRCASE(int p1, int p2) : bcount(StairCount), ecount(0), b(p1), e(p2) {}
  
  int    Count() const { return ecount; }
  bool   empty() const { return ecount == 0; }

  SEGMENT& operator[] (int i) const { return stairs[bcount+i]; }
  
  void append(SEGMENT& s)
  { stairs[++StairCount] = s;
    ecount++;
   }

};



typedef slist<SEGMENT>::item slist_item;


// function prototyps :

static void TreeSearch_nloghoch2n(slist<SEGMENT>& L, slist<SEGMENT>& I, int b, int e,
			   slist<SEGMENT>& R);
static void SearchInStrip(slist<SEGMENT>& L, slist<SEGMENT>& R, int b, int e);

static void InnerIntPoints(const STAIRCASE& D, const slist<SEGMENT>& I);
static int  Loc(const STAIRCASE& D, const SEGMENT s);
static void ReportIntersection(const SEGMENT& s1, const SEGMENT& s2);

static void Split(slist<SEGMENT>& L, STAIRCASE& D);
static void Lrs_from_Rls(slist<SEGMENT>& Rls, slist<SEGMENT>& Lrs, int c);
static void Merge(STAIRCASE& D, slist<SEGMENT>& R);

// GlobalVar

// |report_func| : function to report intersecting pairs
static rep_int_func              report_func = 0;

// |original| : pointer to a map that associates each input segment with an 
//              internally used segment, which is directed from left to rigth 
//              or upwards
//              $O(n)$ space 
static map<SEGMENT,SEGMENT>*     original    = 0; 

// |init_points| : pointer to an array that stores the different endpoints
//                 of the input segments ordered lexicographically
//                 $O(n)$ space
static POINT*    init_points = 0;

// |seg_queue| : list of internal segment sorted, using the ordering defined  
//               by the function |compare2(const SEGMENT&, const SEGMENT&)| 
//               $O(n)$ space
static list<SEGMENT>    seg_queue;

static int compare2(const SEGMENT& s1, const SEGMENT& s2)
{
  int c = compare(s1.start(), s2.start());
  if (c) return c;
  return cmp_slopes(s1, s2);
}


void balaend(const list<SEGMENT>& S, int)
{
  if (init_points) delete[] init_points; 
  if (stairs) delete[] stairs;      

  if (original) delete original;      

  init_points = 0;
  stairs = 0;
  original = 0;
  seg_queue.clear();
}


void BALABAN_SEGMENTS(const list<SEGMENT>& S, rep_int_func rif)
{
  if (S.empty()) return;

  report_func  = rif;

  // allocate space for |original|
  original = new map<SEGMENT,SEGMENT>;

  if( !original ) LEDA_EXCEPTION(1,"balaban: out of memory.");
  
  // sort the endpoints of the segments in $S$ lexico
  sortseq <POINT, int> ps_queue;
  SEGMENT s;
  forall(s,S)
  {
    seq_item sit1 = ps_queue.insert(s.start(), 0);
    seq_item sit2 = ps_queue.insert(s.end(), 0); 

    SEGMENT slex( ps_queue.key(sit1), ps_queue.key(sit2) );
	  
    // adjust segments from left to rigth or upwards
    if ( compare(slex.start(), slex.end()) > 0 ) 
	    slex = SEGMENT( slex.end(), slex.start() );

    (*original)[slex] = s;
    seg_queue.append(slex);
   }
	
  // sort |seg_queue| using the ordering defined by the 
  // function |compare2(const SEGMENT&, const SEGMENT&)|
  seg_queue.sort(compare2);

  int N = ps_queue.size();
  StairCount = 0;

  // allocate space for |init_points| and |stairs|

  init_points = new POINT[N+1];
  stairs      = new SEGMENT[S.length()+1];
  if ( !init_points || !stairs )  
    LEDA_EXCEPTION(1,"balaban: out of memory.");

  // initialize init_points
  POINT* pp = init_points;
  seq_item sit;
  forall_items(sit,ps_queue) *++pp = ps_queue.key(sit);
      
  slist<SEGMENT> L; 
  slist<SEGMENT> I;
  slist<SEGMENT> R;

  // initialize L 
  Lrs_from_Rls(R, L, 1);
      
  // initialize I
  forall(s,seg_queue)
    if (!identical(s.end(),init_points[N])) I.append(s);
      
  // start $TreeSearch$
 if( N > 1 )
 { TreeSearch_nloghoch2n(L, I, 1, N, R);
   Lrs_from_Rls(R, L, N);
  }
  
  balaend(S,N);
}

void TreeSearch_nloghoch2n(slist<SEGMENT>& Lv, slist<SEGMENT>& Iv,
			   int b, int e, slist<SEGMENT>& Rv)
{   
  // 1. if e-b = 1 then SearchInStripbe(Lv, Rv); exit;
  
  if( e-b == 1 )  { SearchInStrip(Lv, Rv, b, e); return; }
  
  // 2. Splitbe(Lv, Qv, Lls); Dv := (Qv, <b,e>);
  //
  //        and
  // 
  // 3. Find Int(Dv, Lls);
  
  STAIRCASE     Dv(b, e);
  
  Split(Lv, Dv);

  // 10. For s ele Iv do find Loc(Dv, s) endfor
  // 11. Find Int(Dv, Iv)
  
  InnerIntPoints(Dv, Iv);

  // 4. c := (b+e)/2;
  
  int   c = (b+e)/2;  
  POINT pc = init_points[c];
  
  // 5. Place segments of Iv
  //     inner for <b,c> into Ils
  //     inner for <c,e> into Irs

  slist<SEGMENT> Ils;
  slist<SEGMENT> Irs;
  
  while (!Iv.empty())
    {
      if( compare( Iv.head().end(), pc) < 0 ) // inner <b,c>
	Ils.append(Iv.pop());
      else
	if( compare(Iv.head().start(), pc) > 0 ) // inner <c, pe>
	  Irs.conc(Iv);
	else 
	  Iv.pop();
    }
  
  // 6. TreeSearch(Lls, Ils, b, c, Rls);

  TreeSearch_nloghoch2n( Lv, Ils, b, c, Rv);

  // 7. If pc is left endpoint of s(c) Lrs := insert s(c) into Rls
  //                           else    Lrs := remove s(c) from Rls
  
  Lrs_from_Rls(Rv, Lv, c); 

  // 8. TreeSearch(Lrs, Irs, c, e, Rrs);

  TreeSearch_nloghoch2n( Lv, Irs, c, e, Rv);
    
  //  9. Find Int(Dv, Rrs)
  // 12. Rv := Mergee(Qv, Rrs)
  
  Merge(Dv, Rv);

  StairCount -= Dv.Count();  // forget these stairs
}

void SearchInStrip(slist<SEGMENT>& L, slist<SEGMENT>& R, int b, int e)
{
  STAIRCASE     D(b, e);
  
  Split(L, D);
  
  if( !L.empty() ) 
  { if( D.empty() ) { L.clear(); return; }
    else SearchInStrip(L, R, b, e);
   }
  
  Merge(D, R);

  StairCount -= D.Count();
}

void InnerIntPoints(const STAIRCASE& D, const slist<SEGMENT>& I)
{ // Precondition : all segments in I are inner from <D.b, D.e>
  // Find intersections between segments in D.Q and I (I is not ordered)
  // O( |I|log|D.Q| + |Int(D,I)|)-time
  
  if ( D.empty() || I.empty() ) return;
  
  SEGMENT s(I.head());
  
  forall(s, I)
    {
      int i = Loc(D, s);   int j = i + 1;
      
      // intersections between s and stairs below s.start()
      while( i > 0 && ( orientation(D[i], s.end()) <= 0 || 
      			!orientation(D[i], s.start()) ) )
      	ReportIntersection(D[i--], s);
      
      // intersections between s and stairs above s.start()
      while ( j<=D.Count() && ( orientation(D[j], s.end()) >= 0 ||
			      !orientation(D[j], s.start()) ) )
	ReportIntersection(D[j++], s);
    }
}

int  Loc(const STAIRCASE& D, const SEGMENT s)
{ // Precondition : s is inner from <D.b, D.e>
  //                D is not empty
   
  if( orientation (D[1], s.start()) < 0 )          return 0;
  if( orientation (D[D.Count()], s.start()) > 0 )  return D.Count();
  
  int locmin = 1;
  int locmax = D.Count(); 
  
  while(locmax-locmin > 1)
    {
      int i = (locmin+locmax)/2;
      SEGMENT sd(D[i]);
      int o = orientation( sd, s.start() ); 
      
      if( o > 0 ) locmin = i;
      else
	if( o < 0 ) locmax = i;
	else { locmin = i; locmax = i; }
    }
  
  return locmin;
}

void ReportIntersection(const SEGMENT& s1, const SEGMENT& s2)
{
  if ( report_func ) report_func( (*original)[s1],(*original)[s2] );
}

void Split(slist<SEGMENT>& L, STAIRCASE& D)
{ 
  slist_item lit = L.first();                 if(!lit) return;
  slist_item lits_prev = nil;
  SEGMENT    s(L.head());
  POINT      pb(init_points[D.b]);
  POINT      pe(init_points[D.e]);
  slist<int> locs;
  int        loc=0, i;

  while ( lit )
    {
      s = L.inf( lit );
      if ( compare(s.end(), pe) >= 0 ) // s is spanning $(|pb|,|pe|)$ 
	if ( D.empty() || cmp_at_x(D[D.Count()], s, pe) <= 0 )
	  {  
	    D.append(s); 
	    lit = L.succ(lit);
	    if ( lits_prev )
	      L.del_succ_item(lits_prev);
	    else L.pop();
	    loc++; 
	  }
	else { locs.append(-loc); lits_prev = lit; lit = L.succ(lit); }
      else   { locs.append(loc); lits_prev = lit; lit = L.succ(lit); }
    }  
 
  if ( D.empty() || L.empty() ) return;

  slist<int>::item liti = locs.first();
  forall(s, L)
    {
      loc = locs.inf(liti); 
      liti = locs.succ(liti);
      if( loc < 0 )
	{
	  i = -loc;
	  // intersections between s and stairs below s
	  ReportIntersection(D[i--], s); // s would be a stair if not	  
	  while( i > 0 &&  cmp_at_x(D[i], s, pe) > 0  )
	    ReportIntersection(D[i--], s);
	}
      else
	{
	  i = loc;
	  // intersections between s and stairs below s
	  while( i > 0 && orientation(D[i], s.end()) <= 0 )
	    ReportIntersection(D[i--], s);
	  i = loc+1;
	  // intersections between s and stairs above s
	  while ( i<=D.Count() && orientation(D[i], s.end()) >= 0 )
	    ReportIntersection(D[i++], s);
	}
    }
  locs.clear();
}

void Lrs_from_Rls(slist<SEGMENT>& R, slist<SEGMENT>& L, int c)
{    
  SEGMENT         s; 
  POINT           pc(init_points[c]);
 
  // put all segments from R to L
  L.conc(R);
  slist_item      lits_prev = nil;
  slist_item      lit = L.first();
  
  while ( lit && orientation(R.inf(lit), pc) > 0  ) 
    { lits_prev = lit; lit = L.succ(lit); }
  
  slist<SEGMENT>   endings;
  slist<SEGMENT>   pass_start;
  slist<SEGMENT>   nuller;
  SEGMENT          si;
  
  while ( lit && !orientation(R.inf(lit), pc) ) 
    { 
      s = R.inf(lit);
      if( identical( s.end(), pc ) ) // segments ending at pc
	{
	  forall(si, endings)    ReportIntersection(s, si);
	  forall(si, pass_start) ReportIntersection(s, si);
	  endings.append(s);
	  lit = L.succ(lit);
	  if ( lits_prev )  L.del_succ_item(lits_prev);
	  else L.pop();
	}
      else                          // segments passing pc
	{  
	  forall(si, endings)    ReportIntersection(s, si);
	  forall(si, pass_start)
	    if( orientation(s, si.start()) ) ReportIntersection(s, si);
	  pass_start.push(s); 
	  lit = L.succ(lit);
	  if ( lits_prev )  L.del_succ_item(lits_prev);
	  else L.pop(); 
	}
    }
 
  while(!seg_queue.empty() && identical(seg_queue.head().start(), pc))
    { 
      s = seg_queue.pop();
      if ( identical( s.end(), pc ) ) // zero length segments
	{
	  forall(si, endings)    ReportIntersection(s, si);
	  forall(si, pass_start) ReportIntersection(s, si);
	  forall(si, nuller)     ReportIntersection(s, si);
	  nuller.append(s);
	}
      else                            // segments starting at pc
	{ 
	  forall(si, endings) ReportIntersection(s, si);
	  forall(si, nuller)  ReportIntersection(s, si);
	  slist_item lit1 = pass_start.first();
	  slist_item lit2 = 0;
	  while(lit1)
	    {
	      si = pass_start.inf(lit1);
	      int o = orientation(si, s.end());
	      if(o) ReportIntersection(s, si);
	      if( o > 0) lit2 = lit1; 
	      lit1 = pass_start.succ(lit1);
	    }
	  if (!lit2) pass_start.push(s);
	  else       pass_start.insert(s, lit2);
	}
    }
 
  forall(s, pass_start) 
    {
	if(lits_prev) 
	  { 
	    L.insert(s, lits_prev); 
	    lits_prev = L.succ(lits_prev); 
	  }
	else  lits_prev = L.push(s);
    }
}

void Merge(STAIRCASE& D, slist<SEGMENT>& R)
{ // Merge the stairs in D and the segments in R and return result in R.
  // Precondition : D.Q, and R are orderd by <(D.e.xcoord)
  
  int        loc = 1, i, j;
  POINT      pb(init_points[D.b]);
  POINT      pe(init_points[D.e]);
  slist_item lit = R.first();
  slist_item lits_prev = nil;
  
  if( D.Count() == 0 )  return;

  while( lit || loc <= D.Count() )
    {
      if( !lit )      
	{ 
	  for(; loc <= D.Count();) R.append(D[loc++]);
	  return; 
	}
      
      SEGMENT s( R.inf(lit) );
      
      int c_x = loc > D.Count() ? 1 : cmp_at_x(s, D[loc], pe);
      
      if ( c_x < 0 )
	{
	  if( compare( s.start(), pb ) > 0 )
	    {
	      i = loc-1; j = loc;
	      
	      while( i > 0 && orientation(D[i], s.start()) <= 0 )
		ReportIntersection(D[i--], s);
	      
	      while( j<=D.Count() && orientation(D[j], s.start()) >= 0)
		ReportIntersection(D[j++], s);
	    }
	  lits_prev = lit; lit = R.succ(lit);
	}
      else
	if ( !c_x ) 
	  {
	    do
	      {
		lits_prev = lit; lit = R.succ(lit);
	      } while( lit && !cmp_at_x(R.inf(lit), D[loc], pe) );
	    
	    while( loc<D.Count() && !cmp_at_x(D[loc+1],D[loc],pe))
	      { R.insert(D[loc++], lits_prev); lits_prev = R.succ(lits_prev); }
	  }
	else 
	  {
	    if(loc > D.Count())
	      {
		if( compare( s.start(), pb ) > 0 )
		  {
		    i = loc-1;
		    
		    while( i > 0 && orientation(D[i], s.start()) <= 0 )
		      ReportIntersection(D[i--], s);
		  }
		lits_prev = lit; lit = R.succ(lit);
	      }
	    else
	      if(lits_prev)
		{ 
		  R.insert(D[loc++], lits_prev); lits_prev = R.succ(lits_prev);
		}
	      else  lits_prev = R.push(D[loc++]);
	  }
    }
}

LEDA_END_NAMESPACE
