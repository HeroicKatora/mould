#ifndef CPP_MOULD_ENGINE_HPP
#define CPP_MOULD_ENGINE_HPP
#include <memory>

#include "argument.hpp"
#include "format_info.hpp"

namespace mould::internal {
  // A formatter into some output.
  class Engine {
  public:
    // All internally compiled items implement the abstract class of
    // their char type.
    Engine(
        // Let's see how much we can do with this and what behaviour we can abstract
        std::string& outbuffer)
      : output(outbuffer)
        { }

    friend class ::mould::Formatter;
    inline void append(const char* begin, const char* end) const {
      output.append(begin, end);
    }
  private:
    std::string& output;
  };

  template<typename T>
  Immediate value_as_immediate(const T&);
}

namespace mould {
  inline void Formatter::append(char arg) const {
    engine.output.push_back(arg);
  }

  inline void Formatter::append(std::string arg) const {
    engine.output.append(arg);
  }

  inline void Formatter::append(const char* arg) const {
    engine.output.append(arg);
  }

  inline void Formatter::append(std::string_view sv) const {
    engine.output.append(sv);
  }

  inline char* Formatter::reserve(size_t length) const {
    const auto start = engine.output.size();
    engine.output.resize(engine.output.size() + length, ' ');
    return engine.output.data() + start;
  }
}

#endif
