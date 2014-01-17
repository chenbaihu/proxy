#pragma once

namespace slib{
class ICleanup
{
public:
  /// No-op constructor.
  ICleanup(void) {}

  /// Destructor.
  virtual ~ICleanup(void);

  /// Cleanup method that, by default, simply deletes itself.
  virtual void cleanup (void *param = nullptr);
};

void cleanup_destroyer(ICleanup *, void *param = 0);
}