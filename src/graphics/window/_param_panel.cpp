/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _param_panel.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/graphics/param_panel.h>

LEDA_BEGIN_NAMESPACE

void param_panel::init(const param_handler& P)
{
  text_item("Please edit parameters of "+P.File);
  param p;
  forall(p, P.ParamList) {
    switch (P.Type[p.Label]) {
       case param_handler::BOOL:   
         bool_item(p.Label,P.get_bool(p.Label));       
         break;
       case param_handler::DOUBLE: 
         real_item(p.Label,P.get_double(p.Label));     
         break;
       case param_handler::INT: 
         int_item(p.Label,P.get_int(p.Label)); 
         break;
       case param_handler::ENUM: 
         choice_item(p.Label,P.get_enum(p.Label),p.Enums);
         break;
       case param_handler::STRING: 
         string_item(p.Label,P.get_string(p.Label));   
         break;
    }

  }
  button("OK",0);
}

LEDA_END_NAMESPACE
