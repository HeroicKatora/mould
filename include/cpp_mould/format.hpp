#ifndef CPP_MOULD_FORMAT_HPP
#define CPP_MOULD_FORMAT_HPP
#include <string_view>

#include "bytecode.hpp"

namespace mould {
  // Interface definition of a format. The values are already recovered from
  // immediates or the input arguments and the kind is resolved.
  struct Format {
    using Number = internal::Immediate;
    using Alignment = internal::Alignment;
    using Sign = internal::Sign;

    Number width /* The width specified in the format */;
    Number precision /* The precision specified in the format */;
    Number padding /* The charcode of the character specified as padding */;

    bool has_width /* If no width was given, this is false and `width` is 0 */;
    bool has_precision /* If no precision was given, this is false and `precision` is 0 */;
    bool has_padding /* If padding was specified, `padding` is 0 otherwise */;
    
    Alignment alignment;
    Sign sign;
  };

  struct FormatterInformation {
    
  };

  // Forward declaration to the type defined in "engine.hpp"
  namespace internal {
    class Engine;
  }

  // Passed to all arguments to encode their representation. The actual
  // implementation is found in "engine.hpp"
  class Formatter {
  public:
    void append(char) const;
    void append(std::string) const;
    void append(const char*) const;
    void append(std::string_view) const;

    char* show_buf(size_t req);
    void put_buf(size_t req);

    inline const Format& format() const {
      return _format;
    }

    constexpr Formatter(internal::Engine& engine, Format format)
      : engine(engine), _format(format)
      { }
  private:
    friend class internal::Engine;
    internal::Engine& engine;
    Format _format;
  };
}

#endif
