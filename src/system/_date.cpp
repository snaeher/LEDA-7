/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _date.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/date.h>

#include <time.h>

LEDA_BEGIN_NAMESPACE

// --------------------------------------------------------------------------
//
// LEDA date class
//
// author:  Sven Thiel (sthiel@mpi-sb.mpg.de)
//
// --------------------------------------------------------------------------

static const char* token_str[] = 
{"delim:", "diy", "dow", "DOW:", "DOW", "dth", "dd", "d", "mm", "m", "M:", "M",
  "w" , "yyyy", "yy", "[yy]yy"   , "*"  , "?" };

#define _DATE_LAST_LANG_ french
static const char* predef_month_names[] = { 
  // english
  "January", "February", "March", "April", "May", "June", "July", "August",
  "September", "October", "November", "December",
  
  // german
#if defined(__WIN32__) || defined(_WIN32) || defined(__NT__)
  "Januar", "Februar", "M\344rz", "April", "Mai", "Juni", "Juli", "August",
  "September", "Oktober", "November", "Dezember",
#else
  "Januar", "Februar", "M\344rz", "April", "Mai", "Juni", "Juli", "August",
  "September", "Oktober", "November", "Dezember",
#endif  
  
  // french
#if defined(__WIN32__) || defined(_WIN32) || defined(__NT__)
  "janvier", "fevrier", "mars", "avril", "mai", "june", "julliet", "ao–t",
  "septembre", "octobre", "novembre", "decembre"
#else
  "janvier", "fevrier", "mars", "avril", "mai", "june", "julliet", "aout",
  "septembre", "octobre", "novembre", "decembre"
#endif  
};

static const char* predef_dow_names[] = {
  // english
  "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday",
  // german
  "Montag","Dienstag","Mittwoch","Donnerstag","Freitag","Samstag","Sonntag",
  // french
  "lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi", "dimanche"
};

#define _DATE_LAST_FMT_ hyphens
static const char* predef_formats[] =
{ "mm/dd/[yy]yy", "dd.mm.[yy]yy", "dd:mm:[yy]yy", "dd-mm-[yy]yy" };

leda_lock date::mutex_language;
date::language date::_language;

string date::month_names[13];
string date::dow_names[8];

leda_lock date::mutex_i_fmt;
date::format date::i_fmt;
string date::i_fmt_str;
slist<int> date::i_fmt_tokens;

leda_lock date::mutex_o_fmt;
date::format date::o_fmt;
string date::o_fmt_str;
slist<int> date::o_fmt_tokens;

string date::swallow_fmt_str = "m/d/[yy]yy;d?m?[yy]yy";
slist<int> date::swallow_tokens;

class date_init {
public:
  date_init() 
  {
    date::set_language(date::english);
    date::set_output_format(date::US_standard);
    date::set_input_format(date::US_standard);  
    date::parse_input_format(date::swallow_fmt_str, date::swallow_tokens);
  }
};
static date_init _init_; // must be the last static definition !!!

date::date(int d, month m, int y)
{
  if (!set_date(d,m,y)) LEDA_EXCEPTION(1, "date: invalid date");
}

date::date(string date_str, bool swallow)
{
  if (!set_date(date_str, swallow)) LEDA_EXCEPTION(1, "date: invalid date");
}

date::language date::get_language() { return _language; }
date::format date::get_input_format() { return i_fmt; }
string date::get_input_format_str() { return i_fmt_str; }
date::format date::get_output_format() { return o_fmt; }
string date::get_output_format_str() { return o_fmt_str; }

// languages ...

void date::set_language(language l)
{
  if (l<=user_def_lang || l>_DATE_LAST_LANG_) 
    LEDA_EXCEPTION(1, "date: invalid language");
  else {
    mutex_language.lock();

    if (l == local)
      get_local_names();
    else {
      copy_month_names(predef_month_names + (l-english)*12);
      copy_dow_names(predef_dow_names + (l-english)*7);
    }
    _language = l;
    
    mutex_language.unlock();
  }
}

