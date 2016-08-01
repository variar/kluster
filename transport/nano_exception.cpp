#include "nano_exception.h"

#include <nn.h>

namespace kluster
{
  namespace transport
  {
    NanoException::NanoException() : err{nn_errno()} {}

    const char* NanoException::what() const throw()
    {
        return nn_strerror(err);
    }
  }
}
