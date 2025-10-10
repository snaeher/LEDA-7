/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _param_handler.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/param_handler.h>

#include <stdlib.h>

LEDA_BEGIN_NAMESPACE

/*
int compare(param_handler* const& p1, param_handler* const& p2)
{ return p2-p1; }
*/

list<param_handler*> param_handler::Objects;
leda_lock               param_handler::Obj_mutex;

static bool atob(string s)
{
  if (s == "true") return true;
  if (s == "false") return false;
  return atoi(s) != 0;
}

static string extend_string(string s, int m)
{
  for(int j=s.length(); j<m; j++) s += " ";
  return s;
}



void param_handler::add_parameter(string s){
  if (!initialized)
    ParamList.append(param(s));
  else
    LEDA_EXCEPTION(1,"add_parameter(string) : handler already initialized.");
}

int param_handler::atoe(param& p){
  int i=0;
  list_item it = p.Enums.first();
  while(p.Enums[it] != p.Value){
    i++;
    it = p.Enums.succ(it);
    if( it == nil )
      LEDA_EXCEPTION(1,"atoe() : could not find enum label in list.");
  }
  return (i);
}

void param_handler::print_help_all()
{
  param_handler* P;
  forall (P, Objects) P->print_help();
}

void param_handler::print_help() const
{
  if (!initialized)
    LEDA_EXCEPTION(1,"print_help() : handler not initialized.");

  param p;
  int maxl = 6;
  int maxf = 5;
  int maxt = 5;
  int maxv = 6;
  forall (p,ParamList) {
    maxl = leda_max(p.Label.length(),maxl);
    maxf = leda_max(p.Flag.length(),maxf);
    maxt = leda_max(p.Type.length(),maxt);
    maxv = leda_max(p.Value.length(),maxv);
  }

  maxl += 2; maxf += 2; maxt += 2; maxv += 2;

  cout << endl << "param_handler("+File+
    ") has the following parameters:\n";
  cout << 
"---------------------------------------------------------------------\n";
  cout << extend_string("Flag",maxf) << 
          extend_string("Label",maxl) <<
          extend_string("Type",maxt) << 
          extend_string("Default Value",maxv) << endl;
  cout << 
"---------------------------------------------------------------------\n";
  forall (p,ParamList) 
  cout << extend_string(p.Flag,maxf) <<
          extend_string(p.Label,maxl) <<
          extend_string(p.Type,maxt) <<
          extend_string(p.Value,maxv) << endl;
}




void param_handler::init_all()
{
  bool help_requested = false;
  param_handler* P;
  forall (P, Objects) P->init(help_requested);

  if (help_requested) {
    print_help_all();
    exit(0);
  }
}