void date::get_local_names()
{
  tm T;
  T.tm_sec   = 0;
  T.tm_min   = 0;
  T.tm_hour  = 0;
  T.tm_mday  = 0;
  T.tm_mon   = 0;
  T.tm_year  = 0;
  T.tm_yday  = 0;
  T.tm_isdst = 0;
    
  char buf[21];

  const char* dow_fmt = "%A"; // full weekday name
  for (int d=1; d<=7; ++d) {
    T.tm_wday = d % 7;
    strftime(buf, 21, dow_fmt, &T);
    dow_names[d] = buf;
  }

  const char* m_fmt = "%B"; // full month name
  for (int m=1; m<=12; ++m) {
    T.tm_mon = m-1;
    strftime(buf, 21, m_fmt, &T);
    month_names[m] = buf;
  }
}

void date::set_month_names(const char* names[])
{
  mutex_language.lock();

  _language = user_def_lang; 
  copy_month_names(names);

  mutex_language.unlock();
}

void date::copy_month_names(const char* names[])
{
  for(int m=1; m<=12; ++m) 
    month_names[m] = names[m-1];
}

void date::set_dow_names(const char* names[])
{
  mutex_language.lock();

  _language = user_def_lang; 
  copy_dow_names(names);

  mutex_language.unlock();
}

void date::copy_dow_names(const char* names[])
{
  for(int d=1; d<=7; ++d) 
    dow_names[d] = names[d-1];
}

// input and output formats ...

date::fmt_token date::get_token(string f, int& pos, int& i1, int& i2)
// gets the token in f at pos and sets pos to the start of the next token
{
  if (pos >= f.length()) return _bad_fmt;
  for (int t = _delim; t <= _skip1; ++t) {
    string ts = token_str[t];
    if (f(pos, pos+ts.length()-1) == ts) {
      pos += ts.length();
      if (t == _delim || t == _DOWc || t == _Mc || t == _skip) 
      { if (pos >= f.length()) return _bad_fmt;
        else 
        { i1 = f[pos++];
          if (t == _DOWc || t == _Mc) 
          { if (!isdigit(i1)) return _bad_fmt;
            else i1 -= '0';
           }
         }
       }
      return fmt_token(t);
    }
  }
  if (f[pos] == '"' || f[pos] == '\'') {
    i1 = pos + 1;
    i2 = f.index(f[pos], i1) - 1; // look for corresp. quote
    pos = i2 + 2;
    if (i2 >= i1) return _text;
    else if (i2-i1 == -1) return _empty_txt; // empty text
    else return _bad_fmt;
  }
  else
    if (f[pos] == ';') { ++pos; return _next_fmt; }
    else { i1 = f[pos++]; return _sep; }
}

bool date::parse_input_format(const string& f, slist<int>& fmt_tokens)
{
  fmt_tokens.clear();
  fmt_token t;
  int pos=0, i1, i2;
  while (pos < f.length()) {
    switch (t = get_token(f, pos, i1, i2)) {
      case _bad_fmt:
      case _diy:
      case _dow: 
      case _DOW: 
      case _DOWc:
      case _week: return false;

      case _empty_txt: break;

      case _text: fmt_tokens.append(t);
                  fmt_tokens.append(i1);
                  fmt_tokens.append(i2);
                  break;

      case _dth:  fmt_tokens.append(_d);     // convert "dth" to "d??"
                  fmt_tokens.append(_skip1);
                  fmt_tokens.append(_skip1);
                  break;

      case _M:    fmt_tokens.append(t);
                  if (pos >= f.length()) return false;
                  else fmt_tokens.append(f[pos++]);
                  break;

      case _delim:if (!fmt_tokens.empty()) return false;
                  // fall-through !
      case _Mc:   
      case _skip:
      case _sep:  fmt_tokens.append(t); 
                  fmt_tokens.append(i1);
                  break;

     default:    fmt_tokens.append(t);
    }
  }
  return true;
}  

