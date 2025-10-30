/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  halfplane_inter.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/map.h>

// -------------------------------------------------
// construction of dual points 
// -------------------------------------------------

LEDA_BEGIN_NAMESPACE

static bool get_dual_point(const LINE& l,POINT& pd)
{ pd = l.dual();
  return l.point1().xcoord() <= l.point2().xcoord();
}


// ---------------------------------------------------------------------------
// handling of vertical lines 
// (if there are verticals, the halfplane intersection of them is computed)
// ---------------------------------------------------------------------------

static bool handle_verticals(const list<LINE>& VL, RAT_TYPE& left, 
                             RAT_TYPE& right, bool& cl, bool& cr)
{
  // returns true if intersection is not empty and false otherwise
  // precondition: VL not empty
  // VL list of verticals 
  // left/right: x coordinates of left/right end of the stripe
  // cl/cr: stripe closed on left/right side ?

 cl = false; cr = false;
 LINE lakt;
 forall(lakt,VL)
 {
  RAT_TYPE y1 = lakt.point1().ycoord();
  RAT_TYPE y2 = lakt.point2().ycoord();
  RAT_TYPE x  = lakt.point1().xcoord();

  if (y1<y2)
  { // this halfplane is open right 
    if (!cl) { cl=true; left=x; }
    else if (x>left) left=x;
  }
  else
  { // this halfplane is open left 
    if (!cr) { cr=true; right=x; }
    else if (x<right) right=x;
  }
 }
 if (cl && cr && left<=right) return true; // closed left and right
 if (!cl && cr) return true;   // open left 
 if (cl && !cr) return true;   // open right
 return false; //verticals, but intersection empty 
}



// ---------------------------------------------------------------------------
// compute from the lines in Lin describing the edge of the (lower/upper) 
// halfplane intersection the rays/lines and the segment chain describing it
// ---------------------------------------------------------------------------

static void build_lists(const list<LINE>& Lin, list<RAY>& LR, list<LINE>& LL, 
                                                              list<SEGMENT>& LS)
{
  LR.clear();
  LL.clear();
  LS.clear();

  if (Lin.size() ==0) return; // input - no lines 

  if (Lin.size() ==1) {       // input - one line; output one line. two rays 
    LINE lact = Lin.head();
    LL.append(lact); 
    POINT p1= lact.point1(), p2 = lact.point2();
    RAY r1(p2,p1);
    VECTOR vec = -(p1.to_vector()-p2.to_vector());
    RAY r2(p2,p2+ vec);
    LR.append(r1); // rays <---- lact.point2() ---->
    LR.append(r2);
    return; 
  }
  
  // input is more than one line .
  LINE l1=Lin.head();
  LINE l2=Lin.tail();
  LL.append(l1); 
  LL.append(l2);

  if (Lin.size() ==2) {
    POINT inter;
    VECTOR vec;
    RAY r1,r2;
    //bool b = 
    l1.intersection(l2,inter);
    
    POINT p1= l1.point1(); if (p1==inter) p1=l1.point2();
    POINT p2= l2.point1(); if (p2==inter) p2=l2.point2();
    
    if (orientation(l1,p2)<0) { //rightturn
      r2 = RAY(inter,p2);
    }
    else {
      //vec= -(p2.to_vector() - inter.to_vector());
      r2 = RAY(inter,p2);
      r2 = r2.reflect(inter);
    }
    
    if (orientation(l2,p1)<0) { //rightturn
      r1 = RAY(inter,p1);
    }
    else {
      //vec= -(p1.to_vector() - inter.to_vector());
      //r1 = RAY(inter, inter + vec);
      r1 = RAY(inter,p1);
      r1 = r1.reflect(inter);
    }
    
    LR.append(r1);
    LR.append(r2);     
  }
  else {
    // size >= 3 
    list_item it;
    LINE  l_akt, l_next;
    POINT prev,akt;

    forall_items(it,Lin) {
     prev=akt;
     l_akt=Lin[it];
     l_next=Lin[Lin.cyclic_succ(it)];
     l_next.intersection(l_akt,akt); 

     if (it != Lin.last() && it != Lin.first()) { // build a segment 
      LS.append(SEGMENT(prev,akt));
     }
    }  
  
    //construct 2 rays
    VECTOR vec;
    SEGMENT seg1=LS.head();
    SEGMENT seg2=LS.tail();
    POINT pt1=Lin.head().point1();
    if (pt1==seg1.source()) pt1=Lin.head().point2();
    POINT pt2=Lin.tail().point1();
    if (pt2==seg2.target()) pt2=Lin.tail().point2(); 
  
    l1=LINE(seg1.source(),seg1.target());
    l2=LINE(seg2.source(),seg2.target());
    RAY r1,r2;
  
    if (orientation(l2,pt2)<0) r2 = RAY(seg2.target(),pt2);
    else {
      vec= -(pt2.to_vector() - seg2.target().to_vector());
      r2 = RAY(seg2.target(), seg2.target() + vec);
    }
    
    if (orientation(l1,pt1)<0) r1 = RAY(seg1.source(),pt1);
    else {
      vec= -(pt1.to_vector() - seg1.source().to_vector());
      r1 = RAY(seg1.source(), seg1.source() + vec);
    }   
    LR.append(r1); LR.append(r2);
  }
}

