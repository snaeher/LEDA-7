#include <LEDA/numbers/interval.h>
#include <LEDA/numbers/real.h>
#include <LEDA/numbers/approximating.h>

LEDA_BEGIN_NAMESPACE

#undef LEDA_VECTOR
#define LEDA_VECTOR growing_array

typedef leda::interval_bound_absolute real_interval;


bigfloat find_minimum(const polynomial<bigfloat>& P, 
                      const polynomial<bigfloat>& Ps, bigfloat l, bigfloat r)
{
   // constant polynomial
   if(P.degree() == 0) return abs(P[0]);

   // first take the endpoints
   rounding_modes old_mode = bigfloat::set_rounding_mode(TO_ZERO);
   bigfloat min = abs(P.evaluate(l));
   bigfloat br = abs(P.evaluate(r));
   if(br<min) min = br;

   // no extrema
   if(Ps.degree() == 0) return min;
   if(Ps.degree() == 1)
   {
     bigfloat::set_rounding_mode(EXACT);
     if(Ps[1]*l >= -Ps[0] || Ps[1]*r <= -Ps[0]) // root of Ps not in I
     {
        bigfloat::set_rounding_mode(old_mode);
        return min;
     }
   }
  
   bigfloat::set_rounding_mode(TO_ZERO);
            real_interval I;

            double dl = (round(l,53,TO_N_INF)).to_double();
            double dr = (round(r,53,TO_P_INF)).to_double();
            I.set_range(dl,dr);

            // construct interval polynomial
            int n = P.degree();
            LEDA_VECTOR<real_interval> IV(n+1);
            for(int i=0; i<n; i++) 
            {
               bigfloat Pip = round(P[i] / P[n],53,TO_P_INF);
               bigfloat Pin = round(P[i] / P[n],53,TO_N_INF);
               IV[i].set_range(Pin.to_double(), Pip.to_double());
            }
            IV[n] = real_interval(1);
            polynomial<real_interval> IP(IV);

            // evaluate polynomial at the interval
            real_interval J = real_interval(P[n]) * IP.evaluate(I);

   // take the minimum
   bigfloat m;

   if(J.is_finite())
   {
      if(J.lower_bound() > 0) m = J.lower_bound();
      else if(J.upper_bound() < 0) m = -J.upper_bound();
      else m = 0;
   }
   else
   {
      m = min;

      // get the exact polynomials
      LEDA_VECTOR<real> RV(n+1); // n = P.degree()
      int i;
      for(i=0; i<=n; i++) RV[i] = real(P[i]);
      polynomial<real> RP(RV);
      LEDA_VECTOR<real> RVs(n); 
      for(i=0; i<n; i++) RVs[i] = real(Ps[i]);
      polynomial<real> RPs(RVs);


      // make RPs squarefree
      polynomial<real> RPss = diff(RPs);
      if(possible_common_roots(RPs,RPss))
      {
         polynomial<real> G = poly_gcd(RPs,diff(RPs));
         if(G.degree() > 0) 
         {
            polynomial<real> q,r;
            RPs.euclidean_division(G,q,r);
            RPs = q;
         }   
      }

      // get the exact roots of the derivative
      LEDA_VECTOR<real> roots;
      int n_roots = real_roots(RPs, roots);

      // check if there is a root in [l,r]
      for(i=0; i<n_roots; i++)
      {
         if(roots[i] >= l && roots[i] <= r)
         {
            real e = abs(RP.evaluate(roots[i]));
            if(e.get_lower_bound()<m) m = e.get_lower_bound();
         }
      }
   }

   // compare the minimum with min
   if(m < min) min = m;
   bigfloat::set_rounding_mode(old_mode);

   return min;
}

