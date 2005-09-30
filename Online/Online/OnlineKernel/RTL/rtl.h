#ifndef _RTL_H
#define _RTL_H

struct EXHDEF   {
  int   flink;
  int  (*exit_handler)(void*);
  void *exit_param;
  int  *exit_status;
};

#define byte_offset(a,b,c) {a *ptr=0; c = (int)&(ptr->b);}

template <class T, class Q> static inline T* add_ptr(T* a, Q b)  {
  return (T*)((void*)((int)(a)+(int)(b)));
}

extern "C" {
  struct EXHDEF;
  typedef void* lib_rtl_handle_t;
#ifndef _RTL_INTERNAL_H
  typedef void* lib_rtl_thread_t;
  typedef void* lib_rtl_lock_t;
  typedef void* lib_rtl_event_t;
  struct lib_rtl_gbl  {
    void*             address;
    void*             internals;
  };
#endif
  typedef lib_rtl_gbl* lib_rtl_gbl_t;
  typedef int (*RTL_ast_t)(void*);
  typedef int (*lib_rtl_rundown_handler_t)(void*);
  inline bool lib_rtl_is_success(int status) { return status&1; }

  /// Create named lock. if the lock_name is NULL the lock is priovate to the currect process.
  int lib_rtl_create_lock (const char* lock_name, lib_rtl_lock_t* lock_handle);
  /// Delete lock
  int lib_rtl_delete_lock (lib_rtl_lock_t lock_handle);
  /// Cancle lock
  int lib_rtl_cancel_lock (lib_rtl_lock_t lock_handle);
  /// Aquire lock
  int lib_rtl_lock        (lib_rtl_lock_t lock_handle);
  /// Release lock
  int lib_rtl_unlock      (lib_rtl_lock_t lock_handle);

  /// Create named global section
  int lib_rtl_create_section(const char* sec_name, int size, lib_rtl_gbl_t* handle);
  /// Map global section a a specific address
  int lib_rtl_map_section(const char* sec_name, int size, lib_rtl_gbl_t* address);
  /// Delete named global section
  int lib_rtl_delete_section(lib_rtl_gbl_t handle);
  /// Unmap global section: address is quadword: void*[2]
  int lib_rtl_unmap_section(lib_rtl_gbl_t handle);
  /// Flush global section to disk file
  int lib_rtl_flush_section(lib_rtl_gbl_t handle);

  int lib_rtl_set_timer(int milli_seconds, int (*ast)(void*), void* ast_param, unsigned int* timer_id);
  int lib_rtl_kill_timer(int timer_id);
  typedef long (*lib_rtl_thread_routine_t)(void*);
  int lib_rtl_start_thread(lib_rtl_thread_routine_t exec, void* thread_arg, lib_rtl_thread_t* handle);
  int lib_rtl_delete_thread(lib_rtl_thread_t handle);
  int lib_rtl_suspend_thread(lib_rtl_thread_t handle);
  int lib_rtl_resume_thread(lib_rtl_thread_t handle);

  int lib_rtl_pid();
  int lib_rtl_ffc (int* start, int* len, const void* base, int* position);
  int lib_rtl_ffs (int* start, int* len, const void* base, int* position);
  int lib_rtl_run_ast (RTL_ast_t astadd, void* bm, int);
  /// Create named event for local process (name==0: event is unnamed)
  int lib_rtl_create_event (const char* name, lib_rtl_event_t* event_flag);
  /// Delete event
  int lib_rtl_delete_event(lib_rtl_event_t event_flag);
  /// Set local event flag
  int lib_rtl_set_event(lib_rtl_event_t event_flag);
  /// Set global event flag
  int lib_rtl_set_global_event(const char* name);
  /// Clear event flag
  int lib_rtl_clear_event(lib_rtl_event_t event_flag);
  /// Wait for event flag
  int lib_rtl_wait_for_event(lib_rtl_event_t event_flag);
  /// Wait asynchronously for an event flag
  int lib_rtl_wait_for_event_a(lib_rtl_event_t flag, lib_rtl_thread_routine_t action, void* param);

  int lib_rtl_signal_message(int, const char* fmt, ...);
  enum {
    LIB_RTL_DEFAULT = 0,
    LIB_RTL_ERRNO = 1,
    LIB_RTL_OS
  };
  /// Declare exit handler
  int lib_rtl_declare_exit(EXHDEF* handler_block);
  int lib_rtl_remove_exit(EXHDEF* handler_block);
  int lib_rtl_remove_rundown( int (*hdlr)(void*) ,void* param);
  int lib_rtl_declare_rundown( int (*hdlr)(void*) ,void* param);
  int lib_rtl_start_debugger();
  int lib_rtl_sleep(int millisecs);
  const char* lib_rtl_error_message(int status);
};
#endif // _RTL_H