// ----------------------------------------------------------
//  compute bounding box of "polyline" Lseg, return it in r
// ----------------------------------------------------------

static bool get_bbox(const list<SEGMENT>& Lseg, RECTANGLE& r)
{
 // return the bounding box of the segment chain in r
 // returns false, if Lseg is empty, true otherwise

 if (Lseg.empty() ) return false;
 RAT_TYPE xmin,xmax,ymin,ymax;
 SEGMENT s;
 // init 
 POINT p=Lseg.tail().target();
 xmin= p.xcoord(); xmax=p.xcoord();
 ymin= p.ycoord(); ymax=p.ycoord();
 
 forall(s,Lseg){
   p=s.source();
   if (p.xcoord() < xmin) xmin=p.xcoord();
   if (p.xcoord() > xmax) xmax=p.xcoord();
   if (p.ycoord() < ymin) ymin=p.ycoord();
   if (p.ycoord() > ymax) ymax=p.ycoord();    
 }
 r = RECTANGLE(POINT(xmin,ymin),POINT(xmax,ymax)); 
 return true;
}

// ---------------------------------------------------------------------
//  construct and return a polygon from the input in Lr and Ls
//  use box for "cutting" back the halfplane intersection  
// ---------------------------------------------------------------------


static GEN_POLYGON construct_polygon(const list<RAY>& Lr, 
                                     const list<SEGMENT>& Ls,  
                                     const RECTANGLE& box, 
                                     list<POINT>& BACK)
{
 list<POINT> ADDIT;
 
 GEN_POLYGON pol;
 SEGMENT s1(box.lower_left(),box.upper_left());
 SEGMENT s2(box.upper_left(),box.upper_right());
 SEGMENT s3(box.upper_right(),box.lower_right());
 SEGMENT s4(box.lower_right(),box.lower_left());
 
 RAY r1=Lr.head(),r2=Lr.tail();
 POINT pout1(0,0),pout2,pact;
 bool b;
 int index;
 SEGMENT segs[4]; // segment array
 segs[0] = s1; segs[1]= s4; segs[2] = s3; segs[3] = s2;
 
 // compute intersection of rays with box 
 
 b=r1.intersection(s1,pout1); 
 if (! b){
   b=r1.intersection(s2,pout1); 
   if (! b){
     b=r1.intersection(s3,pout1);
     if (! b) { b=r1.intersection(s4,pout1); index=1; }
     else index=2;
   }
   else index=3;
 }
 else index=0;
 
 // ADDIT  pointlist of the polygon vertices 
 
 ADDIT.append(pout1);
 //was before :  
 while (! r2.intersection(segs[index],pout2)){
  if (ADDIT.tail()!=segs[index].source() && ADDIT.head()!=segs[index].source()) 
     ADDIT.append(segs[index].source());
  index++;
  if (index==4) index=0;
 }
 if (ADDIT.tail() != pout2 && ADDIT.head() != pout2) ADDIT.append(pout2);
 
 BACK=ADDIT;
 
 SEGMENT sakt;
 forall(sakt,Ls) {
  if (ADDIT.head() != sakt.source() && ADDIT.tail() != sakt.source()) 
     ADDIT.push(sakt.source());
 }
 
 if (! Ls.empty()) {
  if (ADDIT.head() != Ls.tail().target() && ADDIT.tail() != Ls.tail().target()) 
    ADDIT.push(Ls.tail().target());
 }
 else {
  if (! collinear(r1.point1(),r1.point2(),r2.point2())) {
     if (ADDIT.head() != r1.source() && ADDIT.tail() != r1.source())
       ADDIT.push(r1.source());
  }
 }
 
 pol=GEN_POLYGON(ADDIT);
 return pol;
}



