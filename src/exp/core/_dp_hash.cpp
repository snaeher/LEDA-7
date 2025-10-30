/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _dp_hash.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/



//------------------------------------------------------------------------------
// Dynamic Perfect Hashing  [DKMMRT]
//
// Michael Wenzel           ( 1990/91 ) 
//
//------------------------------------------------------------------------------


#include <LEDA/core/impl/dp_hash.h>

#include <stdlib.h>

LEDA_BEGIN_NAMESPACE

// ----------------------------------------------------------------
// random source
// ----------------------------------------------------------------

random_source ran(1,maxprim-1);


// ----------------------------------------------------------------
// allgemeine Funktionen und Konstanten
// ----------------------------------------------------------------


// Universum [1..dp_exp_31-1]
// 2-er Potenzen, Shift Operationen sparen
#define dp_exp_31 2147483648UL
#define dp_exp_30 1073741824
#define dp_exp_29 536870912 
#define dp_exp_28 268435456 
#define dp_exp_27 134217728 
#define dp_exp_26 67108864 
#define dp_exp_25 33554432 


// Konstante fuer Groesse beim Rehashing 
#define _dp_h_c 1

// allgmeine Quadrat- und Multiplikationsfunktion fuer 2-er Potenz
#define sqr(x) ((x)*(x))                  
#define mal_pot2(x,y) ((x)<<(y))

// Berechnung von (k*x)%p
// fuer p=2^31+11  ( kleinste Primzahl > 2^31 )



inline void dpmod(unsigned long k, GenPtr x, unsigned long& dp_erg)
{ unsigned long dp_k1=k>>16;
  unsigned long dp_k2=k&65535;
//  changes for VS2015, 64 bit
//  unsigned long dp_x1=(unsigned long)x >> 16;
//  unsigned long dp_x2=(unsigned long)x & 65535;

#if (defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(_M_X64))
  unsigned long long dp_x1=(unsigned long long)x >> 16;
  unsigned long long dp_x2=(unsigned long long)x & 65535;
#else
  unsigned long dp_x1=(unsigned long)x >> 16;
  unsigned long dp_x2=(unsigned long)x & 65535;
#endif

  unsigned long dp_e1=dp_k1*dp_x1+((dp_k1*dp_x2+dp_k2*dp_x1)>>16);
  unsigned long dp_e2=dp_k2*dp_x2;
  unsigned long dp_e3=((dp_k1*dp_x2+dp_k2*dp_x1)&65535)<<16;
  unsigned long dp_e5=dp_e2+dp_e3;
  if ((dp_e2&dp_exp_31)&&(dp_e3&dp_exp_31))
    dp_e1++;
  else if (((dp_e2&dp_exp_31)||(dp_e3&dp_exp_31))&&(!(dp_e5&dp_exp_31)))
	 dp_e1++; 
  long dp_f=(dp_e5&0x7fffffff)-22*(dp_e1&0x03ffffff);

  if (dp_e1&dp_exp_26)
  { if(dp_f<1476394997)
      dp_f+=671088651;
    else dp_f-=1476395008;
  }

  if (dp_e1&dp_exp_27)
  { if(dp_f<805306346)
      dp_f+=1342177302;
    else dp_f-=805306357;
  }

  if (dp_e1&dp_exp_28)
  { if(dp_f<1610612703)
      dp_f+=536870945;
    else dp_f-=1610612714;
  }

  if (dp_e1&dp_exp_29)
  { if(dp_f<1073741758)
      dp_f+=1073741890;
    else dp_f-=1073741769;
  }

  if (dp_e1&dp_exp_30)
  { if(dp_f<2147483527)
      dp_f+=121;
    else dp_f-=2147483538UL;
  }

  if (dp_e5&dp_exp_31)
  { if (dp_f<0)
      dp_f+=2147483648UL;
    else dp_f-=11;
  }

  if (dp_f<0)
    dp_erg=(unsigned long)(dp_f+2147483659UL);
  else  
    dp_erg=(unsigned long)(dp_f);
}

// ----------------------------------------------------------------
// member-functions of class headertable 
// ----------------------------------------------------------------

//-----------------------------------------------------------------
// insert
// fuegt ein Paar (Schluessel,Information) in das Bucket ein
// berechnet Informationen neu
// returns true, falls Element eingefuegt     


