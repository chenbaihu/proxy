#include "cleanup.h"
namespace slib{
void
ICleanup::cleanup (void *)
{
  delete this;
}

ICleanup::~ICleanup (void)
{
}

void cleanup_destroyer(ICleanup * object, void *param){
  object->cleanup (param);
}

}