#ifndef CPP_MOULD_BYTECODE_HPP
#define CPP_MOULD_BYTECODE_HPP

namespace mould {
  using Codepoint = unsigned char;

  enum OpCode: Codepoint {
    Literal   = 0b0 /* some literal with address embedded */,
    Formatted = 0b1 /* an argument from the environment */,

    ArgumentIndex_Numbered = 0b000,
    ArgumentIndex_Named    = 0b010,
    ArgumentIndex_Auto     = 0b100,

    Stop = 0b11111111,
  };

  template<auto v> constexpr decltype(v) _signed_argument = (v << 1);
  template<auto v> constexpr decltype(v) _unsigned_argument = (v << 1) | 0b1;

  /* Performs conversion of arguments beforehand */
  enum FormattingType: Codepoint {
    Custom           = 0,
    Bool             = 1,

    SignedChar       = _signed_argument<1>,
    UnsignedChar     = _unsigned_argument<1>,
    SignedShort      = _signed_argument<2>,
    UnsignedShort    = _unsigned_argument<2>,
    SignedInt        = _signed_argument<3>,
    UnsignedInt      = _unsigned_argument<3>,
    SignedLong       = _signed_argument<4>,
    UnsignedLong     = _unsigned_argument<4>,
    SignedLongLong   = _signed_argument<5>,
    UnsignedLongLong = _unsigned_argument<5>, /* 11 */

    Float      = 12,
    Double     = 13,
    LongDouble = 14,

    Char   = 15,
    Char16 = 16,
    Char32 = 17,
    WChar  = 18,

    /* A character array where the environment knows the length */
    CharString   = 19,
    Char16String = 20,
    Char32String = 21,
    WCharString  = 22,

    Ptr = 23,

    /* Like string, but unknown length */
    CharPtr   = 24,
    Char16Ptr = 25,
    Char32Ptr = 26,
    WCharPtr  = 27,

    Custom = 0xFF,
  };

  constexpr is_signed_argument(Argument argument) {
    auto count = argument >> 1;
    return count > 0 && count <= 5 && (argument & 0b1 == 0);
  }

  constexpr is_unsigned_argument(Argument argument) {
    auto count = argument >> 1;
    return count > 0 && count <= 5 && (argument & 0b1 == 0);
  }

  enum Formatting: Codepoint {
    Auto     = 0b0000,
    Decimal  = 0b0001,
    Binary   = 0b0010,
    Octal    = 0b0011,
    Hex      = 0b0100,
    HEX      = 0b0101,
    Exponent = 0b0110,
    EXPONENT = 0b0111,
    FPoint   = 0b1000,
    FPOINT   = 0b1001,

    Width_Direct = 0b10000,
  };
}

#endif
