#ifndef CPP_MOULD_CODING_HPP
#define CPP_MOULD_CODING_HPP
/* Interpretable values and their encoding */
#include "bytecode.hpp"

namespace mould {
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
        const auto mask_index = (static_cast<unsigned char>(operation.insert_index) & 1) << 1;
        const auto mask_format = (static_cast<unsigned char>(operation.insert_format) & 1) << 2;
        encoded = (Codepoint) (1 | mask_index | mask_format);
        } break;
      case OpCode::Stop:
        encoded = 0xFF;
        break;
      }
    }

    constexpr OpCode opcode() const {
      if(encoded == 0xFF) return OpCode::Stop;
      if((encoded & 0x1) == 0) return OpCode::Literal;
      else return OpCode::Insert;
    }

    constexpr CodeValue insert_index() const {
      if((encoded & 0x2) == 0) return CodeValue::Auto;
      else return CodeValue::ReadCode;
    }

    constexpr ImmediateValue insert_format() const {
      if((encoded & 0x4) == 0) return ImmediateValue::Auto;
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

  struct EncodedFormat {
    Immediate encoded;

    constexpr EncodedFormat()
      : encoded() {}
    constexpr EncodedFormat(Immediate encoded)
      : encoded(encoded)
      { }

    constexpr EncodedFormat(Formatting format)
      : encoded(0)
    {
      for(int i = 0; i < 6; i++) encoded = (encoded|format.inlines[i]) << 8;

      encoded |= (static_cast<unsigned char>(format.kind) & Immediate{0xF});
      encoded |= (static_cast<unsigned char>(format.width) & Immediate{0x3}) << 4;
      encoded |= (static_cast<unsigned char>(format.precision) & Immediate{0x3}) << 6;
      encoded |= (static_cast<unsigned char>(format.padding) & Immediate{0x3}) << 8;
      encoded |= (static_cast<unsigned char>(format.alignment) & Immediate{0x3}) << 10;
      encoded |= (static_cast<unsigned char>(format.sign) & Immediate{0x3}) << 12;
    }

    constexpr FormatKind kind() {
      return static_cast<FormatKind>(encoded & 0xF);
    }

    constexpr InlineValue width() {
      return static_cast<InlineValue>((encoded >> 4) & 0x3);
    }

    constexpr InlineValue precision() {
      return static_cast<InlineValue>((encoded >> 6) & 0x3);
    }

    constexpr InlineValue padding() {
      return static_cast<InlineValue>((encoded >> 8) & 0x3);
    }

    constexpr Alignment alignment() {
      return static_cast<Alignment>((encoded >> 10) & 0x3);
    }

    constexpr Sign sign() {
      return static_cast<Sign>((encoded >> 6) & 0x3);
    }

    constexpr unsigned char inline_value(unsigned char index) {
      return static_cast<unsigned char>((encoded >> 16 + 8*index) & 0xFF);
    }

    friend constexpr bool operator<<(
      Buffer<Immediate>& buffer,
      EncodedFormat operation)
    {
      return (buffer << operation.encoded);
    }

    friend constexpr bool operator>>(
      Buffer<const Immediate>& buffer,
      EncodedFormat& operation)
    {
      EncodedFormat internal {};
      return (buffer >> internal.encoded)
        ? (operation = internal, true)
        : false;
    }
  };

  struct EncodedStringLiteral {
    Immediate begin;
    Immediate length;

    constexpr EncodedStringLiteral()
      : begin(), length()
      {}

    template<typename CharT = const char>
    constexpr EncodedStringLiteral(CharT* begin, CharT* end)
      : begin((Immediate) begin), length(end - begin)
      { }

    template<typename CharT = const char>
    constexpr CharT* begin_ptr() const { return (CharT*) begin; }

    template<typename CharT = const char>
    constexpr CharT* end_ptr() const { return ((CharT*) begin) + length; }

    friend constexpr bool operator<<(
      Buffer<Immediate>& buffer,
      EncodedStringLiteral literal)
    {
      return (buffer << literal.begin) && (buffer << literal.length);
    }

    friend constexpr bool operator>>(
      Buffer<const Immediate>& buffer,
      EncodedStringLiteral& literal)
    {
      EncodedStringLiteral internal {};
      return ((buffer >> internal.begin) && (buffer >> internal.length))
        ? (literal = internal, true)
        : false;
    }
  };
}

#endif