bool date::parse_output_format(const string& f, slist<int>& fmt_tokens)
{
  fmt_tokens.clear();
  fmt_token t;
  int pos=0, i1, i2;
  while (pos < f.length()) {
    switch (t = get_token(f, pos, i1, i2)) {
      case _bad_fmt:
      case _delim:
      case _skip1: 
      case _skip: return false;

      case _empty_txt: break;

      case _next_fmt: return true; // read only the first format
        
      case _text: fmt_tokens.append(t);
                  fmt_tokens.append(i1);
                  fmt_tokens.append(i2);
                  break;

      case _DOWc:
      case _Mc:
      case _sep:  fmt_tokens.append(t); 
                  fmt_tokens.append(i1);
                  break;

      case _yy_or_yyyy: t = _yyyy; // and fall through

      default:    fmt_tokens.append(t);
    }
  }
  return true;
}  

void date::set_input_format(format f)
{
  if (f <= user_def_fmt || f>_DATE_LAST_FMT_) 
    LEDA_EXCEPTION(1, "date: invalid format");
  else {
    mutex_i_fmt.lock();

    i_fmt = f;
    i_fmt_str = predef_formats[f-1];
    if (!parse_input_format(i_fmt_str, i_fmt_tokens))
      LEDA_EXCEPTION(1, "date: internal error (ifmt)");

    mutex_i_fmt.unlock();
  }
}

void date::set_input_format(string f)
{
  mutex_i_fmt.lock();

  i_fmt     = user_def_fmt;
  i_fmt_str = f;
  bool ok = parse_input_format(f, i_fmt_tokens);

  mutex_i_fmt.unlock();

  if (!ok) LEDA_EXCEPTION(1, "date: bad input format string");
}

bool date::read(istream& i)
{
  mutex_i_fmt.lock();

  string s; char c; int p;

  //s.n.  skip whitespace

  do i >> c; while (is_space(c));

  if (!i) return true;

  i.putback(c);


  //slist_item it = i_fmt_tokens.first();
  slist<int>::item it = i_fmt_tokens.first();

  while (it != nil && i_fmt_tokens[it] != _next_fmt && i) {
    switch (i_fmt_tokens[it]) {
      case _d:
      case _m:     // read one char
                   i.get(c); s += c;
                   // if the following char is a digit, read that too
                   if (i.get(c) && isdigit(c)) s += c; 
                   else if (i) i.putback(c);
                   break;
      case _yyyy:  // read two chars and fall-through!
                   i.get(c); s += c; i.get(c); s += c;
      case _dd:  
      case _mm: 
      case _yy:    // read two chars
                   i.get(c); s += c; i.get(c); s += c; break;
      case _yy_or_yyyy:
                   // read two chars
                   i.get(c); s += c; i.get(c); s += c;
                   // if the following char is a digit, read two more
                   if (i.get(c) && isdigit(c)) { s += c; i.get(c); s += c; }
                   else if (i) i.putback(c);
                   break;
      case _Mc:    // read a certain (fixed) amount of chars
                   it = i_fmt_tokens.succ(it);
                   for (p=0; p<i_fmt_tokens[it]; ++p) { i.get(c); s += c; }
                   break;
      case _delim:
      case _M:
      case _skip:  // read until a certain sep. char is found
                   it = i_fmt_tokens.succ(it);
                   do {i.get(c); s+=c;} while (i && c!=char(i_fmt_tokens[it]));
                   break;
      case _sep:   it = i_fmt_tokens.succ(it); // fall-through
      case _skip1: // read one char
                   i.get(c); s += c; break;
      case _text:  { // read a certain amount of chars (corres. to text-length)
                     it = i_fmt_tokens.succ(it); int i1 = i_fmt_tokens[it];
                     it = i_fmt_tokens.succ(it); int i2 = i_fmt_tokens[it];
                     for (p=i1; p<=i2 && i; ++p) { i.get(c); s += c; }
                     break;
                    }
    }
    it = i_fmt_tokens.succ(it);
   }
  bool ok = update_date(s, i_fmt_tokens, i_fmt_str);

  mutex_i_fmt.unlock();

  return ok;
}

