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
    Immediate auto_index = 0;
    Immediate normal_index = 0;
    EncodedStringLiteral literal {};

    if(op_buffer.empty())
      return {};

    switch(*op_buffer.begin++) {
    case OpCode::Literal:
      if(!(im_buffer >> literal)) return "!!!Missing literal immediate";
      return std::string("Literal: \"")
             + std::string(literal.begin_ptr<const CharT>(), literal.length)
             + "\"";
    case OpCode::Formatted_IndexAuto_FormatAuto:
      return std::string("Formatted argument: index auto (")
             + std::to_string(auto_index++)
             + std::string("), format auto");
    case OpCode::Formatted_IndexAuto_FormatDirect:
      return std::string("Formatted argument: index auto (")
             + std::to_string(auto_index++)
             + std::string("), format direct");
    case OpCode::Formatted_IndexCount_FormatAuto:
      if(!(im_buffer >> normal_index)) return "!!!Missing index immediate";
      auto_index = std::max(auto_index, normal_index + 1);
      return std::string("Formatted argument: index ")
             + std::to_string(normal_index)
             + std::string(", format auto");
    case OpCode::Formatted_IndexCount_FormatDirect:
      if(!(im_buffer >> normal_index)) return "!!!Missing index immediate";
      auto_index = std::max(auto_index, normal_index + 1);
      return std::string("Formatted argument: index ")
             + std::to_string(normal_index)
             + std::string(", format direct");
    case OpCode::Stop:
      op_buffer.begin = op_buffer.end; // Forcibly consume the buffer
      return "Stop";
    default:
      return "!!!Unknown op code";
    }
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
