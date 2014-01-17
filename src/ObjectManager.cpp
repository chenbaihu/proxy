#include <cerrno>

#include "ObjectManager.h"
#include "cleanup.h"

namespace slib {
ObjectManager ObjectManager::instance_;

ObjectManager::ObjectManager()
{
}


ObjectManager::~ObjectManager()
{
  exit_info_.call_hooks();
}


int ObjectManager::at_exit_i(void *object, CLEANUP_FUNC cleanup_hook, void *param, const char* name){
  if (exit_info_.find(object))  {
    // The object has already been registered.
    errno = EEXIST;
    return -1;
  }
  return exit_info_.at_exit_i(object, cleanup_hook, param, name);
}
int ObjectManager::at_exit(ICleanup *object, void *param , const char* name ){
  return ObjectManager::instance()->at_exit_i(
    object,
    (CLEANUP_FUNC)cleanup_destroyer,
    param,
    name);
}

int ObjectManager::remove_at_exit_i(void *object){
  return exit_info_.remove(object);
}
int ObjectManager::remove_at_exit(void *object){
  return ObjectManager::instance()->remove_at_exit_i(object);
}

}
