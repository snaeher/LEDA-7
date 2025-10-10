// Quickhull template versions ...

#include <LEDA/list.h>


#if defined(__GLGP_PROVIDE_LINE_NUMBERS__)

#define __GLGP_LINE__ __LINE__
// if it is an additonal parameter ...
#define __GLGP_LINE_PARAMETER__ ,__LINE__

#else

#define __GLGP_LINE__
// if it is an additonal parameter ...
#define __GLGP_LINE_PARAMETER__ 

#endif


LEDA_BEGIN_NAMESPACE

// Graham scan variants ...

// requirements: sort needs compare/less
// orientation

template<class KERNEL>
void convex_hull_lower_t(list<typename KERNEL::POINT>& ein, list<typename KERNEL::POINT>& aus)
{
 typedef typename KERNEL::POINT            POINT;
 typedef typename KERNEL::ORIENTATION      ORIENTATION;
 
 ORIENTATION __ORIENTATION;

 aus.clear();
 ein.sort(); // problematic (maybe we should use std::list ???)

 POINT p,x,y,qi;
 int a,n,fl;
 
 n=ein.length(); 
 
 p=ein.Pop(); aus.push_front(p); 
 p=ein.Pop(); aus.push_front(p); 
  
 for(a=3;a<=n;a++){
    y=aus.pop();  // second point ...
    x=aus.pop();  // first point ... 
    qi=ein.Pop(); // new point ...
    fl= __ORIENTATION(x,y,qi);
    while ((fl>=0) && (! aus.empty())){
             y=x; x= aus.pop(); 
             fl = __ORIENTATION(x,y,qi);
             } 
    if (fl>=0) { 
     aus.push_front(x); aus.push_front(qi);
    }    
    else {                      
     aus.push_front(x);
     aus.push_front(y);
     aus.push_front(qi); // qi is "top of stack"
    }
 }
}

template<class KERNEL>
void convex_hull_upper_t(list<typename KERNEL::POINT>& ein, list<typename KERNEL::POINT>& aus)
{
 typedef typename KERNEL::POINT            POINT;
 typedef typename KERNEL::ORIENTATION      ORIENTATION;
 
 ORIENTATION __ORIENTATION;

 aus.clear();
 ein.sort();  // problematic !!!

 POINT p,x,y,qi;
 int a,n,fl;
 
 n=ein.length(); 
 
 p=ein.Pop(); aus.push_back(p); 
 p=ein.Pop(); aus.push_back(p); 
  
 for(a=3;a<=n;a++){
    y=aus.Pop();  
    x=aus.Pop(); 
    qi=ein.Pop(); 
    fl = __ORIENTATION(x,y,qi);
    while ((fl<=0) && (! aus.empty())){
             y=x; x= aus.Pop(); 
             fl = __ORIENTATION(x,y,qi);
             } 
    if (fl<=0) { 
     aus.push_back(x); aus.push_back(qi);
    }    
    else {                      
     aus.push_back(x); 
     aus.push_back(y);
     aus.push_back(qi); // qi is "top of stack"
    }
 }
}


// ----------------------------------------------------------------------
// "normal" Quickhull
// ----------------------------------------------------------------------

// requirements: cmp_signed_dist to line through two points ...
// orientation
// collinear
// special orientation with two fix points ...

