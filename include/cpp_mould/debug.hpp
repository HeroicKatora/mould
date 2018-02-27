#ifndef CPP_MOULD_DEBUG_HPP
#define CPP_MOULD_DEBUG_HPP
#include <iterator>

#include "bytecode.hpp"

namespace mould {
  using ByteCodeBuffer = Buffer<const Codepoint>;
  using ImmediateBuffer = Buffer<const Immediate>;

  template<size_t N>
  constexpr ByteCodeBuffer byte_code_buffer(const Codepoint(&buffer)[N]) {
    return { std::begin(buffer), std::end(buffer) };
  }

  template<size_t N>
  constexpr ImmediateBuffer immediate_buffer(const Immediate(&buffer)[N]) {
    return { std::begin(buffer), std::end(buffer) };
  }

  template<typename CharT = const char>
  std::string describe_next_byte_code(
    ByteCodeBuffer& op_buffer,
    ImmediateBuffer& im_buffer
  ) {
    EncodedStringLiteral literal {};

    if(op_buffer.empty())
      return {};

    switch(*op_buffer.begin++) {
    case OpCode::Literal:
      if(!(im_buffer >> literal)) return "!!!Missing immediate literal";
      return std::string("Literal: \"")
             + std::string(literal.begin_ptr<const CharT>(), literal.length)
             + "\"";
    case OpCode::Formatted_IndexAuto_FormatAuto:
      break;
    case OpCode::Formatted_IndexAuto_FormatDirect:
      break;
    case OpCode::Formatted_IndexCount_FormatAuto:
      break;
    case OpCode::Formatted_IndexCount_FormatDirect:
      break;
    case OpCode::Stop:
      op_buffer.begin = op_buffer.end; // Forcibly consume the buffer
      return "Stop";
    }
    return "!!!Unknown op code";
  }

  template<typename CharT>
  struct Descriptor {
    ByteCodeBuffer op_buffer;
    ImmediateBuffer im_buffer;

    template<typename Formatter>
    constexpr Descriptor(const Formatter& formatter) :
        op_buffer { std::begin(formatter.code), std::end(formatter.code) },
        im_buffer { std::begin(formatter.immediates), std::end(formatter.immediates) }
        { }

    constexpr bool empty() const {
      op_buffer.empty() || im_buffer.empty();
    };

    constexpr operator bool() const {
      return !empty();
    }

    std::string describe_next_byte_code() {
      if(empty()) return {};
      return ::mould::describe_next_byte_code<CharT>(op_buffer, im_buffer);
    }
  };

  template<typename Formatter>
  constexpr auto descriptor(const Formatter& formatter)
  -> Descriptor<typename Formatter::CharT> {
    return Descriptor<typename Formatter::CharT> { formatter };
  }

}

#endif