bool headertable::insert(GenPtr a,GenPtr b,stp& erg,int& bed,bool& rehash,
			 stp anf,stp ende)
{ 
  unsigned long dp_erg=0;
  if (!wj)                             // leere Headertable
  { 
    wj=1;

    if (!tj)
    { 
      mj=1;
      tj=new subtable(a,b);            // einfach einfuegen
      erg=tj;
    }
    else                               // Tafel war angelegt
    {
      if (mj==1)                       // nur einelementig  
      {
	tj->set_s(a,b);
	erg=tj;
      }
      else                             // groessere Tafel
        if (mj<=4)                     // nur 2 oder 4-elementig  
        {
	  tj[0].set_s(a,b);
          erg=tj;
        }
	else
        {
          dpmod(kj,a,dp_erg);
          dp_erg=dp_erg%mal_pot2(sqr(mj),1);
          tj[dp_erg].set_s(a,b);
          erg=tj+dp_erg;
        }
    }
   
    bed+=2; 
    rehash=false;
    return true;         
  }                                    // leere Tafel jetzt mit Element

  if (wj==1)                           // Tafel mit einem Element
  { 
    if (mj==1)
    {
      if (a==tj->key)                   // gleiches Element
      { 
        tj->inf=b;
        erg=tj;
        rehash=false;
        return false; 
      }
      else
      { 
        wj=2;                          // Einfuegen
        bed+=6;
        if (bed>=0)                    // Platz genug?
        { 
          rehash=true;
          return true; 
        }

        mj=2;                          // Speicher anfordern
				       // und Elemente verteilen
        stp lj=tj;
        tj=new subtable[2];
        tj[0] = (*lj);
        tj[1].set_s(a,b);
        erg = tj+1;
    
        if ((lj>ende)||(lj<anf)) 
          delete lj;
      }

      rehash = false;
      return true;
    }

    if (mj<=4)
    {
      if (a==tj[0].key)               // gleiches Element
      { 
        tj[0].inf=b;
        erg=tj;
        rehash=false;
        return false; 
      }
      else
      { 
	wj = 2;
        bed+=6;
	tj[1].set_s(a,b);
        erg = tj+1;
	rehash=false;
	return true;
      }
    }
    else                               // groessere Tafel
    {
      dpmod(kj,a,dp_erg);
      dp_erg=dp_erg%mal_pot2(sqr(mj),1);

      if (a==tj[dp_erg].key)            // gleiches Element
      { 
	tj[dp_erg].inf = b;
	erg=tj+dp_erg;
	rehash=false;
	return false;
      }

      wj=2;
      bed+=6;

      if ( tj[dp_erg].key == EMPTY )    // Position leer
      { 
        tj[dp_erg].set_s(a,b);
        erg=tj+dp_erg;
	rehash=false;
	return true;
      }
      else                             // lokales Rehash
      { 
	stp lj = new subtable(tj[dp_erg]);
	tj[dp_erg].clear();
	int subsize = mal_pot2(sqr(mj),1);
        int injektiv=0;            
				       // injektive Funktion suchen
    
        while (!injektiv)
        { injektiv=1;
          ran >> kj;

          unsigned long dp_erg=0;
          dpmod(kj,lj->key,dp_erg);
          dp_erg=dp_erg%subsize;
          tj[dp_erg]=(*lj);

          dpmod(kj,a,dp_erg);
          dp_erg=dp_erg%subsize;
          if ( tj[dp_erg].key == EMPTY )
          {
	    tj[dp_erg].set_s(a,b);
	    erg=tj+dp_erg;
          }
          else
          {
            tj[dp_erg].clear(); 
            injektiv=0;
          }
        }                              // Elemente injektiv verteilt 

        delete lj;
        rehash=false;
        return true;           
      }
    }
  }                                    // Tafel enthaelt jetzt 2 Elemente

  if (wj<4)                            // Tafel mit 2 oder 3 Elementen
  { 
    for (int i1=0; i1<wj ; i1++)
      if (a==tj[i1].key)                // gleiches Element
      { 
        tj[i1].inf=b;
        erg=tj+i1;
        rehash=false;
        return false; 
      }

				       // neues Element
    if (wj==2)
      bed+=10;
    else
      bed+=14;

    if (mj==2)
    {
      if (bed>=0)                      // Platz genug?
      { 
        rehash=true;
        return true; 
      }

      mj=4;                            // Speicher anfordern
      stp lj=tj;
      tj=new subtable[4];

      int i1;
      for (i1=0; i1<wj ; i1++)
        tj[i1] = lj[i1];
      tj[i1].set_s(a,b);
      erg = tj+wj;
    
      if ((lj>ende)||(lj<anf)) 
        delete lj;

      wj++;       
      rehash = false;
      return true;
    }

    if (mj==4)
    {
      tj[wj].set_s(a,b);
      erg = tj+wj;
      wj++;       
      rehash=false;
      return true;
    }
    else                               // groessere Tafel
    {
      dpmod(kj,a,dp_erg);
      dp_erg=dp_erg%mal_pot2(sqr(mj),1);
      if ( tj[dp_erg].key == EMPTY )    // Position leer
      { 
        tj[dp_erg].set_s(a,b);
        erg=tj+dp_erg;
        wj++;       
	rehash=false;
	return true;
      }
      else                             // lokales Rehash
      {
	stp lj = new subtable[++wj];
        int i1=0;
                                            // Elemente in Liste kopieren
        for (int i2=0; i1<wj-1 ; i2++ )
        { 
	  if ( tj[i2].key != EMPTY  )
          { 
	    lj[i1++]=tj[i2];  
            tj[i2].clear(); 
	  }
        }
        lj[i1].set_s(a,b);

        int subsize = mal_pot2(sqr(mj),1);
        int injektiv=0;                     // injektive Funktion suchen
    
        while (!injektiv)
        { injektiv=1;
          ran >> kj;

          for (i1=0; (i1<wj) && injektiv ; i1++)
          { 
            dpmod(kj,lj[i1].key,dp_erg);
            dp_erg=dp_erg%subsize;

            if ( tj[dp_erg].key == EMPTY )
              tj[dp_erg]=lj[i1];
            else
            { 
	      injektiv=0;
              for (int g=0;g<i1;g++)     // belegte Positionen loeschen
              { 
	        GenPtr help=lj[g].key;
                dpmod(kj,help,dp_erg);
                dp_erg=dp_erg%subsize;  
                tj[dp_erg].key = EMPTY ; 
       	      }
            }
          }                            // Elemente injektiv verteilt  
        }       

        delete lj;
        rehash=false;
        return true;           
      }                                // lokales Rehash beendet
    }
  }                                    // Tafel enthaelt jetzt 2 oder 3 Elemente

  if (wj==4)                           // Tafel mit 4 Elementen
  { 
    for (int i1=0; i1<4; i1++)
      if (a==tj[i1].key)                // gleiches Element
      { 
        tj[i1].inf=b;
        erg=tj+i1;
        rehash=false;
        return false; 
      }
  
				       // neues Element einfuegen
    bed+=18;

    if (bed>=0)                        // Platz genug?
    { 
      rehash=true;
      return true; 
    }

    mj=8;                               // Speicher anfordern
					// und Elemente verteilen
    stp lj=tj;
    tj=new subtable[128];
    int injektiv=0;       
					// injektive Funktion suchen
    
    while (!injektiv)
    {
      injektiv=1;
      ran >>kj;

      int i1;
      for (i1=0; (i1<4) && injektiv ; i1++)
      { 
        dpmod(kj,lj[i1].key,dp_erg);
        dp_erg=dp_erg%128;

        if ( tj[dp_erg].key == EMPTY )
          tj[dp_erg]=lj[i1];
        else
        { 
	  injektiv=0;
          for (int g=0;g<i1;g++)     // belegte Positionen loeschen
          { 
	    GenPtr help=lj[g].key;
            dpmod(kj,help,dp_erg);
            dp_erg=dp_erg%128;  
            tj[dp_erg].key = EMPTY ; 
       	  }
        }
      }
      if (injektiv)                  // letztes Element hashen
      {
        dpmod(kj,a,dp_erg);
        dp_erg=dp_erg%128;
        if ( tj[dp_erg].key == EMPTY )
        { 
          tj[dp_erg].set_s(a,b);
          erg=tj+dp_erg;
        }
        else
        { 
          injektiv=0;
          for (int g=0;g<i1;g++)     // belegte Positionen loeschen
          { 
            GenPtr help=lj[g].key;
            dpmod(kj,help,dp_erg);
            dp_erg=dp_erg%128;  
            tj[dp_erg].clear() ; 
          }
        }                            // letztes Element 
      }             
    }                                // Elemente injektiv verteilt 

    if ((lj>ende)||(lj<anf)) 
      delete lj;

    wj=5;                          
    rehash=false;
    return true;           
  }                             // Tafel enthaelt jetzt 5 Elemente

                                // Tafel mit >= 5 Elementen
				// injektives Hashing auf Bucket

  int subsize=mal_pot2(sqr(mj),1);
  dpmod(kj,a,dp_erg);
  dp_erg=dp_erg%subsize;

  if ( tj[dp_erg].key == a)         // gleiches Element
  { 
    tj[dp_erg].set_s(a,b);
    erg=tj+dp_erg;
    rehash=false;
    return false;      
  }

  int oldscard=wj;
  int subcard=(++wj);
  bed+=mal_pot2(subcard,2)-2;

  if ( tj[dp_erg].key == EMPTY  )    // Position leer -> einfuegen
  { 
    tj[dp_erg].set_s(a,b);
    erg=tj+dp_erg;
    rehash = false;
    return true; 
  }
  else                         // Tafelelement besetzt             
   
    if (subcard<=mj)           // aber noch Platz in Tafel
    {  
      stp lj = new subtable[subcard];
      int i1=0;
                               // Elemente in Liste kopieren
      for (int i2=0; i1<oldscard  ; i2++ )
      {
	if ( tj[i2].key != EMPTY  )
        {
          lj[i1++]=tj[i2];  
          tj[i2].clear(); 
        }
      }
      lj[i1].set_s(a,b);
                                 // lokales Rehash , alle Elemente in Liste lj

      int injektiv=0;
      while (!injektiv)          // injektive Funktion suchen
      { 
	injektiv=1;
        ran >> kj;

        for (i1=0; (i1<subcard) && injektiv ; i1++)
        { 
          dpmod(kj,lj[i1].key,dp_erg);
          dp_erg=dp_erg%subsize;
          if ( tj[dp_erg].key == EMPTY )
          {
            tj[dp_erg]=lj[i1]; 
	    erg=tj+dp_erg;
          }
          else
          {
	    injektiv=0;                // Injektivitaet verletzt
            for (int g=0;g<i1;g++)     // belegte Positionen loeschen
            {
	      GenPtr help=lj[g].key;
	      dpmod(kj,help,dp_erg);
              dp_erg=dp_erg%subsize;  
              tj[dp_erg].key = EMPTY ; 
       	    }
          }
        }                       // Elemente getestet

      }                         // naechster Versuch oder fertig

      delete lj;
      rehash = false;
      return true; 
    }                             // subcard<=mj

    else                          // |Wj|>Mj , d.h. Groesse der
                  	          // Subtable verdoppeln
    { 
      if (bed>=0)                 // Kontrolle des Platzverbrauchs
      {
        rehash = true;
        return true;    
      }
      else                        // Platz in Ordnung             
                                  // Untertafel verdoppeln
      { 
        // int oldssize= mal_pot2(sqr(mj),1); ( never used)
        int i1=0;
                                  // Elemente in Liste retten
        stp lj = new subtable[subcard]; 
        for (int i2=0; i1<oldscard ; i2++)
          if ( tj[i2].key != EMPTY  )
            lj[i1++]=tj[i2];
        lj[i1].set_s(a,b);

        for ( ; mj<wj ; mj<<=1 ) ;    // Subtable vergroessern
        subsize = mal_pot2(sqr(mj),1); 

        if ((tj>ende)||(tj<anf))      // Speicherverwaltung
          delete tj;

        tj=new subtable[subsize]; 
        int injektiv=0;
                                      // injektive Funktion suchen
        while (!injektiv)       
        {
          injektiv=1;
          ran >> kj;
          for (i1=0; (i1<subcard) && injektiv ; i1++)
          {
            dpmod(kj,lj[i1].key,dp_erg);
            dp_erg=dp_erg%subsize;
            if ( tj[dp_erg].key == EMPTY )
            { 
              tj[dp_erg]=lj[i1]; 
              erg=tj+dp_erg;
            }

             else                       // Injektivitaet verletzt
             {
	       injektiv=0;
               for (int g=0;g<i1;g++)   // Subtables saeubern
               {
		 GenPtr help=lj[g].key;
		 dpmod(kj,help,dp_erg);
                 dp_erg=dp_erg%subsize;
		 tj[dp_erg].key = EMPTY ; 
	       }

	     }
          }                             // alle Elemente getestet

        }                               // naechster Versuch oder fertig

    	delete lj;
  	rehash = false;
	return true;
      }
    }
  }                       // insert
   
