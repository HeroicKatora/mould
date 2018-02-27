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

    bool read(ByteCodeBuffer& code, ImmediateBuffer& immediates) {
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

    bool _read_literal(
      ByteCodeBuffer& code,
      ImmediateBuffer& immediates)
    {
      return (immediates >> literal);
    }

    constexpr bool _read_insert_format(
      ByteCodeBuffer& code,
      ImmediateBuffer& immediates)
    {
      unsigned char inline_index = 0;
      if(!(immediates >> format)) {
        return false;
      }

      switch(format.width()) {
      case InlineValue::Immediate:
        if(!(immediates >> width))
          return false;
        break;
      case InlineValue::Inline: [[falltrough]]
      case InlineValue::Parameter:
        width = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      }

      switch(format.precision()) {
      case InlineValue::Immediate:
        if(!(immediates >> precision))
          return false;
        break;
      case InlineValue::Inline: [[falltrough]]
      case InlineValue::Parameter:
        precision = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      }

      switch(format.padding()) {
      case InlineValue::Immediate:
        if(!(immediates >> padding))
          return false;
        break;
      case InlineValue::Inline: [[falltrough]]
      case InlineValue::Parameter:
        padding = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
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

    while(operation.read(op_buffer, im_buffer)) {
      switch(operation.opcode.opcode()) {
      case OpCode::Literal:
        return std::string("Literal: \"")
               + std::string(
                  operation.literal.begin_ptr<const CharT>(),
                  operation.literal.end_ptr<const CharT>())
               + "\"";
      case OpCode::Insert:
        return std::string("Insert format");
      case OpCode::Stop:
        op_buffer.begin = op_buffer.end; // Forcibly consume the buffer
        return "Stop";
      default:
        op_buffer.begin = op_buffer.end; // Forcibly consume the buffer
        return "!!!Unknown op code";
      }
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
