#include <stdio.h>
#include <string.h>

#define MAX_WORD     0xFFFFFFFF
 
#if defined(__svr4__)
#define School_Add      _School_Add
#define School_Add_To   _School_Add_To
#define School_Sub      _School_Sub
#define School_Sub_From _School_Sub_From
#endif

typedef unsigned long  word;
typedef unsigned int sz_t;


static void do_nothing(word* dummy) {};


#define ADD_LOOP_BODY(i,res) {\
asm volatile ("movl "#i"(%1),%0" : "=r"(aa) : "r"(a));\
asm volatile ("adcl "#i"(%1),%0" : "=r"(aa) : "r"(b));\
asm volatile ("movl %1,"#i"(%0)" : "=r"(res): "r"(aa));\
}

#define SUB_LOOP_BODY(i,res) {\
asm volatile ("movl "#i"(%1),%0" : "=r"(aa) : "r"(a));\
asm volatile ("sbbl "#i"(%1),%0" : "=r"(aa) : "r"(b));\
asm volatile ("movl %1,"#i"(%0)" : "=r"(res): "r"(aa));\
}

#define ADD_TO_LOOP_BODY(i) {\
asm volatile ("movl "#i"(%1),%0" : "=r"(aa) : "r"(b));\
asm volatile ("adcl %1,"#i"(%0)" : "=r"(a) : "r"(aa));\
}

#define SUB_FROM_LOOP_BODY(i) {\
asm volatile ("movl "#i"(%1),%0" : "=r"(aa) : "r"(b));\
asm volatile ("sbbl %1,"#i"(%0)" : "=r"(a) : "r"(aa));\
}


#define SAVE_CARRY {\
  asm volatile ("movl $0,%0" :"=r"(ca):);\
  asm volatile ("adc  %1,%0" :"=r"(ca)    : "r"(ca),"0"(ca) );\
}

#define RESTORE_CARRY {\
    asm volatile ("add  $0xffffffff,%0" :"=r"(ca)    : "0"(ca) );\
}


/*
#define ADD_LOOP_BODY(i,res) {\
asm volatile ("ld      [%1+"#i"],%0" : "=r"(aa) : "r"(a));\
asm volatile ("ld      [%1+"#i"],%0" : "=r"(bb) : "r"(b));\
asm volatile ("addxcc  %2,%1,%0"     : "=r"(aa) : "0"(aa), "r"(bb) );\
asm volatile ("st      %1,[%0+"#i"]" : "=r"(res): "r"(aa));\
}

#define SUB_LOOP_BODY(i,res) {\
asm volatile ("ld      [%1+"#i"],%0" : "=r"(aa) : "r"(a));\
asm volatile ("ld      [%1+"#i"],%0" : "=r"(bb) : "r"(b));\
asm volatile ("subxcc  %2,%1,%0"     : "=r"(aa) : "r"(bb), "0"(aa) );\
asm volatile ("st      %1,[%0+"#i"]" : "=r"(res): "r"(aa));\
}

#define SAVE_CARRY {\
    asm volatile ("addx  %%g0,%%g0,%0" : "=r"(ca) :  );\
}

#define RESTORE_CARRY {\
    asm volatile ("addcc 0xffffffff,%1,%0" : "=r"(ca) : "0"(ca) );\
}
*/


extern sz_t School_Add(word *a, sz_t a_used, word *b, sz_t b_used, word* sum)
{
  /* compute sum = a + b   
     Preconditions: a_used >= b_used 
                    a != b
   */

  int n = b_used / 16;
  int r = b_used % 16;

  word* s_stop = sum + a_used;
  word  aa;
  word  ca = 0;

  a  -= (16-r);
  b  -= (16-r);
  sum -= (16-r);

  RESTORE_CARRY;

  switch (r) {
      case 15: ADD_LOOP_BODY(4, sum);
      case 14: ADD_LOOP_BODY(8, sum);
      case 13: ADD_LOOP_BODY(12,sum);
      case 12: ADD_LOOP_BODY(16,sum);
      case 11: ADD_LOOP_BODY(20,sum);
      case 10: ADD_LOOP_BODY(24,sum);
      case  9: ADD_LOOP_BODY(28,sum);
      case  8: ADD_LOOP_BODY(32,sum);
      case  7: ADD_LOOP_BODY(36,sum);
      case  6: ADD_LOOP_BODY(40,sum);
      case  5: ADD_LOOP_BODY(44,sum);
      case  4: ADD_LOOP_BODY(48,sum);
      case  3: ADD_LOOP_BODY(52,sum);
      case  2: ADD_LOOP_BODY(56,sum);
      case  1: ADD_LOOP_BODY(60,sum);
     }

  SAVE_CARRY;

  a += 16;
  b += 16;
  sum += 16;

  while (n--)
  { 
    RESTORE_CARRY;

    ADD_LOOP_BODY(0, sum);
    ADD_LOOP_BODY(4, sum);
    ADD_LOOP_BODY(8, sum);
    ADD_LOOP_BODY(12,sum);
    ADD_LOOP_BODY(16,sum);
    ADD_LOOP_BODY(20,sum);
    ADD_LOOP_BODY(24,sum);
    ADD_LOOP_BODY(28,sum);
    ADD_LOOP_BODY(32,sum);
    ADD_LOOP_BODY(36,sum);
    ADD_LOOP_BODY(40,sum);
    ADD_LOOP_BODY(44,sum);
    ADD_LOOP_BODY(48,sum);
    ADD_LOOP_BODY(52,sum);
    ADD_LOOP_BODY(56,sum);
    ADD_LOOP_BODY(60,sum);

    SAVE_CARRY;

    a += 16;
    b += 16;
    sum += 16;
   }

  //do_nothing(b);

  if (sum != a)  
  { int m = (int)(a_used-b_used);
    memcpy(sum,a,m*sizeof(word));
    sum[m] = 0;
   }

  if (ca) /* propagate carry */
  { while (++*sum == 0) sum++;
    if (sum == s_stop) a_used++;
   }


  return a_used;
}



extern sz_t School_Add_To(word *a, sz_t a_used, word *b, sz_t b_used)
{


  // a += b
  // Precondition:  a_used >= b_used

  int n = b_used / 16;
  int r = b_used % 16;

  word* a_stop = a + a_used;
  word  aa;
  word  ca = 0;

  a  -= (16-r);
  b  -= (16-r);

  RESTORE_CARRY;

  switch (r) {
      case 15: ADD_LOOP_BODY(4, a);
      case 14: ADD_LOOP_BODY(8, a);
      case 13: ADD_LOOP_BODY(12,a);
      case 12: ADD_LOOP_BODY(16,a);
      case 11: ADD_LOOP_BODY(20,a);
      case 10: ADD_LOOP_BODY(24,a);
      case  9: ADD_LOOP_BODY(28,a);
      case  8: ADD_LOOP_BODY(32,a);
      case  7: ADD_LOOP_BODY(36,a);
      case  6: ADD_LOOP_BODY(40,a);
      case  5: ADD_LOOP_BODY(44,a);
      case  4: ADD_LOOP_BODY(48,a);
      case  3: ADD_LOOP_BODY(52,a);
      case  2: ADD_LOOP_BODY(56,a);
      case  1: ADD_LOOP_BODY(60,a);
     }

  SAVE_CARRY;

  a += 16;
  b += 16;

  while (n--)
  { 
    RESTORE_CARRY;

    ADD_LOOP_BODY(0, a);
    ADD_LOOP_BODY(4, a);
    ADD_LOOP_BODY(8, a);
    ADD_LOOP_BODY(12,a);
    ADD_LOOP_BODY(16,a);
    ADD_LOOP_BODY(20,a);
    ADD_LOOP_BODY(24,a);
    ADD_LOOP_BODY(28,a);
    ADD_LOOP_BODY(32,a);
    ADD_LOOP_BODY(36,a);
    ADD_LOOP_BODY(40,a);
    ADD_LOOP_BODY(44,a);
    ADD_LOOP_BODY(48,a);
    ADD_LOOP_BODY(52,a);
    ADD_LOOP_BODY(56,a);
    ADD_LOOP_BODY(60,a);

    SAVE_CARRY;

    a += 16;
    b += 16;
   }

  do_nothing(b);

  if (ca) /* propagate carry */
  { while (++*a == 0) a++;
    if (a == a_stop) a_used++;
   }

  return a_used;
}




extern sz_t School_Sub(word *a, sz_t a_used, word *b, sz_t b_used, word* diff)
{
  /* diff = a-b
     Precondition: a > b 
   */

  int n = b_used / 16;
  int r = b_used % 16; 

  word* d_stop = diff + a_used;
  word  aa;
  word  ca = 0;


  a  -= (16-r);
  b  -= (16-r);
  diff -= (16-r);

  RESTORE_CARRY;

  switch (r) {
      case 15: SUB_LOOP_BODY(4, diff);
      case 14: SUB_LOOP_BODY(8, diff);
      case 13: SUB_LOOP_BODY(12,diff);
      case 12: SUB_LOOP_BODY(16,diff);
      case 11: SUB_LOOP_BODY(20,diff);
      case 10: SUB_LOOP_BODY(24,diff);
      case  9: SUB_LOOP_BODY(28,diff);
      case  8: SUB_LOOP_BODY(32,diff);
      case  7: SUB_LOOP_BODY(36,diff);
      case  6: SUB_LOOP_BODY(40,diff);
      case  5: SUB_LOOP_BODY(44,diff);
      case  4: SUB_LOOP_BODY(48,diff);
      case  3: SUB_LOOP_BODY(52,diff);
      case  2: SUB_LOOP_BODY(56,diff);
      case  1: SUB_LOOP_BODY(60,diff);
     }

  SAVE_CARRY;

  a += 16;
  b += 16;
  diff += 16;

  while (n--)
  { 
    RESTORE_CARRY;

    SUB_LOOP_BODY(0, diff);
    SUB_LOOP_BODY(4, diff);
    SUB_LOOP_BODY(8, diff);
    SUB_LOOP_BODY(12,diff);
    SUB_LOOP_BODY(16,diff);
    SUB_LOOP_BODY(20,diff);
    SUB_LOOP_BODY(24,diff);
    SUB_LOOP_BODY(28,diff);
    SUB_LOOP_BODY(32,diff);
    SUB_LOOP_BODY(36,diff);
    SUB_LOOP_BODY(40,diff);
    SUB_LOOP_BODY(44,diff);
    SUB_LOOP_BODY(48,diff);
    SUB_LOOP_BODY(52,diff);
    SUB_LOOP_BODY(56,diff);
    SUB_LOOP_BODY(60,diff);

    SAVE_CARRY;

    a += 16;
    b += 16;
    diff += 16;
   }


  do_nothing(b);

  if (diff != a)
  { int m = (int)(a_used-b_used);
    memcpy(diff,a,m*sizeof(word));
   }

  if (ca)
    while (--*diff == MAX_WORD) diff++;

  while (*--d_stop == 0) a_used--;

  return a_used;
}




extern sz_t School_Sub_From(word *a, sz_t a_used, word *b, sz_t b_used)
{
  /* a -= b
     Precondition: a > b 
   */

  int n = b_used / 16;
  int r = b_used % 16; 

  word* a_stop = a + a_used;
  word  aa;
  word  ca = 0;

  a  -= (16-r);
  b  -= (16-r);

  RESTORE_CARRY;

  switch (r) {
      case 15: SUB_LOOP_BODY(4, a);
      case 14: SUB_LOOP_BODY(8, a);
      case 13: SUB_LOOP_BODY(12,a);
      case 12: SUB_LOOP_BODY(16,a);
      case 11: SUB_LOOP_BODY(20,a);
      case 10: SUB_LOOP_BODY(24,a);
      case  9: SUB_LOOP_BODY(28,a);
      case  8: SUB_LOOP_BODY(32,a);
      case  7: SUB_LOOP_BODY(36,a);
      case  6: SUB_LOOP_BODY(40,a);
      case  5: SUB_LOOP_BODY(44,a);
      case  4: SUB_LOOP_BODY(48,a);
      case  3: SUB_LOOP_BODY(52,a);
      case  2: SUB_LOOP_BODY(56,a);
      case  1: SUB_LOOP_BODY(60,a);
     }

  SAVE_CARRY;

  a += 16;
  b += 16;

  while (n--)
  { 
    RESTORE_CARRY;

    SUB_LOOP_BODY(0, a);
    SUB_LOOP_BODY(4, a);
    SUB_LOOP_BODY(8, a);
    SUB_LOOP_BODY(12,a);
    SUB_LOOP_BODY(16,a);
    SUB_LOOP_BODY(20,a);
    SUB_LOOP_BODY(24,a);
    SUB_LOOP_BODY(28,a);
    SUB_LOOP_BODY(32,a);
    SUB_LOOP_BODY(36,a);
    SUB_LOOP_BODY(40,a);
    SUB_LOOP_BODY(44,a);
    SUB_LOOP_BODY(48,a);
    SUB_LOOP_BODY(52,a);
    SUB_LOOP_BODY(56,a);
    SUB_LOOP_BODY(60,a);

    SAVE_CARRY;

    a += 16;
    b += 16;
   }

  do_nothing(b);

  if (ca)
    while (--*a == MAX_WORD) a++;

  while (*--a_stop == 0) a_used--;

  return a_used;
}