bigfloat find_maximum(const polynomial<bigfloat>& P, 
                      const polynomial<bigfloat>& Ps, bigfloat l, bigfloat r)
{
   // constant polynomial
   if(P.degree() == 0) return abs(P[0]);

   // first take the endpoints
   rounding_modes old_mode = bigfloat::set_rounding_mode(TO_INF);
   bigfloat max = abs(P.evaluate(l));
   bigfloat br = abs(P.evaluate(r));
   if(max<br) max = br;

   // no extrema:
   if(Ps.degree() == 0) return max;
   if(Ps.degree() == 1)
   {
     bigfloat::set_rounding_mode(EXACT);
     if(Ps[1]*l >= -Ps[0] || Ps[1]*r <= -Ps[0]) // root of Ps not in I
     {
        bigfloat::set_rounding_mode(old_mode);
        return max;
     }
   }

   bigfloat::set_rounding_mode(TO_INF);
            real_interval I;

            double dl = (round(l,53,TO_N_INF)).to_double();
            double dr = (round(r,53,TO_P_INF)).to_double();
            I.set_range(dl,dr);

            // construct interval polynomial
            int n = P.degree();
            LEDA_VECTOR<real_interval> IV(n+1);
            for(int i=0; i<n; i++) 
            {
               bigfloat Pip = round(P[i] / P[n],53,TO_P_INF);
               bigfloat Pin = round(P[i] / P[n],53,TO_N_INF);
               IV[i].set_range(Pin.to_double(), Pip.to_double());
            }
            IV[n] = real_interval(1);
            polynomial<real_interval> IP(IV);

            // evaluate polynomial at the interval
            real_interval J = real_interval(P[n]) * IP.evaluate(I);

   bigfloat m;
   if(J.is_finite())
   {
      // take the maximum
      if(J.lower_bound() > 0) m = J.upper_bound();
      else if(J.upper_bound() < 0) m = -J.lower_bound();
      else 
      {
         m = -J.lower_bound();
         if(m < J.upper_bound()) m = J.upper_bound();
      }
   }
   else
   {
      m = max;

      // get the exact polynomials
      LEDA_VECTOR<real> RV(n+1); // n = P.degree()
      int i;
      for(i=0; i<=n; i++) RV[i] = real(P[i]);
      polynomial<real> RP(RV);
      LEDA_VECTOR<real> RVs(n); 
      for(i=0; i<n; i++) RVs[i] = real(Ps[i]);
      polynomial<real> RPs(RVs);

      // make RPs squarefree
      polynomial<real> RPss = diff(RPs);
      if(possible_common_roots(RPs,RPss))
      {
         polynomial<real> G = poly_gcd(RPs,diff(RPs));
         if(G.degree() > 0) 
         {
            polynomial<real> q,r;
            RPs.euclidean_division(G,q,r);
            RPs = q;
         } 
      }

      // get the exact roots of the derivative
      LEDA_VECTOR<real> roots;
      int n_roots = real_roots(RPs, roots);

      // check if there is a root in [l,r]
      for(i=0; i<n_roots; i++)
      {
         if(roots[i] >= l && roots[i] <= r)
         {
            real e = abs(RP.evaluate(roots[i]));
            if(e.get_upper_bound()>m) m = e.get_upper_bound();
         }
      }
   }

   // compare the maximum with Max
   if(m > max) max = m;
   bigfloat::set_rounding_mode(old_mode);

   return max;
}
bigfloat newton(const polynomial<bigfloat>& P, bigfloat l, bigfloat r, sz_t prec)
{
    rounding_modes old_mode = bigfloat::set_rounding_mode(TO_INF);

    
        
        // max of P are at the endpoints of the interval
        // rounding mode has to be TO_INF
        bigfloat bl = abs(P.evaluate(l));
        bigfloat br = abs(P.evaluate(r));
        bigfloat maxP = bl;
        if(bl<br) maxP = br;


        // min of P'
        bigfloat::set_rounding_mode(EXACT);
        polynomial<bigfloat> Ps = diff(P);
        polynomial<bigfloat> Pss = diff(Ps);
        bigfloat minPs = find_minimum(Ps, Pss, l,r);
        #ifdef CHECK_REALS
        bigfloat maxPs = find_maximum(Ps, Pss, l,r);
        #endif

        // max of P''
        polynomial<bigfloat> Psss = diff(Pss);
        bigfloat maxPss = find_maximum(Pss, Psss,l,r);

        bigfloat minPs2 = mul(minPs,minPs,prec, TO_ZERO); 
        bigfloat c = mul(maxPss, maxP, prec,TO_INF);
        c = div(c,minPs2,prec,TO_INF);
        //bigfloat c = (maxPss * maxP)/(minPs * minPs);

        bigfloat::set_rounding_mode(EXACT);
        while(c>=0.25)
        {
           if((-2*ilog2(r-l)).to_long()> (long) bigfloat::get_precision()) 
                      bigfloat::set_precision((-2*ilog2(r-l)).to_long());
                      
           bigfloat h = (l+r)/2;
           bigfloat Ph = P.evaluate(h);
           if(Ph == 0) return h; // as rounding_mode = EXACT, h is the exact root

           bool case_r;
           if(Ph * P.evaluate(l)<0) 
           {
              r = h;
              case_r = true;
           }
           else 
           {
              l=h;  
              case_r = false;
           }

           

              // max of P
              // rounding mode has to be TO_INF
              bigfloat::set_rounding_mode(TO_INF);
              if(case_r) br = abs(P.evaluate(r));
              else bl = abs(P.evaluate(l));
              maxP = bl;
              if(bl<br) maxP = br;
              
              // min of P'
              minPs = find_minimum(Ps, Pss, l,r);
           #ifdef CHECK_REALS
              maxPs = find_maximum(Ps, Pss, l,r);
           #endif
              
              // max of P''
              maxPss = find_maximum(Pss, Psss, l,r);
             
              minPs2 = mul(minPs,minPs,prec, TO_ZERO); 
              c = mul(maxPss, maxP, prec,TO_INF);
              c = div(c,minPs2,prec,TO_INF);
              //c = (maxPss * maxP)/(minPs * minPs);

              bigfloat::set_rounding_mode(EXACT);


        }

        
        bigfloat start_value;
        integer start_prec;

        start_prec = -ilog2(r-l);
        start_value = (l+r)/2;
        
        // max of P'''
        bigfloat::set_rounding_mode(EXACT);
        polynomial<bigfloat> Pssss = diff(Psss);
        bigfloat maxPsss = find_maximum(Psss, Pssss, l,r);

        bigfloat Ch = mul(maxPsss, maxP, prec,TO_INF);
        bigfloat C = div(Ch,minPs2,prec,TO_INF);
        Ch = div(maxPss,minPs,prec,TO_INF);
        C = add(C,Ch,prec,TO_INF);
        bigfloat minPs3 = mul(minPs2,minPs,prec, TO_ZERO); 
        Ch = mul(maxPss,maxPss,prec,TO_INF);
        Ch = mul(Ch,maxP,prec,TO_INF);
        Ch = add(Ch,Ch,prec,TO_INF);
        Ch = div(Ch,minPs3,prec,TO_INF);
        C = add(C,Ch,prec,TO_INF);
        //bigfloat C = (maxPsss*maxP)/(minPs*minPs) + maxPss/minPs + (2*maxPss*maxPss*maxP)/(minPs*minPs*minPs);


        
        int logc = (ilog2(C)).to_long();
        int logC = logc;
        bool fast = true;
        if(start_prec -1 <= logc)
        {
           logc = (ilog2(c)).to_long();
           fast = false;
        }



        bigfloat it; // iteration value
        bigfloat ne; // next iteration value

        ne = start_value;
        int lp = start_prec.to_long();
        sz_t old_prec = bigfloat::set_precision(start_prec.to_long());
        int p_bound = prec+1;


    bigfloat::set_rounding_mode(EXACT);
    while(1==1)
    {
      if (lp >= p_bound) break;

      it=ne; // |it-alpha| <= 2^{-lp}

            if(fast)
            {
               lp*=2; 
               lp += 1;
               lp -= logc;
            }
            else lp -= logc;
            lp = lp - 1;
            if(lp > p_bound) lp = p_bound;

      bigfloat P_it = P.evaluate(it);
      bigfloat Ps_it = Ps.evaluate(it);

      if(P_it == 0) break; // root found exactly
      if(abs(P_it) <= 1)
      {
         if(abs(Ps_it)>=1) 
              bigfloat::set_precision(lp+3);
         else bigfloat::set_precision(lp+3-ilog2(abs(Ps_it)).to_long());
      }
      else 
      {
         if(abs(Ps_it)>=1) 
              bigfloat::set_precision(lp+3+ilog2(abs(P_it)).to_long()); 
         else bigfloat::set_precision(lp+3+ilog2(abs(P_it)).to_long()-ilog2(abs(Ps_it)).to_long()); 
      }
      ne = it - P_it/Ps_it;
     
      if(abs(ne) < 1) ne = round(ne, lp+2,TO_NEAREST);
      else ne = round(ne, lp+2+ilog2(abs(ne)).to_long(),TO_NEAREST);

      if(!fast)
      {
        
        if(logC < lp-1)
        {
          fast = true;
          logc = logC;
        }

      }
    }

    bigfloat result;
    if(ne == 0) result = round(ne, prec+1,TO_NEAREST);
    else result = round(ne,prec+1+ilog2(abs(ne)).to_long(),TO_NEAREST);
#ifdef CHECK_REALS
        bigfloat::set_rounding_mode(EXACT);
        assert(abs(P.evaluate(result)) <= maxPs * ipow2(-prec));
#endif

    bigfloat::set_precision(old_prec);
    bigfloat::set_rounding_mode(old_mode);

  return result;


}

LEDA_END_NAMESPACE


