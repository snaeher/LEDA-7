#ifndef UNDO_H
#define UNDO_H

#include <LEDA/core/stack.h>
#include <LEDA/system/event.h>


LEDA_BEGIN_NAMESPACE

// ---------------------------------------------------------------------

class undo_item {
  public :
  virtual void undo() = 0;
  void operator () () { undo(); }
  virtual ~undo_item() {}
};

// ---------------------------------------------------------------------

class undo_stack {

  friend class finish_complex_command_item;

  stack<undo_item*> _s;
  stack<int> _size_stack;
  int _undo_level;
  int _complex_level;

  protected :

  virtual void start_undo();
  virtual void finish_undo();
  virtual void internal_undo();

  public :

  EVENT1<undo_stack&> undo_event;
  EVENT1<undo_stack&> undone_event;

  undo_stack();
  virtual ~undo_stack();

  virtual void push(undo_item *it);
  virtual undo_item* pop();

  virtual void undo();
  void operator () () { undo(); }

  virtual void clear_stack( bool complete_clear = false, 
			    bool ignore_complex_level = false);

  virtual void clear() { clear_stack(true); }

  bool empty() const;
  int  size() const;
  int  undo_level() const;
  bool processing_undo() const;

  virtual void start_complex_command();
  virtual void finish_complex_command(bool ignore_empty = true);
  int  complex_level() const;
};

// -------------------------------------------------------

class undo_redo_stack : public undo_stack {

  undo_stack _redo_stack;

  event_item it[4];

  protected :

  virtual void on_undo(undo_stack& u);
  virtual void on_undone(undo_stack& u);

  virtual void on_redo(undo_stack& u);
  virtual void on_redone(undo_stack& u);

  public :

  EVENT1<undo_stack&>& redo_event;
  EVENT1<undo_stack&>& redone_event;

  void push(undo_item *it);

  undo_redo_stack();
  ~undo_redo_stack();

  void redo_clear() { _redo_stack.clear(); }
  bool redo_empty()      const    { return _redo_stack.empty();           }
  int  redo_size()       const    { return _redo_stack.size();            }  
  bool processing_redo() const    { return _redo_stack.processing_undo(); }
  int  redo_level()      const    { return _redo_stack.undo_level();      }
  int  redo_complex_level() const { return _redo_stack.complex_level();   }

  void clear_stack( bool complete_clear, bool ignore_complex_level);

  virtual void redo() { _redo_stack.undo(); }

};

LEDA_END_NAMESPACE


#endif


