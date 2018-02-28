#ifndef CPP_MOULD_FORMAT_HPP
#define CPP_MOULD_FORMAT_HPP
#include <string_view>

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

  // Forward declaration to the type defined in "engine.hpp"
  namespace internal {
    class Engine;
  }

  // Passed to all arguments to encode their representation. The actual
  // implementation is found in "engine.hpp"
  class Formatter {
  public:
    void append(std::string) const;
    void append(const char*) const;
    void append(std::string_view) const;

    // Get a mutable buffer into which the encoded value can be written. This
    // may fail, in which case a 0-length buffer is returned.
    std::string_view reserve(size_t length) const;

    Format& format() const;
  private:
    friend class internal::Engine;
    internal::Engine& engine;
    Format _format;

    constexpr Formatter(internal::Engine& engine, Format format)
      : engine(engine), _format(format)
      { }
  };
}

#endif