//-----------------------------------------------------------------
// dinsert
// fuegt ein Paar (Schluessel,Information) in das Bucket ein
// benutzt Informationen aus Konstruktor oder Rehash_all
// gibt true zurueck, falls erfolgreich


int headertable::dinsert(GenPtr a,GenPtr b,
			  stp ende,stp& wo,stp& erg)

{
  if (mj==1)                    // nur ein Element in Tafel 
  {				// und Tafel anlegen  
    tj=wo;
    wo++; 
    tj->set_s(a,b);    
    erg=tj;
    return true;  
  }                             // leere Tafel jetzt mit Element

  if (mj==2)                    // zwei Elemente in Tafel 
  {
    if (!tj)       		// und Tafel anlegen
    { 
      wj=1;
      tj=wo;
      wo+=2; 
      tj[0].set_s(a,b);    
      erg=tj;
      return true;  
    }                           // leere Tafel jetzt mit Element
    else
    { 
      if (a==tj[0].key)
      {
	tj[0].inf = b;
	erg = tj;
	return false;
      }
      wj=2;
      tj[1].set_s(a,b);
      erg=tj+1;
      return true;
    }
  }

  if (mj<=4)                    // max 4 Elemente in Tafel 
  {
    if (!tj)       		// und Tafel anlegen
    { 
      wj=1;
      tj=wo;
      wo+=4; 
      tj[0].set_s(a,b);    
      erg=tj;
      return true;  
    }                           // leere Tafel jetzt mit Element
    else
    { 
      for (int i1=0; i1<wj; i1++)
        if (a==tj[i1].key)
        {
	  tj[i1].inf = b;
	  erg = tj+i1;
	  return false;
        }

      tj[wj].set_s(a,b);
      erg=tj+wj;
      wj++;
      return true;
    }
  }



  if (!tj)                      // Tafel muss angelegt werden
				// Tafel mit meheren Elementen
				// erstes Element kommt hinein
  { 
    int q=mal_pot2(sqr(mj),1);  // Platz anfordern aus Pool
    tj=wo;
    wo+=q; 
    if (wo>ende+1)
      LEDA_EXCEPTION(1,"memory allocation error");

    ran >> kj;     // erstes Element einfuegen
    unsigned long dp_erg=0;
    dpmod(kj,a,dp_erg);
    dp_erg=dp_erg%q;
    tj[dp_erg].set_s(a,b);
    erg=tj+dp_erg;
    wj = 1;                     // jetzt aktuelles wj
    return true;       
  }                             // leere Tafel jetzt mit 1.Element

                                // Tafel ist schon angelegt und enthaelt Element
                                // Tafel bekommt mindestens 5 Elemente
  unsigned long dp_erg=0;
  int subsize=mal_pot2(sqr(mj),1);
  dpmod(kj,a,dp_erg);
  dp_erg=dp_erg%subsize;

  if ( tj[dp_erg].key == a)           // gleicher Schluessel
  { 
    tj[dp_erg].set_s(a,b);
    erg=tj+dp_erg;
    return false;      
  }

  if ( tj[dp_erg].key == EMPTY  )     // Position leer
  { tj[dp_erg].set_s(a,b);   
    erg=tj+dp_erg;
  }

  else                          // Position besetzt -> lokales Rehash 
  {  
				// Elemente sammeln
     stp lj = new subtable[wj+1];
     int i1=0;
     int i2=0;
     for (i2=0; i1<wj ; i2++)   // Elemente in Liste kopieren
     { if ( tj[i2].key != EMPTY  )
       { lj[i1++]=tj[i2];  
         tj[i2].clear() ; 
       }
     }

     lj[i1++].set_s(a,b);         // i1 = wj+1

                                  // lokales Rehash , alle Elemente in Liste lj
     int injektiv=0;
                                  // injektive Funktion suchen
     while (!injektiv)         
     {
       injektiv=1;
       ran >> kj;

       for (i2=0; (i2<i1) && injektiv ; i2++)
       { 
         dpmod(kj,lj[i2].key,dp_erg);
	 dp_erg=dp_erg%subsize;
         if ( tj[dp_erg].key == EMPTY )
	 {
	   tj[dp_erg]=lj[i2]; 
	   erg=tj+dp_erg;
         }
         else                      // Injektivitaet verletzt
         {
	   injektiv=0;
           for (int g=0;g<i2;g++)  // Subtables saeubern
           {
	     GenPtr help=lj[g].key;
             unsigned long dp_erg=0;
	     dpmod(kj,help,dp_erg);
             dp_erg=dp_erg%subsize;
             tj[dp_erg].key = EMPTY ;
	   }
         }
       }                           // alle Elemente getestet 

     }                             // neuer Versuch oder fertig

     delete lj;
  }

  wj++;                           // aktuelle Anzahl updaten
  return true;

}


