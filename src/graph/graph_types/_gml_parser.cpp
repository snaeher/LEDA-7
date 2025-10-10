/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _gml_parser.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


//------------------------------------------------------------------- //
// class gmlparser, extendible parser for the GML format              //
//                                                                    //
// by David Alberts (1997)                                            //
//------------------------------------------------------------------- //

#include <LEDA/graph/gml_parser.h>
#include <LEDA/core/string.h>
#include <string.h>
#include <stdlib.h>


LEDA_BEGIN_NAMESPACE

// ------------------------ class gml_object ------------------------ //

gml_object::~gml_object()
{
  if(the_type == gml_list)
  {
    gml_object* g;
    forall(g,the_list) delete g;
  }
}

static int gml_indent = 0;

ostream& operator<<(ostream& out, const gml_object& g)
{
  for(int i=0; i<gml_indent; i++) out << ' ';
  out << g.the_key << " ";

  switch(g.the_type)
  {
    case gml_int:    { out << g.the_int; break; }
    case gml_double: { out << g.the_double; break; }
    case gml_string: { out << '"' << g.the_string << '"'; break; }
    case gml_list:
    {
      gml_indent +=2;

      out << "[";

      gml_object* h;
      forall(h,g.the_list)
      {
        out << "\n";
        out << *h;
      }

      gml_indent -=2;

      if(g.the_list.size())
      {
        out << "\n";
        for(int i=0; i<gml_indent; i++) out << ' ';
        out << "]";
      }
      else out << " ]";

      break;
    }
    default:         { out << 42; break; }
  }

  return out;
}



// ------------------------ class gml_objecttree ----------------------- //


bool gml_objecttree::read_string(string s)
{
  // reads from string.
 
  root_key = string("istream");

  if (in_chars)  delete[] in_chars;

  in_length = s.length();
  in_chars = new char[in_length+1];
  strcpy(in_chars,s.cstring());
  
  // initialize line break structure and object tree
  init_lines();
  init_tree();

  return build();
}


bool gml_objecttree::read(istream& ins)
{
  // reads from ins.
 
  root_key = string("istream");

  if(in_chars)  delete[] in_chars;

  if(ins.fail()) return false;

  pos = 0;

  // in_length = int(ins.tellg());

  string s;
  s.read_file(ins);
  in_length = s.length();
  in_chars = new char[in_length+1];
  strcpy(in_chars,s.cstring());
  
  // initialize line break structure and object tree
  init_lines();
  init_tree();

  return build();
}

bool gml_objecttree::read(const char* filename)
{
  ifstream ins(filename);
  bool b = read(ins);
  root_key = string(filename);
  return b;
}


void gml_objecttree::init_lines()
{
  lines.clear();
  line_starts.clear();

  lines.insert(0,1);
  line_starts.insert(1,0);

  int line_counter = 2;

  for(pos = 0; pos < in_length; pos++)
  {
    if(current_char() == '\n')
    {
      lines.insert(pos+1,line_counter);
      line_starts.insert(line_counter,pos+1);
      line_counter++;
    }
  }

  pos = 0;
}

void gml_objecttree::init_tree()
{
  // initialize root object
  objects.the_key = root_key.cstring();
  objects.the_type = gml_list;
  objects.the_list.clear();
  objects.parent = 0;

  // initialize context
  syntax_errors = false;
  bracket_level = 0;
  current_context = &objects;
  current_object = new gml_object;
} 


bool gml_objecttree::build()
{
  while(!syntax_errors && (pos < in_length))
    if(read_key()) read_value();

  delete current_object;
  current_object = 0;

  if(!syntax_errors && bracket_level) print_error("unterminated list");

  return !syntax_errors;
}

bool gml_objecttree::read_key()
{
  bool context_switch = false;

  do
  {
    skip_white();
    if(current_char() == ']')
    {
      context_switch = true;
      current_context = current_context->parent;
      if(--bracket_level < 0)
      {
        print_error("too many closing ]");
        return false;
      }
      advance();
    }
    else context_switch = false;

  } while(context_switch);

  if(!isalpha(current_char()))
  {
    if(pos < in_length - 1)  // EOF otherwise
    {
      if(current_char() == '[')
        print_error("list without key word");
      else
        print_error("key word does not start with a letter");
    }
    return false;
  }

  // find delimiter
  int delim = next_white();
  if(delim == -1) return false;

  // delimit and store key
  in_chars[delim] = '\0';
  current_object->the_key = &(in_chars[pos]);
  current_object->key_pos = pos;

  // skip past key
  advance(delim-pos+1);

  return true;
}


