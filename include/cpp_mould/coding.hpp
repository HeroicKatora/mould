#ifndef CPP_MOULD_CODING_HPP
#define CPP_MOULD_CODING_HPP
/* Interpretable values and their encoding */
#include "bytecode.hpp"

namespace mould::internal {
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

  struct EncodedOperation {
    Codepoint encoded;

    constexpr EncodedOperation()
      : encoded() {}
    constexpr EncodedOperation(Codepoint encoded)
      : encoded(encoded)
      { }
    constexpr EncodedOperation(Operation operation) : encoded() {
      switch(operation.type) {
      case OpCode::Literal:
        encoded = 0;
        break;
      case OpCode::Insert: {
        const auto mask_format = (static_cast<unsigned char>(operation.insert_format) & 1) << 1;
        encoded = (Codepoint) (1 | mask_format);
        } break;
      }
    }

    constexpr OpCode opcode() const {
      if((encoded & 0x1) == 0) return OpCode::Literal;
      else return OpCode::Insert;
    }

    constexpr ImmediateValue insert_format() const {
      if((encoded & 0x2) == 0) return ImmediateValue::Auto;
      else return ImmediateValue::ReadImmediate;
    }

    friend constexpr bool operator<<(
      Buffer<Codepoint>& buffer,
      EncodedOperation operation)
    {
      return (buffer << operation.encoded);
    }

    friend constexpr bool operator>>(
      Buffer<const Codepoint>& buffer,
      EncodedOperation& operation)
    {
      EncodedOperation internal {};
      return (buffer >> internal.encoded)
        ? (operation = internal, true)
        : false;
    }
  };

  struct EncodedFormatDescription {
    Immediate encoded;

    constexpr EncodedFormatDescription()
      : encoded() {}
    constexpr EncodedFormatDescription(Immediate encoded)
      : encoded(encoded)
      { }

    constexpr EncodedFormatDescription(FormatDescription format)
      : encoded(0)
    {
      for(int i = 0; i < 6; i++) encoded = (encoded|format.inlines[i]) << 8;

      encoded |= (static_cast<unsigned char>(format.kind) & Immediate{0xF});
      encoded |= (static_cast<unsigned char>(format.width) & Immediate{0x3}) << 4;
      encoded |= (static_cast<unsigned char>(format.precision) & Immediate{0x3}) << 6;
      encoded |= (static_cast<unsigned char>(format.padding) & Immediate{0x3}) << 8;
      encoded |= (static_cast<unsigned char>(format.alignment) & Immediate{0x3}) << 10;
      encoded |= (static_cast<unsigned char>(format.sign) & Immediate{0x3}) << 12;
      encoded |= (static_cast<unsigned char>(format.index) & Immediate{0x3}) << 14;
    }

    constexpr FormatDescription FullDescription() const {
      FormatDescription description = {};
      description.kind = kind();
      description.width = width();
      description.precision = precision();
      description.padding = padding();
      description.alignment = alignment();
      description.sign = sign();
      description.index = index();

      for(int i = 0; i < 6; i++) description.inlines[i] = inline_value(i);
      return description;
    }

    constexpr FormatKind kind() const {
      return static_cast<FormatKind>(encoded & 0xF);
    }

    constexpr InlineValue width() const {
      return static_cast<InlineValue>((encoded >> 4) & 0x3);
    }

    constexpr InlineValue precision() const {
      return static_cast<InlineValue>((encoded >> 6) & 0x3);
    }

    constexpr InlineValue padding() const {
      return static_cast<InlineValue>((encoded >> 8) & 0x3);
    }

    constexpr Alignment alignment() const {
      return static_cast<Alignment>((encoded >> 10) & 0x3);
    }

    constexpr Sign sign() const {
      return static_cast<Sign>((encoded >> 12) & 0x3);
    }

    constexpr InlineValue index() const {
      return static_cast<InlineValue>((encoded >> 14) & 0x3);
    }

    constexpr unsigned char inline_value(unsigned char index) const {
      return static_cast<unsigned char>((encoded >> (16 + 8*index)) & 0xFF);
    }

