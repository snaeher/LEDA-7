/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _d_int_set.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------------
//
//  S. Naeher (1993)
//  modified by K. Reinert (1997)
//  homogenized with sets by M. Seel (1997 the day after)
//------------------------------------------------------------------------

#include <LEDA/core/d_int_set.h>

/*
	The implementation of maintains the following invariant:
	If NoOfElements > 0, then V[0] != 0 and V[NoOfWords-1] != 0.
*/



LEDA_BEGIN_NAMESPACE


bool d_int_set::member(int x) const{ 
  if( V == nil )
    return false;
  short w = CalcOffset(x);
  if( w < Offset || w >= Offset+NoOfWords )
    return false;
  else {
    return (V[w-Offset] & (word(1) << CalcBit(x))) != 0;
  }
}

bool d_int_set::empty() const 
{ return (NoOfElements == 0 ? true : false);  }

int  d_int_set::size()  const 
{ return NoOfElements; }

/*
#include <regtest.h>
*/

int  d_int_set::choose() const
{ 
  LEDA_PRECOND(V != nil, "d_int_set::choose: empty set!")
  random_source S; 
  int o,b;

  do {
    o=S(Offset,Offset+NoOfWords-1); 
    b=S(0,SIZE_OF_ULONG-1);
  } while (CalcVal(b,o) > max());

  for(int i=o; i<Offset+NoOfWords; i++){
    word w = V[i-Offset];
    w >>= b;
    int j;
    for(j=b; j<SIZE_OF_ULONG; (j++,b=0)){
      if( (w & (word)1) == 1) {
        return(CalcVal(j, i));
      }
      w >>= 1;
    }
  }

 // never reached
 return -1;
}



void d_int_set::get_element_list(list<int>& l) const {
  l.clear();

  if( V == nil ) return;
  int i;
  for(i=0; i<NoOfWords; i++){
    word w = V[i];
    int j;
    for(j=0; j<SIZE_OF_ULONG; j++){
      if( (w & (word)1) == 1)
        l.append(CalcVal(j, i+Offset));
      w >>= 1;
    }
  }
}



int d_int_set::min() const{
  if ( V == nil ) 
    LEDA_EXCEPTION(1,"d_int_set::min: set empty.");

  word w = V[0];
  int j;
  for(j=0; j<SIZE_OF_ULONG; j++) {
    if( w & word(1) )
      return CalcVal(j, Offset);
    w >>= 1;
  }
  LEDA_EXCEPTION(1,"d_int_set::min: invariant error.");
  return -1;
}

int d_int_set::max() const{
  if ( V == nil ) 
    LEDA_EXCEPTION(1,"d_int_set::max: set empty.");

  word w = V[NoOfWords-1];
  int j;
  word mask = 1;
  mask <<= (SIZE_OF_ULONG-1);
  for(j=(SIZE_OF_ULONG-1); j>=0; j--){
    if( w & mask )
      return CalcVal(j, Offset+NoOfWords-1);
    w <<= 1;
  }
  LEDA_EXCEPTION(1,"d_int_set::max: invariant error.");
  return -1;
}

int d_int_set::next_element(int x) const
{
  short i = CalcOffset(x) - Offset;
  if (i < 0 || i >= NoOfWords) {
    LEDA_EXCEPTION(1, "d_int_set::get_next_element: x not contained in set.");
    return MAXINT;
  }

  word w = V[i];
  int j = CalcBit(x) + 1;
  w >>= j;

  if (w == 0 || j >= SIZE_OF_ULONG) {
    do {
      if (++i == NoOfWords) return MAXINT;
    } while (V[i] == 0);
    w = V[i]; j = 0;
  }

  while ((w & 1) == 0) { 
    ++j; w >>= 1; 
  }

  return CalcVal(j, Offset + i);
}

