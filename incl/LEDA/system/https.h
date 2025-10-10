/*******************************************************************************
+
+  LEDA 7.2.1  
+
+
+  http.h
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifndef LEDA_HTTPS_H
#define LEDA_HTTPS_H

#if !defined(LEDA_ROOT_INCL_ID)
#define LEDA_ROOT_INCL_ID 667777
#include <LEDA/internal/PREAMBLE.h>
#endif

#include <LEDA/system/http.h>
#include <LEDA/system/tls_socket.h>

LEDA_BEGIN_NAMESPACE

class  https : public http {
public:
 https() { tsock = new tls_socket; }
};

LEDA_END_NAMESPACE

#if LEDA_ROOT_INCL_ID == 667777
#undef LEDA_ROOT_INCL_ID
#include <LEDA/internal/POSTAMBLE.h>
#endif


#endif