// ----------------------------------------------------------------
// lookup
// sucht nach Eintrag mit Schluessel a
// gibt Pointer auf Eintrag zurueck, falls gefunden
// 0 sonst

stp headertable::lookup(GenPtr a)

{ 
   if (!wj)  return 0;              // Headertable leer

   if (mj==1)                       // Headertable einelementig
   { 
     if (a==tj->key) 
       return tj;
     else
       return 0;
   }

   if (mj<=4)                       // Tafel mit max 4 Elementen
   { 
     for (int i1=0; i1<wj; i1++)
       if (a==tj[i1].key) 
         return tj+i1;
     return 0;
   }
                                    // Headertable mit mehr als 4 Subtables
   unsigned long dp_erg=0;
   dpmod(kj,a,dp_erg);
   dp_erg=dp_erg%(mal_pot2(sqr(mj),1));   // Position

   if (tj[dp_erg].key==a)
     return tj+dp_erg;
   else
     return 0; 
 } 

// ----------------------------------------------------------------
// del
// loescht Element in Tafel
// gibt true zurueck, wenn erfolgreich

bool headertable::del(GenPtr a,stp anf,stp ende)

{
  if (!wj) return false;             // leere Headertable

  if (mj==1)                         // Headertable einelementig
  { 
    if (a==tj->key) 
    {
      wj=0;                          // Schluessel gefunden
      tj->clear() ;
      if ((tj<anf)||(tj>ende))
      {
	delete tj;
	tj = 0;
	mj = 0;
      } 
      return true;    
    }
    else
      return false;
  }
	 
  if (mj<=4)           
  { 
    if (wj>1)                        // Headertable mit mind 2 Elementen
    {
      for (int i1=0; i1<wj; i1++)
      { 
	if (tj[i1].key==a)            // Element gefunden
        { 
          wj--;
          tj[i1]=tj[wj];             // Loch fuellen
          tj[wj].clear();
          return true;
        }
      }
      return false;
    }
    else                               // ein Element in Bucket
    {
      if (tj[0].key==a) 
      {
	wj=0;                          // Schluessel gefunden
        tj[0].clear() ;
        if ((tj<anf)||(tj>ende))
        {
	  delete tj;
	  tj = 0;
	  mj = 0;
        } 
        return true;    
      }
      else
        return false;
    }
  }
	 
				      // Headertable mit mehreren Subtables
  unsigned long dp_erg=0;
  dpmod(kj,a,dp_erg);
  dp_erg=dp_erg%(mal_pot2(sqr(mj),1));   // Position

  if (tj[dp_erg].key==a)
  {
    wj--;                             // Schluessel gefunden
    tj[dp_erg].clear() ;

    if (wj==0)                        // Bucket nun leer
      if ((tj<anf)||(tj>ende))
      {
	delete tj;
	tj = 0;
	mj = 0;
      } 
    return true;    
  } 
  else return false;
}

