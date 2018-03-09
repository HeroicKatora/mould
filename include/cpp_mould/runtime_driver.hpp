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
      std::string& output,
      const TypeErasedByteCode<char>& byte_code,
      const TypeErasedArgument* args_begin,
      const TypeErasedArgument* args_end)
      : engine(output),
        format_buffer(byte_code.format_buffer()),
        iterator(byte_code.code_buffer(), byte_code.immediate_buffer()),
        args_begin(args_begin), args_end(args_end)
    { }

    DriverResult execute();
  private:
    Engine engine;
    const Buffer<const char> format_buffer;
    FullOperationIterator iterator;
    const TypeErasedArgument* args_begin;
    const TypeErasedArgument* args_end;
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

    RuntimeDriver driver {output, buffer, std::begin(untyped_args), std::end(untyped_args)};
    const auto result = driver.execute();

    switch(result.type) {
    case DriverResultType::Ok:
      return output;
    case DriverResultType::UnsupportedFormatting:
      return std::string("Formatting not supported: ") + describe(result.cause_kind);
    case DriverResultType::FormattingError:
      return std::string("Error while formatting");
    }
  }
}

namespace mould::internal {
  DriverResult RuntimeDriver::execute() {

    while(!iterator.code_buffer.empty()) {
      auto latest = *iterator;
      switch(latest.operation.operation.type) {
      case OpCode::Literal:
        engine.append(
          format_buffer.begin() + latest.literal.offset,
          format_buffer.begin() + latest.literal.offset + latest.literal.length);
        break;
      case OpCode::Insert: {
          auto& formatting = latest.formatting;

          auto& argument = args_begin[formatting.index_value];
          auto formatting_fn = argument.formatter_for(formatting.kind);

          if(!formatting_fn)
            return DriverResult {
              DriverResultType::UnsupportedFormatting,
              formatting.kind
            };

          auto format = Format {
            formatting.width_value,
            formatting.precision_value,
            formatting.padding_value,

            formatting.width == FormatArgument::Auto,
            formatting.precision == FormatArgument::Auto,
            formatting.padding == FormatArgument::Auto,
            
            formatting.alignment,
            formatting.sign
          };

          Formatter formatter {engine, format};
          auto result = formatting_fn(argument.argument, formatter);
          if(result == FormattingResult::Error)
            return DriverResult {
              DriverResultType::FormattingError,
              formatting.kind
            };
        } break;
      }
    }

    return DriverResult {
      DriverResultType::Ok,
      FormatKind::Auto,
    };
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
