#ifndef CPP_MOULD_DEBUG_HPP
#define CPP_MOULD_DEBUG_HPP
#include <iterator>
#include <sstream>

#include "bytecode.hpp"

namespace mould::internal {
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

  constexpr const char* describe(ImmediateValue val) {
    if(val == ImmediateValue::Auto) return "Auto";
    else return "Immediate";
  }

  constexpr const char* describe(FormatKind kind) {
    switch(kind) {
    case FormatKind::Auto: return "Auto";
    case FormatKind::Decimal: return "Decimal";
    case FormatKind::Binary: return "Binary";
    case FormatKind::Octal: return "Octal";
    case FormatKind::Hex: return "Hex";
    case FormatKind::HEX: return "HEX";
    case FormatKind::Exponent: return "Exponent";
    case FormatKind::EXPONENT: return "EXPONENT";
    case FormatKind::FPoint: return "Floating Point";
    case FormatKind::FPOINT: return "FLOATING POINT";
    case FormatKind::Pointer: return "Pointer";
    case FormatKind::String: return "String";
    }
  }

  constexpr const char* describe(InlineValue kind) {
    switch(kind) {
    case InlineValue::Auto: return "Auto";
    case InlineValue::Immediate: return "Immediate";
    case InlineValue::Inline: return "Inline";
    case InlineValue::Parameter: return "Parameter";
    }
  }

  constexpr const char* describe(Alignment align) {
    switch(align) {
    case Alignment::Default: return "Default";
    case Alignment::Left: return "Left";
    case Alignment::Right: return "Right";
    case Alignment::Center: return "Center";
    }
  }

  constexpr const char* describe(Sign sign) {
    switch(sign) {
    case Sign::Default: return "Default";
    case Sign::Always: return "Always";
    case Sign::Pad: return "Pad";
    }
  }

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
      case OpCode::Insert: {
        std::stringstream description(std::string{});
        description << "Insert: format (" << describe(operation.opcode.insert_format());
        description << ") kind (" << describe(operation.format.kind());
        description << ") width (" << describe(operation.format.width())
                    << ", " << operation.width;
        description << ") precision (" << describe(operation.format.precision())
                    << ", " << operation.precision;
        description << ") padding (" << describe(operation.format.padding())
                    << ", " << operation.padding;
        description << ") alignment (" << describe(operation.format.alignment());
        description << ") sign (" << describe(operation.format.sign());
        description << ")";
        return description.str();
      }
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

    std::string operator*() {
      if(empty()) return {};
      return describe_next_byte_code<CharT>(op_buffer, im_buffer);
    }
  };
}

namespace mould {
  template<typename Formatter>
  constexpr auto descriptor(const Formatter& formatter)
  -> internal::Descriptor<typename Formatter::CharT> {
    return internal::Descriptor<typename Formatter::CharT> { formatter };
  }
}

#endif