static RECTANGLE stretch(const RECTANGLE& input)
{
  // stretch input rectangle 

  RAT_TYPE xmin=input.xmin() - RAT_TYPE(50.0); 
  RAT_TYPE ymin=input.ymin() - RAT_TYPE(50.0);
  RAT_TYPE xmax=input.xmax() + RAT_TYPE(50.0);
  RAT_TYPE ymax=input.ymax() + RAT_TYPE(50.0);  
  return RECTANGLE(POINT(xmin,ymin),POINT(xmax,ymax));
}

static bool on_line(const LINE& l , const SEGMENT& seg)
{ return l.contains(seg.source()) && l.contains(seg.target()); }

static void handle_head(list<POINT>& hull_part){ 
 if (hull_part.size() < 2) return;
 POINT p1 = hull_part.head();
 POINT p2 = hull_part[hull_part.get_item(1)];
 if (POINT::cmp_x(p1,p2) == 0)  hull_part.pop_front(); 
}

static void handle_tail(list<POINT>& hull_part){ 
 if (hull_part.size() < 2) return;
 int v = hull_part.size()-2;
 POINT p1 = hull_part.tail();
 POINT p2 = hull_part[hull_part.get_item(v)]; 
 if (POINT::cmp_x(p1,p2) == 0)  hull_part.pop_back();  
}

static void elim_hull(list<POINT>& hull_part, bool kind) 
// kind true  upper, false lower hull 
{ 
 if (kind) { handle_head(hull_part); handle_tail(hull_part); }
 else { handle_tail(hull_part); handle_head(hull_part); }
}


// -----------------------------------------------------------------------
//  computes the halfplane intersection of the lines in Lin
// -----------------------------------------------------------------------

void  HALFPLANE_INTERSECTION(const list<LINE>& Lin, list<LINE>& Lout)
{
 list<SEGMENT> Lseg;
 Lout.clear();
 
 HALFPLANE_INTERSECTION(Lin, Lseg);
 SEGMENT siter;
 
 forall(siter, Lseg){
  Lout.append(LINE(siter.source(), siter.target()));
 }
}

