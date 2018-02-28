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
  bool decimal_formatter(const void* value, Formatter formatter);

  struct TypeErasedArgument {
    template<typename T>
    TypeErasedArgument(const T& value)
      : argument((const void*) std::addressof(value)),
        decimal(decimal_formatter<T>), as_value(value)
      { }

    using formatting_function = bool (*)(const void*, Formatter);

    const void*     argument;
    const Immediate as_value;

    formatting_function decimal;

    formatting_function formatter_for(FormatKind kind) const {
      return decimal;
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
        return output;
      }
    }
    return "!!!Failure, no stop but end of buffer";
  }

  template<typename T>
  bool decimal_formatter(const void* value, Formatter formatter) {
    return ::mould::format_decimal(*reinterpret_cast<const T*>(value), formatter);
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
