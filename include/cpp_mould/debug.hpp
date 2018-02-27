#ifndef CPP_MOULD_DEBUG_HPP
#define CPP_MOULD_DEBUG_HPP
#include <iterator>

#include "bytecode.hpp"

namespace mould {
  template<size_t N>
  constexpr ByteCodeBuffer byte_code_buffer(const Codepoint(&buffer)[N]) {
    return { std::begin(buffer), std::end(buffer) };
  }

  template<size_t N>
  constexpr ImmediateBuffer immediate_buffer(const Immediate(&buffer)[N]) {
    return { std::begin(buffer), std::end(buffer) };
  }

  // Holds an encoded operation and room for the maximum amount of additions.
  struct DebuggableOperation {
    EncodedOperation opcode;

    EncodedStringLiteral literal;

    Codepoint index;
    EncodedFormat format;

    Immediate width;
    Immediate precision;
    Immediate padding;

    constexpr bool read(ByteCodeBuffer& code, ImmediateBuffer& immediates) {
      if(!(code >> opcode)) {
        return false;
      }
      switch(opcode.opcode()) {
      case OpCode::Stop:
        return true;
      case OpCode::Insert:
        if(opcode.insert_index() == CodeValue::ReadCode
           && !(code >> index)) {
          return false;
        }
        if(opcode.insert_format() == ImmediateValue::ReadImmediate
           && !_read_insert_format(code, immediates)) {
          return false;
        }
        return true;
      case OpCode::Literal:
        return _read_literal(code, immediates);
      default:
        return false;
      }
    }

    constexpr bool _read_literal(
      ByteCodeBuffer& code,
      ImmediateBuffer& immediates)
    {
      return (immediates >> literal);
    }

    constexpr bool _read_insert_format(
      ByteCodeBuffer& code,
      ImmediateBuffer& immediates)
    {
      if(!(immediates >> format)) {

      }
      return true;
    }
  };

  template<typename CharT = const char>
  std::string describe_next_byte_code(
    ByteCodeBuffer& op_buffer,
    ImmediateBuffer& im_buffer
  ) {
    Immediate auto_index = 0;
    Immediate normal_index = 0;

    DebuggableOperation operation {};
    EncodedStringLiteral literal {};

    while(operation.read(op_buffer, im_buffer)) {
      switch(operation.opcode.opcode()) {
      case OpCode::Literal:
        if(!(im_buffer >> literal))
          return "!!!Missing literal immediate";
        return std::string("Literal: \"")
               + std::string(literal.begin_ptr<const CharT>(), literal.length)
               + "\"";
      case OpCode::Insert:
        return std::string("Insert format");
      case OpCode::Stop:
        op_buffer.begin = op_buffer.end; // Forcibly consume the buffer
        return "Stop";
      default:
        return "!!!Unknown op code";
      }
    }
    op_buffer.begin = op_buffer.end; // Forcibly consume the buffer
    return std::string("No more code");
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