// ---------------------------------------------------------
// give_elements()
// haengt Elemente der Tafel an Liste an
// gibt Anzahl der Elemente zurueck


int headertable::give_elements(stp& wo,stp anf,stp ende)

{ 
  int j=0;

  if (!wj) { 
	     if ( tj && ((tj<anf)||(tj>ende))) 
             {
	       if (mj>1)
                 delete tj;
	       else 
                 delete tj;

	       tj = 0;
	       mj = 0;
             }
	     return 0;
	   }

  if (mj==1)                    // Headertable einelementig
  { 
    (*wo)=(*tj);                // Element kopieren
    wo++;
    if ((tj<anf)||(tj>ende))     // gebe Speicher frei
    {
      delete tj;
      tj = 0;
      mj = 0;
    }
    return 1; 
  }

  if (mj<=4)                     // Headertable maximal vierelementig
  { 
    j=0;   
    while ( (tj[j].key != EMPTY) && (j<mj) )
    { 
      (*wo)=tj[j];
      wo++;
      j++;
    }

    if ((tj<anf)||(tj>ende))     // gebe Speicher frei
    { 
      delete tj;
      tj = 0;
      mj = 0;
    }
    return j; 
  }

				 // Headertable mit meheren Elementen
  bool weiter=true;
  int subsize=mal_pot2(sqr(mj),1);
  j=0;

  for (int k=0;(k<subsize)&&weiter;k++)  // suche Elemente
    if ( tj[k].key != EMPTY  )
    {                                    // haenge Element an Liste
      (*wo)=tj[k];
      wo++; j++;
      if (j>=wj) weiter=false; 
    }
  if ((tj<anf)||(tj>ende))               // gebe Speicher frei
  { 
    delete tj;
    tj = 0;
    mj = 0;
  }
  return j;
}

