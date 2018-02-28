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
      : format_string(code.format_buffer()),
        byte_code(code.code_buffer()),
        immediates(code.immediate_buffer()),
        begin(begin), end(end)
        { }

    friend class ::mould::Formatter;

    std::string execute();
  private:
    Buffer<const char> format_string;
    ByteCodeBuffer  byte_code;
    ImmediateBuffer immediates;

    std::string output;
    TypeErasedArgument* begin;
    TypeErasedArgument* end;
  };


  using formatting_function = FormattingResult (*)(const void*, Formatter);

  template<typename T>
  constexpr formatting_function auto_formatter();
  template<typename T>
  constexpr formatting_function decimal_formatter();
  template<typename T>
  constexpr formatting_function string_formatter();


  struct TypeErasedFormatter {
    template<typename T>
    constexpr static TypeErasedFormatter Construct() {
      return TypeErasedFormatter {
        auto_formatter<T>(),
        decimal_formatter<T>(),
        string_formatter<T>(),
      };
    }

    formatting_function automatic;
    formatting_function decimal;
    formatting_function string;

    constexpr formatting_function formatter_for(FormatKind kind) const {
      switch(kind) {
      case FormatKind::Auto: return automatic;
      case FormatKind::Decimal: return decimal;
      case FormatKind::String: return string;
      }
    }
  };

  template<typename T>
  Immediate value_as_immediate(const T&);

  struct TypeErasedArgument {
    template<typename T>
    TypeErasedArgument(const T& value)
      : formatter(TypeErasedFormatter::Construct<T>()),
        argument((const void*) std::addressof(value)),
        as_value(value_as_immediate(value))
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
    unsigned auto_index = 0;

    DebuggableOperation operation = {};

    while(operation.read(byte_code, immediates) == ReadStatus::NoError) {
      switch(operation.opcode.opcode()) {
      case OpCode::Literal:
        output.append(
          format_string.begin + operation.literal.offset,
          format_string.begin + operation.literal.offset + operation.literal.length);
        break;
      case OpCode::Insert: {
          unsigned index = auto_index;

          auto& formatting = operation.formatting;
          auto& description = formatting.format;

          if(description.index == InlineValue::Inline) {
            index = formatting.index;
          }

          if(index >= auto_index)
            auto_index = index + 1;

          auto& argument = begin[index];
          auto formatting_fn = argument.formatter_for(description.kind);

          // FIXME: we can determine at compile time which functions are needed!
          if(!formatting_fn)
            return std::string("Formatting not supported: ") + describe(description.kind);

          auto format = Format {
            formatting.width,
            formatting.precision,
            formatting.padding,
            description.alignment,
            description.sign
          };

          Formatter formatter {*this, format};
          auto result = formatting_fn(argument.argument, formatter);
          if(result == FormattingResult::Error)
            return std::string("Error while formatting");
        } break;
      }
    }
    return std::move(output);
  }

  template<typename T>
  inline static FormattingResult _decimal_formatter(const void* value, Formatter formatter) {
    return ::mould::format_decimal(*reinterpret_cast<const T*>(value), formatter);
  }

  template<typename T>
  inline static FormattingResult _string_formatter(const void* value, Formatter formatter) {
    return ::mould::format_string(*reinterpret_cast<const T*>(value), formatter);
  }

  template<typename T>
  constexpr formatting_function auto_formatter() {
    constexpr auto selection = decltype(format_auto(std::declval<const T&>()))::value;
    if constexpr(selection == AutoFormattingChoice::Decimal) {
      return decimal_formatter<T>();
    } else if constexpr(selection == AutoFormattingChoice::String) {
      return string_formatter<T>();
    } else {
      return nullptr;
    }
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

  template<typename T>
  Immediate value_as_immediate(const T& val) {
    if constexpr(!std::is_constructible<Immediate, const T&>::value) {
      return static_cast<Immediate>(-1);
    } else {
      return static_cast<Immediate>(val);
    }
  }
}

namespace mould {
  inline void Formatter::append(std::string arg) const {
    engine.output.append(arg);
  }

  inline void Formatter::append(const char* arg) const {
    engine.output.append(arg);
  }

  inline void Formatter::append(std::string_view sv) const {
    engine.output.append(sv);
  }
}

#endif
