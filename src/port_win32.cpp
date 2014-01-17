// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "port_win32.h"

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <windows.h>

namespace slib {
  namespace port {

    static void PthreadCall(const char* label, int result) {
      if (result != 0) {
        fprintf(stderr, "pthread %s: %s\n", label, strerror(result));
        abort();
      }
    }

    Mutex::Mutex() {
      InitializeCriticalSection(&cs);
      //throw std::runtime_error("init mutex fail");
    }

    Mutex::~Mutex() { DeleteCriticalSection(&cs); }

    void Mutex::Lock() { EnterCriticalSection(&cs); }

    void Mutex::Unlock() { LeaveCriticalSection(&cs); }

    CondVar::CondVar(Mutex* mu) : mu_(mu) { InitializeConditionVariable(&cond); }

    CondVar::~CondVar() {  //need do nothing
    }

    void CondVar::Wait() {
      SleepConditionVariableCS(&cond, &this->mu_->cs, INFINITE);
    }

    void CondVar::Signal() { WakeConditionVariable(&cond); }

    void CondVar::SignalAll() { WakeAllConditionVariable(&cond); }

  }  // namespace port
}  // namespace leveldb