void d_int_set::insert(int x)
{
  short w = CalcOffset(x); 

  if( w >= Offset+NoOfWords )
  { if( V == nil ) Offset = w;
    short newNoOfWords = w-Offset+1;
    word* tmp = new word[newNoOfWords];
    int i;
    for(i=0; i<NoOfWords; i++) tmp[i] = V[i];
    for(i=NoOfWords; i<newNoOfWords; i++) tmp[i] = (word)0;
    if( V != nil ) delete[] V;
    V = tmp;
    NoOfWords = newNoOfWords;
  }

  if( w < Offset )
  { if( V == nil ) Offset = w+1;
    short newNoOfWords = NoOfWords+Offset-w;
    word* tmp = new word[newNoOfWords];
    int i;
    for(i=0; i<Offset-w; i++) tmp[i] = (word)0;
    for(i=Offset-w; i<newNoOfWords; i++) tmp[i] = V[i-Offset+w];
    delete[] V;
    V = tmp;
    NoOfWords = newNoOfWords;
    Offset    = w;
  }

  w -= Offset;
  int pos = CalcBit(x);
  word tmp = ((word(1) << pos) | V[w]);
  if( V[w] != tmp ){
    V[w] = tmp;
    NoOfElements++;
  }
}


void d_int_set::del(int x){ 
  short w = CalcOffset(x);
  int p = CalcBit(x);

  if( w >= Offset+NoOfWords || w < Offset || V == nil )
    return;

  word tmp = V[w-Offset] & (~(word(1) << p));
  if( tmp != V[w-Offset]){
    V[w-Offset] = tmp;
    NoOfElements--;

    if(NoOfElements == 0){
      delete[] V;
      Offset    = 0;
      NoOfWords = 0;
      V         = nil;
    }
    else{
      if(w == Offset){
        int count = 0;
        while( V[count] == 0 )
          count++;
          
        if( count > 0){
          short newNoOfWords = short(NoOfWords-count);
          word* tmp = new word[newNoOfWords];

          for(int i=0; i<newNoOfWords; i++)
            tmp[i] = V[i+count];

          delete[] V;
          V = tmp;
          NoOfWords = newNoOfWords;
          Offset    = short(Offset+count);
        }
      }
      else
        if(w == Offset+NoOfWords-1){
          int count = NoOfWords-1;
          while( V[count] == 0 )
            count--;
          if( count < NoOfWords-1){
            short newNoOfWords = short(count+1);
            word* tmp = new word[newNoOfWords];

            for(int i=0; i<newNoOfWords; i++)
              tmp[i] = V[i];

            delete[] V;
            V = tmp;
            NoOfWords = newNoOfWords;
          }
      }
    }
  }
}




bool d_int_set::operator!=(const d_int_set& C) const
{ return (! operator==(C)); }
bool d_int_set::operator<=(const d_int_set& C) const
{ return subset(C); }
bool d_int_set::operator>=(const d_int_set& C) const
{ return C.subset(*this); }
bool d_int_set::operator<(const d_int_set& C) const
{ return proper_subset(C); }
bool d_int_set::operator>(const d_int_set& C) const
{ return C.proper_subset(*this); }


d_int_set::d_int_set(){ 
  V            = nil;
  Offset       = 0;
  NoOfWords    = 0;
  NoOfElements = 0;
} 


d_int_set::d_int_set(const d_int_set& C){ 
  NoOfElements = C.NoOfElements;
  Offset       = C.Offset;
  NoOfWords    = C.NoOfWords;

  int i;
  if(NoOfWords > 0){
    V = new word[NoOfWords];
    for(i=0; i<NoOfWords; i++)
      V[i] = C.V[i];
  }
  else
    V = nil;
}



d_int_set& d_int_set::operator=(const d_int_set& C){ 
  if (this == &C) return *this;

  if( V != nil ) delete[] V;
  NoOfElements = C.NoOfElements;
  Offset       = C.Offset;
  NoOfWords    = C.NoOfWords;

  int i;
  if(NoOfWords > 0){
    V = new word[NoOfWords];
    for(i=0; i<NoOfWords; i++)
      V[i] = C.V[i];
  }
  else
    V = nil;

  return *this;
}


bool d_int_set::operator==(const d_int_set& C) const {
// Due to the invariant stated above, the code below is correct.
// I.e. we return true iff get_element_list() == C.get_element_list().

  if(Offset != C.Offset) return false;
  if(NoOfWords != C.NoOfWords) return false;
  if(NoOfElements != C.NoOfElements) return false;

  int i;
  for(i=0; i<NoOfWords; i++)
    if(V[i] != C.V[i]) return false;
  return true;
}


void d_int_set::clear(){ 
  delete[] V;
  V            = nil;
  NoOfElements = 0;
  NoOfWords    = 0;
  Offset       = 0;
}


 


