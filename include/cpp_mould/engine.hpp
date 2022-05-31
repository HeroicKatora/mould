#ifndef CPP_MOULD_ENGINE_HPP
#define CPP_MOULD_ENGINE_HPP
#include <memory>
#include <ostream>
#include <span>

#include "argument.hpp"
#include "format_info.hpp"

namespace mould::internal {
  // A formatter into some output.
  class Engine {
  public:
    virtual ~Engine() {}

    friend class ::mould::Formatter;
    virtual void append(const char* begin, const char* end) = 0;
    virtual void append(char) = 0;
  };

  class StringEngine: public Engine {
  public:
    // All internally compiled items implement the abstract class of
    // their char type.
    StringEngine(
        // Let's see how much we can do with this and what behaviour we can abstract
        std::string& outbuffer)
      : output(outbuffer)
        { }

    inline void append(const char* begin, const char* end) override {
      output.append(begin, end);
    }
    inline void append(char c) override {
      output.push_back(c);
    }
  private:
    std::string& output;
  };

  template<typename RdBuf>
  class BufferStreamEngine: public Engine {
  public:
    BufferStreamEngine(
        char* buffer, size_t buf_size,
        RdBuf* rdbuf
    ) : buffer(buffer), streambuf(rdbuf),
        free(buffer), end(buffer + buf_size)
      { }

    ~BufferStreamEngine() {
      flush();
    }

    inline void append(const char* begin, const char* end) override {
      size_t len = end - begin;
      if (len <= end - free) {
        for (;len --> 0;) *free++ = *begin++;
      } else {
        flush();
        streambuf->sputn(begin, len);
      }
    }

    inline void append(char c) override {
      if (free == end) {
        flush();
      }

      *free++ = c;
    }

    inline void flush() {
      if (free == buffer)
        return;

      streambuf->sputn(buffer, free - buffer);
      free = buffer;
    }

    inline std::string_view take() {
      size_t len = free - buffer;
      free = buffer;
      return { buffer, len };
    }
  private:
    char* buffer;
    RdBuf* streambuf;
    char* free;
    const char* end;
  };

  template<typename T>
  Immediate value_as_immediate(const T&);

  constexpr size_t constexpr_str_len(const char* arg) {
    size_t i = 0;
    for (;*arg++; i++);
    return i;
  }
}

namespace mould {
  inline void Formatter::append(char arg) const {
    engine.append(arg);
  }

  inline void Formatter::append(std::string arg) const {
    engine.append(arg.data(), arg.data() + arg.size());
  }

  inline void Formatter::append(const char* arg) const {
    engine.append(arg, arg + mould::internal::constexpr_str_len(arg));
  }

  inline void Formatter::append(std::string_view sv) const {
    engine.append(sv.data(), sv.data() + sv.size());
  }
}

#endif