// ----------------------------------------------------------------
// member-functions of class dp_hash
// ----------------------------------------------------------------

// -------------------------------------------------------
// rehash_all
//
// stellt Headertables neu auf, um Platzverbrauch korrekt
// zu halten
// fuegt Element dann neu ein

stp dp_hash::rehash_all( GenPtr x, GenPtr y )

{ 
  int i,i1,j,nsave,platz;
  unsigned long dp_erg=0;

  stp erg=0;
  stp l=new subtable[n];         // Sammle Elemente in Liste l

  i=0;
  stp pos = l;
  nsave = ( x == EMPTY ? n : n-1 );

  while ( (i<sM) && (nsave>0) )  // Sammle Element aus allen Headertables
  { 
    if (htablep[i])
    {
      nsave -= htablep[i]->give_elements(pos,anf,ende) ;
      delete htablep[i];
    }
    i++;   
  }

  if ( x != EMPTY )               // fuege neues Element hinzu
    l[n-1].set_s(x,y);

  free ((char*)htablep);          // Speicher freigeben
  if (anf) 
    delete anf; 
					    // neue Parameter setzen

  M=int((1+_dp_h_c)*n);
  sM = int(((4.0/3.0)*wursechs)*(1+_dp_h_c)*n);

                    			  // Speicher allokieren
  htablep=(htp*) calloc(sM,sizeof(htp));  // schneller als new+init
  int* buckets=new int[n];

  if (!htablep)
    LEDA_EXCEPTION(1,"memory overflow");

  platz=n;
  i1=0;
  while (!i1)                    // Top-Funktion suchen
  { 
    bed=0;
    ran >> k;

    for (i=0;i<n;i++)           // Hashe alle Elemente
    {
      GenPtr help=l[i].key;
      dpmod(k,help,dp_erg);
      dp_erg=dp_erg%sM;
      buckets[i] = (int)dp_erg;      // Merke Headertable

      if (!htablep[dp_erg]) 
        htablep[dp_erg] = new headertable;

                                // Aendere Headertables
      int f=++(htablep[dp_erg]->wj);
      int* groesse=&(htablep[dp_erg]->mj);

      if (f<=2)
	 (*groesse)++;
      else
        if (*groesse<f)
        { 
	  (*groesse)<<=1;      
  
	  if (*groesse==4)       // vergroessere Feld
	    platz++;
          else
	    if (*groesse==8)     // Uebergang von Feld auf Tafel
	      platz+=123;
            else
	      platz+=3*sqr(*groesse)/2; 
        }
	else                     // Tafel nicht vergroessert
	  platz--;

      bed+=mal_pot2(f,2)-2; 
    }                            // alle Elemente gehasht

                                 // bed-=(((8.0*sqr(M))/sM)+2*M);
    double _bed=(wursechs+2)*M;   // Vereinfachung durch Einsetzen von sM
    bed-=int(_bed);
    i1=(bed<0);                  // kontrolliere Platz

    if (!i1)                     // nicht erfolgreich, saeubere Headertables
    {
      platz=n;
      for (j=0; j<sM; j++)
        if (htablep[j]) 
	{ 
	  delete htablep[j];
	  htablep[j] = 0 ;   
	}
    }

  }                              // Top-Funktion gefunden


  anf=new subtable[platz];        // allokiere Speicher fuer alle Subtables
  wo=anf;
  ende=anf+platz-1;

  for (i=0; i<n; i++)             // fuege Elemente wieder ein
  { 
    int bucket=buckets[i];  
    htablep[bucket]->dinsert(l[i].key,l[i].inf,ende,wo,erg);
  }

  delete buckets;
  delete l;
  return erg;
}

