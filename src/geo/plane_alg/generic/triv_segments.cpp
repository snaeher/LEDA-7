/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  triv_segments.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


LEDA_BEGIN_NAMESPACE


typedef void (*rep_int_func) (const SEGMENT&, const SEGMENT&);

void TRIVIAL_SEGMENTS(const list<SEGMENT>& L,  rep_int_func report_intersection)
{
  POINT inter;

  int N = L.length();

  if (N == 0) return;

  SEGMENT* A = new SEGMENT[N];
  SEGMENT* p = A;

  SEGMENT s;
  forall(s,L) *p++ = s;

  SEGMENT* stop = p;
  
  for(SEGMENT* a = A; a < stop; a++ )
  { 
    POINT as = a->source();
    POINT at = a->target();

    for(SEGMENT* b = a+1; b < stop; b++ )
    { 
      POINT bs = b->source();
      POINT bt = b->target();

      int o1 = orientation(*a,bs); 
      int o2 = orientation(*a,bt);

      if (o1 == o2 && ( o1 != 0 || o2 != 0)) continue;

      int o3 = orientation(*b,as); 
      int o4 = orientation(*b,at);
	
      if (o1 == 0 && o2 == 0 && o3 == 0 && o4 == 0)
        { 
          if ( compare(as,at) > 0 )
          { at = a->source(); 
            as = a->target(); 
           }

          if ( compare(bs,bt) > 0 )
          { bt = b->source(); 
            bs = b->target(); 
           }
          
          if ((compare(as,bs) <= 0 && compare(at,bs) >= 0) || 
              (compare(bs,as) <= 0 && compare(bt,as) >= 0) )
             report_intersection(*a,*b);
         }
       else
         if (o1 != o2 && o3 != o4) report_intersection(*a,*b);
      }
   }

  delete[] A;
}


LEDA_END_NAMESPACE
