#pragma once

#include <exception>

namespace kluster
{
  namespace transport
  {
    class NanoException : public std::exception
        {
        public:
            NanoException();
            virtual const char *what() const throw ();
            int nano_errno() const {return err;}
        private:
            int err;
        };

  }
}
