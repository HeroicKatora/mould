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

  template<typename T>
  constexpr auto decimal_formatter(int)
  -> decltype(::mould::format_decimal(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    using ImplementationCanary = decltype(::mould::format_decimal(std::declval<const T&>(), std::declval<Formatter>()));
    if constexpr(std::is_same<NotImplemented, ImplementationCanary>::value) {
      return nullptr;
    } else {
      return ::mould::format_decimal;
    }
  }

  template<typename T>
  constexpr auto string_formatter(int)
  -> decltype(::mould::format_string(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    using ImplementationCanary = decltype(::mould::format_string(std::declval<const T&>(), std::declval<Formatter>()));
    if constexpr(std::is_same<NotImplemented, ImplementationCanary>::value) {
      return nullptr;
    } else {
      return ::mould::format_string;
    }
  }

  template<typename T>
  constexpr auto decimal_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }

  template<typename T>
  constexpr auto string_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }

  template<typename T>
  constexpr auto auto_formatter() {
    constexpr auto selection = decltype(format_auto(std::declval<const T&>()))::value;
    if constexpr(selection == AutoFormattingChoice::Decimal) {
      return decimal_formatter<T>(0);
    } else if constexpr(selection == AutoFormattingChoice::String) {
      return string_formatter<T>(0);
    } else {
      return nullptr;
    }
  }

  using type_erase_formatting_function = FormattingResult (*)(const void*, Formatter);

  template<typename T, FormattingResult (*fn)(const T&, Formatter)>
  FormattingResult type_erase_function(const void* self, Formatter formatter) {
    return fn(*reinterpret_cast<const T*>(self), formatter);
  }

  struct TypeErasedFormatter {
    template<typename T>
    constexpr static TypeErasedFormatter Construct() {
      return TypeErasedFormatter {
        type_erase_function<T, auto_formatter<T>()>,
        type_erase_function<T, decimal_formatter<T>(0)>,
        type_erase_function<T, string_formatter<T>(0)>,
      };
    }

    type_erase_formatting_function automatic;
    type_erase_formatting_function decimal;
    type_erase_formatting_function string;

    constexpr type_erase_formatting_function formatter_for(FormatKind kind) const {
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

    type_erase_formatting_function formatter_for(FormatKind kind) const {
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

  inline char* Formatter::reserve(size_t length) const {
    const auto start = engine.output.size();
    engine.output.resize(engine.output.size() + length, ' ');
    return engine.output.data() + start;
  }
}

#endif
