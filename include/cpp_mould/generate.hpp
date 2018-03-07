#ifndef CPP_MOULD_GENERATE_HPP
#define CPP_MOULD_GENERATE_HPP
/* Constructor helpers for iterating format, generating bytecode.
 * Not necessarily constexpr, compared to `compile.hpp`.
 */
#include <cstddef>

#include "bytecode.hpp"
#include "coding.hpp"

namespace mould::internal {

  // Stores all codepoints and immediate values of an operation in encoded form.
  // This can be piped to both kinds of buffers.
  struct BuiltOperation {
    EncodedOperation operation;
    bool noop;

    Immediate _immediates[4];
    unsigned char used_immediates;

    constexpr BuiltOperation()
      : operation{}, noop(true), _immediates{}, used_immediates{}
      {}

    constexpr unsigned char codepoints() const {
      return noop ? 0 : 1;
    }

    constexpr unsigned char immediates() const {
      return used_immediates;
    }

    constexpr void append_immediate(Immediate value) {
      _immediates[used_immediates++] = value;
    }
  };

  // Holds an operation and all possible values it would require.  Can then
  // generate a `minimal` opcode for the operation.
  struct OperationBuilder {
    Operation op;

    // The values for a literal operation.
    EncodedStringLiteral literal;

    // The values for an insert operation.
    Formatting format;

    constexpr void set_formatting(Formatting format) {
      format = format;
      op.insert_format = ImmediateValue::ReadImmediate;
    }

    constexpr void unset_formatting() {
      format = {};
      op.insert_format = ImmediateValue::Auto;
    }

    constexpr BuiltOperation Build();
  };

  template<typename CharT>
  struct StringLiteral {
    BuiltOperation operation;
  };

  template<typename CharT>
  struct FormatSpecifier {
    Buffer<CharT> buffer;
    BuiltOperation operation;

    constexpr FormatSpecifier()
      : buffer{nullptr, nullptr}, operation{}
      {}
  };

  template<typename CharT>
  struct CompilationInput {
    Buffer<CharT> full_input /* the complete input string */;
    Buffer<CharT> buffer /* the remaining buffer */;
  };

  template<typename CharT>
  constexpr bool consume_unsigned(Buffer<CharT>& inner, unsigned& target) {
    bool a_char = false;
    unsigned result = 0;
    for(;;inner._begin++) {
      const auto chr = *inner.begin();
      if(chr < '0' || chr > '9')
        break;
      result = result * 10 + (chr - '0');
      a_char = true;
    }
    target = result;
    return a_char;
  }

  template<typename CharT>
  constexpr void consume_index(Buffer<CharT>& inner, Formatting& target) {
    unsigned index = 0;
    if(consume_unsigned(inner, index)) {
      if(index < 0x100) {
        target.format.index = InlineValue::Immediate;
        target.index = index;
      }
    }
    if(*inner.begin() == ':')
      inner._begin++;
  }

  template<typename CharT>
  constexpr void consume_align(Buffer<CharT>& inner, Formatting& target) {
    Alignment specified = Alignment::Default;
    switch(*inner.begin()) {
    case '<': specified = Alignment::Left; break;
    case '>': specified = Alignment::Right; break;
    case '=': specified = Alignment::Default; break;
    case '^': specified = Alignment::Center; break;
    default:
      return;
    }
    target.format.alignment = specified;
    inner._begin++;
  }

  template<typename CharT>
  constexpr void consume_sign(Buffer<CharT>& inner, Formatting& target) {
    Sign specified = Sign::Default;
    switch(*inner.begin()) {
    case '+': specified = Sign::Always; break;
    case '-': specified = Sign::Default; break;
    case ' ': specified = Sign::Pad; break;
    default:
      return;
    }
    inner._begin++;
    target.format.sign = specified;
  }

  template<typename CharT>
  constexpr void consume_width(Buffer<CharT>& inner, Formatting& target) {
    unsigned specified = 0;
    if(!consume_unsigned(inner, specified))
      return;
    target.format.width = InlineValue::Immediate;
    target.width = specified;
  }

  template<typename CharT>
  constexpr void consume_precision(Buffer<CharT>& inner, Formatting& target) {
    if(*inner.begin() != '.')
      return;
    inner._begin++;
    unsigned specified = 0;
    if(!consume_unsigned(inner, specified))
      return;
    target.format.precision = InlineValue::Immediate;
    target.precision = specified;
  }

