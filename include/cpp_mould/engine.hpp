#ifndef CPP_MOULD_ENGINE_HPP
#define CPP_MOULD_ENGINE_HPP

#include "bytecode.hpp"

namespace mould {
  // Interface definition of a format. The values are already recovered from
  // immediates or the input arguments and the kind is resolved.
  struct Format {
    internal::Immediate width;
    internal::Immediate precision;
    internal::Immediate padding;
    internal::Alignment alignment;
    internal::Sign sign;
  };

  // A formatter into some output.
  class Engine {
  public:
    // All internally compiled items implement the abstract class of
    // their char type.
    Engine(const internal::TypeErasedByteCode<char>& codebuffer);
  private:
  };
}

#endif
