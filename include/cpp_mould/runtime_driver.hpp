#ifndef CPP_MOULD_RUNTIME_DRIVER_HPP
#define CPP_MOULD_RUNTIME_DRIVER_HPP
#include "debug.hpp"
#include "format.hpp"
#include "format_info.hpp"
#include "engine.hpp"


namespace mould::internal {
  enum struct DriverResultType {
    Ok,
    UnsupportedFormatting, // A deferred formatting was not found
    FormattingError, // Some formatting was unsuccessful
  };

  struct DriverResult {
    DriverResultType type;

    // The formatting kind that caused the error, applicable to UnsupportedFormatting
    FormatKind cause_kind;
  };

  struct TypeErasedArgument;

  class RuntimeDriver {
  public:
    RuntimeDriver(
      const TypeErasedByteCode<char>& byte_code,
      const TypeErasedArgument* args_begin,
      const TypeErasedArgument* args_end)
      : output(),
        format_buffer(byte_code.format_buffer()),
        byte_code(byte_code.code_buffer()),
        immediates(byte_code.immediate_buffer()),
        args_begin(args_begin), args_end(args_end),
        engine(output)
    { }

    DriverResult execute();
    std::string result() &&;
  private:
    std::string output;
    const Buffer<const char> format_buffer;
    Buffer<const Codepoint> byte_code;
    Buffer<const Immediate> immediates;
    const TypeErasedArgument* args_begin;
    const TypeErasedArgument* args_end;
    Engine engine;
  };

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

    type_erased_formatting_function formatter_for(FormatKind kind) const {
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
    std::string output;
    TypeErasedArgument untyped_args [sizeof...(Arguments)]
      = {TypeErasedArgument{arguments}...};

    RuntimeDriver driver {buffer, std::begin(untyped_args), std::end(untyped_args)};
    const auto result = driver.execute();

    switch(result.type) {
    case DriverResultType::Ok:
      return std::move(driver).result();
    case DriverResultType::UnsupportedFormatting:
      return std::string("Formatting not supported: ") + describe(result.cause_kind);
    case DriverResultType::FormattingError:
      return std::string("Error while formatting");
    }
  }
}

namespace mould::internal {
  DriverResult RuntimeDriver::execute() {
    unsigned auto_index = 0;

    DebuggableOperation operation = {};

    while(operation.read(byte_code, immediates) == ReadStatus::NoError) {
      switch(operation.opcode.opcode()) {
      case OpCode::Literal:
        output.append(
          format_buffer.begin + operation.literal.offset,
          format_buffer.begin + operation.literal.offset + operation.literal.length);
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

          auto& argument = args_begin[index];
          auto formatting_fn = argument.formatter_for(description.kind);

          // FIXME: we can determine at compile time which functions are needed!
          if(!formatting_fn)
            return DriverResult {
              DriverResultType::UnsupportedFormatting,
              description.kind
            };

          auto format = Format {
            formatting.width,
            formatting.precision,
            formatting.padding,
            description.alignment,
            description.sign
          };

          Formatter formatter {engine, format};
          auto result = formatting_fn(argument.argument, formatter);
          if(result == FormattingResult::Error)
            return DriverResult {
              DriverResultType::FormattingError,
              description.kind
            };
        } break;
      }
    }

    return DriverResult {
      DriverResultType::Ok,
      FormatKind::Auto,
    };
  }

  std::string RuntimeDriver::result() && {
    return output;
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

#endif
