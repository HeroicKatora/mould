#ifndef CPP_MOULD_ENGINE_HPP
#define CPP_MOULD_ENGINE_HPP
#include <algorithm>
#include <memory>
#include <ostream>
#include <span>

#include "argument.hpp"
#include "format_info.hpp"

namespace mould::internal {
  // A character interface into some output. Do not worry, these virtual class
  // usually get resolved and aren't the main overhead.
  class Engine {
  public:
    virtual ~Engine() {}

    virtual void append(const char* begin, const char* end) = 0;
    virtual void append(char) = 0;

    virtual char* show_buf(size_t) = 0;
    virtual void put_buf(size_t) = 0;
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
    inline char* show_buf(size_t) override {
      return nullptr;
    }
    inline void put_buf(size_t) override {}
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
      const size_t len = end - begin;
      if (len <= end - free) {
        switch (len) {
        case 2: *free++ = *begin++;
        case 1: *free++ = *begin++;
        case 0: break;
        default:
          std::copy_n(begin, len, free);
          free = free + len;
        }
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

    inline char* show_buf(size_t len) override {
      return len <= end - free ? free : nullptr;
    }

    inline void put_buf(size_t len) override {
      free = free + len;
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

  inline char* Formatter::show_buf(size_t req) {
    return engine.show_buf(req);
  }

  inline void Formatter::put_buf(size_t req) {
    return engine.put_buf(req);
  }
}

#endif
