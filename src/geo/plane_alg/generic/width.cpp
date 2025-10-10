/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  width.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



LEDA_BEGIN_NAMESPACE

RAT_TYPE WIDTH(const list<POINT>& L, LINE& l1, LINE& l2)
{ 
  if ( L.empty() )
    LEDA_EXCEPTION(1,"WIDTH applies only to non-empty sets");

  list<POINT> CH = CONVEX_HULL(L);

  if ( CH.length() == 1 )
    { l1 = l2 = LINE(L.head(), VECTOR(INT_TYPE(1),INT_TYPE(1))); return 0; }
  if ( CH.length() == 2 )
    { l1 = l2 = LINE(CH.head(), CH.tail()); return 0; }
  
  list_item p_it = CH.first();
  list_item q_it = CH.cyclic_succ(p_it);
  list_item m_it = q_it;
  list_item it;

  LINE l(CH[p_it],CH[q_it]);

  RAT_TYPE min_sqr_width = 0; RAT_TYPE sqr_dist;

  // find vertex with maximal distance from l

  forall_items(it,CH) 
  { if ( (sqr_dist = l.sqr_dist(CH[it])) > min_sqr_width )
    { min_sqr_width = sqr_dist;
      m_it = it;
    }
  }
  l1 = l; l2 = LINE(CH[m_it], CH[q_it] - CH[p_it]);

  
    do  // move caliber to next edge
    { 
      list_item r_it = CH.cyclic_succ(q_it);
      LINE l(CH[q_it],CH[r_it]);
      RAT_TYPE cur_sqr_dist = l.sqr_dist(CH[m_it]);
      list_item new_m_it = m_it;
      it = CH.cyclic_succ(m_it);
      
      while ( (sqr_dist = l.sqr_dist(CH[it])) >= cur_sqr_dist )
      { new_m_it = it; it = CH.cyclic_succ(it); 
        cur_sqr_dist = sqr_dist;
      }
      
      if ( cur_sqr_dist < min_sqr_width )
      { min_sqr_width = cur_sqr_dist;
        l1 = l; l2 = LINE(CH[new_m_it], CH[r_it] - CH[q_it]);
      }

      p_it = q_it; q_it = r_it; m_it = new_m_it;
    } while ( p_it != CH.first() );


  return min_sqr_width;
}


LEDA_END_NAMESPACE