template<class KERNEL>
void  Quickhull_t(const list<typename KERNEL::POINT>& zg, 
                  list<typename KERNEL::POINT>& result, 
		  const typename KERNEL::POINT& r, 
		  const typename KERNEL::POINT& l)
{
 typedef typename KERNEL::POINT                    POINT;
 
 typedef typename KERNEL::ORIENTATION              ORIENTATION;
 typedef typename KERNEL::COLLINEAR                COLLINEAR;
 typedef typename KERNEL::RIGHTTURN                RIGHTTURN;
 
 // new: dist comparison
 typedef typename KERNEL::COMPARE_SIGNED_DIST_TO_LINE      COMPARE_SIGNED_DIST_TO_LINE;
 // new special orientation with two fix points ...
 typedef typename KERNEL::ORIENTATION_SPECIAL      ORIENTATION_SPECIAL;
  
 ORIENTATION          __ORIENTATION;
 COLLINEAR            __COLLINEAR;
 RIGHTTURN            __RIGHTTURN;
 COMPARE_SIGNED_DIST_TO_LINE  __COMPARE_SIGNED_DIST_TO_LINE;
 
 // add checks ...
#if defined(CHECK_KERNEL_TRAITS)
#endif

 POINT p,h;
 list<POINT> ein1;
 list<POINT> ein2;
 list<POINT> result2;

 h=zg.head();
 
 forall(p,zg){
  int sc = __COMPARE_SIGNED_DIST_TO_LINE(l,r,p,h __GLGP_LINE_PARAMETER__);
 
  if (sc>0) h=p; 
  if (sc==0 && __RIGHTTURN(r,h,p __GLGP_LINE_PARAMETER__)) h=p;   
 }
 
 if (__COLLINEAR(r,h,l __GLGP_LINE_PARAMETER__)) { result.append(r); result.append(l); return; }
  
 // now we eliminate and build the lists for the recursive calls ...
 if (zg.size() >10) {
  ORIENTATION_SPECIAL O1(r,h);
  ORIENTATION_SPECIAL O2(l,h);
 
  forall(p,zg){ 
   if (O1(p __GLGP_LINE_PARAMETER__)<=0) ein1.append(p);
   if (O2(p __GLGP_LINE_PARAMETER__)>=0) ein2.append(p);
  } 
 }
 else {
  forall(p,zg){ 
   if (__ORIENTATION(r,h,p __GLGP_LINE_PARAMETER__)<=0) ein1.append(p);
   if (__ORIENTATION(l,h,p __GLGP_LINE_PARAMETER__)>=0) ein2.append(p);
  } 
 } 
 // recursive calls ...
 Quickhull_t<KERNEL>(ein1, result, r,h);
 Quickhull_t<KERNEL>(ein2, result2, h,l);
 
 result2.pop(); // h doppelt 
 result.conc(result2);
} 

// ----------------------------------------------------------------------
//   this parameter influences the recursion depth of Quickhull
//   where we switch to Graham Scan !
// ----------------------------------------------------------------------

#if !defined(GEOMLEP_QUICKHULL_SWITCH_DEPTH)
#define GEOMLEP_QUICKHULL_SWITCH_DEPTH  4
#endif

// Quickhull using Graham scan at some recursion depth ...

template<class KERNEL>
void  Quickhull_graham_t(list<typename KERNEL::POINT>& zg, 
                         list<typename KERNEL::POINT>& result, 
		         const typename KERNEL::POINT& r, 
		         const typename KERNEL::POINT& l, 
		         int depth,
		         bool up)
{
 typedef typename KERNEL::POINT                    POINT;
 
 typedef typename KERNEL::ORIENTATION              ORIENTATION;
 typedef typename KERNEL::COLLINEAR                COLLINEAR;
 typedef typename KERNEL::RIGHTTURN                RIGHTTURN;
 
 // new: dist comparison
 typedef typename KERNEL::COMPARE_SIGNED_DIST_TO_LINE      COMPARE_SIGNED_DIST_TO_LINE;
 // new special orientation with two fix points ...
 typedef typename KERNEL::ORIENTATION_SPECIAL      ORIENTATION_SPECIAL;
  
 ORIENTATION          __ORIENTATION;
 COLLINEAR            __COLLINEAR;
 RIGHTTURN            __RIGHTTURN;
 COMPARE_SIGNED_DIST_TO_LINE  __COMPARE_SIGNED_DIST_TO_LINE;

 // add checks ...
#if defined(CHECK_KERNEL_TRAITS)
#endif

 if (depth > GEOMLEP_QUICKHULL_SWITCH_DEPTH) {
   if (up) convex_hull_upper_t<KERNEL>(zg, result);
   else convex_hull_lower_t<KERNEL>(zg, result);
   return;
 }

 POINT p,h;
 list<POINT> ein1;
 list<POINT> ein2;
 list<POINT> result2;

 h=zg.head();
 
 forall(p,zg){
  int sc = __COMPARE_SIGNED_DIST_TO_LINE(l,r,p,h  __GLGP_LINE_PARAMETER__);
 
  if (sc>0) h=p; 
  if (sc==0 && (__RIGHTTURN(r,h,p __GLGP_LINE_PARAMETER__))) h=p;   // was orientation < 0
 }
 
 if (__COLLINEAR(r,h,l __GLGP_LINE_PARAMETER__)) { result.append(r); result.append(l); return; }
  
 // now we eliminate and build the lists for the recursive calls ...
 if (zg.size() >10) {
  ORIENTATION_SPECIAL O1(r,h);
  ORIENTATION_SPECIAL O2(l,h);
 
  forall(p,zg){ 
   if (O1(p __GLGP_LINE_PARAMETER__)<=0) ein1.append(p);
   if (O2(p __GLGP_LINE_PARAMETER__)>=0) ein2.append(p);
  } 
 }
 else {
  forall(p,zg){ 
   if (__ORIENTATION(r,h,p __GLGP_LINE_PARAMETER__)<=0) ein1.append(p); 
   if (__ORIENTATION(l,h,p __GLGP_LINE_PARAMETER__)>=0) ein2.append(p); 
  } 
 } 
 // recursive calls ...
 depth++;
 Quickhull_graham_t<KERNEL>(ein1, result, r,h, depth, up);
 Quickhull_graham_t<KERNEL>(ein2, result2, h,l, depth, up);
 
 result2.pop(); // h doppelt 
 result.conc(result2);
} 