void param_handler::init(bool& help_requested) 
{
  if (initialized) return;
  initialized = true;

  param p;

  forall (p,ParamList) {
    // we don't allow double entries
    if (Type[p.Label] != UNDEF) 
      LEDA_EXCEPTION(1,"init(): "+(p.Label)+" already defined.");
    if (Type[p.Flag] != UNDEF) 
      LEDA_EXCEPTION(1,"init(): "+(p.Flag)+" already defined.");

    // here we count the necessary memory slots and set the types
    if (p.Type == "bool") {
     Type[p.Label] = Type[p.Flag] = BOOL;
      AnzBool++; 
    } else if (p.Type == "double") {
      Type[p.Label] = Type[p.Flag] = DOUBLE;
      AnzDouble++;
    } else if (p.Type == "int") { 
      Type[p.Label] = Type[p.Flag] = INT;
      AnzInt++;
    } else if (p.Type == "string") {
      Type[p.Label] = Type[p.Flag] = STRING;
      AnzString++;
    } else if (p.Type.head(4) == "enum") {
      Type[p.Label] = Type[p.Flag] = ENUM;
        AnzEnum++;
    } else
      LEDA_EXCEPTION(1,"init(): wrong type.");
  }

  // we allocate the memory:
  PBool   = new  bool[AnzBool];
  PInt    = new  int[AnzInt];
  PDouble = new  double[AnzDouble];
  PString = new  string[AnzString];
  PEnum   = new  int[AnzEnum];
  POffset = new  int[AnzEnum];
 

  // default values
  int ib(0),id(0),ii(0),is(0),ie(0);
  forall (p,ParamList) {
    switch (Type[p.Label]) {
      // we set the the index for the accessors of the cells
      // via the labels and the flags
      case BOOL: 
        PBool[ib] = atob(p.Value);
        Index[p.Label] = ib; Index[p.Flag] = ib;
        ib++;
        break;
      case DOUBLE:
        PDouble[id] = atof(p.Value);
        Index[p.Label] = id; Index[p.Flag] = id;
        id++;
        break;
      case INT: 
        PInt[ii] = atoi(p.Value);
        Index[p.Label] = ii; Index[p.Flag] = ii;
        ii++;
        break;
      case STRING: 
        PString[is] = p.Value;
        Index[p.Label] = is; Index[p.Flag] = is;
        is++;
        break;
      case ENUM: 
        PEnum[ie]   = atoe(p);
        POffset[ie] = p.Offset;
        Index[p.Label] = ie; Index[p.Flag] = ie;
        ie++;
        break;
    }
  }

  // file values
  if (FileInit) {
    ifstream from(File);
    if (from) 
      from >> (*this);
  }

  // command line values
  /* now for the command line params which overwrite the
     until now set default values:

     we allow that argv[i] (which are all separated by blanks) 
     1<=i<argc can be:
     >> a flag identifier
     >> a value string
     >> a flag + a value string not separated by a space 
  */
 
  if(Argv != 0){
    int i=1;
    while(i < Argc){
      string arg = argument(i);

      if(arg == "-h" || arg == "-?" || arg == "-help") {
        ++i;
        help_requested = true;
        continue;
      }

      if(is_flag(arg)) { 
        /* now Argv[i] is a flag => Argv[i+1] is the corresponding
           value or if the flag is a bool toggle Argv[i+1] is
           again a flag */
          
        if(Type[arg] == BOOL){
          if( Argc > i+1){ 
            if( is_bool( argument(i+1)) ){ 
              PBool[Index[arg]] =   atob(argument(i+1));
              i+=2;
            }
            else{
              toggle(PBool[Index[arg]]); 
              i++;
            }
          }
          else{
            toggle(PBool[Index[arg]]); 
            i++;
          }
        }
        else { 
         switch (Type[arg]) {
           case DOUBLE: PDouble[Index[arg]] = 
                          atof(argument(i+1)); break;
           case INT:    PInt[Index[arg]] =    
                          atoi(argument(i+1)); break;
           case ENUM:   PEnum[Index[arg]] =   
                          atoi(argument(i+1))-
                          POffset[Index[arg]]; break;   
           case STRING: PString[Index[arg]] = argument(i+1); break;
           default: LEDA_EXCEPTION(1,"init(): "+arg+
                                  " is no allowed flag.");
         }
         i++;
       }
     } 
     else { // arg is not only a flag but flag+value
       int b = has_flag_prefix(arg);
       if(!b)
         i++;
       else {
         string fl  = arg.head(b);
         string val = arg(b,arg.length());
         switch (Type[fl]) {
           case BOOL:   PBool[Index[fl]] =   atob(val); break;
           case DOUBLE: PDouble[Index[fl]] = atof(val); break;
           case INT:    PInt[Index[fl]] =    atoi(val); break;
           case ENUM:   PEnum[Index[fl]] =   
                          atoi(val)-POffset[Index[fl]]; break;
           case STRING: PString[Index[fl]] = val; break;
           default: LEDA_EXCEPTION(1,"init(): "+fl+
                                  " is no allowed flag.");
         }
         i++;
       }
     }
   }
 }
}


void  param_handler::set_parameter(const string& label, 
                                       int val) 
{
 if(Type[label]==UNDEF) 
    LEDA_EXCEPTION(1,"set_parameter(int): "+label+" not defined.");
 if(Type[label]==INT) 
   PInt[Index[label]] = val;
 if(Type[label]==ENUM) 
   PEnum[Index[label]] = val;
 return;
/*
 LEDA_EXCEPTION(1,"set_parameter(int): "+label+
               " wrong type access. Type was "+
               string("%d",Type[label])+" expected was type "+
               string("%d",INT));
*/
}
  
void  param_handler::set_parameter(const string& label, 
                                       double val) 
{
  if(Type[label]==UNDEF) 
    LEDA_EXCEPTION(1,"set_parameter(double): "+label+" not defined.");
  if(Type[label]!=DOUBLE) 
    LEDA_EXCEPTION(1,"set_parameter(double): "+label+
                  " wrong type access. Type was "+
                  string("%d",Type[label])+" expected was type "
                  +string("%d",(int)DOUBLE));
  PDouble[Index[label]] = val;
}
  
