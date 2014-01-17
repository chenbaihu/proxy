#pragma once
#include <vector>

namespace slib{
  typedef void(*CLEANUP_FUNC)(void *object, void *param);
  class Cleanup_Info_Node{
  public:
    void *object;
    CLEANUP_FUNC cleanup_hook;
    void *param;
    const char* name;
    Cleanup_Info_Node(void *obj, CLEANUP_FUNC clean, void *p, const char* n) :object(obj), cleanup_hook(clean), param(p), name(n){}
  };
class OS_Exit_Info
{
public:
  OS_Exit_Info();
  ~OS_Exit_Info();

  /// Use to register a cleanup hook.
  int at_exit_i(void *object, CLEANUP_FUNC cleanup_hook, void *param, const char* name = 0);

  /// Look for a registered cleanup hook object.  Returns true if already
  /// registered, false if not.
  bool find(void *object);

  /// Remove a registered cleanup hook object.  Returns true if removed
  /// false if not.
  bool remove(void *object);

  /// Call all registered cleanup hooks, in reverse order of
  /// registration.
  void call_hooks();
private:
  std::vector<Cleanup_Info_Node> all;
};

}