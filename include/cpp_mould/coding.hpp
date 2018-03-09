#ifndef CPP_MOULD_CODING_HPP
#define CPP_MOULD_CODING_HPP
/* Interpretable values and their encoding */
#include "bytecode.hpp"

namespace mould::internal {
  static_assert(sizeof(size_t) == sizeof(void*));
  static_assert(sizeof(size_t) == sizeof(const char*));

  template<typename Symbol>
  constexpr bool operator<<(Buffer<Symbol>& buffer, Symbol imm) {
    if(buffer.empty()) return false;
    *buffer._begin++ = imm;
    return true;
  }

  template<typename Symbol>
  constexpr bool operator>>(Buffer<const Symbol>& buffer, Symbol& imm) {
    if(buffer.empty()) return false;
    imm = *buffer._begin++;
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

    constexpr Operation FullOperation() const {
      Operation operation = {};
      operation.type = opcode();
      operation.insert_format = insert_format();
      return operation;
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
      ByteCodeOutputBuffer& buffer,
      EncodedOperation operation)
    {
      return (buffer << operation.encoded);
    }

    friend constexpr bool operator>>(
      ByteCodeBuffer& buffer,
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
      for(int i = 5; i >= 0; i--) encoded = (encoded << 8)|format.inlines[i];
      encoded = encoded << 16;

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
      ImmediateOutputBuffer& buffer,
      EncodedFormatDescription operation)
    {
      return (buffer << operation.encoded);
    }

    friend constexpr bool operator>>(
      ImmediateBuffer& buffer,
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

  enum struct FormatArgument {
    Auto,
    Value,
    Parameter,
  };

  struct Formatting {
    FormatKind kind;

    FormatArgument width;
    FormatArgument precision;
    FormatArgument padding;
    Alignment alignment;
    Sign sign;
    FormatArgument index;

    Immediate width_value;
    Immediate precision_value;
    Immediate padding_value;
    Codepoint index_value;

    constexpr Formatting()
      : kind(FormatKind::Auto), width(FormatArgument::Auto), precision(FormatArgument::Auto),
      padding(FormatArgument::Auto), alignment(Alignment::Default), sign(Sign::Default),
      index(FormatArgument::Auto), width_value(), precision_value(), padding_value(), index_value()
    { }

    constexpr static InlineValue _determine_value_kind(FormatArgument arg, Immediate value) {
      if(arg == FormatArgument::Auto)
        return InlineValue::Auto;
      
      if(arg == FormatArgument::Value && value < 256)
        return InlineValue::Inline;
      else if(arg == FormatArgument::Value)
        return InlineValue::Immediate;
      
      if(arg == FormatArgument::Parameter && value < 256)
        return InlineValue::Parameter;

      throw "Parameter indices can only go to 255";
    }

    constexpr static FormatArgument _determine_argument_kind(InlineValue value_kind) {
      switch(value_kind) {
      case InlineValue::Auto: return FormatArgument::Auto;
      case InlineValue::Immediate: return FormatArgument::Value;
      case InlineValue::Inline: return FormatArgument::Value;
      case InlineValue::Parameter: return FormatArgument::Parameter;
      }
    }

    constexpr EncodedFormatting compress() const {
      FormatDescription final_format = {};

      final_format.kind = kind;

      final_format.width = _determine_value_kind(width, width_value);
      final_format.precision = _determine_value_kind(precision, precision_value);
      final_format.padding = _determine_value_kind(padding, padding_value);

      final_format.alignment = alignment;
      final_format.sign = sign;
      final_format.index = index == FormatArgument::Auto ? InlineValue::Auto : InlineValue::Inline;

      unsigned char used_inlines = 0;

      if(final_format.width == InlineValue::Inline || final_format.width == InlineValue::Parameter)
        final_format.inlines[used_inlines++] = (Codepoint) width_value;
      if(final_format.precision == InlineValue::Inline || final_format.precision == InlineValue::Parameter)
        final_format.inlines[used_inlines++] = (Codepoint) precision_value;
      if(final_format.padding == InlineValue::Inline || final_format.padding == InlineValue::Parameter)
        final_format.inlines[used_inlines++] = (Codepoint) padding_value;

      if(final_format.index == InlineValue::Inline /* Parameter is not (yet) allowed */)
        final_format.inlines[used_inlines++] = index_value;

      EncodedFormatting compressed = {};

      auto encoded_format = EncodedFormatDescription{ final_format };
      compressed.append_immediate(encoded_format.encoded);

      if(final_format.width == InlineValue::Immediate)
        compressed.append_immediate(width_value);
      if(final_format.precision == InlineValue::Immediate)
        compressed.append_immediate(precision_value);
      if(final_format.padding == InlineValue::Immediate)
        compressed.append_immediate(padding_value);

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

      auto decoded_format = format.FullDescription();
      decoded.kind = decoded_format.kind;
      decoded.sign = decoded_format.sign;
      decoded.alignment = decoded_format.alignment;

      decoded.width = _determine_argument_kind(decoded_format.width);
      switch(decoded_format.width) {
      case InlineValue::Immediate:
        if(!(immediates >> decoded.width_value))
          return ReadStatus::MissingWidth;
        break;
      case InlineValue::Inline: [[fallthrough]];
      case InlineValue::Parameter:
        decoded.width_value = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      }

      decoded.precision = _determine_argument_kind(decoded_format.precision);
      switch(decoded_format.precision) {
      case InlineValue::Immediate:
        if(!(immediates >> decoded.precision_value))
          return ReadStatus::MissingPrecision;
        break;
      case InlineValue::Inline: [[fallthrough]];
      case InlineValue::Parameter:
        decoded.precision_value = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      }

      decoded.padding = _determine_argument_kind(decoded_format.padding);
      switch(decoded_format.padding) {
      case InlineValue::Immediate:
        if(!(immediates >> decoded.padding_value))
          return ReadStatus::MissingPadding;
        break;
      case InlineValue::Inline: [[fallthrough]];
      case InlineValue::Parameter:
        decoded.padding_value = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      }

      decoded.index = _determine_argument_kind(decoded_format.index);
      switch(decoded_format.index) {
      case InlineValue::Inline:
        decoded.index_value = format.inline_value(inline_index++);
      case InlineValue::Auto:
        break;
      default:
        return ReadStatus::InvalidIndex;
      }

      target = decoded;
      return ReadStatus::NoError;
    }
  };

  constexpr size_t insert_count(ByteCodeBuffer buffer) {
    EncodedOperation op = {};
    size_t count = 0;
    while(buffer >> op) {
      if(op.opcode() == OpCode::Insert)
        count += 1;
    }
    return count;
  }

  struct DecodedOperation {
    Operation operation;

    constexpr DecodedOperation()
      : operation(Operation::Uninitialized())
    { }

    friend constexpr ReadStatus operator>>(
      ByteCodeBuffer& buffer,
      DecodedOperation& operation)
    {
      EncodedOperation internal_op = {};
      if(!(buffer >> internal_op))
        return ReadStatus::MissingOpcode;
      operation.operation = internal_op.FullOperation();
      return ReadStatus::NoError;
    }
  };

  struct FullOperation {
    DecodedOperation operation;
    EncodedStringLiteral literal;
    Formatting formatting;
  };

  struct FullOperationIterator {
    ByteCodeBuffer code_buffer;
    ImmediateBuffer imm_buffer;

    FullOperation latest;
    ReadStatus status;
    int auto_index;

    constexpr FullOperationIterator(ByteCodeBuffer code, ImmediateBuffer imm)
      : code_buffer(code), imm_buffer(imm), latest(), status(ReadStatus::NoError),
        auto_index(0)
    { }

    constexpr FullOperation operator*();
    constexpr FullOperationIterator& operator++();
  };

  constexpr FullOperationIterator& FullOperationIterator::operator++() {
    return *this;
  }

  constexpr FullOperation FullOperationIterator::operator*() {
    if(status != ReadStatus::NoError)
      return latest;

    if((status = code_buffer >> latest.operation) != ReadStatus::NoError) {
      return latest;
    }

    switch(latest.operation.operation.type) {
    case OpCode::Insert:
      if((status = imm_buffer >> latest.formatting) != ReadStatus::NoError)
        return latest;
      if(latest.formatting.index == FormatArgument::Auto)
        latest.formatting.index_value = auto_index++;
      else
        auto_index = std::max(auto_index, latest.formatting.index_value + 1);
      break;
    case OpCode::Literal:
      if((status = imm_buffer >> latest.literal) != ReadStatus::NoError)
        return latest;
      break;
    }

    return latest;
  }
}

#endif
