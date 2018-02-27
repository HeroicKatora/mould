#ifndef CPP_MOULD_BYTECODE_HPP
#define CPP_MOULD_BYTECODE_HPP
#include <cstddef>

namespace mould {
  using Codepoint = unsigned char;
  using Immediate = size_t;

  enum struct OpCode: Codepoint {
    // TODO: idea, we could encode a short (2**7 - 1) skip with literal, as
    // an offset from the previously written literal, the length of format
    // string should not be that long, typically. This would avoid an immediate
    // begin pointer. A skip of 0 would then mean loading the absolute string
    // begin pointer intermediate, i.e. for the first Literal.
    Literal   = 0 /* some literal with address embedded */,

     /* an argument from the environment, formatted */
    Formatted_IndexAuto_FormatAuto = 1,
    Formatted_IndexAuto_FormatDirect = 3,
    Formatted_IndexCount_FormatAuto = 5,
    Formatted_IndexCount_FormatDirect = 7,

    Stop = 0xFF,
  };

  enum struct FormattingKind: unsigned char {
    Auto     = 0, /* The kind is automatically chosen by the parameter */

    Decimal  = 1,
    Binary   = 2,
    Octal    = 3,
    Hex      = 4,
    HEX      = 5,
    Exponent = 6,
    EXPONENT = 7,
    FPoint   = 8,
    FPOINT   = 9,
    Pointer  = 10,

    String   = 11,
  };

  enum struct FlagValueKind: unsigned char {
    Auto      = 0 /* Deduced from input type or default */,
    Immediate = 1 /* Extra intermediate following the Formatting specifier */,
    Inline    = 2 /* Stored in the 8 bit inline extension array. */,
    Parameter = 3 /* Given as an additional parameter, index is auto */,
  };

  enum struct Alignment: unsigned char {
    Default = 0,
    Left    = 1,
    Right   = 2,
    Center  = 3,
  };

  enum struct Sign: unsigned char {
    Default = 0,
    Always  = 1,
    Pad     = 2,
  };

  struct Formatting {
    /* If everything is auto , this is encoded in the opcode */
    FormattingKind kind;      /* 8 bits */
    // TODO:technically only 4 bit for the kind, can we store something here?
    // Maybe we could save and then preload the number of additional immediates.
    // Doesn't sound too good though.

    FlagValueKind  width;     /* 2 bits */
    FlagValueKind  precision; /* 2 bits */

    /* value is a character, inline of immediate can depend on char type */
    FlagValueKind  padding;   /* 2 bits */
    Alignment      alignment; /* 2 bits */

    // 16 bit used.

    // 6 inline values (48 bit) for all kinds of fancy stuff.
    // TODO: only 2 this for 32bit systems.
    unsigned char inlines[6];
  };

  template<typename CharT>
  struct Buffer {
    CharT* begin;
    CharT* end;

    constexpr Buffer(CharT* begin, CharT* end)
      : begin(begin), end(end)
      { }

    template<size_t N>
    constexpr Buffer(CharT (&buffer)[N])
      : begin(buffer), end(buffer + N)
      { }

    constexpr bool empty() const { return begin == end; }
  };
}

#endif
