#include "OS_Exit_Info.h"

namespace slib{
  OS_Exit_Info::OS_Exit_Info()
  {
  }


  OS_Exit_Info::~OS_Exit_Info()
  {
  }



  int OS_Exit_Info::at_exit_i(void *object, CLEANUP_FUNC cleanup_hook, void *param, const char* name){
    //TODO:new ACE_Cleanup_Info_Node
    //add to the front of registered_objects_
    all.push_back(Cleanup_Info_Node(object, cleanup_hook, param, name));
    return 0;
  }


  bool OS_Exit_Info::find(void *object){
    for (Cleanup_Info_Node n : all){
      if (n.object == object) return true;
    }
    return false;
  }


  bool OS_Exit_Info::remove(void *object){
    auto end = all.end();
    for (auto i = all.begin(); i != end; ++i){
      if (i->object == object){
        all.erase(i);
        return true;
      }
    }
    return false;
  }

  void OS_Exit_Info::call_hooks(){
    size_t i = all.size();
    while (i){
      i--;
      Cleanup_Info_Node& n = all[i];
      n.cleanup_hook(n.object, n.param);
    }
  }
}