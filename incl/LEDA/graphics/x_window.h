/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  x_window.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_X_WINDOW_H
#define LEDA_X_WINDOW_H


#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 360999
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/graphics/X11/cursorfont.h>

LEDA_BEGIN_NAMESPACE


// windows, events, colors ...

enum { 
  key_press_event, 
  key_release_event, 
  button_press_event, 
  button_release_event,
  display_event,
  configure_event, 
  exposure_event, 
  motion_event, 
  destroy_event, 
  timer_event,
  device_connect_event,
  volume_connect_event,
  no_event 
};


extern const char* event_name[];


enum key_codes {
  KEY_BACKSPACE,
  KEY_RETURN,
  KEY_ESCAPE,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_UP,
  KEY_DOWN,
  KEY_HOME,
  KEY_END,
  KEY_PAGEUP,
  KEY_PAGEDOWN,
  KEY_TAB,
  KEY_PRINT,
  KEY_INSERT,
  KEY_DELETE,
  KEY_PAGE_UP,
  KEY_PAGE_DOWN,
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12
};


enum color_values {

  invisible = (int)0x01000000,
  DEF_COLOR = (int)0x02000000,
  black     = (int)0x00000000,
  white     = (int)0x00ffffff,
  red       = (int)0x00ff0000,
  green     = (int)0x0000ee00,
  blue      = (int)0x000000cd,
  yellow    = (int)0x00ffff00,
  violet    = (int)0x00a020f0,
  orange    = (int)0x00ff8c00,
  cyan      = (int)0x0000ffff,
  brown     = (int)0x00a0522d,
  pink      = (int)0x00ff00ff,
  green2    = (int)0x000cb3a0,
  blue2     = (int)0x006495ed,
  grey1     = (int)0x00d9d9d9,
  grey2     = (int)0x00b3b3b3,
  grey3     = (int)0x00737373,
  ivory     = (int)0x00ffffe4

};


enum point_style {
  pixel_point, 
  cross_point, 
  plus_point, 
  circle_point, 
  disc_point, 
  rect_point, 
  box_point
};


enum line_style {
  solid,
  dashed,
  dotted,
  dashed_dotted
};


enum text_mode {
  transparent,
  opaque
};


enum drawing_mode {
  src_mode,
  xor_mode,
  or_mode,
  and_mode,
  diff_mode
};


enum grid_style {
  invisible_grid,
  point_grid,
  line_grid
};


LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 360999
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif

#endif
