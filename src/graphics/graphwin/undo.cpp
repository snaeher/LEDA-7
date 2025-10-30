/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  undo.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#ifdef LEDA_GLOBAL_UNDO
#include <LEDA/undo.h>
#else
#include "undo.h"
#endif

LEDA_BEGIN_NAMESPACE



undo_item* undo_stack::pop() {
  _size_stack.push(_size_stack.pop() - 1);
  return _s.pop();
}

void undo_stack::clear_stack(bool complete_clear, bool ignore_complex_level) {

  undo_item* it;

  if (complete_clear) {

    while (!_s.empty()) { it = _s.pop(); if (it) delete it; }
    _size_stack.clear();
    
    int i = _complex_level;
    
    _complex_level = 0;
    _size_stack.push(0);
    
    if (!ignore_complex_level) for(;i;i--) start_complex_command(); 
  }
  else {
    while (!empty()) { it = pop(); if (it) delete it; }
  }

}

int undo_stack::size() const { return _size_stack.top(); }
bool undo_stack::empty() const { return size() == 0; }

class finish_complex_command_item : public undo_item {
  undo_stack& _u;
  int _size;
  public :
  finish_complex_command_item(undo_stack& u, int size) : 
    _u(u) , _size(size) {}
  void undo() {
    _u._size_stack.push(_size);
    while (_u.size()) _u.undo();
    _u._s.pop();
    _u._size_stack.pop();
  }
};
  
void undo_stack::start_complex_command() { 
  _complex_level++;
  _size_stack.push(-1);
  push(nil); 
}

class dummy_undo_item : public undo_item { public : void undo() {} };

void undo_stack::finish_complex_command(bool ignore_empty) {

  if (_s.empty()) return;

  int s = _size_stack.pop();

  switch (s) {
    case 0  : {
      _s.pop();
      if (!ignore_empty) push(new dummy_undo_item);
      break;
    }
    case 1  : {
      undo_item *it = _s.pop();
      if (_s.top()) {
	_s.push(it);
	push(new finish_complex_command_item(*this,s));
      }
      else {
	_s.pop(); // _s.top() == start_complex_command ?
	push(it);
      }
      break;
    }
    default : {
      push(new finish_complex_command_item(*this,s));
      break;
    }
  }
  _complex_level--;
}

int undo_stack::undo_level() const { return _undo_level; }
int undo_stack::complex_level() const { return _complex_level; }

bool undo_stack::processing_undo() const { return _undo_level != 0; }

void undo_stack::push(undo_item *it) { 
  _s.push(it);  
  _size_stack.push(_size_stack.pop() + 1);
}

void undo_stack::start_undo() {
  _undo_level++;
  undo_event(*this);
}

void undo_stack::finish_undo() {
  undone_event(*this);
  _undo_level--;
}

void undo_stack::internal_undo() {
  undo_item *it = pop();
  it->undo();
  delete(it); 
}

void undo_stack::undo() {
  start_undo();
  internal_undo();
  finish_undo();
}

undo_stack::undo_stack() { 
  _undo_level = 0;
  _complex_level = 0;
  _size_stack.push(0);
}

undo_stack::~undo_stack() { 
  undo_item* it;
  while (!_s.empty()) { it = _s.pop(); if (it) delete it; }
}

// ---------------------------------------------------------------------

void undo_redo_stack::push(undo_item *It) { 
  if (processing_undo()) _redo_stack.push(It);
  else {
    if ((!processing_redo()) && (It != nil)) _redo_stack.clear();
    undo_stack::push(It);
  }
}

void undo_redo_stack::on_undo(undo_stack&) {
  _redo_stack.start_complex_command();
}

void undo_redo_stack::on_undone(undo_stack&) {
  _redo_stack.finish_complex_command();
}

void undo_redo_stack::on_redo(undo_stack&) {
  start_complex_command();
}

void undo_redo_stack::on_redone(undo_stack&) {
  finish_complex_command();
}

undo_redo_stack::undo_redo_stack() : 
  redo_event(_redo_stack.undo_event), redone_event(_redo_stack.undone_event) {
    it[0] = attach(undo_event,*this,&undo_redo_stack::on_undo);
    it[1] = attach(undone_event,*this,&undo_redo_stack::on_undone);
    it[2] = attach(redo_event,*this,&undo_redo_stack::on_redo);
    it[3] = attach(redone_event,*this,&undo_redo_stack::on_redone);
}

undo_redo_stack::~undo_redo_stack() { 
  detach(it,4);
}

void undo_redo_stack::clear_stack( bool complete_clear, 
				   bool ignore_complex_level) { 
  undo_stack::clear_stack(complete_clear,ignore_complex_level); 
  _redo_stack.clear_stack(complete_clear,ignore_complex_level); 
}
// ---------------------------------------------------------------------


LEDA_END_NAMESPACE



