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
  constexpr FormatKind get_format_kind(Buffer<CharT> inner_format) {
    switch(inner_format.end()[-1]) {
    case 'b': return FormatKind::binary;
    case 'c': return FormatKind::character;
    case 'd': return FormatKind::decimal;
    case 'e': return FormatKind::exponent;
    case 'E': return FormatKind::EXPONENT;
    case 'f': return FormatKind::fpoint;
    case 'F': return FormatKind::FPOINT;
    // case 'g': return FormatKind:: | "G" | "n" |
    // Note: gGn are all similar to fpoint/decimal so I neglect them for now
    // Also, they do not appear in C and all formats should not be affected
    // by locale by design. Maybe this will get revisited later.
    case 'o': return FormatKind::octal;
    case 's': return FormatKind::string;
    case 'p': return FormatKind::pointer;
    case 'x': return FormatKind::hex;
    case 'X': return FormatKind::HEX;
    // | "%" // FIXME: support percentage
    default:
      return FormatKind::Auto;
    }
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

    const Buffer<CharT> inner_format = {format_buffer._begin + 1, format_buffer._end - 1};

    builder.format.format.kind = get_format_kind(inner_format);

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
