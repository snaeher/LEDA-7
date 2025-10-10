/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _part_pers.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/part_pers.h>

////////////////////////////////////////////////////////////////////////////
// Framework for partial persistent data structures
//
// Sven Thiel (1998)
//
////////////////////////////////////////////////////////////////////////////

LEDA_BEGIN_NAMESPACE

Anchor::Anchor(PPersBase& base, int num_ps, int num_is, int num_rps) 
  : my_base(base), 
    num_fields(num_ps+num_is), num_ptrs(num_ps), num_rev_ptrs(num_rps)
{
  cur_stamp = base.register_anchor(this);

  docked_node = live_node = (PPersNode*) std_memory.allocate_bytes(node_size());
  new (live_node) PPersNode(*this, cur_stamp);

  PtrIndex idx;
  for (idx = ptrs_begin(); idx < ptrs_end(); ++idx) 
    live_node->ptr(idx) = nil;


  corresp_rev_ptr_for_lp = new PtrIndex[num_ptrs];

  first_free_rp = num_rps+1;
  if (num_rps > 0) {
    rev_ptrs = (new ReversePtr[num_rps]) - 1;
    PtrIndex idx;
    for (idx = 1; idx <= num_rps; ++idx)
      free_rev_ptr(idx);
  }
  else
    rev_ptrs = ((ReversePtr*) 0) - 1;

}

void Anchor::destroy_nodes()
{ 
  PPersNode* cur_node = live_node;
  while (cur_node) {
    PPersNode* next_node = cur_node->get_pred();
    InfoIndex idx;
    for (idx = infos_begin(); idx < infos_end(); ++idx)
      if (cur_node->is_orig_ref(idx))
        clear_info(cur_node->info(idx), idx);

    std_memory.deallocate_bytes(cur_node, node_size());

    cur_node = next_node;
  }
  docked_node = live_node = nil;
}

Anchor::~Anchor()
{ 
  delete[] corresp_rev_ptr_for_lp;
  delete[] (rev_ptrs + 1);

} 

void Anchor::p_delete()
{
  PtrIndex idx;
  for (idx = ptrs_begin(); idx < ptrs_end(); ++idx)
    delete_corresp_rp_for_lp(idx);
}

void Anchor::find_node_and_dock(TimeStamp new_cur_stamp, PPersNode* hint)
{
  if (cur_stamp != new_cur_stamp) {
    // look for node with new_cur_stamp in its valid interval ...
    PPersNode* cur_node = hint;
    while (new_cur_stamp < cur_node->get_stamp()) {
      cur_node = cur_node->get_pred();
    }
    // ... and dock to it
    dock_to_node(cur_node, new_cur_stamp);
  }
}

PtrIndex Anchor::allocate_rev_ptr()
{ 
  if (first_free_rp > num_rev_ptrs) {
    // no free ReversePtrs left, so double their number

    // copy the old rev_ptrs
    ReversePtr* old_rev_ptrs = rev_ptrs;
    rev_ptrs = (new ReversePtr[2*num_rev_ptrs]) - 1;
    PtrIndex idx;
    for (idx = 1; idx <= num_rev_ptrs; ++idx)
      rev_ptrs[idx] = old_rev_ptrs[idx];
    delete[] (old_rev_ptrs + 1);

    // mark the remaining ones as free
    num_rev_ptrs *= 2; first_free_rp = num_rev_ptrs + 1;
    for (; idx <= num_rev_ptrs; ++idx)
      free_rev_ptr(idx);
  }

  // return index of first free ReversePtr and update first_free_rp
  PtrIndex free_rp = first_free_rp;
  first_free_rp = - rev_ptrs[first_free_rp].lp_idx;
  return free_rp;
}

void Anchor::delete_corresp_rp_for_lp(PtrIndex lp_idx)
{
  const PPersNode* pointee = live_node->ptr(lp_idx);
  if (pointee) {
    PtrIndex rp_idx = corresp_rev_ptr_for_lp[lp_idx];
    pointee->get_anchor().free_rev_ptr(rp_idx);
  }
}

void Anchor::install_live_ptr(PtrIndex lp_idx, Anchor* target)
{
  if (target) {
    // set pointer in live_node
    live_node->ptr(lp_idx) = target->live_node;

    // and the ReversePtr in the new target
    PtrIndex rp_idx = target->allocate_rev_ptr();
    target->rev_ptrs[rp_idx] = ReversePtr(this, lp_idx); 
    corresp_rev_ptr_for_lp[lp_idx] = rp_idx;
  }
  else
    live_node->ptr(lp_idx) = nil;
}

