#pragma once

class IRunnable{
public:
  /// Run by a daemon thread to handle deferred processing.
  virtual unsigned svc(void) = 0;
  virtual ~IRunnable(){}
};