void  param_handler::set_parameter(const string& label, 
                                       bool val) 
{
  if(Type[label]==UNDEF) 
    LEDA_EXCEPTION(1,"set_parameter(bool): "+label+" not defined.");
  if(Type[label]!=BOOL) 
    LEDA_EXCEPTION(1,"set_parameter(bool): "+label+
                  " wrong type access. Type was "+
                  string("%d",Type[label])+" expected was type "+
                  string("%d",(int)BOOL));
   PBool[Index[label]] = val;
}
  
void  param_handler::set_parameter(const string& label, 
                                       string val) 
{
  if(Type[label]==UNDEF) 
    LEDA_EXCEPTION(1,"set_parameter(string): "+label+" not defined.");
  if(Type[label]!=STRING) 
    LEDA_EXCEPTION(1,"set_parameter(string): "+label+
                  " wrong type access. Type was "+
                  string("%d",Type[label])+" expected was type "+
                  string("%d",(int)STRING));
  PString[Index[label]] = val;
}



double& param_handler::get_double(const string& label) const
{
  if(Type[label]==UNDEF) 
    LEDA_EXCEPTION(1,"access: "+label+" not defined.");
  if(Type[label]!=DOUBLE)
    LEDA_EXCEPTION(1,"get_double(): "+label+
                  " wrong type access. Type was "+
                  string("%d",Type[label])+" expected was type "+
                  string("%d",(int)DOUBLE));
   return PDouble[Index[label]];
}

int& param_handler::get_int(const string& label) const
{
  if(Type[label]==UNDEF) 
    LEDA_EXCEPTION(1,"access: "+label+" not defined.");

  if(Type[label]!=INT)
    LEDA_EXCEPTION(1,"get_int(): "+label+
                  " wrong type access. Type was "+
                  string("%d",Type[label])+" expected was type "+
                  string("%d",(int)INT)+" or "+string("%d",(int)ENUM));

   return PInt[Index[label]];
}
  
int&  param_handler::get_enum(const string& label) const
{
  if(Type[label]==UNDEF) 
    LEDA_EXCEPTION(1,"access: "+label+" not defined.");

  if(Type[label]!=ENUM)
    LEDA_EXCEPTION(1,"get_enum(): "+label+
                  " wrong type access. Type was "+
                  string("%d",Type[label])+" expected was type "+
                  string("%d",(int)INT)+" or "+string("%d",(int)ENUM));
  return PEnum[Index[label]];  

}

bool& param_handler::get_bool(const string& label) const
{
  if(Type[label]==UNDEF) 
    LEDA_EXCEPTION(1,"access: "+label+" not defined.");
  if(Type[label]!=BOOL) 
    LEDA_EXCEPTION(1,"get_bool(): "+label+
                  " wrong type access. Type was "+
                  string("%d",Type[label])+" expected was type "+
                  string("%d",(int)BOOL));
  return PBool[Index[label]];
}
  
string& param_handler::get_string(const string& label) const
{
  if(Type[label]==UNDEF) 
    LEDA_EXCEPTION(1,"access: "+label+" not defined.");
  if(Type[label]!=STRING) 
    LEDA_EXCEPTION(1,"get_string(): "+label+
                  " wrong type access. Type was "+
                  string("%d",Type[label])+" expected was type "+
                  string("%d",(int)STRING));
  return PString[Index[label]];
}



param::param(string s) 
{
  int l = s.length();
  int a = 0;
  int b = 0;
  Offset = 0;

  while(b < l) 
    if(s[b++] == ':') 
      break;
  if(s[b-1] != ':') 
     LEDA_EXCEPTION(1,
     "param(string s): Delimiter ':' not found after first field");
  Label = s(a,b-2); 

  a = b;
  while(b < l) 
    if(s[b++] == ':') 
      break;
  if(s[b-1] != ':') 
     LEDA_EXCEPTION(1,
     "param(string s): Delimiter ':' not found after second field");
  Flag = s(a,b-2);

  a = b;
  while(b < l) 
    if(s[b++] == ':') 
      break;
  if(s[b-1] != ':') 
     LEDA_EXCEPTION(1,
     "param(string s): Delimiter ':' not found after third field");
  Type = s(a,b-2);
  a = b;

  if( l <= b-1) 
     LEDA_EXCEPTION(1,
     "param(string s): No default value found in fourth field");
  Value =  s(a,l-1);

 
  if(Type.head(4) == "enum"){
    a=5;
    b=5;
    l=Type.length();

    bool end = false;
    while( ! end ){
      if(Type[b] == ')')
        end = true; 
      else 
        if(Type[b] != ',' ){
          b++;
          continue;     
        }
      string s(Type(a,b-1));     
      Enums.append(s);
      b++;
      a = b;
    }

    string f = Enums.front();
    if ( (a=f.index('=')) >= 0 ) {
      Enums[Enums.first()] = f.head(a);
      Offset = atoi( f(a+1, f.length()-1) );
    }
  }

}