void d_int_set::_join(const d_int_set& C){ 
  if( C.empty() )
     return;
  if( empty() )
  {
    operator=(C);
    return;
  }

  int start    = Offset;          
  int end      = NoOfWords+Offset;
  bool realloc = false;

  if( C.Offset < start ){
    start   = C.Offset;
    realloc = true;
  }

  if( C.Offset+C.NoOfWords > end ){
    end     = C.Offset+C.NoOfWords;
    realloc = true;
  }
  
  int i;
  if( realloc ){
    word* tmp = new word[end-start];
    for(i=start; i<end; i++)
      tmp[i-start] = (word)0;
    for(i=C.Offset; i<C.Offset+C.NoOfWords; i++)
      tmp[i-start] = C.V[i-C.Offset];
    for(i=Offset; i<Offset+NoOfWords; i++)
      tmp[i-start] |= V[i-Offset];
    Offset    = (short)start;
    NoOfWords = short(end-start);
    delete[] V;
    V = tmp;
  } 
  else
    for(i=C.Offset; i<C.Offset+C.NoOfWords; i++)
      V[i-Offset] |= C.V[i-C.Offset];

  {
    NoOfElements = 0;
    for(i=0; i<NoOfWords; i++){
      word w = V[i];
      int j;
      for(j=0; j<SIZE_OF_ULONG; j++){
        if( (w & 1) == (word)1 )
         NoOfElements++;
        w >>= 1;
      }
    }
  }
    
}





void d_int_set::_intersect(const d_int_set& C){ 
  if( C.empty() ){
    clear();
    return;
  }
  if( empty() )
    return;

  int start    = Offset;          
  int end      = NoOfWords+Offset;

  if( C.Offset > start )
    start = C.Offset;

  if( C.Offset+C.NoOfWords < end )
    end = C.Offset+C.NoOfWords;
  
  int i;
  for(i=start; i<end; i++)
    V[i-Offset] &= C.V[i-C.Offset];

  {
    for(i=start; i<end; i++)
      if( V[i-Offset] == (word)0 )
        start++;
      else
        break;
     
    for(i=end-1; i>=start; i--)
      if( V[i-Offset] == (word)0 )
        end--;
      else
        break;

    if( end - start <= 0 ){
      delete[] V;
      V = nil;
      Offset       = 0;
      NoOfWords    = 0;
      NoOfElements = 0;
    }
    else
      if( end-start < NoOfWords ){
        word* tmp = new word[end-start];
        for(i=start; i<end; i++)
          tmp[i-start] = V[i-Offset];
        Offset    = (short)start;
        NoOfWords = short(end-start);
        delete[] V;
        V = tmp;
      }
  }

  {
    NoOfElements = 0;
    for(i=0; i<NoOfWords; i++){
      word w = V[i];
      int j;
      for(j=0; j<SIZE_OF_ULONG; j++){
        if( (w & 1) == (word)1 )
         NoOfElements++;
        w >>= 1;
      }
    }
  }
    
}




void d_int_set::_diff(const d_int_set& C){ 
  if( C.empty() || empty() )
     return;

  int start = Offset;          
  int end   = NoOfWords+Offset;

  if( C.Offset > start )
    start = C.Offset;

  if( C.Offset+C.NoOfWords < end )
    end = C.Offset+C.NoOfWords;
  
  int i;
  for(i=start; i<end; i++)
    V[i-Offset] &= (~C.V[i-C.Offset]);
  
  start = Offset;
  end   = NoOfWords+Offset;

  {
    for(i=start; i<end; i++)
      if( V[i-Offset] == (word)0 )
        start++;
      else
        break;
     
    for(i=end-1; i>=start; i--)
      if( V[i-Offset] == (word)0 )
        end--;
      else
        break;

    if( end - start <= 0 ){
      delete[] V;
      V = nil;
      Offset       = 0;
      NoOfWords    = 0;
      NoOfElements = 0;
    }
    else
      if( end-start < NoOfWords ){
        word* tmp = new word[end-start];
        for(i=start; i<end; i++)
          tmp[i-start] = V[i-Offset];
        Offset    = (short)start;
        NoOfWords = short(end-start);
        delete[] V;
        V = tmp;
      }
  }

  {
    NoOfElements = 0;
    for(i=0; i<NoOfWords; i++){
      word w = V[i];
      int j;
      for(j=0; j<SIZE_OF_ULONG; j++){
        if( (w & 1) == (word)1 )
         NoOfElements++;
        w >>= 1;
      }
    }
  }

}


