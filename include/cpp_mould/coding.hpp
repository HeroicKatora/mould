#ifndef CPP_MOULD_CODING_HPP
#define CPP_MOULD_CODING_HPP
/* Interpretable values and their encoding */
#include "bytecode.hpp"

namespace mould {
  static_assert(sizeof(size_t) == sizeof(void*));
  static_assert(sizeof(size_t) == sizeof(const char*));

  template<typename Symbol>
  constexpr bool operator<<(Buffer<Symbol>& buffer, Symbol imm) {
    if(buffer.begin == buffer.end) return false;
    *buffer.begin++ = imm;
    return true;
  }

  template<typename Symbol>
  constexpr bool operator>>(Buffer<const Symbol>& buffer, Symbol& imm) {
    if(buffer.begin == buffer.end) return false;
    imm = *buffer.begin++;
    return true;
  }

  struct EncodedStringLiteral {
    Immediate begin;
    Immediate length;

    EncodedStringLiteral()
      : begin(), length()
      {}

    template<typename CharT = const char>
    constexpr EncodedStringLiteral(CharT* begin, CharT* end)
      : begin((Immediate) begin), length(end - begin)
      { }

    template<typename CharT = const char>
    constexpr CharT* begin_ptr() const { return (CharT*) begin; }

    template<typename CharT = const char>
    constexpr CharT* end_ptr() const { return (CharT*) length; }

    friend constexpr bool operator<<(Buffer<Immediate>& buffer, EncodedStringLiteral literal) {
      return (buffer << literal.begin) && (buffer << literal.length);
    }

    friend constexpr bool operator>>(Buffer<const Immediate>& buffer, EncodedStringLiteral& literal) {
      Immediate begin = 0, length = 0;
      if((buffer >> begin) && (buffer >> length)) {
        literal.begin = begin;
        literal.length = length;
        return true;
      }
      return false;
    }
  };

  struct EncodedOpcode {
    Codepoint opcode;
    Codepoint index;
    Immediate formatting[8];

    unsigned char formatting_count;
  };
}

#endif