  template<typename CharT>
  constexpr void consume_kind(Buffer<CharT>& inner, Formatting& target) {
    FormatKind specified = FormatKind::Auto;

    switch(*inner.begin()) {
    case 'b': specified = FormatKind::binary;
    case 'c': specified = FormatKind::character;
    case 'd': specified = FormatKind::decimal;
    case 'e': specified = FormatKind::exponent;
    case 'E': specified = FormatKind::EXPONENT;
    case 'f': specified = FormatKind::fpoint;
    case 'F': specified = FormatKind::FPOINT;
    // case 'g': return FormatKind:: | "G" | "n" |
    // Note: gGn are all similar to fpoint/decimal so I neglect them for now
    // Also, they do not appear in C and all formats should not be affected
    // by locale by design. Maybe this will get revisited later.
    case 'o': specified = FormatKind::octal;
    case 's': specified = FormatKind::string;
    case 'p': specified = FormatKind::pointer;
    case 'x': specified = FormatKind::hex;
    case 'X': specified = FormatKind::HEX;
    // | "%" // FIXME: support percentage
    default:
      return;
    }

    inner._begin++;
    target.format.kind = specified;
  }

  template<typename CharT>
  constexpr bool get_string_literal(
    CompilationInput<CharT>& input,
    StringLiteral<CharT>& literal)
  {
    auto& buffer = input.buffer;
    const auto literal_begin = buffer.begin();
    while(!buffer.empty() && *buffer.begin() != '{') buffer._begin++;

    OperationBuilder builder = {};
    builder.op = { OpCode::Literal };
    builder.literal = EncodedStringLiteral {
      static_cast<Immediate>(literal_begin - input.full_input.begin()),
      static_cast<Immediate>(buffer.begin() - literal_begin)
    };

    literal.operation = builder.Build();

    return true;
  }

  template<typename CharT>
  constexpr bool get_format_specifier(
    CompilationInput<CharT>& input,
    FormatSpecifier<CharT>& format)
  {
    auto& buffer = input.buffer;
    const auto begin = buffer.begin();

    for(;; buffer._begin++) {
      if(buffer.empty()) {
        return false;
      }
      if(*buffer.begin() == '}') {
        buffer._begin++;
        break;
      }
    }

    const Buffer<CharT> format_buffer = { begin, buffer.begin() };

    OperationBuilder builder = {};
    builder.op = Operation::Insert(ImmediateValue::Auto);

    if(format_buffer.length() < 2)
      return false;

    if(format_buffer.length() > 2)
      builder.set_formatting(Formatting{});

    Buffer<CharT> inner_format = {format_buffer._begin + 1, format_buffer._end - 1};
    consume_index(inner_format, builder.format);
    consume_align(inner_format, builder.format);
    consume_sign(inner_format, builder.format);
    consume_width(inner_format, builder.format);
    consume_precision(inner_format, builder.format);
    consume_kind(inner_format, builder.format);

    format.buffer = format_buffer;
    format.operation = builder.Build();

    return true;
  }

  constexpr BuiltOperation OperationBuilder::Build() {
    BuiltOperation built = {};
    built.operation = op;
    built.noop = (op.type == OpCode::Literal && literal.length == 0);
    switch(op.type) {
    case OpCode::Literal:
      built.append_immediate(literal.offset);
      built.append_immediate(literal.length);
      return built;
    case OpCode::Insert: {
        if(op.insert_format == ImmediateValue::Auto)
          return built;
        auto compressed = format.compress();
        if(compressed.used_immediates == 0) {
          op.insert_format = ImmediateValue::Auto;
          built.operation = op;
          return built;
        }
        for(int i = 0; i < compressed.used_immediates; i++)
          built.append_immediate(compressed._immediates[i]);
        return built;
      };
    }
  }

  constexpr bool operator<<(
    ByteCodeOutputBuffer& output,
    const BuiltOperation& built)
  {
    if(built.noop)
      return true;

    if(!(output << built.operation))
      return false;

    return true;
  }

  constexpr bool operator<<(
    ImmediateOutputBuffer& output,
    const BuiltOperation& built)
  {
    if(built.noop)
      return true;

    for(auto i = 0; i < built.used_immediates; i++) {
      if(!(output << built._immediates[i]))
        return false;
    }

    return true;
  }
}

#endif
