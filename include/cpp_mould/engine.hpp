#ifndef CPP_MOULD_ENGINE_HPP
#define CPP_MOULD_ENGINE_HPP
#include <memory>

#include "argument.hpp"
#include "format.hpp"

namespace mould::internal {
  struct TypeErasedArgument;

  // A formatter into some output.
  class Engine {
    using ByteCodeInterface = internal::TypeErasedByteCode<char>;
    using ByteCodeBuffer = internal::ByteCodeBuffer;
    using ImmediateBuffer = internal::ImmediateBuffer;
  public:
    // All internally compiled items implement the abstract class of
    // their char type.
    Engine(
        const ByteCodeInterface& code,
        TypeErasedArgument* begin,
        TypeErasedArgument* end)
      : byte_code(code.code_buffer()), immediates(code.immediate_buffer()),
        begin(begin), end(end)
        { }

    friend class ::mould::Formatter;

    std::string execute();
  private:
    ByteCodeBuffer  byte_code;
    ImmediateBuffer immediates;

    std::string output;
    TypeErasedArgument* begin;
    TypeErasedArgument* end;
  };

  template<typename T>
  constexpr FormatKind auto_formatter();

  using formatting_function = bool (*)(const void*, Formatter);

  template<typename T>
  constexpr formatting_function decimal_formatter();
  template<typename T>
  constexpr formatting_function string_formatter();


  struct TypeErasedFormatter {
    template<typename T>
    static TypeErasedFormatter Construct() {
      TypeErasedFormatter formatter;
      formatter.decimal = decimal_formatter<T>();
      formatter.string = string_formatter<T>();
      // Hacky, this gets assigned to itself (nullptr) when return is Auto
      formatter.automatic = formatter.formatter_for(auto_formatter<T>());
      return formatter;
    }

    formatting_function automatic;
    formatting_function decimal;
    formatting_function string;

    formatting_function formatter_for(FormatKind kind) const {
      switch(kind) {
      case FormatKind::Auto: return automatic;
      case FormatKind::Decimal: return decimal;
      case FormatKind::String: return string;
      }
    }
  };

  struct TypeErasedArgument {
    template<typename T>
    TypeErasedArgument(const T& value)
      : formatter(TypeErasedFormatter::Construct<T>()),
        argument((const void*) std::addressof(value)),
        as_value(value)
      { }

    TypeErasedFormatter formatter;
    const void*         argument;
    const Immediate     as_value;

    formatting_function formatter_for(FormatKind kind) const {
      return formatter.formatter_for(kind);
    }
  };
}

namespace mould {
  template<typename ... Arguments>
  std::string format(
    const internal::TypeErasedByteCode<char>& buffer,
    Arguments&&... arguments)
  {
    using namespace internal;
    TypeErasedArgument untyped_args [sizeof...(Arguments)]
      = {TypeErasedArgument{arguments}...};
    Engine engine {buffer, untyped_args, untyped_args +  sizeof...(Arguments) };
    return engine.execute();
  }
}

#include "debug.hpp" // Cheat for the moment, lets take some slow stuff.

namespace mould::internal {
  std::string Engine::execute() {
    Immediate auto_index = 0;
    Immediate normal_index = 0;

    DebuggableOperation operation;

    while(operation.read(byte_code, immediates)) {
      switch(operation.opcode.opcode()) {
      case OpCode::Literal:
        output.append(
          operation.literal.begin_ptr<const char>(),
          operation.literal.end_ptr<const char>());
        break;
      case OpCode::Insert: {
          auto index = auto_index;
          if(operation.opcode.insert_index() == CodeValue::ReadCode) {
            index = operation.index;
            auto_index = std::max(auto_index, index + 1);
          }
          auto& argument = begin[index];
          auto formatting_fn = argument.formatter_for(operation.format.kind());
          auto format = Format {
            operation.width,
            operation.precision,
            operation.padding,
            operation.format.alignment(),
            operation.format.sign()
          };
          Formatter formatter {*this, format};
          formatting_fn(argument.argument, formatter);
        } break;
      case OpCode::Stop:
        // We can clear the internal buffer
        return std::move(output);
      }
    }
    return "!!!Failure, no stop but end of buffer";
  }

  template<typename T, typename=decltype(::mould::format_auto<T>())>
  struct auto_format_impl;
  template<typename T>
  struct auto_format_impl<T, NoAuto> { constexpr static FormatKind value = FormatKind::Auto; };
  template<typename T>
  struct auto_format_impl<T, AutoDecimal> { constexpr static FormatKind value = FormatKind::Decimal; };
  template<typename T>
  struct auto_format_impl<T, AutoString> { constexpr static FormatKind value = FormatKind::String; };


  template<typename T>
  bool _decimal_formatter(const void* value, Formatter formatter) {
    return ::mould::format_decimal(*reinterpret_cast<const T*>(value), formatter);
  }

  template<typename T>
  bool _string_formatter(const void* value, Formatter formatter) {
    return ::mould::format_string(*reinterpret_cast<const T*>(value), formatter);
  }

  template<typename T>
  constexpr FormatKind auto_formatter() {
    return auto_format_impl<T>::value;
  }

  template<typename T>
  constexpr formatting_function decimal_formatter() {
    using ImplementationCanary =
      decltype(::mould::format_decimal(std::declval<const T&>(), std::declval<Formatter>()));
    if constexpr(std::is_same<NotImplemented, ImplementationCanary>::value) {
      return nullptr;
    } else {
      return _decimal_formatter<T>;
    }
  }

  template<typename T>
  constexpr formatting_function string_formatter() {
    using ImplementationCanary =
      decltype(::mould::format_string(std::declval<const T&>(), std::declval<Formatter>()));
    if constexpr(std::is_same<NotImplemented, ImplementationCanary>::value) {
      return nullptr;
    } else {
      return _string_formatter<T>;
    }
  }
}

namespace mould {
  void Formatter::append(std::string arg) const {
    engine.output.append(arg);
  }

  void Formatter::append(const char* arg) const {
    engine.output.append(arg);
  }

  void Formatter::append(std::string_view sv) const {
    engine.output.append(sv);
  }
}

#endif