void d_int_set::_symdiff(const d_int_set& C){ 
  if( C.empty() )
     return;

  int start    = Offset;          
  int end      = NoOfWords+Offset;
  bool realloc = false;

  if( C.Offset < start ){
    start   = C.Offset;
    realloc = true;
  }

  if( C.Offset+C.NoOfWords > end ){
    end     = C.Offset+C.NoOfWords;
    realloc = true;
  }
  
  int i;
  if( realloc ){
    word* tmp = new word[end-start];
    for(i=start; i<end; i++)
      tmp[i-start] = (word)0;
    for(i=C.Offset; i<C.Offset+C.NoOfWords; i++)
      tmp[i-start] = C.V[i-C.Offset];
    for(i=Offset; i<Offset+NoOfWords; i++)
      tmp[i-start] ^= V[i-Offset];
    Offset    = (short)start;
    NoOfWords = short(end-start);
    delete[] V;
    V = tmp;
  } 
  else
    for(i=C.Offset; i<C.Offset+C.NoOfWords; i++)
      V[i-Offset] ^= C.V[i-C.Offset];

  {
    for(i=start; i<end; i++)
      if( V[i-Offset] == (word)0 )
        start++;
      else
        break;
     
    for(i=end-1; i>=start; i--)
      if( V[i-Offset] == (word)0 )
        end--;
      else
        break;

    if( end - start <= 0 ){
      delete[] V;
      V = nil;
      Offset       = 0;
      NoOfWords    = 0;
      NoOfElements = 0;
    }
    else
      if( end-start < NoOfWords ){
        word* tmp = new word[end-start];
        for(i=start; i<end; i++)
          tmp[i-start] = V[i-Offset];
        Offset    = (short)start;
        NoOfWords = short(end-start);
        delete[] V;
        V = tmp;
      }
  }

  {
    NoOfElements = 0;
    for(i=0; i<NoOfWords; i++){
      word w = V[i];
      int j;
      for(j=0; j<SIZE_OF_ULONG; j++){
        if( (w & 1) == (word)1 )
         NoOfElements++;
        w >>= 1;
      }
    }
  }

}



bool d_int_set::subset(const d_int_set& C) const { 
  if(C.NoOfElements < NoOfElements)
    return false;

  // The following check is necessary! 
  // (Ow. we would return false, if *this is empty and C is not.)
  if(NoOfElements == 0)
    return true;

  if(Offset < C.Offset || Offset+NoOfWords > C.Offset+C.NoOfWords)
    return false;
   
  int i;
  for(i=Offset; i<Offset+NoOfWords; i++){
    unsigned long c1 = V[i-Offset];
    unsigned long c2 = C.V[i-C.Offset];
	if ( (c1 & c2) != c1 ) return false;
  }
  return true;
}

        
bool d_int_set::proper_subset(const d_int_set& C) const { 
 return( subset(C) && (NoOfElements < C.NoOfElements) );
}



istream& operator>>(istream& I, d_int_set& C)
{
  int i;
  while (I >> i) C.insert(i);
  return I;
}

//#define DEBUG

ostream& operator<<(ostream& O, const d_int_set& C)
{
#ifdef DEBUG
 O  << "\nDebug output of d_int_set:\n"
    << "Offset       = " << C.Offset << endl
    << "NoOfWords    = " << C.NoOfWords << endl
    << "NoOfElements = " << C.NoOfElements << " {" << endl;
 for(int i=0; i<C.NoOfWords; i++){
   word w = C.V[i];
   O << "Word " << i << " : ";
   for(int j=0; j<(d_int_set::SIZE_OF_ULONG); j++){
     if( (w & 1) == (word)1 )
       O << C.CalcVal(j, i+C.Offset) << " ";
       //O << j+(i+C.Offset)*C::SIZE_OF_ULONG << " ";
     w >>= 1;
   }
   O << endl;
 }
 O << "}" << endl;
#else
  for(int i=0; i<C.NoOfWords; i++){
    word w = C.V[i];
    for(int j=0; j<(d_int_set::SIZE_OF_ULONG); j++){
      if( (w & 1) == (word)1 )
        O << C.CalcVal(j, i+C.Offset) << " ";
      w >>= 1;
    }
  }
#endif
 return O;
}

LEDA_END_NAMESPACE
