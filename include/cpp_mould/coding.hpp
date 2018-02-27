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

    static constexpr EncodedOperation Literal() {
      return { (Codepoint) 0 };
    }

    static constexpr EncodedOperation Insert(CodeValue index, ImmediateValue format) {
      const auto mask_index = (static_cast<unsigned char>(index) & 1) << 1;
      const auto mask_format = (static_cast<unsigned char>(format) & 1) << 2;
      return { (Codepoint) (1 | mask_index | mask_format) };
    }

    static constexpr EncodedOperation Stop() {
      return { (Codepoint) 0xFF };
    }

    constexpr OpCode opcode() const {
      if(encoded == 0xFF) return OpCode::Stop;
      if(encoded & 0x1 == 0) return OpCode::Literal;
      else return OpCode::Insert;
    }

    constexpr CodeValue insert_index() const {
      if(encoded & 0x2 == 0) return CodeValue::Auto;
      else return CodeValue::ReadCode;
    }

    constexpr ImmediateValue insert_format() const {
      if(encoded & 0x4 == 0) return ImmediateValue::Auto;
      else return ImmediateValue::ReadImmediate;
    }

    friend constexpr bool operator<<(Buffer<Codepoint>& buffer, EncodedOperation operation) {
      return (buffer << operation.encoded);
    }

    friend constexpr bool operator>>(Buffer<const Codepoint>& buffer, EncodedOperation& operation) {
      EncodedOperation internal {};
      (buffer >> internal.encoded) ? (operation = internal, true) : false;
    }
  };

  struct EncodedFormat {
    Immediate encoded;

    constexpr EncodedFormat()
      : encoded() {}
    constexpr EncodedFormat(Codepoint encoded)
      : encoded(encoded)
      { }
    constexpr EncodedFormat(
      FormatKind kind,
      InlineValue width,
      InlineValue precision,
      InlineValue padding,
      Alignment alignment,
      Sign sign,
      unsigned char inlines[6])
      : encoded(0)
    {
      for(int i = 0; i < 6; i++) encoded = (encoded|inlines[i]) << 8;

      encoded |= (static_cast<unsigned char>(kind) & Immediate{0xF});
      encoded |= (static_cast<unsigned char>(width) & Immediate{0x3}) << 4;
      encoded |= (static_cast<unsigned char>(precision) & Immediate{0x3}) << 6;
      encoded |= (static_cast<unsigned char>(padding) & Immediate{0x3}) << 8;
      encoded |= (static_cast<unsigned char>(alignment) & Immediate{0x3}) << 10;
      encoded |= (static_cast<unsigned char>(sign) & Immediate{0x3}) << 12;
    }

    constexpr FormatKind format_kind(Immediate) {
      return static_cast<FormatKind>(encoded & 0xF);
    }

    constexpr InlineValue format_width(Immediate) {
      return static_cast<InlineValue>((encoded >> 4) & 0x3);
    }

    constexpr InlineValue format_precision(Immediate) {
      return static_cast<InlineValue>((encoded >> 6) & 0x3);
    }

    constexpr InlineValue format_padding(Immediate) {
      return static_cast<InlineValue>((encoded >> 8) & 0x3);
    }

    constexpr Alignment format_alignment(Immediate) {
      return static_cast<Alignment>((encoded >> 10) & 0x3);
    }

    constexpr Sign format_sign(Immediate) {
      return static_cast<Sign>((encoded >> 6) & 0x3);
    }

    constexpr unsigned char inline_value(unsigned char index) {
      return static_cast<unsigned char>((encoded >> 16 + 8*index) & 0xFF);
    }

    friend constexpr bool operator<<(Buffer<Immediate>& buffer, EncodedFormat operation) {
      return (buffer << operation.encoded);
    }

    friend constexpr bool operator>>(Buffer<const Immediate>& buffer, EncodedFormat& operation) {
      EncodedFormat internal {};
      (buffer >> internal.encoded) ? (operation = internal, true) : false;
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
    constexpr CharT* end_ptr() const { return (CharT*) length; }

    friend constexpr bool operator<<(Buffer<Immediate>& buffer, EncodedStringLiteral literal) {
      return (buffer << literal.begin) && (buffer << literal.length);
    }

    friend constexpr bool operator>>(Buffer<const Immediate>& buffer, EncodedStringLiteral& literal) {
      EncodedStringLiteral internal {};
      ((buffer >> internal.begin) && (buffer >> internal.length))
      ? (literal = internal, true)
      : false;
    }
  };
}

#endif