void date::set_output_format(format f)
{
  if (f <= user_def_fmt || f>_DATE_LAST_FMT_) 
    LEDA_EXCEPTION(1, "date: invalid format");
  else {
    mutex_o_fmt.lock();

    o_fmt = f;
    o_fmt_str = predef_formats[f-1];
    if (!parse_output_format(o_fmt_str, o_fmt_tokens))
      LEDA_EXCEPTION(1, "date: internal error (ofmt)");

    mutex_o_fmt.unlock();
  }
}

void date::set_output_format(string f)
{
  mutex_o_fmt.lock();

  o_fmt     = user_def_fmt;
  o_fmt_str = f;
  bool ok = parse_output_format(f, o_fmt_tokens);

  mutex_o_fmt.unlock();

  if (!ok) LEDA_EXCEPTION(1, "date: bad output format string");
}

bool date::set_date(int d, month m, int y)
{
  if (!is_valid(d, m, y)) return false;
  ymd = (y<<9) + (int(m)<<5) + d;
  return true;
}

int date::days_since_zero() const
{
  // days from 1.1.1 to 1.1.get_year()
  return days_in_years(get_year() - 1)

  // days from 1.1.1 to 1.get_month().get_year()
       + days_in_months(get_month() - 1, get_year())

  // days from 1.1.1 to get_day().get_month().get_year()
       + get_day() - 1;
}

bool date::update_date(int dsz)
{
  if (!is_valid(dsz)) return false;

  int full_years = 1 + dsz * 400 / (365*400 + 97); // upper bound
  dsz -= days_in_years(full_years);
  if (dsz < 0) { dsz += days_in_year(full_years--); }

  int full_months = 1 + dsz / 31; // upper bound
  dsz -= days_in_months(full_months, full_years+1);
  if (dsz < 0) { dsz += days_in_month(full_months--, full_years+1); }

  return set_date(dsz+1, month(full_months+1), full_years+1);
}

int date::extract_number(const string& s, int& pos, int& num, int max_digits)
{
  int num_of_digits = 0;
  num = 0;
  while (max_digits > num_of_digits && pos < s.length() && isdigit(s[pos])) {
    ++num_of_digits;
    num *= 10; num += s[pos++] - '0';
  }
  return num_of_digits;
}

void date::comp_yyyy_from_yy(int& y)
{
  y += reference_year - reference_year % 100;
  if (y < reference_year) y += 100;
}

int date::get_month_by_name(string month_name)
{
  int m;
  for (m=1; m<=12; ++m)
    if ( month_names[m].index(month_name) == 0 ) break;
  return (m <= 12) ? m : -1;
}