// --------------------------------------------------------
// insert
//
// fuegt Element in die entsprechende Headertable ein

stp dp_hash::insert(GenPtr x,GenPtr y)

{ 
  // change for VS 2015, 64 bit
  // if ( (unsigned long)x>maxuni )  
#if (defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(_M_X64))
  if ( (unsigned long long)x>maxuni )
    LEDA_EXCEPTION(2,string("dp_hash: key %d out of range",x));
#else
  if ( (unsigned long)x>maxuni )
    LEDA_EXCEPTION(2,string("dp_hash: key %d out of range",x));
#endif
  copy_key(x);
  copy_inf(y);

  unsigned long dp_erg=0;
  dpmod(k,x,dp_erg);
  dp_erg=dp_erg%sM;                   // Toptafel

  bool rehash = false;
  stp  erg    = 0;

  if ( !htablep[dp_erg] ) 
    htablep[dp_erg] = new headertable;

  if ( htablep[dp_erg]->insert(x,y,erg,bed,rehash,anf,ende) )  n++;

  if ( rehash )  erg=rehash_all(x,y);

  return erg;
}

// --------------------------------------------------------
// del
//
// loescht Element aus entsprechender Headertable

void dp_hash::del(GenPtr x)

{ 
  // change for VS 2015, 64 bit
  //  if ( (unsigned long)x>maxuni )  
#if (defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(_M_X64))
	if ( (unsigned long long)x>maxuni ) 
    LEDA_EXCEPTION(2,string("key %d out of range",x));
#else
	if ( (unsigned long)x>maxuni )  
    LEDA_EXCEPTION(2,string("key %d out of range",x));
#endif
// s.n. :

stp p = lookup(x);
if (p==0) return;
clear_key(p->key);
clear_inf(p->inf);


  unsigned long dp_erg=0;
  dpmod(k,x,dp_erg);
  dp_erg=dp_erg%sM;                   // Toptafel

  if ( !htablep[dp_erg] ) return;     // Headertable nicht vorhanden
  				      // in Toptafel loeschen

  if ( htablep[dp_erg]->del(x,anf,ende) ) n--;     

  if ( !htablep[dp_erg]->wj )
  { 
    delete htablep[dp_erg];
    htablep[dp_erg] = 0;
  }

  if ((n*(1+3*_dp_h_c)<M) && (n>3))
    rehash_all(); 

} 

// -------------------------------------------------------
// lookup,change_inf
//
// suchen in Headertable nach Element mit Schluessel
// change_inf setzt Information des Elementes auf y


stp dp_hash::lookup(GenPtr x) const 

{ 
  // change for VS 2015, 64 bit
  //  if ( (unsigned long)x>maxuni )  
#if (defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(_M_X64))
	if ( (unsigned long long)x>maxuni )
    LEDA_EXCEPTION(2,string("key %d out of range",x));
#else
	if ( (unsigned long)x>maxuni )  
    LEDA_EXCEPTION(2,string("key %d out of range",x));
#endif

  unsigned long dp_erg=0;
  dpmod(k,x,dp_erg);
  dp_erg=dp_erg%sM;                   // Toptafel

  if (!htablep[dp_erg]) 
    return 0;

  stp y=htablep[dp_erg]->lookup(x);
  return y ;

}
 
