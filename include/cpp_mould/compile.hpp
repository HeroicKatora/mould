#ifndef CPP_MOULD_COMPILE_HPP
#define CPP_MOULD_COMPILE_HPP
/* The compilation process transforms a format string into byte code.
 */
#include <iterator>

#include "bytecode.hpp"
#include "generate.hpp"

namespace mould::internal {
  template<size_t N, typename CharT = const char>
  constexpr CompilationInput<CharT> format_buffer(CharT (&format_str)[N]) {
    std::string_view view { std::begin(format_str) };
    Buffer<CharT> buffer = { view.begin(), view.end() };
    return { buffer, buffer };
  }

  template<size_t N, typename CharT = const char>
  constexpr size_t bytecode_count(CharT (&format_str)[N]) {
    auto remaining = format_buffer(format_str);
    size_t count = 0;

    StringLiteral<CharT> literal_spec = { };
    FormatSpecifier<CharT> format_spec = { };

    while(!remaining.buffer.empty()) {
      /* Parse the next literal */
      if(!get_string_literal(remaining, literal_spec)) {
        break;
      }

      count += literal_spec.operation.codepoints();

      if(remaining.buffer.empty()) break;

      if(!get_format_specifier(remaining, format_spec)) {
        break;
      }

      count += format_spec.operation.codepoints();
    }

    return count;
  }

  template<size_t N, typename CharT = const char>
  constexpr size_t immediate_count(CharT (&format_str)[N]) {
    auto remaining = format_buffer(format_str);
    size_t count = 0;

    StringLiteral<CharT> literal_spec;
    FormatSpecifier<CharT> format_spec;

    while(!remaining.buffer.empty()) {
      /* Parse the next literal */
      if(!get_string_literal(remaining, literal_spec)) {
        break;
      }

      count += literal_spec.operation.immediates();

      if(remaining.buffer.empty()) break;

      if(!get_format_specifier(remaining, format_spec)) {
        break;
      }

      count += format_spec.operation.immediates();
    }

    return count;
  }

  template<size_t N, typename CharT = const char>
  auto char_type(CharT (&format_str)[N])
    -> typename std::remove_const<CharT>::type;

  template<size_t N, typename CharT = const char>
  constexpr size_t buffer_size(CharT (&format_str)[N]) {
    return N;
  }
  template<auto& format_str>
  constexpr size_t ByteOpCount = bytecode_count(format_str);

  template<auto& format_str>
  constexpr size_t ImmediateCount = immediate_count(format_str);

  template<auto& format_str>
  using CharType = decltype(char_type(format_str));

  template<size_t OP_COUNT, size_t IM_COUNT, typename _CharT>
  struct ByteCode: TypeErasedByteCode<_CharT> {
    using CharT = _CharT;
    Buffer<const CharT> format_string;

    Codepoint code[OP_COUNT];
    Immediate immediates[IM_COUNT];

    bool error;

    template<size_t N>
    constexpr ByteCode(const CharT (&format)[N])
      : format_string{ std::begin(format), std::end(format) }, code(),
        immediates(), error(false)
      {}

    Buffer<const CharT> format_buffer() const override {
      return { format_string };
    }

    ByteCodeBuffer code_buffer() const override {
      return { code };
    }

    ImmediateBuffer immediate_buffer() const override {
      return { immediates };
    }
  };

  template<auto& format_str>
  constexpr auto _compile()
  -> internal::ByteCode<
      internal::ByteOpCount<format_str>,
      internal::ImmediateCount<format_str>,
      internal::CharType<format_str>> {
    using namespace internal;
    ByteCode<
      ByteOpCount<format_str>,
      ImmediateCount<format_str>,
      CharType<format_str>
    > bytecode { format_str };

    Buffer<Codepoint> op_output = { bytecode.code };
    Buffer<Immediate> im_output = { bytecode.immediates };
    auto remaining = format_buffer(format_str);

    StringLiteral<const CharType<format_str>> literal_spec;
    FormatSpecifier<const CharType<format_str>> format_spec;

    while(!remaining.buffer.empty()) {
      /* Parse the next literal */
      if(!get_string_literal(remaining, literal_spec)) {
        bytecode.error = true;
        break;
      }

      op_output << literal_spec.operation;
      im_output << literal_spec.operation;

      if(remaining.buffer.empty()) break;

      if(!get_format_specifier(remaining, format_spec)) {
        bytecode.error = true;
        break;
      }

      op_output << format_spec.operation;
      im_output << format_spec.operation;
    }

    return bytecode;
  }
}

namespace mould {
  template<auto& format_str>
  struct CompiledFormatString 
    : internal::TypeErasedByteCode<decltype(internal::char_type(format_str))> {
    constexpr static auto data = internal::_compile<format_str>();
    using CharT = typename decltype(data)::CharT;

    internal::Buffer<const CharT> format_buffer() const override {
      return { data.format_string };
    }

    internal::ByteCodeBuffer code_buffer() const override {
      return { data.code };
    }

    internal::ImmediateBuffer immediate_buffer() const override {
      return { data.immediates };
    }
  };

  template<auto& format_str>
  constexpr auto compile()
  -> CompiledFormatString<format_str>
  {
    return CompiledFormatString<format_str>{};
  }
}

#endif

