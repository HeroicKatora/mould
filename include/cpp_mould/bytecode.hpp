#ifndef CPP_MOULD_BYTECODE_HPP
#define CPP_MOULD_BYTECODE_HPP
#include <cstddef>

namespace mould {
  using Codepoint = unsigned char;
  using Immediate = size_t;

  enum OpCode: Codepoint {
    Literal   = 0 /* some literal with address embedded */,

     /* an argument from the environment, formatted */
    Formatted_IndexAuto_FormatAuto = 1,
    Formatted_IndexAuto_FormatDirect = 2,
    Formatted_IndexCount_FormatAuto = 3,
    Formatted_IndexCount_FormatDirect = 4,

    Stop = 0xFF,
  };

  enum Formatting: Codepoint {
    /* If everything is auto , this is encoded in the opcode */
    Auto     = 0,

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

    /* Room for 4 more */

    Flag_Width_Auto       = 0b0'0000,
    Flag_Width_Direct     = 0b1'0000,

    Flag_Precision_Auto   = 0b0'00000,
    Flag_Precision_Direct = 0b1'00000,

    /* Could just always read padding, if we need another fast flag */
    Flag_Padding_Auto     = 0b0'000000,
    Flag_Padding_Direct   = 0b1'000000,

    Flag_Extended_None    = 0b0'0000000,
    Flag_Extended_Control = 0b1'0000000 /* Allows fill character, justifying, sign control */,
  };

  enum ExtendedFormatting: Codepoint {
    /* Auto is necessary, as one can be chosen for all types */
    Align_Left   = 0b00,
    Align_Right  = 0b01,
    Align_Center = 0b10,

    Sign_Auto    = 0b00'00,
    Sign_Always  = 0b01'00,
    Sign_Pad     = 0b10'00,
  };

  template<typename CharT>
  struct Buffer {
    CharT* begin;
    CharT* end;

    constexpr bool empty() const { return begin == end; }
  };
}

#endif