// ----------------------------------------------------------------------


// -----------------------------------------------------------------------------------------
// requirements:
// - cmp_xy / cmp_yx
// - special orientation (with two fix points)
// -----------------------------------------------------------------------------------------

template<class KERNEL,class INPUT_ITERATOR>
list<typename KERNEL::POINT> QUICK_HULL_ITER(INPUT_ITERATOR pstart, INPUT_ITERATOR pend)
{
 typedef typename KERNEL::POINT                    POINT;
 
 typedef typename KERNEL::COMPARE_XY_POINTS        COMPARE_XY_POINTS;
 // new special orientation with two fix points ...
 typedef typename KERNEL::ORIENTATION_SPECIAL      ORIENTATION_SPECIAL;
 // new YX points comparison 
 typedef typename KERNEL::COMPARE_YX_POINTS        COMPARE_YX_POINTS; 

 COMPARE_XY_POINTS __COMPARE_XY_POINTS;
 COMPARE_YX_POINTS __COMPARE_YX_POINTS; 

 // add checks ...
#if defined(CHECK_KERNEL_TRAITS)
#endif

 list<POINT> erg1; 
 list<POINT> erg2;
 list<POINT> erg3;
 list<POINT> erg4;
 list<POINT> inp1; 
 list<POINT> inp2;
 list<POINT> inp3;
 list<POINT> inp4; 
 
 list<POINT> eingabe;
 
 INPUT_ITERATOR pnext = pstart, piter;
 if (pstart != pend) pnext++;
 
 if (pstart==pend || pnext==pend) {
   if (pstart!=pend) eingabe.push_back(*pstart);
   return eingabe; 
 } 
 
 POINT p = *pstart;
 POINT le=p;
 POINT ri=p;
 POINT bo=p;
 POINT to=p;
 
 for(piter=pstart;piter!=pend;piter++){
   p = *piter; 
   if (__COMPARE_XY_POINTS(p,le __GLGP_LINE_PARAMETER__) == -1) le=p;
   if (__COMPARE_XY_POINTS(p,ri __GLGP_LINE_PARAMETER__) ==  1) ri=p;  
   if (__COMPARE_YX_POINTS(p,bo __GLGP_LINE_PARAMETER__) == -1) bo=p;  
   if (__COMPARE_YX_POINTS(p,to __GLGP_LINE_PARAMETER__) ==  1) to=p;    
 }
  
 ORIENTATION_SPECIAL  O1(le,ri);
 ORIENTATION_SPECIAL  O2(to,ri);
 ORIENTATION_SPECIAL  O3(le,to);
 ORIENTATION_SPECIAL  O4(ri,bo);
 ORIENTATION_SPECIAL  O5(bo,le); 
 
 // now compute the 4 input lists inp1, ..., inp4

 for(piter=pstart;piter!=pend;piter++){
    p = *piter; 
    int o1 = O1(p __GLGP_LINE_PARAMETER__);
    if (o1==1) {  
      if (O2(p __GLGP_LINE_PARAMETER__)==1)  inp1.push(p);
      if (O3(p __GLGP_LINE_PARAMETER__)==1)  inp3.push(p);
    }
    else { 
      if (O4(p __GLGP_LINE_PARAMETER__)==1)  inp2.push(p);
      if (O5(p __GLGP_LINE_PARAMETER__)==1)  inp4.push(p);
    }
 }
 inp1.push(to); inp1.push(ri); 
 inp2.push(bo); inp2.push(ri);
 inp3.push(to); inp3.push(le);
 inp4.push(le); inp4.push(bo); 
 
 if (__COMPARE_XY_POINTS(ri, to __GLGP_LINE_PARAMETER__) != 0) Quickhull_t<KERNEL>(inp1, erg1, ri,to); else erg1.push(to);
 if (__COMPARE_XY_POINTS(ri, bo __GLGP_LINE_PARAMETER__) != 0) Quickhull_t<KERNEL>(inp2, erg2, bo,ri); else erg2.push(bo);
 if (__COMPARE_XY_POINTS(le, to __GLGP_LINE_PARAMETER__) != 0) Quickhull_t<KERNEL>(inp3, erg3, to,le); else erg3.push(to);
 if (__COMPARE_XY_POINTS(le, bo __GLGP_LINE_PARAMETER__) != 0) Quickhull_t<KERNEL>(inp4, erg4, le,bo); else erg4.push(bo);
 
 if (__COMPARE_XY_POINTS(erg3.tail(),erg4.head() __GLGP_LINE_PARAMETER__) == 0 ) erg4.pop(); 
 erg3.conc(erg4);
 if (__COMPARE_XY_POINTS(erg3.tail(),erg2.head() __GLGP_LINE_PARAMETER__) == 0) erg2.pop();
 erg3.conc(erg2);
 if (__COMPARE_XY_POINTS(erg3.tail(),erg1.head() __GLGP_LINE_PARAMETER__) == 0) erg1.pop(); 
 erg3.conc(erg1); 
 
 erg3.Pop(); // last == first
 
 return erg3;
}