ostream& operator<<(ostream& O, const param& p) 
{
  O << p.Label+":"+p.Flag+":"+p.Type+":"+p.Value <<endl;
  return O;
}

istream& operator>>(istream& I, param& p)
{
  string line;
  line.read(I,'\n');
  if( line.length() > 0 )
    p = param(line);
  return I;
}



ostream& operator<<(ostream& to, const param_handler& P) 
{
  if (!P.initialized)
    LEDA_EXCEPTION(1,
      "operator<<(ostream&, param_handler&) : handler not initialized");

  param p;
  int v;
  forall(p,P.ParamList){
    int i=0;
    switch (P.Type[p.Label]) {
       case param_handler::BOOL:   
         if( P.PBool[P.Index[p.Label]] ) 
           p.Value = "true"; 
         else 
           p.Value = "false"; 
         break;
       case param_handler::DOUBLE: 
           p.Value = string("%f",P.PDouble[P.Index[p.Label]]); break;
       case param_handler::INT: 
           p.Value = string("%d",P.PInt[P.Index[p.Label]]); break;
       case param_handler::ENUM: 
           v =  P.PEnum[P.Index[p.Label]];
           while( i < v )
             i++; 
           p.Value = p.Enums[p.Enums.get_item(i)]; 
           break;
       case param_handler::STRING: 
           p.Value = string(P.PString[P.Index[p.Label]]); 
           break;
       case param_handler::UNDEF:
         LEDA_EXCEPTION(1,"UNDEF in switch");

    }
    to << p;
  }
  return to;
}


istream& operator>>(istream& from, param_handler& P)
{
  if (!P.initialized)
    LEDA_EXCEPTION(1,
      "operator>>(istream&, param_handler&) : handler not initialized");

  string line;
  while( from ) {
    line.read(from,'\n');
    if( line.length() > 0 ) {
      param p(line);
      switch (P.Type[p.Label]) {
      case param_handler::UNDEF: 
        break; // ignore
      case param_handler::BOOL: 
        P.set_parameter(p.Label, atob(p.Value));
        break;
      case param_handler::DOUBLE:
        P.set_parameter(p.Label, atof(p.Value));
        break;
      case param_handler::INT: 
        P.set_parameter(p.Label, atoi(p.Value));
        break;
      case param_handler::STRING: 
        P.set_parameter(p.Label, p.Value);
        break;
      case param_handler::ENUM: 
        P.set_parameter(p.Label, P.atoe(p));
        break;
      }
    }
  }

  return from;
}



param_handler::param_handler(const char* file, bool ir) :
  Type(UNDEF), Index(-1), initialized(false)
{
  Obj_mutex.lock();
  Objects.append(this);
  Obj_mutex.unlock();

  AnzBool = AnzDouble = AnzInt = AnzString = AnzEnum = 0;

  File = file; FileInit = ir;
  if( FileInit ){
    ifstream from(file);
    if(!from) 
      FileInit = false;
  }

  Argc = 1;
  Argv = 0;
}

param_handler::param_handler(int argc, char** argv, 
                                     const char* file, bool ir)  :
  Type(UNDEF), Index(-1) , initialized(false)
{
  Obj_mutex.lock();
  Objects.append(this);
  Obj_mutex.unlock();

  AnzBool = AnzDouble = AnzInt = AnzString = AnzEnum = 0;

  File = file; FileInit = ir;
  if( FileInit ){
    ifstream from(file);
    if(!from) 
      FileInit = false;
  }

  Argc = argc;
  Argv = argv;
}


param_handler::~param_handler(){
  ofstream to(File);
  if (to) {
    to << *this;
    to.close();
  }
  else
    LEDA_EXCEPTION(1,"~param_handler(): cannot open "+File+".");

  delete[] PBool;
  delete[] PInt;
  delete[] PDouble;
  delete[] PString;
  delete[] PEnum;
  delete[] POffset;

  Obj_mutex.lock();

  //Objects.remove(this);

  list_item it;
  forall_items(it,Objects) 
      if (Objects[it] == this) Objects.del_item(it);

  Obj_mutex.unlock();
}


void param_handler::write_parameter_file() const
{ ofstream o(File); o << *this; }

void param_handler::write_parameter_file(const string& paramfile) const 
{ ofstream o(paramfile); o << *this; }

LEDA_END_NAMESPACE
