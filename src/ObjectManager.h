#pragma once
#include "OS_Exit_Info.h"
namespace slib{
  class ICleanup;
  class ObjectManager
  {
  private:
    static ObjectManager instance_;
  public:  
    ~ObjectManager();


    static ObjectManager* instance() {
      return &instance_;
    }
    /**
    * Register an ACE_Cleanup object for cleanup at process
    * termination.  The object is deleted via the
    * <ace_cleanup_destroyer>.  If you need more flexiblity, see the
    * @c other at_exit method below.  For OS's that do not have
    * processes, cleanup takes place at the end of <main>.  Returns 0
    * on success.  On failure, returns -1 and sets errno to: EAGAIN if
    * shutting down, ENOMEM if insufficient virtual memory, or EEXIST
    * if the object (or array) had already been registered.
    */
    static int at_exit(ICleanup *object, void *param = 0, const char* name = 0);
    static int remove_at_exit(void *object);
    /// Remove an object for deletion at program termination.
    /// See description of static version above for return values.
    int remove_at_exit_i(void *object);
  private:
    ObjectManager();
    int at_exit_i(void *object, CLEANUP_FUNC cleanup_hook, void *param, const char* name);
    OS_Exit_Info exit_info_;
  };

}