void Anchor::copy_live_node()
{
  PPersNode* clone = (PPersNode*) std_memory.allocate_bytes(node_size());
  new (clone) PPersNode(live_node, cur_stamp);

  // copy pointer fields
  FieldIndex idx;
  for (idx = ptrs_begin(); idx < ptrs_end(); ++idx) {
    PPersNode* pointee = clone->ptr(idx) = live_node->ptr(idx);
    if (pointee && pointee->get_stamp() == cur_stamp) 
    {
      live_node->ptr(idx) = pointee->get_pred();
    }
  }

  // copy information fields (only the GenPtrs)
  for (idx = infos_begin(); idx < infos_end(); ++idx)
    clone->info(idx) = live_node->info(idx);

  // now dock to new live node
  docked_node = live_node = clone;

  if (rand_int(0, bridge_build_parameter*num_rev_ptrs-1) == 0)
    {
      PtrIndex rev_idx;
      for (rev_idx = 1; rev_idx <= num_rev_ptrs; ++rev_idx)
        if (!rev_ptrs[rev_idx].is_unused()) {
          Anchor& pointee_anchor = *rev_ptrs[rev_idx].target;
          
          // copy live node of pointee's anchor if necessary
          if (pointee_anchor.live_node->get_stamp() != cur_stamp) {
            pointee_anchor.dock_to_live_node(cur_stamp);
            pointee_anchor.copy_live_node();
          }

          // lift the live pointer pointing to this anchor's family
          pointee_anchor.live_node->ptr(rev_ptrs[rev_idx].lp_idx) = live_node;
        }
    }

  else
    {
      PtrIndex rev_idx;
      for (rev_idx = 1; rev_idx <= num_rev_ptrs; ++rev_idx)
        if (!rev_ptrs[rev_idx].is_unused()) {
          Anchor& pointee_anchor = *rev_ptrs[rev_idx].target;
          pointee_anchor.live_node->ptr(rev_ptrs[rev_idx].lp_idx) = live_node;
        }
    }

}

void Anchor::assign_live_ptr(PtrIndex idx, Anchor* new_val)
{
  if (live_node->get_stamp() != cur_stamp)
    copy_live_node();
  move_live_ptr_to(new_val, idx);
}

void Anchor::assign_live_info(InfoIndex idx, GenPtr new_val)
{
  if (live_node->get_stamp() != cur_stamp) {
    copy_live_node();
  }
  else {
    if (live_node->is_orig_ref(idx))
      clear_info(live_node->info(idx), idx);
  }

  live_node->info(idx) = new_val;
  live_node->mark_orig_ref(idx);
}

Anchor* Anchor::follow_ptr(PtrIndex idx) const
{
  PPersNode* pointee = docked_node->ptr(idx);
  if (!pointee) return nil;

  Anchor& pointees_anchor = pointee->get_anchor();
  pointees_anchor.find_node_and_dock(cur_stamp, pointee);

  return &pointees_anchor;
}

PPersBase::PPersBase(int num_eps) : 
  num_entry_ptrs(num_eps), last_anchor(nil)
{ 
  cur_version_rep = new PPersVersion_rep(this, 0, num_entry_ptrs);
  newest_time_stamp = 0;
}

void PPersBase::destroy_anchors() 
{ 
  Anchor* cur_anchor = last_anchor;
  while (cur_anchor) {
    Anchor* next_anchor = cur_anchor->get_pred();
    cur_anchor->destroy_nodes();
    delete cur_anchor;
    cur_anchor = next_anchor;
  }
  last_anchor = nil;
}

TimeStamp PPersBase::register_anchor(Anchor* a)
{ 
  a->set_pred(last_anchor); last_anchor = a;

  return get_current_stamp();
}

PPersVersion PPersBase::get_empty_version() const
{
  PPersVersion empty_version(cur_version_rep);
  (PPersVersion_rep*&)cur_version_rep = 0;
  return empty_version;
}

void PPersBase::begin_update(Version& v)
{
#ifndef LEDA_CHECKING_OFF
  if (v.get_rep()->get_stamp() != newest_time_stamp)
    LEDA_EXCEPTION(-1, "PPersBase: not newest version used in update");
#endif

  if (!v.is_sole_reference()) {
    v = v.create_next();
    newest_time_stamp = v.get_stamp();
  }

  cur_version_rep = v.get_rep();
}

void PPersBase::end_update()
{
  // make entry_ptrs point to live nodes
  PtrIndex idx;
  for (idx = entry_ptrs_begin(); idx < entry_ptrs_end(); ++idx) {
    PPersNode*& cur_entry_ptr = cur_version_rep->entry_ptr(idx);
    if (cur_entry_ptr) {
      Anchor& pointee_anchor = cur_entry_ptr->get_anchor();
      cur_entry_ptr = pointee_anchor.get_live_node();
    }
  }
}

Anchor* PPersBase::follow_entry_ptr(PtrIndex idx) const
{
  PPersNode* pointee = cur_version_rep->entry_ptr(idx);
  if (!pointee) return nil;
 
  Anchor& pointees_anchor = pointee->get_anchor();
  TimeStamp cur_stamp = get_current_stamp();
  if (pointees_anchor.get_current_stamp() != cur_stamp)
    pointees_anchor.dock_to_node(pointee, cur_stamp);

  return &pointees_anchor;
}

LEDA_END_NAMESPACE
