#ifndef CPP_MOULD_GENERATE_HPP
#define CPP_MOULD_GENERATE_HPP
/* Constructor helpers for iterating format, generating bytecode */
#include <cstddef>

#include "bytecode.hpp"
#include "coding.hpp"

namespace mould {
  template<typename CharT>
  struct Buffer {
    const CharT* begin;
    const CharT* end;

    constexpr bool empty() const { return begin == end; }
  };

  template<typename CharT>
  struct StringLiteral {
    EncodedStringLiteral literal;
    constexpr bool empty() const { return literal.length == 0; }
  };

  template<typename CharT>
  struct FormatSpecifier {
    Buffer<CharT> buffer;
    bool complete;
  };

  template<typename CharT>
  constexpr auto get_string_literal(Buffer<CharT>& buffer)
  -> StringLiteral<CharT> {
    const auto begin = buffer.begin;
    while(buffer.begin < buffer.end && *buffer.begin != '{') buffer.begin++;
    return { { begin, buffer.begin } };
  }

  template<typename CharT>
  constexpr auto get_format_specifier(Buffer<CharT>& buffer)
  -> const FormatSpecifier<CharT> {
    const auto begin = buffer.begin;

    for(;;buffer.begin++) {
      if(buffer.begin == buffer.end) {
        return { { begin, buffer.begin }, false };
      }
      if(*buffer.begin == '}') {
        buffer.begin++;
        break;
      }
    }

    const Buffer<CharT> format_buffer = { begin, buffer.begin };

    return { format_buffer, true };
  }

}

#endif