// -----------------------------------------------------------------------------------
// variants for computing only lower/upper hull

template<class KERNEL,class INPUT_ITERATOR>
list<typename KERNEL::POINT> QUICK_HULL_UPPER_ITER(INPUT_ITERATOR pstart, INPUT_ITERATOR pend)
{
 typedef typename KERNEL::POINT                    POINT;
 
 typedef typename KERNEL::COMPARE_XY_POINTS        COMPARE_XY_POINTS;
 // new special orientation with two fix points ...
 typedef typename KERNEL::ORIENTATION_SPECIAL      ORIENTATION_SPECIAL;
 // new YX points comparison 
 typedef typename KERNEL::COMPARE_YX_POINTS        COMPARE_YX_POINTS; 

 COMPARE_XY_POINTS __COMPARE_XY_POINTS;
 COMPARE_YX_POINTS __COMPARE_YX_POINTS; 

 // add checks ...
#if defined(CHECK_KERNEL_TRAITS)
#endif

 list<POINT> erg1; 
 list<POINT> erg3;
 list<POINT> inp1; 
 list<POINT> inp3;

 list<POINT> eingabe;
 
 INPUT_ITERATOR pnext = pstart, piter;
 if (pstart != pend) pnext++;
 
 if (pstart==pend || pnext==pend) {
   if (pstart!=pend) eingabe.push_back(*pstart);
   return eingabe; 
 }
 
 POINT p = *pstart;
 POINT le=p;
 POINT ri=p;
 POINT to=p;

 for(piter=pstart;piter!=pend;piter++){
   p = *piter; 
   if (__COMPARE_XY_POINTS(p,le __GLGP_LINE_PARAMETER__) == -1) le=p;
   if (__COMPARE_XY_POINTS(p,ri __GLGP_LINE_PARAMETER__) ==  1) ri=p;    
   if (__COMPARE_YX_POINTS(p,to __GLGP_LINE_PARAMETER__) ==  1) to=p;    
 }
  
 ORIENTATION_SPECIAL O1(le,ri);
 ORIENTATION_SPECIAL O2(to,ri);
 ORIENTATION_SPECIAL O3(le,to);
 
 // now compute the 2 input lists
 
 for(piter=pstart;piter!=pend;piter++){
    p = *piter;
    int o1 = O1(p __GLGP_LINE_PARAMETER__);
    if (o1==1) {  
      if (O2(p __GLGP_LINE_PARAMETER__)==1)  inp1.push(p);
      if (O3(p __GLGP_LINE_PARAMETER__)==1)  inp3.push(p);
    }
 }
 inp1.push(to); inp1.push(ri); 
 inp3.push(to); inp3.push(le);
 
 if (__COMPARE_XY_POINTS(ri,to __GLGP_LINE_PARAMETER__) != 0) Quickhull_t<KERNEL>(inp1, erg1, ri,to); else erg1.push(to);
 if (__COMPARE_XY_POINTS(le,to __GLGP_LINE_PARAMETER__) != 0) Quickhull_t<KERNEL>(inp3, erg3, to,le); else erg3.push(to);

 if (__COMPARE_XY_POINTS(erg3.tail(),erg1.head() __GLGP_LINE_PARAMETER__) == 0 ) erg1.pop(); 
 erg3.conc(erg1); 
 
 if (__COMPARE_XY_POINTS(erg3.head(),erg3.tail() __GLGP_LINE_PARAMETER__) == 0 && erg3.size() > 1) erg3.pop(); 
 
 return erg3;
}