void gml_objecttree::read_value()
{
  skip_white();
  switch(current_char())
  {
    case '"':
    {
      // we are reading a string...
      advance();
      int delim = next_quot();
      if(delim == -1)
      {
        print_error("unterminated string");
      }
      else
      {
        current_object->the_type = gml_string;
        in_chars[delim] = '\0';
        current_object->the_string = &(in_chars[pos]);
        current_object->where =
          current_context->the_list.append(current_object);
        current_object->parent = current_context;
        current_object = new gml_object;
        advance(delim-pos+1);
      }
      break;
    }

    case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9': case '+': case '-': case '.':
    {
      // we are reading a number...
      int delim = next_white();
      if(delim == -1)
      {
        print_error("unterminated number");
        return;
      }
      in_chars[delim] = '\0';
      char* nval = &(in_chars[pos]);

      // check whether nval is an integer...
      char* rest;
      long l = strtol(nval,&rest,10);

      if(!strlen(rest))     // ...yes, it is
      {
        current_object->the_type = gml_int;
        current_object->the_int = int(l);
        current_object->where = 
          current_context->the_list.append(current_object);
        current_object->parent = current_context;
        current_object = new gml_object;
      }
      else                  // ...no, it is not
      {
        // check whether nval is a double...
        double d = strtod(nval,&rest);

        if(!strlen(rest))   // ...yes, it is
        {
          current_object->the_type = gml_double;
          current_object->the_double = d;
          current_object->where =
            current_context->the_list.append(current_object);
          current_object->parent = current_context;
          current_object = new gml_object;
        }
        else                // ...no, it is not
        {
          print_error("malformed number");
          return;
        }
      }

      advance(delim-pos+1);
      break;
    }

    case '[':
    {
      // we are reading a list...
      bracket_level++;
      current_object->the_type = gml_list;
      current_object->where =
        current_context->the_list.append(current_object);
      current_object->parent = current_context;

      // switch context...
      current_context = current_object;
      current_object = new gml_object;

      advance();
      break;
    }

    case '\0':
    {
      print_error("missing value");
      break;
    }

    default:
    {
      print_error("malformed value");
      break;
    }
  }
}

void gml_objecttree::advance(int n)
// advances the current position by n characters
{
  if(pos + n > in_length)
  {
    pos = in_length;
    print_error("unexpected end of file");
  }
  else pos += n;
}

void gml_objecttree::skip_white()
// skips white space.
{
  bool keep_on = true;
  do
  {
    while(isspace(current_char())) advance();
    if(current_char() == '#')  // beginning of a comment...
      while(current_char() != '\n') advance();
    else keep_on = false;
  } while(keep_on);
}

int gml_objecttree::next_white()
// returns position of next white space character in in_chars or -1
{
  int p = pos;
  while(!isspace(in_chars[p]) && (p < in_length)) p++;

  return (p < in_length) ? p : -1;
}

int gml_objecttree::next_quot()
// returns position of next " or -1
{
  int p = pos;
  while((in_chars[p] != '"') && (p < in_length)) p++;

  return (p < in_length) ? p : -1;
}

void gml_objecttree::line_col(int position, int& line, int& col)
{
  line = lines.inf(lines.locate_pred(position));
  int start = line_starts.inf(line_starts.lookup(line));
  col = position - start + 1;
}

void gml_objecttree::print_error(const char* type)
{
  int line, col;
  line_col(pos,line,col);

  string msg("%s(line %d, column %d): %s\n",get_root_key(),line,col,type);
  LEDA_EXCEPTION(0,msg.cstring());

  syntax_errors = true;
}


gml_object* gml_objecttree::first_object()
{
  current_context = &objects;
  if(current_context->the_list.size())
    current_object = current_context->the_list.head();
  else
    current_object = 0;
  return current_object;
}


gml_object* gml_objecttree::next_object()
{
  if(!current_object) return first_object();

  slist_item lit = current_context->the_list.succ(current_object->where);
  if(!lit) return 0;
  current_object = current_context->the_list.inf(lit);
  return current_object;
}

gml_object* gml_objecttree::down()
{
  if(   (current_object->the_type == gml_list)
     && (current_object->the_list.size())      )
  {
    current_context = current_object;
    current_object = current_object->the_list.head();
    return current_object;
  }
  else return 0;
}

gml_object* gml_objecttree::up()
{
  if(current_context == &objects) return 0;

  slist_item lit = 
    current_context->parent->the_list.succ(current_context->where);
  if(!lit) return 0;
  current_context = current_context->parent;
  current_object = current_context->the_list.inf(lit);
  return current_object;
}

