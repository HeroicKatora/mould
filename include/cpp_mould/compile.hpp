#ifndef CPP_MOULD_COMPILE_HPP
#define CPP_MOULD_COMPILE_HPP
/* The compilation process transforms a format string into byte code.
 */
#include <iterator>

#include "bytecode.hpp"
#include "generate.hpp"

namespace mould {
  template<size_t N, typename CharT = const char>
  constexpr size_t bytecode_count(CharT (&format_str)[N]) {
    return N;
  }

  template<size_t N, typename CharT = const char>
  constexpr size_t immediate_count(CharT (&format_str)[N]) {
    return 4*N;
  }

  template<size_t N, typename CharT = const char>
  auto char_type(CharT (&format_str)[N]) -> CharT;

  template<size_t N, typename CharT = const char>
  constexpr size_t buffer_size(CharT (&format_str)[N]) {
    return N;
  }

  template<size_t N, typename CharT = const char>
  constexpr Buffer<CharT> format_buffer(CharT (&format_str)[N]) {
    return { std::begin(format_str), std::end(format_str) - 1 /* \0 term */ };
  }

  template<auto& format_str>
  constexpr size_t ByteOpCount = bytecode_count(format_str);

  template<auto& format_str>
  constexpr size_t ImmediateCount = bytecode_count(format_str);

  template<auto& format_str>
  using CharType = decltype(char_type(format_str));

  template<size_t OP_COUNT, size_t IM_COUNT, typename _CharT>
  struct ByteCode {
    using CharT = _CharT;
    Codepoint code[OP_COUNT];
    Immediate immediates[IM_COUNT];

    bool error;
    constexpr ByteCode() : code(), immediates(), error(false)
      {}
  };

  template<auto& format_str>
  constexpr auto compile()
  -> ByteCode<ByteOpCount<format_str>, ImmediateCount<format_str>, CharType<format_str>> {
    ByteCode<ByteOpCount<format_str>, ImmediateCount<format_str>, CharType<format_str>> bytecode;

    Buffer<Codepoint> op_output = { bytecode.code };
    Buffer<Immediate> im_output = { bytecode.immediates };
    auto remaining = format_buffer(format_str);

    while(!remaining.empty()) {
      /* Parse the next literal */
      const auto literal = get_string_literal(remaining);
      if(!literal.empty()) {
        /* Write the string write bytecode */
        op_output << (Codepoint) OpCode::Literal;
        im_output << literal.literal;
      }

      if(remaining.empty()) break;
      const auto format_spec = get_format_specifier(remaining);

      if(!format_spec.complete) {
        bytecode.error = true;
        break;
      }
    }

    op_output << (Codepoint) OpCode::Stop;
    return bytecode;
  }
}

#endif