stp dp_hash::change_inf(GenPtr x,GenPtr y)      
{ 
  // change for VS 2015, 64 bit
  //  if ( (unsigned long)x>maxuni )  
#if (defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(_M_X64))
	if ( (unsigned long long)x>maxuni )
    LEDA_EXCEPTION(2,string("key %d out of range",x));
#else
	if ( (unsigned long)x>maxuni )
    LEDA_EXCEPTION(2,string("key %d out of range",x));
#endif

  unsigned long dp_erg=0;
  dpmod(k,x,dp_erg);
  dp_erg=dp_erg%sM;                   // Toptafel


  if (!htablep[dp_erg])
    return 0;

  stp t = htablep[dp_erg]->lookup(x) ;
  if (t)
    t->inf = y ;
  return t;

}

// -------------------------------------------------------
// clear
//
// loescht alle Elemente und
// setzt Hashing auf Leerzustand

void dp_hash::clear()

{ 
  stp l = new subtable[n];
  stp pos=l;

  for (int i=0;i<sM;i++)           // Headertables loeschen
    if (htablep[i])
    { 
      htablep[i]->give_elements(pos,anf,ende);
      delete htablep[i];                         
    }

  free ((char*)htablep) ;          // Speicher freigeben

  if (anf) 
    delete anf;

  delete l;

  n = 0;                           // Leerinitialisierung
  M=4;
  sM=13;
  ran >> k;
  bed=-17;
  htablep=(htp*) calloc(sM,sizeof(htp));
  anf = wo = ende = 0;

}

// ------------------------------------------------------------
// Konstruktoren und Destruktor

dp_hash::dp_hash()

{ 
  n=0;
  M=4;
  sM=13;
  //ran >> k;
  k = 9061960;
  bed=-17;
  htablep=(htp*) calloc(sM,sizeof(htp));
  anf = wo = ende = 0;

}

dp_hash::dp_hash(int an,GenPtr* keys,GenPtr* inhalte)   
                                          // wie rehash_all
                                          // die Elemente stehen aber schon in Listen
{ 
  int i,i1,j,nsave,platz;
  unsigned long dp_erg=0;
  stp erg=0;

  n=an;
  M=int((1+_dp_h_c)*n);
  sM = int(((4.0/3.0)*wursechs)*(1+_dp_h_c)*n);

  htablep=(htp*) calloc(sM,sizeof(htp));
  int* buckets = new int[n];

  if (!htablep)
    LEDA_EXCEPTION(1,"memory overflow");

  platz=n;
  i1=0;

  while (!i1)
  {
    bed=0;
    ran >> k;

    for (i=0;i<n;i++)
    {
      GenPtr help=keys[i];
	  // change for VS 2015, 64 bit
      // if ( (unsigned long)help>maxuni )  
#if (defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(_M_X64))
	  if ( (unsigned long long)help>maxuni )
         LEDA_EXCEPTION(2,string("key %d out of range",help));
#else
	  if ( (unsigned long)help>maxuni )  
         LEDA_EXCEPTION(2,string("key %d out of range",help));
#endif
      dpmod(k,help,dp_erg);
      dp_erg=dp_erg%sM;
      buckets[i] = (int)dp_erg;

      if (!htablep[dp_erg]) 
	htablep[dp_erg] = new headertable;

      int f=++(htablep[dp_erg]->wj);
      int* groesse=&(htablep[dp_erg]->mj);

      if (f<=2)
	 (*groesse)++;
      else
        if (*groesse<f)
        { 
	  (*groesse)<<=1;      
  
	  if (*groesse==4)       // vergroessere Feld
	    platz++;
          else
	    if (*groesse==8)     // Uebergang von Feld auf Tafel
	      platz+=123;
            else
	      platz+=3*sqr(*groesse)/2; 
        }
	else                     // Tafel nicht vergroessert
	  platz--;

      bed+=mal_pot2(f,2)-2; 
    }
	
                                 // bed-=(((8.0*sqr(M))/sM)+2*M);
    double _bed=(wursechs+2)*M;   // Vereinfachung durch Einsetzen von sM
    bed-=int(_bed);
    i1=(bed<0);

    if (!i1)
    { 
      platz=n;
      for (j=0; j<sM; j++)
	if (htablep[j]) 
	{ 
	  delete htablep[j];
	  htablep[j] = 0;   
	}
    }
  }

  nsave=an;
  anf=new subtable[platz];

  wo=anf;
  ende=wo+platz-1;
  n=0;

  for (i=0; i<nsave; i++)
  {
    int bucket = buckets[i];
    n += (htablep[bucket]->dinsert(keys[i],inhalte[i],ende,wo,erg));
  } 

  delete buckets;

  if ((n*(1+3*_dp_h_c)<M) && (n>3))
    rehash_all();   

}

dp_hash::~dp_hash()
{ 
  clear();                            // loesche alles

  free ((char*)htablep);              // gebe Speicher frei
  if (anf) delete anf;

  n=M=sM=0;
  k=0;
  anf=wo=ende=0;
  htablep=0;

}

LEDA_END_NAMESPACE