template<class KERNEL,class INPUT_ITERATOR>
list<typename KERNEL::POINT> QUICK_HULL_LOWER_ITER(INPUT_ITERATOR pstart, INPUT_ITERATOR pend)
{
 typedef typename KERNEL::POINT                    POINT;
 
 typedef typename KERNEL::COMPARE_XY_POINTS        COMPARE_XY_POINTS;
 // new special orientation with two fix points ...
 typedef typename KERNEL::ORIENTATION_SPECIAL      ORIENTATION_SPECIAL;
 // new YX points comparison 
 typedef typename KERNEL::COMPARE_YX_POINTS        COMPARE_YX_POINTS; 

 COMPARE_XY_POINTS __COMPARE_XY_POINTS;
 COMPARE_YX_POINTS __COMPARE_YX_POINTS; 

 // add checks ...
#if defined(CHECK_KERNEL_TRAITS)
#endif

 list<POINT> erg2;
 list<POINT> erg4;
 list<POINT> inp2;
 list<POINT> inp4; 
 
 list<POINT> eingabe;
 
 INPUT_ITERATOR pnext = pstart, piter;
 if (pstart != pend) pnext++;
 
 if (pstart==pend || pnext==pend) {
   if (pstart!=pend) eingabe.push_back(*pstart);
   return eingabe; 
 }
 
 POINT p= *pstart;
 POINT le=p;
 POINT ri=p;
 POINT bo=p;
 
 for(piter=pstart;piter!=pend;piter++){
   p = *piter;
   if (__COMPARE_XY_POINTS(p,le __GLGP_LINE_PARAMETER__) == -1) le=p;
   if (__COMPARE_XY_POINTS(p,ri __GLGP_LINE_PARAMETER__) ==  1) ri=p;  
   if (__COMPARE_YX_POINTS(p,bo __GLGP_LINE_PARAMETER__) == -1) bo=p;   
 }
  
 ORIENTATION_SPECIAL O1(le,ri);
 ORIENTATION_SPECIAL O4(ri,bo);
 ORIENTATION_SPECIAL O5(bo,le); 
 
 // now compute the 2 input lists
 
 for(piter=pstart;piter!=pend;piter++){
    p = *piter;
   
    int o1 = O1(p __GLGP_LINE_PARAMETER__);
    if (o1==-1) {  
      if (O4(p __GLGP_LINE_PARAMETER__)==1)  inp2.push(p);
      if (O5(p __GLGP_LINE_PARAMETER__)==1)  inp4.push(p);
    }
 }
 inp2.push(bo); inp2.push(ri);
 inp4.push(le); inp4.push(bo); 
 
 if (__COMPARE_XY_POINTS(ri,bo __GLGP_LINE_PARAMETER__) != 0) Quickhull_t<KERNEL>(inp2, erg2, bo,ri); else erg2.push(bo);
 if (__COMPARE_XY_POINTS(le,bo __GLGP_LINE_PARAMETER__) != 0) Quickhull_t<KERNEL>(inp4, erg4, le,bo); else erg4.push(bo);
 
 if (__COMPARE_XY_POINTS(erg4.tail(),erg2.head() __GLGP_LINE_PARAMETER__) == 0 ) erg2.pop(); 
 erg4.conc(erg2);
 
 if (__COMPARE_XY_POINTS(erg4.head(),erg4.tail() __GLGP_LINE_PARAMETER__) == 0 && erg4.size() > 1) erg4.pop();

 return erg4;
}


// -----------------------------------------------------------------------------------

