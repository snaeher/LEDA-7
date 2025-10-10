/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  v_anim.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/anim/view.h>

LEDA_BEGIN_NAMESPACE

void view::scene_begin(int steps) 
{ max_steps = steps;
  Anims.clear();     
}

void view::add_animation(anim_item a) { Anims.append(a); }

int view::scene_end() 
{ first_step = false;
  
  anim_item it;
  if (max_steps == 0)
  { forall(it,Anims)
      if (it->steps() > max_steps) max_steps = it->steps();
   }
     
  forall(it,Anims) it->init(max_steps);
    
  return max_steps;
}

void view::scene_step(int n) 
{ if (!first_step) 
  { first_step = true;
    
    anim_item it;    
    forall(it,Anims) it->start();
  }

  for (int i = 0; i < n && max_steps > 0; i++, max_steps--)
  { anim_item it;    
    forall(it,Anims) it->step();
    wait(sleep_time);
  }

  if (max_steps == 0) scene_finish();
}

void view::scene_finish() 
{ while (!Anims.empty()) Anims.pop()->finish(); 
  max_steps = 0;      
}

void view::scene_play()
{ anim_item it;
  forall(it,Anims) it->start();    
  
  while (max_steps-- > 0)
  { forall(it,Anims) it->step();
    wait(sleep_time);
  }
  
  scene_finish();
}

LEDA_END_NAMESPACE
