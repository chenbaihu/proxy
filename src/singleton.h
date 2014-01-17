#pragma once

#include "cleanup.h"
#include "ObjectManager.h"

namespace slib{
  template <class TYPE, class LOCK>
  class Singleton : public ICleanup
  {
  public:
    /// Global access point to the Singleton.
    static TYPE *instance(void){
      auto tmp = singleton_;
      //asm ("mb"); 
      if (tmp == nullptr){
        //lock guider
        tmp = singleton_;
        if (tmp == nullptr){
          tmp = new Singleton<TYPE, LOCK>();
          //asm ("mb"); 
          singleton_ = tmp;          
          ObjectManager::at_exit(singleton_);
        }
      }
      return &singleton_->instance_;
    }

    /// Cleanup method, used by cleanup_destroyer to destroy this Singleton.
    virtual void cleanup(void *param = 0){
      ObjectManager::remove_at_exit(this);
      delete this;
      singleton_ = nullptr;
    }
   
  protected:
    /// Default constructor.
    Singleton(void){}
    virtual ~Singleton() throw() {};

    /// Contained instance.
    TYPE instance_;

    LOCK mutex;

    /// Pointer to the Singleton (ACE_Cleanup) instance.
    static Singleton<TYPE, LOCK> *singleton_;

  }; template <class TYPE, class LOCK> Singleton<TYPE, LOCK> *
    Singleton<TYPE, LOCK>::singleton_ = NULL;
}

