#ifndef CPP_MOULD_GENERATE_HPP
#define CPP_MOULD_GENERATE_HPP
/* Constructor helpers for iterating format, generating bytecode.
 * Not necessarily constexpr, compared to `compile.hpp`.
 */
#include <cstddef>

#include "bytecode.hpp"
#include "coding.hpp"

namespace mould {

  // Stores all codepoints and immediate values of an operation in encoded form.
  // This can be piped to both kinds of buffers.
  struct BuiltOperation {
    EncodedOperation operation;
    bool noop;

    Codepoint _extension;
    bool used_extension;

    Immediate _immediates[4];
    unsigned char used_immediates;

    constexpr BuiltOperation()
      : operation{}, noop(true), _extension{}, used_extension{}, _immediates{},
      used_immediates{}
      {}

    constexpr unsigned char codepoints() const {
      return noop
        ? 0
        : (used_extension ? 2 : 1);
    }

    constexpr unsigned char immediates() const {
      return used_immediates;
    }

    constexpr void use_codepoint(Codepoint extension) {
      used_extension = true;
      _extension = extension;
    }

    constexpr void append_immediate(Immediate value) {
      _immediates[used_immediates++] = value;
    }
  };

  struct FormattingArguments {
    Formatting format;
    Immediate width, precision, padding;

    constexpr FormattingArguments compress() const;
  };

  // Holds an operation and all possible values it would require.  Can then
  // generate a `minimal` opcode for the operation.
  struct OperationBuilder {
    Operation op;

    // The values for a literal operation.
    EncodedStringLiteral literal;

    // The values for an insert operation.
    Codepoint index;
    FormattingArguments format;

    constexpr void set_index(Codepoint index) {
      index = index;
      op.insert_index = CodeValue::ReadCode;
    }

    constexpr void unset_index() {
      index = 0;
      op.insert_index = CodeValue::Auto;
    }

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
  constexpr bool get_string_literal(
    Buffer<CharT>& buffer,
    StringLiteral<CharT>& literal)
  {
    const auto begin = buffer.begin;
    while(buffer.begin < buffer.end && *buffer.begin != '{') buffer.begin++;

    OperationBuilder builder = {};
    builder.op = { OpCode::Literal };
    builder.literal = { begin, buffer.begin };

    literal.operation = builder.Build();

    return true;
  }

  template<typename CharT>
  constexpr bool get_format_specifier(
    Buffer<CharT>& buffer,
    FormatSpecifier<CharT>& format)
  {
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
    builder.op = Operation::Insert(CodeValue::Auto, ImmediateValue::Auto);

    format.buffer = format_buffer;
    format.operation = builder.Build();

    return true;
  }

  constexpr FormattingArguments FormattingArguments::compress() const {
    return *this;
  }

  constexpr BuiltOperation OperationBuilder::Build() {
    BuiltOperation built = {};
    built.operation = op;
    built.noop = (op.type == OpCode::Literal && literal.length == 0);
    switch(op.type) {
    case OpCode::Stop:
      return built;
    case OpCode::Literal:
      built.append_immediate(literal.begin);
      built.append_immediate(literal.length);
      return built;
    case OpCode::Insert: {
        if(op.insert_index == CodeValue::ReadCode)
          built.use_codepoint(index);
        if(op.insert_format == ImmediateValue::Auto)
          return built;
        auto final_format = format.compress();
        auto encoded_format = EncodedFormat{final_format.format};
        built.append_immediate(encoded_format.encoded);
        if(final_format.format.width == InlineValue::Immediate)
          built.append_immediate(final_format.width);
        if(final_format.format.precision == InlineValue::Immediate)
          built.append_immediate(final_format.precision);
        if(final_format.format.padding == InlineValue::Immediate)
          built.append_immediate(final_format.padding);
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
    if(built.used_extension && !(output << built._extension))
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
