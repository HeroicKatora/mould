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
    Formatting formatting;

    constexpr ReadStatus read(ByteCodeBuffer& code, ImmediateBuffer& immediates) {
      if(!(code >> opcode)) {
        return ReadStatus::MissingOpcode;
      }
      switch(opcode.opcode()) {
      case OpCode::Insert:
        if(opcode.insert_format() == ImmediateValue::Auto)
          return ReadStatus::NoError;
        else if(opcode.insert_format() == ImmediateValue::ReadImmediate) {
          return (immediates >> formatting);
        }
        return ReadStatus::InvalidFormatImmediate;
      case OpCode::Literal:
        return (immediates >> literal);
      default:
        return ReadStatus::InvalidOpcode;
      }
    }
  };

  constexpr const char* describe(ReadStatus status) {
    switch(status) {
    case ReadStatus::NoError: return "NoError";
    case ReadStatus::MissingOpcode: return "MissingOpcode";
    case ReadStatus::MissingFormatImmediate: return "MissingFormatImmediate";
    case ReadStatus::MissingLiteralImmediate: return "MissingLiteralImmediate";
    case ReadStatus::MissingWidth: return "MissingWidth";
    case ReadStatus::MissingPrecision: return "MissingPrecision";
    case ReadStatus::MissingPadding: return "MissingPadding";
    case ReadStatus::InvalidIndex: return "InvalidIndex";
    case ReadStatus::InvalidOpcode: return "InvalidOpcode";
    case ReadStatus::InvalidFormatImmediate: return "InvalidFormatImmediate";
    }
  }

  constexpr const char* describe(ImmediateValue val) {
    if(val == ImmediateValue::Auto) return "Auto";
    else return "Immediate";
  }

  constexpr const char* describe(FormatKind kind) {
    switch(kind) {
    case FormatKind::Auto: return "Auto";
#define CPP_MOULD_DESCRIBE_FORMAT_KIND(kind)\
    case FormatKind:: kind: return  #kind;

    CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_DESCRIBE_FORMAT_KIND)
#undef CPP_MOULD_DESCRIBE_FORMAT_KIND
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
    const Buffer<CharT>& format_string,
    ByteCodeBuffer& op_buffer,
    ImmediateBuffer& im_buffer
  ) {
    Immediate auto_index = 0;
    Immediate normal_index = 0;
    ReadStatus status = ReadStatus::NoError;

    DebuggableOperation operation {};

    while(!op_buffer.empty()) {
      if((status = operation.read(op_buffer, im_buffer)) != ReadStatus::NoError)
        return describe(status);
      switch(operation.opcode.opcode()) {
      case OpCode::Literal:
        return std::string("Literal: \"")
               + std::string(
                  operation.literal.offset + format_string.begin,
                  operation.literal.length)
               + "\"";
      case OpCode::Insert: {
        std::stringstream description(std::string{});
        description << "Insert: format (" << describe(operation.opcode.insert_format());
        description << ") kind (" << describe(operation.formatting.format.kind);
        description << ") width (" << describe(operation.formatting.format.width)
                    << ", " << operation.formatting.width;
        description << ") precision (" << describe(operation.formatting.format.precision)
                    << ", " << operation.formatting.precision;
        description << ") padding (" << describe(operation.formatting.format.padding)
                    << ", " << operation.formatting.padding;
        description << ") alignment (" << describe(operation.formatting.format.alignment);
        description << ") sign (" << describe(operation.formatting.format.sign);
        description << ")";
        return description.str();
      }
      default:
        op_buffer.begin = op_buffer.end; // Forcibly consume the buffer
        return "!!!Unknown op code";
      }
    }
  }

  template<typename CharT>
  struct Descriptor {
    Buffer<const CharT> format_buffer;
    ByteCodeBuffer op_buffer;
    ImmediateBuffer im_buffer;

    constexpr Descriptor(const TypeErasedByteCode<CharT>& formatter) :
        format_buffer { formatter.format_buffer() },
        op_buffer { formatter.code_buffer() },
        im_buffer { formatter.immediate_buffer() }
        { }

    constexpr bool empty() const {
      return op_buffer.empty();
    };

    constexpr operator bool() const {
      return !empty();
    }

    std::string operator*() {
      if(empty()) return {};
      return describe_next_byte_code<const CharT>(format_buffer, op_buffer, im_buffer);
    }
  };
}

namespace mould {
  template<typename CharT>
  constexpr auto descriptor(const internal::TypeErasedByteCode<CharT>& formatter)
  -> internal::Descriptor<CharT> {
    return internal::Descriptor<CharT> { formatter };
  }
}

#endif
