/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _filtered_rational.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/numbers/filtered_rational.h>

LEDA_BEGIN_NAMESPACE

filtered_rational::filtered_rational(int n, int d)
{ if (d == 0) LEDA_EXCEPTION(1,"filtered_rational: Zero denominator!");
  if (d < 0) { num = -n; den = -d; }
  else { num = n; den = d; }
 }

filtered_rational::filtered_rational(const filtered_integer& n, 
                                     const filtered_integer& d)
{ if (d == 0) LEDA_EXCEPTION(1,"filtered_rational: Zero denominator!");
  if (d < 0) { num = -n; den = -d; }
  else { num = n; den = d; }
 }




// operators

filtered_rational& filtered_rational::operator+= (const filtered_rational& r)
{ if (den == r.den)
    num += r.num;
  else
    { num = num*r.den + r.num*den;
      den *= r.den;
     }
  return *this;
}

filtered_rational& filtered_rational::operator-= (const filtered_rational& r)
{ if (den == r.den)
    num -= r.num;
  else
    { num = num*r.den - r.num*den;
      den *= r.den;
     }
  return *this;
}

filtered_rational& filtered_rational::operator*= (const filtered_rational& r)
{ num *= r.num;
  den *= r.den;
  return *this;
}


filtered_rational& filtered_rational::operator/= (const filtered_rational& r)
{ if (r == 0) LEDA_EXCEPTION(1,"division by 0");
  if (r < 0)
    { num *= -r.den;
      den *= -r.num;
     }
   else
    { num *= r.den;
      den *= r.num;
     }
  return *this;
}


filtered_rational& filtered_rational::operator= (const filtered_rational& r)
{ if (this == &r) return *this;
  num = r.num;
  den = r.den;
  return *this;
}



filtered_rational filtered_rational::inverse()
{ if (num == 0) LEDA_EXCEPTION(1,"Zero denominator!");
  if (num > 0)
     return filtered_rational(den,num);
  else
     return filtered_rational(-den,-num);
}




int filtered_rational::cmp(const filtered_rational& x, const filtered_rational& y)
{ int xsign = sign(x.num);
  int ysign = sign(y.num);

  if (xsign == 0) return -ysign;
  if (ysign == 0) return  xsign;

  if (xsign == ysign) 
    return compare(x.num*y.den, y.num*x.den);
  else 
    return compare(xsign,ysign);
}



ostream& operator<< (ostream& s, const filtered_rational& r)
{  s << r.num << "/" << r.den; 
   return s; 
 }


istream& operator>> (istream& in, filtered_rational& r)
{ 
   // Format: "r.num / r.den"

   char c;
   do in.get(c); while (isspace(c));
   in.putback(c);

   filtered_integer rx;
   in >> rx;

   do in.get(c); while (isspace(c));
   if (c != '/')   LEDA_EXCEPTION(1,"filtered_rational input: syntax error."); 

   do in.get(c); while (isspace(c));
   in.putback(c);

   filtered_integer ry;
   in >> ry;

   r = filtered_rational(rx,ry);

   return in;
}

LEDA_END_NAMESPACE
