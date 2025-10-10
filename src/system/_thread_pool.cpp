/*******************************************************************************
+
+  LEDA 7.2  
+
+
+  _thread_pool.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/core/string.h>
#include <LEDA/system/thread_pool.h>

using namespace leda;


#if defined(LEDA_MULTI_THREAD)

thread_pool::thread_pool(unsigned max_threads, bool use_queue_) :
    max_workers(max_threads), use_queue(use_queue_) {
  if (max_threads < 1)
    LEDA_EXCEPTION(1, string("illegal maximum number of threads: %u\n",
                             max_threads));
  waiting = false;
  cancelling = false;
  cur_workers = 0;
  idle_workers = 0;
}

void thread_pool::create_worker() {
  thread_t worker = thread_create(worker_thread, this);
  workers.append(worker);
  cur_workers++;
}

// void thread_pool::enqueue(void *(*func)(void *), void *arg) {
//  job_s *job = new job_s(func, arg);

void thread_pool::enqueue(job *job) {
  mutex.lock();
  
  if (use_queue) {
    job_queue.append(job);
  } else {
    job_queue.push(job);
  }
  
  if (idle_workers > 0) {
    workcv.signal();
  } else if (cur_workers < max_workers) {
    create_worker();
  }
  mutex.unlock();
}

void thread_pool::wait() {
  mutex.lock();
  while (!job_queue.empty()) {
    waiting = true;
    waitcv.wait(mutex);
  }
  mutex.unlock();
}

thread_pool::~thread_pool() {
  mutex.lock();
  
  /* mark pool as being cancelled, wakeup idle workers */
  cancelling = true;
  
  /* the workers will check pool->cancelling and terminate */
  workcv.broadcast();
  
  /* wait for all active workers to finish */
  while (!active.empty()) {
    waiting = true;
    waitcv.wait(mutex);
  }
  
  /* the last worker to terminate will wake us up */
  while (cur_workers != 0)
    busycv.wait(mutex);
  
  mutex.unlock();
  
  while (!workers.empty())
    thread_join(workers.pop());
  
  /* There should be no pending jobs, but just in case... */
  while (!job_queue.empty())
    delete job_queue.pop();
}

void thread_pool::worker_cleanup() {
  cur_workers--;
  if (cancelling) {
    if (cur_workers == 0)
      busycv.broadcast();
  }
  
  mutex.unlock();
}

void thread_pool::notify_waiters() {
  if (job_queue.empty() && active.empty()) {
    waiting = false;
    waitcv.broadcast();
  }
}

void thread_pool::job_cleanup() {
  slist<thread_id_t>::item it, itnext;

  thread_id_t my_tid = thread_self();
  
  mutex.lock();
  
  /*
   * remove the item containing my thread-id from the active-list
   */
  /* linear search, could be made faster by remembering the inserted list-item
   * and removing it; but this implies a doubly linked list (no inline) and the
   * overhead is marginal for sensible values of max_workers */
  it = active.first();
  if (active[it] == my_tid) {
    active.pop();
  } else {
    while ((itnext = active.succ(it)) != NULL) {
      if (active[itnext] == my_tid) {
        active.del_succ_item(it);
        break;
      }
      it = itnext;
    }
  }
  
  /* maybe someone is wait()-ing, check whether we are the last worker and -
   * if so - notify him / them */
  if (waiting)
    notify_waiters();
}


void * thread_pool::worker_thread(void *arg) {
  thread_pool *pool = (thread_pool *)arg;
  job *job;
  
  /*
   * This is the worker's main loop. It will only be left if a
   * timeout occurs or if the pool is being destroyed.
   */
  pool->mutex.lock();
  unsigned long my_tid = thread_self();

  while (true) {
    pool->idle_workers++;
    if (pool->waiting)
      pool->notify_waiters();
    
    while (pool->job_queue.empty() && !pool->cancelling)
      pool->workcv.wait(pool->mutex);
    
    pool->idle_workers--;
    if (pool->cancelling)
      break;
    
    if (pool->job_queue.empty())
      continue;
    
    job = pool->job_queue.pop();
    
    pool->active.push(my_tid);
    pool->mutex.unlock();
    
    /* Call the specified job function */
    (*job)();
    delete job;
    
    pool->job_cleanup();
  }
  
  pool->worker_cleanup();
  return NULL;
}

#else

// dummy functions

thread_pool::thread_pool(unsigned max_threads, bool use_queue_) : max_workers(0) {}
thread_pool::~thread_pool() { }

void  thread_pool::create_worker() { }
void  thread_pool::enqueue(job *job) {}
void  thread_pool::wait() { }
void  thread_pool::worker_cleanup() { }
void  thread_pool::notify_waiters() { }
void  thread_pool::job_cleanup() {}
void* thread_pool::worker_thread(void *arg) { return 0; }

#endif
