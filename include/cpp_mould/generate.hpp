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

  struct CompressedFormatting {
    Immediate _immediates[4];
    unsigned char used_immediates;

    constexpr void append_immediate(Immediate value) {
      _immediates[used_immediates++] = value;
    }
  };

  struct FormattingArguments {
    Formatting format;
    Immediate width, precision, padding;

    Codepoint index;

    constexpr CompressedFormatting compress() const;
  };

  // Holds an operation and all possible values it would require.  Can then
  // generate a `minimal` opcode for the operation.
  struct OperationBuilder {
    Operation op;

    // The values for a literal operation.
    EncodedStringLiteral literal;

    // The values for an insert operation.
    FormattingArguments format;

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
  constexpr bool get_string_literal(
    CompilationInput<CharT>& input,
    StringLiteral<CharT>& literal)
  {
    auto& buffer = input.buffer;
    const auto literal_begin = buffer.begin;
    while(buffer.begin < buffer.end && *buffer.begin != '{') buffer.begin++;

    OperationBuilder builder = {};
    builder.op = { OpCode::Literal };
    builder.literal = EncodedStringLiteral {
      static_cast<Immediate>(literal_begin - input.full_input.begin),
      static_cast<Immediate>(buffer.begin - literal_begin)
    };

    literal.operation = builder.Build();

    return literal_begin < buffer.begin;
  }

  template<typename CharT>
  constexpr bool get_format_specifier(
    CompilationInput<CharT>& input,
    FormatSpecifier<CharT>& format)
  {
    auto& buffer = input.buffer;
    const auto begin = buffer.begin;

    for(;; buffer.begin++) {
      if(buffer.begin == buffer.end) {
        return false;
      }
      if(*buffer.begin == '}') {
        buffer.begin++;
        break;
      }
    }

    const Buffer<CharT> format_buffer = { begin, buffer.begin };

    OperationBuilder builder = {};
    builder.op = Operation::Insert(ImmediateValue::Auto);

    format.buffer = format_buffer;
    format.operation = builder.Build();

    return true;
  }

  constexpr CompressedFormatting FormattingArguments::compress() const {
    auto final_format = format;

    unsigned char used_inlines = 0;
    if(format.index == InlineValue::Inline)
      final_format.inlines[used_inlines++] = index;

    CompressedFormatting compressed = {};

    auto encoded_format = EncodedFormat{ final_format };
    compressed.append_immediate(encoded_format.encoded);

    if(final_format.width == InlineValue::Immediate)
      compressed.append_immediate(width);
    if(final_format.precision == InlineValue::Immediate)
      compressed.append_immediate(precision);
    if(final_format.padding == InlineValue::Immediate)
      compressed.append_immediate(padding);

    return compressed;
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