void  HALFPLANE_INTERSECTION(const list<LINE>& Lin2, list<SEGMENT>& Lseg)
{
 Lseg.clear();
 if (Lin2.size() == 1){ Lseg.append(SEGMENT(Lin2.head().point1(),Lin2.head().point2())); return; }

 LINE lakt;
 list<LINE> Lin;
 forall(lakt,Lin2) Lin.append(lakt.reverse());

 list<LINE> Lout1, Lout2;

 if (Lin.empty()) return;
 
 list<LINE> Vert; // vertical lines 
 list<LINE> Ln;   // other (non-vertical) lines 
 
 // get vertical and nonvertical lines 
 forall(lakt,Lin) { 
  if (lakt.is_vertical()) Vert.append(lakt); // vertical 
  else Ln.append(lakt);                      // nonvertical 
 }

 // handle the vertical lines 
 bool closed_right=false, closed_left=false;
 RAT_TYPE p_left=0,p_right=0;

 bool something; 

 if (! Vert.empty()) {
  something=handle_verticals(Vert, p_left, p_right, closed_left, closed_right);
  if (! something) return; // case, that intersection of the verticals was empty 
 
  if (Ln.empty()){ // only verticals 
    if (closed_left) Lseg.append(SEGMENT(POINT(p_left,RAT_TYPE(0)),POINT(p_left,RAT_TYPE(100))));
    if (closed_right) Lseg.append(SEGMENT(POINT(p_right,RAT_TYPE(100)),POINT(p_right,RAT_TYPE(0))));    
    return;
  }
 }
 
 // now handle the non-vertical lines 

 POINT Hp;
 list<POINT> Hcand_lower; // for lower hull 
 list<POINT> Hcand_upper; // for upper hull 
 map<POINT,LINE> M1;
 map<POINT,LINE> M2; 

 forall(lakt,Ln) { // compute the lists of dual points for upper and lower hull 
   bool flag = get_dual_point(lakt,Hp);
   if (flag) {
    Hcand_lower.append(Hp); // will be used for the lower hull 
    M1[Hp]=lakt; // mark the line of the dual point 
   }
   else {
    Hcand_upper.append(Hp); // will be used for the upper hull 
    M2[Hp]=lakt; // mark the line of the dual point 
   }
 }

 // compute the upper/lower hulls 
 list<POINT> lower = LOWER_CONVEX_HULL(Hcand_lower);

 // eliminate vertices at start/end of hull part, if necessary
 elim_hull(lower,false);

 list<POINT> upper = UPPER_CONVEX_HULL(Hcand_upper);
 
 // eliminate vertices at start/end of hull part, if necessary 
 elim_hull(upper, true);
 
 // now get the corresponding lines of the hull points 
 forall(Hp,lower) Lout1.append(M1[Hp]);
 forall(Hp,upper) Lout2.append(M2[Hp]);
 
 list<LINE> Lhelp;
 if (Lout1.size() == 1 && Vert.empty()) Lhelp.append(Lout1.head());
 if (Lout2.size() == 1 && Vert.empty()) Lhelp.append(Lout2.head()); 

 list<LINE> Lline1,Lline2;  //bordering lines (l_first-seg-l_last)
 list<SEGMENT> Lseg1,Lseg2; //bordering segments
 list<RAY> LR1,LR2;
 
 // build lists from the output of chull
 // Lout1/Lout2 input lists of lines 
 build_lists(Lout1,LR1,Lline1,Lseg1);
 build_lists(Lout2,LR2,Lline2,Lseg2);

 LINE l1,l2,l3,l4;

 list<POINT> INTER;
 POINT P1,P2,P3,P4,Pakt;
 bool flag;
 
 if (! Lline1.empty()){ l1 = Lline1.head(); l2 = Lline1.tail(); }
 if (! Lline2.empty()){ l3 = Lline2.head(); l4 = Lline2.tail(); }
  
  // compute intersections  
  if (Lline1.size()>0 && Lline2.size()>0){
   flag = l1.intersection(l3,P1); if (flag) INTER.append(P1);
   flag = l1.intersection(l4,P2); if (flag) INTER.append(P2);
   flag = l2.intersection(l3,P3); if (flag) INTER.append(P3);
   flag = l2.intersection(l4,P4); if (flag) INTER.append(P4);
  }
  
  // if there were verticals too
  // compute intersection points with verticals, store them in INTER
  if (! Vert.empty()){
    if (closed_left){
      LINE lhelp(POINT(p_left,RAT_TYPE(-1000)),POINT(p_left,RAT_TYPE(1000)));
      if (Lline1.size()>0){
       flag = lhelp.intersection(l1,P1); if (flag) { INTER.append(P1); }
       flag = lhelp.intersection(l2,P1); if (flag) { INTER.append(P1); }
      }
      if (Lline2.size()>0){
       flag = lhelp.intersection(l3,P1); if (flag) { INTER.append(P1); }
       flag = lhelp.intersection(l4,P1); if (flag) { INTER.append(P1); } 
      } 
    }
    if (closed_right){
      LINE lhelp(POINT(p_right,RAT_TYPE(1000)),POINT(p_right,RAT_TYPE(-1000)));    
      if (Lline1.size()>0){
       flag = lhelp.intersection(l1,P1); if (flag) { INTER.append(P1); }
       flag = lhelp.intersection(l2,P1); if (flag) { INTER.append(P1); }
      }
      if (Lline2.size()>0){
       flag = lhelp.intersection(l3,P1); if (flag) { INTER.append(P1); }
       flag = lhelp.intersection(l4,P1); if (flag) { INTER.append(P1); }
      }           
    }
  }
 
  RECTANGLE rec1,rec2,rec;
  
  // get bounding boxes of segment chains
  bool fl1= get_bbox(Lseg1,rec1);
  bool fl2= get_bbox(Lseg2,rec2);


 if (Lline1.size()!=0 ){
  if (!fl1) { // init box 
   rec1 = RECTANGLE(Lline1.head().point1(),Lline1.head().point2());
   rec1 = rec1.include(Lline1.tail().point1());
   rec1 = rec1.include(Lline1.tail().point2());
  }
 }

 if (Lline2.size()!=0 ){
  if (!fl2) { // init box 
   rec2 = RECTANGLE(Lline2.head().point1(),Lline2.head().point2());
   rec2 = rec2.include(Lline2.tail().point1());
   rec2 = rec2.include(Lline2.tail().point2());  
  }
 }
 
 // include points of rays 
 RAY r;
 forall(r,LR1) { 
    rec1 = rec1.include(r.point1()); 
    rec1 = rec1.include(r.point2());
 }

 forall(r,LR2) { 
   rec2 = rec2.include(r.point1()); 
   rec2 = rec2.include(r.point2()); 
 }
  
 // change the bounding boxes with the POINTS from segment intersection 
 forall(Pakt,INTER){ rec1 = rec1.include(Pakt); rec2 = rec2.include(Pakt); }

 if (Lline1.size() != 0) rec = rec1; else rec = rec2;

 if (Lline1.size() != 0 && Lline2.size() != 0) rec = rec.include(rec2);

 rec= stretch(rec); 
  
 //build 2 polygons 
 list<POINT> BACK1,BACK2;
  
 GEN_POLYGON pol1,pol2;
 if (! Lline1.empty()) pol1 = construct_polygon(LR1,Lseg1,rec,BACK1);
 if (! Lline2.empty()) pol2 = construct_polygon(LR2,Lseg2,rec,BACK2);

  BACK1.conc(BACK2);

  GEN_POLYGON pol3;

  // now compute polygon intersection, if necessary 
  
  if (! (Lline1.size()==0 || Lline2.size()==0)) pol3 = pol1.intersection(pol2);
  else {
   if (Lline1.size()==0) pol3=pol2; else pol3=pol1;
  }
  
  // if there are verticals, we have to do a second polygon intersection 
  if (! Vert.empty()){
    RAT_TYPE xmin,xmax,ymin,ymax;
    ymin= rec.ymin();
    ymax= rec.ymax();
    
    // first build a polygon for our vertical stripe 
    
    if (! closed_left) xmin=rec.xmin(); else xmin=p_left;
    if (! closed_right) xmax=rec.xmax(); else xmax=p_right;
    list<POINT> hlp;
    hlp.append(POINT(xmin,ymin)); hlp.append(POINT(xmax,ymin));
    hlp.append(POINT(xmax,ymax)); hlp.append(POINT(xmin,ymax));
    
    BACK1.append(POINT(xmin,ymin)); BACK1.append(POINT(xmax,ymin));
    BACK1.append(POINT(xmax,ymax)); BACK1.append(POINT(xmin,ymax));
        
    GEN_POLYGON VP(hlp);
    // another intersection operation 
    pol3 = pol3.intersection(VP);
  }

  // now the halfplane-intersection is constructed in form of a polygon 

  list<POINT> PV = pol3.vertices();
  POINT pvakt;
  Lseg.clear();
  
  list<SEGMENT> part2;
  flag = true;
  
  // we have to separate the edges not present in the hp - intersection
  
  POLYGON sp;
  SEGMENT seg;
  forall_polygons(sp,pol3){
    forall_segments(seg,sp){
      if (! (BACK1.search(seg.source()) && BACK1.search(seg.target())) ||
             (!Lhelp.empty() && (on_line(Lhelp.head(),seg) || on_line(Lhelp.tail(),seg)))
          )
        if (flag) part2.append(seg); // second part of segment chain 
	else Lseg.append(seg); // first part of segment chain 
	
      else flag = false;
    }
  }
  
  // concatenate the two segment chains   
  Lseg.conc(part2);

}


LEDA_END_NAMESPACE