bool date::update_date(const string date_str, const slist<int>& fmt_tokens,
                       const string& fmt_str)
{
  int len = date_str.length();
  //slist_item it = fmt_tokens.first();
  slist<int>::item it = fmt_tokens.first();
  while (it != nil) {
    int d = get_day(), m = get_month(), y = get_year();

    int pos = 0; // current pos in date_str
    while (it != nil && fmt_tokens[it] != _next_fmt && pos<len) {
      switch (fmt_tokens[it]) {
        case _delim: pos = len+1; break; // skip it
        case _dd:   if (extract_number(date_str, pos, d, 2) != 2 ) pos = len+1;
                    break;
        case _d:    if (extract_number(date_str, pos, d, 2)  < 1 ) pos = len+1;
                    break;
        case _mm:   if (extract_number(date_str, pos, m, 2) != 2 ) pos = len+1;
                    break;
        case _m:    if (extract_number(date_str, pos, m, 2)  < 1 ) pos = len+1;
                    break;
        case _yy:   if (extract_number(date_str, pos, y, 2) != 2 ) pos = len+1;
                    else comp_yyyy_from_yy(y);
                    break;
        case _yyyy: if (extract_number(date_str, pos, y, 4) != 4 ) pos = len+1;
                    break;
        case _yy_or_yyyy:
                    {
                      int num_of_digits = extract_number(date_str, pos, y, 4);
                      if (num_of_digits == 2) comp_yyyy_from_yy(y);
                      else if (num_of_digits != 4) pos = len+1;
                      break;
                    }
        case _Mc:   {
                      it = fmt_tokens.succ(it);
                      int end_pos = pos + fmt_tokens[it] - 1;
                      m = get_month_by_name(date_str(pos, end_pos));
                      if (m < 0)
                        pos = len + 1;
                      else
                        pos = end_pos + 1;
                      break;
                    }
        case _M:    {
                      it = fmt_tokens.succ(it);
                      int marker_pos=date_str.index(char(fmt_tokens[it]),pos);
                      m = get_month_by_name(date_str(pos, marker_pos-1));
                      if (marker_pos <= pos || m < 0) 
                        pos = len + 1;
                      else
                        pos = marker_pos + 1;
                      break;
                    }
        case _skip: {
                      it = fmt_tokens.succ(it);
                      int marker_pos=date_str.index(char(fmt_tokens[it]),pos);
                      if (marker_pos < pos) 
                        pos = len+1;
                      else 
                        pos = marker_pos + 1;
                      break;
                    }
        case _skip1: ++pos; break;
        case _text: {
                      it = fmt_tokens.succ(it); int i1 = fmt_tokens[it];
                      it = fmt_tokens.succ(it); int i2 = fmt_tokens[it];
                      if (fmt_str(i1,i2) != date_str(pos,pos+i2-i1)) 
                        pos = len+1;
                      else
                        pos += i2-i1 + 1;
                      break;
                    }
        case _sep : {
                      it = fmt_tokens.succ(it);
                      if (char(fmt_tokens[it]) != date_str[pos++])
                        pos = len+1;
                      break;
                    }
      }
      it = fmt_tokens.succ(it);
    }

    if ((it == nil || fmt_tokens[it] == _next_fmt) && pos == len)
      if (set_date(d,month(m),y)) return true;
    while (it != nil && fmt_tokens[it] != _next_fmt) it = fmt_tokens.succ(it);
    if (it != nil) it = fmt_tokens.succ(it);
  }
  return false;
}

void date::set_to_current_date()
{
  time_t clock;
  time(&clock);
  tm* T = localtime(&clock); 

  if (!set_date(T->tm_mday, month(T->tm_mon+1), 1900 + T->tm_year))
    LEDA_EXCEPTION(1, "date: internal error (cur_date)");
}

string date::ordinal_number(int n)
{
  if (n>10 && n<20) return string("%dth",n);
  switch (n%10) {
    case 1:  return string("%dst",n);
    case 2:  return string("%dnd",n);
    case 3:  return string("%drd",n);
    default: return string("%dth",n);
  }
}

string date::get_date() const
{
  mutex_o_fmt.lock();

  string res;
  //slist_item it = o_fmt_tokens.first();
  slist<int>::item it = o_fmt_tokens.first();
  while (it != nil) {
    switch (o_fmt_tokens[it]) {
      case _diy:  res += string("%d",get_day_in_year()); break;
      case _DOWc: it = o_fmt_tokens.succ(it); 
                  res += get_dow_name().head(o_fmt_tokens[it]); break;
      case _DOW:  res += get_dow_name(); break;
      case _dow:  res += string("%d",get_day_of_week()); break;
      case _dth:  res += ordinal_number(get_day()); break;
      case _dd:   res += string("%.2d",get_day()); break;
      case _d:    res += string("%d",get_day()); break;
      case _mm:   res += string("%.2d",(int)get_month()); break;
      case _m:    res += string("%d",(int)get_month()); break;
      case _Mc:   it = o_fmt_tokens.succ(it);
                  res += get_month_name().head(o_fmt_tokens[it]); break;
      case _M:    res += get_month_name(); break;
      case _week: res += string("%d",get_week()); break;
      case _yy:   res += string("%.2d",get_year() % 100); break;
      case _yyyy: res += string("%.4d",get_year()); break;
      case _text: {
                    it = o_fmt_tokens.succ(it); int i1 = o_fmt_tokens[it];
                    it = o_fmt_tokens.succ(it); int i2 = o_fmt_tokens[it];
                    res += o_fmt_str(i1, i2); break;
                  }
      case _sep:  it = o_fmt_tokens.succ(it);
                  res += char(o_fmt_tokens[it]); break;
    }
    it = o_fmt_tokens.succ(it);
  }

  mutex_o_fmt.unlock();

  return res;
}