gml_object* gml_objecttree::parent()
{
  if(current_context == &objects) return 0;

  current_object = current_context;
  current_context = current_context->parent;

  return current_object;
}

bool gml_objecttree::last_in_list()
{
  if(!current_object) return true;
  return (current_object->where == current_context->the_list.last());
}


// ------------------------ class gml_pattern ----------------------- //

gml_pattern::~gml_pattern()
{
  dic_item d;
  forall_items(d,children) 
  { gml_pattern* p =  children.inf(d);
    delete p;
   }
}



// ------------------------ class gml_parser ------------------------ //

void gml_parser::add_rule(const slist<char*>& cont,
                          gml_rule r, gml_value_type t)
{
  reset_path();
  make_context(cont);
  current_pattern->add_rule(r,t);
}

void gml_parser::make_context(const slist<char*>& cont)
{
  char* key;
  forall(key,cont) append(key);
}

void gml_parser::append(const char* key)
{
  dic_item there = current_pattern->children.lookup((gml_charp)key);
  if(there)
  {
    current_pattern = current_pattern->children.inf(there);
  }
  else
  {
    gml_pattern* pat = new gml_pattern;
    pat->the_key = key;
    current_pattern->children.insert((gml_charp)key,pat);
    pat->parent = current_pattern;
    current_pattern = pat;
  }
}


void gml_parser::goback()
{
  if(current_pattern != &patterns)
    current_pattern = current_pattern->parent;
}


bool gml_parser::parse_string(string in_str)
{
  bool result = false;
  T = new gml_objecttree;
  T->read_string(in_str);
  if(!T->errors())
  { reset_path();
    init_rules();
    result = parse();
    cleanup();
  }
  else parse_error = true;
  delete T;
  return result;
}



bool gml_parser::parse(const char* filename)
{
  bool result = false;
  T = new gml_objecttree;
  T->read(filename);
  if(!T->errors())
  { reset_path();
    init_rules();
    result = parse();
    cleanup();
  }
  else parse_error = true;
  delete T;
  return result;
}

bool gml_parser::parse(istream& ins)
{
  bool result = false;
  T = new gml_objecttree;
  T->read(ins);
  if(!T->errors())
  { reset_path();
    init_rules();
    result = parse();
    cleanup();
  }
  else parse_error = true;
  delete T;
  return result;
}


bool gml_parser::parse(gml_objecttree& obj)
{
  bool result = false;
  T = &obj;
  if(!T->errors())
  {
    reset_path();
    init_rules();
    result = parse();
    cleanup();
  }
  else parse_error = true;
  return result;
}



bool gml_parser::parse()
{
  if(!patterns.children.size()) return true;   // nothing to be done

  reset_path();
  current_object = T->first_object();
  parse_error = false;
  bool down;

  while(current_object && !parse_error)
  {
    parse_object();

    down = false;

    if(   (current_object->get_type() == gml_list)
       &&  current_object->has_children() )
    // current object is a non-empty list,
    // proceed to first child if there is some pattern for the list
    {
      if(current_pattern->children.size())
      {
        dic_item dit = 
          current_pattern->children.lookup((gml_charp)current_object->get_key());
        if(dit)
        {
          gml_pattern* below = current_pattern->children.inf(dit);
          if(below->children.size())
          {
            current_object = T->down();
            current_pattern = below;
            down = true;
          }
        }
      }
    }

    if(!down)
    {
      if(T->last_in_list()) list_end();
      else current_object = T->next_object();
    }

  }

  return !parse_error;
}

void gml_parser::parse_object()
{
  dic_item dit = current_pattern->children.lookup((gml_charp)current_object->get_key());
  if(!dit) return;
  else
  {
    gml_value_type object_type = current_object->get_type();
    if(object_type == gml_none) return;

    gml_pattern* pat = current_pattern->children.inf(dit);

    gml_rule r;
    forall(r,pat->rules[object_type])
      parse_error = parse_error || !interpret(r,current_object);
  }
}

void gml_parser::list_end()
{
  while((current_pattern != &patterns) && T->last_in_list())
  {
    current_object = T->parent();

    gml_rule r;
    forall(r,current_pattern->rules[gml_list])
      parse_error = parse_error || !list_end(r,current_object);

    current_pattern = current_pattern->parent;
  }

  current_object = T->next_object();
}


void gml_parser::print_error(const gml_object& gobj, const char* type)
{
  int line, col;
  T->line_col(gobj,line,col);

  string msg("%s(line %d, column %d): %s\n",T->get_root_key(),line,col,type);
  LEDA_EXCEPTION(0,msg.cstring());

  parse_error = true;
}

LEDA_END_NAMESPACE