    friend constexpr bool operator<<(
      Buffer<Immediate>& buffer,
      EncodedFormatDescription operation)
    {
      return (buffer << operation.encoded);
    }

    friend constexpr bool operator>>(
      Buffer<const Immediate>& buffer,
      EncodedFormatDescription& operation)
    {
      EncodedFormatDescription internal {};
      return (buffer >> internal.encoded)
        ? (operation = internal, true)
        : false;
    }
  };

  enum struct ReadStatus {
    NoError,
    MissingOpcode,
    MissingFormatImmediate,
    MissingLiteralImmediate,
    MissingWidth,
    MissingPrecision,
    MissingPadding,
    InvalidIndex,
    InvalidOpcode,
    InvalidFormatImmediate,
  };

  struct EncodedFormatting {
    Immediate _immediates[4];
    unsigned char used_immediates;

    constexpr void append_immediate(Immediate value) {
      _immediates[used_immediates++] = value;
    }
  };

  struct EncodedStringLiteral {
    Immediate offset;
    Immediate length;

    constexpr EncodedStringLiteral()
      : offset(), length()
      {}

    constexpr EncodedStringLiteral(Immediate offset, Immediate length)
      : offset(offset), length(length)
      { }

    friend constexpr bool operator<<(
      Buffer<Immediate>& buffer,
      EncodedStringLiteral literal)
    {
      return (buffer << literal.offset) && (buffer << literal.length);
    }

    friend constexpr ReadStatus operator>>(
      Buffer<const Immediate>& buffer,
      EncodedStringLiteral& literal)
    {
      EncodedStringLiteral internal {};
      if(!(buffer >> internal.offset))
        return ReadStatus::MissingLiteralImmediate;
      if(!(buffer >> internal.length))
        return ReadStatus::MissingLiteralImmediate;
      literal = internal;
      return ReadStatus::NoError;
    }
  };

  struct Formatting {
    FormatDescription format;

    Immediate width, precision, padding;
    Codepoint index;

    constexpr EncodedFormatting compress() const {
      auto final_format = format;

      unsigned char used_inlines = 0;
      if(format.index == InlineValue::Inline)
        final_format.inlines[used_inlines++] = index;

      EncodedFormatting compressed = {};

      auto encoded_format = EncodedFormatDescription{ final_format };
      compressed.append_immediate(encoded_format.encoded);

      if(final_format.width == InlineValue::Immediate)
        compressed.append_immediate(width);
      if(final_format.precision == InlineValue::Immediate)
        compressed.append_immediate(precision);
      if(final_format.padding == InlineValue::Immediate)
        compressed.append_immediate(padding);

      return compressed;
    }

    friend constexpr ReadStatus operator>>(
      ImmediateBuffer& immediates,
      Formatting& target)
    {
      Formatting decoded = {};
      EncodedFormatDescription format = {};

      unsigned char inline_index = 0;
      if(!(immediates >> format)) {
        return ReadStatus::MissingFormatImmediate;
      }

      decoded.format = format.FullDescription();

      switch(format.width()) {
      case InlineValue::Immediate:
        if(!(immediates >> decoded.width))
          return ReadStatus::MissingWidth;
        break;
      case InlineValue::Inline: [[falltrough]]
      case InlineValue::Parameter:
        decoded.width = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      }

      switch(format.precision()) {
      case InlineValue::Immediate:
        if(!(immediates >> decoded.precision))
          return ReadStatus::MissingPrecision;
        break;
      case InlineValue::Inline: [[falltrough]]
      case InlineValue::Parameter:
        decoded.precision = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      }

      switch(format.padding()) {
      case InlineValue::Immediate:
        if(!(immediates >> decoded.padding))
          return ReadStatus::MissingPadding;
        break;
      case InlineValue::Inline: [[falltrough]]
      case InlineValue::Parameter:
        decoded.padding = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      }

      switch(format.index()) {
      case InlineValue::Inline:
        decoded.index = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      default:
        return ReadStatus::InvalidIndex;
      }

      target = decoded;
      return ReadStatus::NoError;
    }
  };
}

#endif