bool date::add_to_month(int months)
{
  int y = get_year() + months / 12;
  int m = get_month()+ months % 12;
  if (m > 12) { ++y; m -= 12; }
  else if (m < 1)  { --y; m += 12; }
  int d = leda_min(get_day(), days_in_month(m, y));
  return set_date(d, month(m), y);
}

bool date::add_to_year(int years)
{
  int y = get_year() + years;
  int m = get_month();
  int d = leda_min(get_day(), days_in_month(m, y)); //e.g.:29.2.1996->28.2.1997
  return set_date(d, month(m), y);
}

int date::get_week() const 
{ 
  int diy_next_Sunday = get_day_in_year()+ 7-get_day_of_week();
  if (diy_next_Sunday < 4)
    return (3+days_in_year(get_year()-1) + diy_next_Sunday)/7;
  if (diy_next_Sunday - days_in_year(get_year()) >= 4)
    return 1;
  return (3+diy_next_Sunday)/7;
}

date& date::operator+=(int d)
{ 
  if ( set_date(get_day()+d, get_month(), get_year()) ) return *this;
  if ( !update_date(days_since_zero() + d) ) 
    LEDA_EXCEPTION(1, "date: date out of range in an arithmetic operation");
  return *this;
}

int date::months_until(const date& D2) const
{
  if (D2 < *this) return - D2.months_until(*this);

  int res = D2.get_month() - get_month() + 12 * (D2.get_year() - get_year());
  return D2.get_day() >= get_day() || D2.is_last_day_in_month() ? res : res-1;
}

int date::years_until(const date& D2) const
{
  int ymd1 = get_ymd(), ymd2 = D2.get_ymd();
  if (ymd2 < ymd1) return - D2.years_until(*this);

  if (D2.get_day()==28 && D2.get_month()==2 && !is_leap_year(D2.get_year()))
    ++ymd2; // ymd rep. of "29.2.(D2.get_year())"
  return (ymd2 - ymd1) >> 9;
}

bool date::is_valid(int d, month m, int y)
{ return y>=1 && y<=9999 && m>=1 && m<=12 && d>=1 && d<=days_in_month(m,y); }

bool date::is_valid(int dsz)
{ return dsz >= 0 && dsz <= 3652058; }

bool date::is_valid(string date_str, bool swallow)
{
  date dummy(1,date::Jan,1);
  return dummy.set_date(date_str, swallow);
}

bool date::is_last_day_in_month() const
{ return get_day() == days_in_month(get_month(),get_year()); }


bool date::set_date(const string date_str, bool swallow)
{ 
  mutex_i_fmt.lock();
  mutex_language.lock();
  bool ok = swallow ? update_date(date_str, swallow_tokens, swallow_fmt_str) : 
                      update_date(date_str, i_fmt_tokens, i_fmt_str); 
  mutex_language.unlock();
  mutex_i_fmt.unlock();

  return ok;
}


//static

int date::days_in_month(int m, int y) 
{
  if (m >= 8) return 31 - m%2;
  else
    if (m==2) return is_leap_year(y) ? 29 : 28; 
    else      return 30 + m%2;
}

int date::days_in_months(int m, int y)
{
  int res = m * 30;
  if (m >= 2) res -= is_leap_year(y) ? 1 : 2; // correction for February
  return res + (m + (m<8 ? 1:2)) / 2;
}

int date::days_in_year(int y) { return is_leap_year(y) ? 366 : 365; }

int date::days_in_years(int full_years)
{ return full_years*365 + (full_years/4 - full_years/100 + full_years/400); }

LEDA_END_NAMESPACE
