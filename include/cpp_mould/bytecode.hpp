#ifndef CPP_MOULD_BYTECODE_HPP
#define CPP_MOULD_BYTECODE_HPP
#include <cstddef>

namespace mould::internal {
  using Codepoint = unsigned char;
  using Immediate = size_t;

  enum struct OpCode: unsigned char {
    // TODO: idea, we could encode a short (2**7 - 1) skip with literal, as
    // an offset from the previously written literal, the length of format
    // string should not be that long, typically. This would avoid an immediate
    // begin pointer. A skip of 0 would then mean loading the absolute string
    // begin pointer intermediate, i.e. for the first Literal.
    Literal   = 0 /* some literal with address embedded */,

     /* an argument from the environment, formatted */
    Insert = 1,

    /* the format string has ended */
    Stop = 2,
  };

  enum struct CodeValue: unsigned char {
    Auto     = 0 /* Can be inferred from the environment */,
    ReadCode = 1 /* Should be read from the codepoint buffer */,
  };

  enum struct ImmediateValue: unsigned char {
    Auto          = 0 /* Can be inferred */,
    ReadImmediate = 1 /* Read from the immediate buffer */,
  };

  struct Operation {
    OpCode         type;
    CodeValue      insert_index /* Only interesting for OpCode::insert */;
    ImmediateValue insert_format /* Only interesting for OpCode::insert */;

    static constexpr Operation Literal() {
      return { OpCode::Literal, CodeValue::Auto, ImmediateValue::Auto };
    }

    static constexpr Operation Insert(CodeValue index, ImmediateValue format) {
      return { OpCode::Insert, index, format };
    }

    static constexpr Operation Stop() {
      return { OpCode::Stop, CodeValue::Auto, ImmediateValue::Auto };
    }
  };

  enum struct FormatKind: unsigned char {
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

  enum struct InlineValue: unsigned char {
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
    FormatKind  kind;      /* 4 bits */

    InlineValue width;     /* 2 bits */
    InlineValue precision; /* 2 bits */

    /* value is a character, inline of immediate can depend on char type */
    InlineValue padding;   /* 2 bits */
    Alignment   alignment; /* 2 bits */

    Sign        sign; /* 2 bits */

    // TODO:technically only 14 bit for the kind, can we store something here?
    // Maybe we could save and then preload the number of additional immediates.
    // Doesn't sound too good though.
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

  using ByteCodeBuffer = Buffer<const Codepoint>;
  using ImmediateBuffer = Buffer<const Immediate>;

  using ByteCodeOutputBuffer = Buffer<Codepoint>;
  using ImmediateOutputBuffer = Buffer<Immediate>;

  template<typename CharT>
  class TypeErasedByteCode {
  public:
    virtual Buffer<const CharT> format_buffer() const = 0;
    virtual ByteCodeBuffer code_buffer() const = 0;
    virtual ImmediateBuffer immediate_buffer() const = 0;
  };
}

#endif