template<class KERNEL,class INPUT_ITERATOR>
list<typename KERNEL::POINT> QUICK_HULL_GRAHAM_ITER(INPUT_ITERATOR pstart, INPUT_ITERATOR pend)
{
 typedef typename KERNEL::POINT                    POINT;
 
 typedef typename KERNEL::COMPARE_XY_POINTS        COMPARE_XY_POINTS;
 // new special orientation with two fix points ...
 typedef typename KERNEL::ORIENTATION_SPECIAL      ORIENTATION_SPECIAL;
 // new YX points comparison 
 typedef typename KERNEL::COMPARE_YX_POINTS        COMPARE_YX_POINTS; 

 COMPARE_XY_POINTS __COMPARE_XY_POINTS;
 COMPARE_YX_POINTS __COMPARE_YX_POINTS; 

 // add checks ...
#if defined(CHECK_KERNEL_TRAITS)
#endif
 
 list<POINT> erg1; 
 list<POINT> erg2;
 list<POINT> erg3;
 list<POINT> erg4;
 list<POINT> inp1; 
 list<POINT> inp2;
 list<POINT> inp3;
 list<POINT> inp4; 
 
 list<POINT> eingabe;
 
 INPUT_ITERATOR pnext = pstart, piter;
 if (pstart != pend) pnext++;
 
 if (pstart==pend || pnext==pend) {
   if (pstart!=pend) eingabe.push_back(*pstart);
   return eingabe; 
 }
 
 POINT p = *pstart;
 POINT le=p;
 POINT ri=p;
 POINT bo=p;
 POINT to=p;
 
 // iterate on the input container to find points with maximum x/y coords ...
 for(piter=pstart;piter!=pend;piter++){
   p = *piter;
 
   // x ...
   if (__COMPARE_XY_POINTS(p,le __GLGP_LINE_PARAMETER__) == -1) le=p;
   if (__COMPARE_XY_POINTS(p,ri __GLGP_LINE_PARAMETER__) ==  1) ri=p;  
   
   // y ...
   if (__COMPARE_YX_POINTS(p,bo __GLGP_LINE_PARAMETER__) == -1) bo=p;  
   if (__COMPARE_YX_POINTS(p,to __GLGP_LINE_PARAMETER__) ==  1) to=p;   
 }
  
 ORIENTATION_SPECIAL O1(le,ri);
 ORIENTATION_SPECIAL O2(to,ri);
 ORIENTATION_SPECIAL O3(le,to);
 ORIENTATION_SPECIAL O4(ri,bo);
 ORIENTATION_SPECIAL O5(bo,le); 
    
 for(piter=pstart;piter!=pend;piter++){
    p = *piter;
 
    int o1 = O1(p __GLGP_LINE_PARAMETER__);
    if (o1==1) {  // leftturn ...
      if (O2(p __GLGP_LINE_PARAMETER__)==1)  inp1.push(p);
      if (O3(p __GLGP_LINE_PARAMETER__)==1)  inp3.push(p);
    }
    else { // rightturn ...
      if (O4(p __GLGP_LINE_PARAMETER__)==1)  inp2.push(p);
      if (O5(p __GLGP_LINE_PARAMETER__)==1)  inp4.push(p);
    }
 }
 inp1.push(to); inp1.push(ri); 
 inp2.push(bo); inp2.push(ri);
 inp3.push(to); inp3.push(le);
 inp4.push(le); inp4.push(bo); 
 
 if (__COMPARE_XY_POINTS(ri,to __GLGP_LINE_PARAMETER__) != 0) Quickhull_graham_t<KERNEL>(inp1, erg1, ri, to, 1, true);  else erg1.push(to);
 if (__COMPARE_XY_POINTS(ri,bo __GLGP_LINE_PARAMETER__) != 0) Quickhull_graham_t<KERNEL>(inp2, erg2, bo, ri, 1, false); else erg2.push(bo);
 if (__COMPARE_XY_POINTS(le,to __GLGP_LINE_PARAMETER__) != 0) Quickhull_graham_t<KERNEL>(inp3, erg3, to, le, 1, true);  else erg3.push(to);
 if (__COMPARE_XY_POINTS(le,bo __GLGP_LINE_PARAMETER__) != 0) Quickhull_graham_t<KERNEL>(inp4, erg4, le, bo, 1, false); else erg4.push(bo);
 
 if (__COMPARE_XY_POINTS(erg3.tail(),erg4.head() __GLGP_LINE_PARAMETER__) == 0) erg4.pop(); 
 erg3.conc(erg4);
 if (__COMPARE_XY_POINTS(erg3.tail(),erg2.head() __GLGP_LINE_PARAMETER__) == 0) erg2.pop();
 erg3.conc(erg2);
 if (__COMPARE_XY_POINTS(erg3.tail(),erg1.head() __GLGP_LINE_PARAMETER__) == 0) erg1.pop(); 
 erg3.conc(erg1); 
 
 erg3.Pop(); // last == first
 
 return erg3;
}


LEDA_END_NAMESPACE




