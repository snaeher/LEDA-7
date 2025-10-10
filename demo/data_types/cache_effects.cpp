/*******************************************************************************
+
+  LEDA 4.4.1  
+
+
+  cache_effects.c
+
+
+  Copyright (c) 1995-2003
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/list.h>
#include <LEDA/core/IO_interface.h>

using namespace leda;


int main(){

IO_interface R("Illustration of Cache Effect");

R.write_demo("This demo illustrates the effect of caching on the \
running time of a list traversal algorithm. We allocate a list \
of n items and traverse it. We then permute the list and traverse \
it again. ");

list<int> L;
int n = R.read_int("number of items in list = ",1000000);

float T, UT; int s; list_item it;
T = used_time();

for (int i = 0; i < n; i++) L.append(i);

UT = used_time(T);
R.write_demo("time to build list = ", UT);
R.write_table("", UT);

it = L.first();
while (it != nil) { it = L.succ(it); s++; }
UT = used_time(T);
R.write_demo("used time for non-permuted list = ", UT);
R.write_table(" & ", UT);

L.permute();

UT = used_time(T);
R.write_demo("time to permute list = ", UT);
R.write_table(" & ", UT);

T = used_time();
it = L.first();
while (it != nil) { it = L.succ(it); s++; }
UT = used_time(T);
R.write_demo("used time for permuted list = ", UT);
R.write_table(" & ", UT);

return s;
}



 
