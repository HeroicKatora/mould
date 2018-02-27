#ifndef CPP_MOULD_GENERATE_HPP
#define CPP_MOULD_GENERATE_HPP
/* Constructor helpers for iterating format, generating bytecode.
 * Not necessarily constexpr, compared to `compile.hpp`.
 */
#include <cstddef>

#include "bytecode.hpp"
#include "coding.hpp"

namespace mould {
  template<typename CharT>
  struct StringLiteral {
    EncodedOperation operation;

    EncodedStringLiteral literal;

    constexpr bool empty() const { return literal.length == 0; }
  };

  template<typename CharT>
  struct FormatSpecifier {
    Buffer<CharT> buffer;

    EncodedOperation operation;

    Codepoint index;
    bool index_auto;

    EncodedFormat format;
    bool format_auto;

    Immediate immediates[3]; // width, precision, padding
    unsigned char used_immediates;

    constexpr FormatSpecifier()
      : buffer{nullptr, nullptr}, operation{}, index{}, index_auto{},
        format{}, format_auto{}, immediates{}, used_immediates{}
      { }

    constexpr FormatSpecifier(
      Buffer<CharT> buffer, EncodedOperation operation, Codepoint index,
      bool index_auto, EncodedFormat format, bool format_auto,
      const Immediate (&_immediates)[3], unsigned char used_immediates)
      : buffer{buffer}, operation{operation}, index{index},
        index_auto{index_auto}, format{format}, format_auto{format_auto},
        immediates{}, used_immediates{used_immediates}
      {
        for(int i = 0; i < 3; i++) {
          immediates[i] = _immediates[i];
        }
      }
  };

  template<typename CharT>
  constexpr bool get_string_literal(
    Buffer<CharT>& buffer,
    StringLiteral<CharT>& literal)
  {
    const auto begin = buffer.begin;
    while(buffer.begin < buffer.end && *buffer.begin != '{') buffer.begin++;
    literal = StringLiteral<CharT> {
      { Operation::Literal() },
      { begin, buffer.begin }
    };
    return true;
  }

  template<typename CharT>
  constexpr bool get_format_specifier(
    Buffer<CharT>& buffer,
    FormatSpecifier<CharT>& format)
  {
    const auto begin = buffer.begin;

    for(;; buffer.begin++) {
      if(buffer.begin == buffer.end) {
        return false;
      }
      if(*buffer.begin == '}') {
        buffer.begin++;
        break;
      }
    }

    const Buffer<CharT> format_buffer = { begin, buffer.begin };
    Immediate immediate_buffer[3] = {};

    format = FormatSpecifier<CharT> {
      format_buffer,
      { Operation::Insert(CodeValue::Auto, ImmediateValue::Auto) },
      0, true,
      { Formatting{} }, true,
      immediate_buffer, 0
    };
    return true;
  }

  template<typename CharT>
  constexpr bool operator<<(
    ByteCodeOutputBuffer& output,
    const StringLiteral<CharT>& literal)
  {
    return (output << literal.operation);
  }

  template<typename CharT>
  constexpr bool operator<<(
    ImmediateOutputBuffer& output,
    const StringLiteral<CharT>& literal)
  {
    return (output << literal.literal);
  }

  template<typename CharT>
  constexpr bool operator<<(
    ByteCodeOutputBuffer& output,
    const FormatSpecifier<CharT>& format)
  {
    return (output << format.operation)
      && (format.index_auto || (output << format.index));
  }

  template<typename CharT>
  constexpr bool operator<<(
    ImmediateOutputBuffer& output,
    const FormatSpecifier<CharT>& format)
  {
    if(!(output << format.format))
      return false;
    for(auto i = 0; i < format.used_immediates; i++) {
      if(!(output << format.immediates[i]))
        return false;
    }
    return true;
  }
}

#endif
