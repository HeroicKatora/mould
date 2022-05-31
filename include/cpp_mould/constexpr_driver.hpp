#ifndef CPP_MOULD_CONSTEXPR_DRIVER_HPP
#define CPP_MOULD_CONSTEXPR_DRIVER_HPP
#include <tuple>
#include "argument.hpp"
#include "engine.hpp"
#include "format.hpp"
#include "format_info.hpp"

namespace mould::internal::constexpr_driver {
  /* Resolved expression representation */
  template<size_t N>
  struct ExpressionInformation {
    int indices[N];
  };

  template<typename T>
  struct TypedArgumentExpression {
    int argument_index;
    FormattingResult (*function)(const T&, Formatter);
    FullOperation operation;

    constexpr TypedArgumentExpression initialize(FullOperation operation) {
      const auto info = TypedFormatter<T>::get(operation);
      return { argument_index, info.function, operation };
    }
  };

  struct LiteralExpression {
    size_t offset, length;

    constexpr LiteralExpression initialize(FullOperation operation) {
      return { operation.literal.offset, operation.literal.length };
    }
  };

  template<typename T>
  constexpr auto _expression_data()
  -> ExpressionInformation<std::size(T::data.code)> {
    ExpressionInformation<std::size(T::data.code)> result = {{}};

    int* output_ptr = &result.indices[0];
    FullOperationIterator iterator{T::data.code, T::data.immediates};

    while(!iterator.code_buffer.empty()) {
      auto op = *iterator;
      if(op.operation.operation.type == OpCode::Insert) {
        *output_ptr++ = op.formatting.index_value;
      } else {
        *output_ptr++ = -1;
      }
    }

    return result;
  }

  template<typename T>
  constexpr auto ExpressionData = _expression_data<T>();

  /* Compile the useful data, retrieve the specific formatting functions */
  template<int Index, typename ArgsTuple>
  constexpr auto uninitialized_expression() {
    if constexpr(Index < 0) {
      return LiteralExpression { 0, 0 };
    } else {
      using type = typename std::tuple_element<Index, ArgsTuple>::type;
      return TypedArgumentExpression<type> { Index, nullptr };
    }
  }

  template<typename ... E>
  struct CompiledFormatExpressions {
    std::tuple<E...> expressions;
    constexpr static CompiledFormatExpressions Compile(E ... expressions) {
      return {
        { expressions ... }
      };
    }

    template<size_t index>
    using ExpressionType = typename std::tuple_element<index, std::tuple<E...>>::type;
  };

  template<typename T, typename ... E>
  constexpr auto initialize(E ... expressions) 
  -> CompiledFormatExpressions<E...> {
    FullOperationIterator iterator{T::data.code, T::data.immediates};
    return CompiledFormatExpressions<E...>::Compile(expressions.initialize(*iterator) ...);
  }

  template<typename T, typename ArgsTuple, size_t ... indices>
  constexpr auto build_expressions(std::index_sequence<indices...>) {
    auto& data = ExpressionData<T>;
    return initialize<T>(uninitialized_expression<data.indices[indices], ArgsTuple>() ...);
  }

  template<typename Format, typename ... Arguments>
  constexpr auto CompiledExpressions = build_expressions<Format, std::tuple<Arguments...>>(
    std::make_index_sequence<std::size(Format::data.code)>{});

  /* Run the engine with arguments */
  template<typename EngineImpl>
  struct ExpressionContext {
    EngineImpl& engine;
    Buffer<const char> format_buffer;
  };

  template<typename EngineImpl, typename ExpT>
  struct Eval;

  template<typename EngineImpl>
  struct Eval<EngineImpl, LiteralExpression> {
    template<typename Format, size_t index, typename ... Arguments>
    static inline auto evaluate(
      const ExpressionContext<EngineImpl>& context,
      Arguments& ... args) 
    {
      constexpr auto& expression = std::get<index>(CompiledExpressions<Format, Arguments...>.expressions);
      // This is going to be determined at compile time, so the switch actually
      // make sense. The compiler doesn't seem to like inlining complex
      // functions at that point, so it doesn't realize the simplification of
      // `append(char*, char*)` for the special case of a single character.
      if (expression.length == 0) {} else if (expression.length == 1) {
        context.engine.append(context.format_buffer.begin()[expression.offset]);
      } else {
        context.engine.append(
          context.format_buffer.begin() + expression.offset, 
          context.format_buffer.begin() + expression.offset + expression.length);
      }
    }
  };

  template<FormatArgument type, Immediate value, typename ... Arguments>
  constexpr Immediate get_value(const Arguments& ... args) {
    if constexpr(type == FormatArgument::Auto) {
      return 0;
    } else if constexpr(type == FormatArgument::Parameter) {
      return std::get<value>(std::tie(args...));
    } else {
      return value;
    }
  }

  template<typename EngineImpl, typename T>
  struct Eval<EngineImpl, TypedArgumentExpression<T>> {
    template<typename Format, size_t index, typename ... Arguments>
    static inline auto evaluate(
      const ExpressionContext<EngineImpl>& context,
      Arguments& ... args)
    {
      constexpr auto& expression = std::get<index>(CompiledExpressions<Format, Arguments...>.expressions);
      constexpr auto& formatting = expression.operation.formatting;
      constexpr auto fn = expression.function;
#define CPP_MOULD_CONSTEXPR_EVAL_ASSERT(fkind) \
      if constexpr(formatting.kind == FormatKind:: fkind) { \
        static_assert(fn != nullptr, "Requested formatting (" #fkind ") not implemented"); \
      } 

      CPP_MOULD_CONSTEXPR_EVAL_ASSERT(Auto)
      CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_CONSTEXPR_EVAL_ASSERT)
#undef CPP_MOULD_CONSTEXPR_EVAL_ASSERT
      const auto& argument = std::get<ExpressionData<Format>.indices[index]>(std::tie(args...));
      ::mould::Format format {
        // values
        get_value<formatting.width, formatting.width_value>(args...),
        get_value<formatting.precision, formatting.precision_value>(args...),
        get_value<formatting.padding, formatting.padding_value>(args...),

        // flags
        formatting.width != FormatArgument::Auto,
        formatting.precision != FormatArgument::Auto,
        formatting.padding != FormatArgument::Auto,

        formatting.alignment,
        formatting.sign
      };
      fn(argument, ::mould::Formatter{context.engine, format});
    }
  };

  struct Ignore {
    template<typename ... I>
    Ignore(I&& ...) {}
  };

  template<typename EngineImpl, typename Format, typename ... Arguments, size_t ... Indices>
  inline auto _eval(ExpressionContext<EngineImpl> context, std::index_sequence<Indices...>, Arguments& ... args) {
    using Compiled = decltype(CompiledExpressions<Format, Arguments...>);
    Ignore ignore{(Eval<EngineImpl, typename Compiled::template ExpressionType<Indices>>
        ::template evaluate<Format, Indices>(context, args...), 0
      ) ...};
  }

  template<typename Format, typename EngineImpl, typename ... Arguments>
  inline auto eval(EngineImpl engine, Arguments ... args) {
    ExpressionContext<EngineImpl> context {
      engine,
      Format::data.format_buffer()
    };
    return _eval<EngineImpl, Format>(
      context,
      std::make_index_sequence<std::size(Format::data.code)>{},
      args... );
  }
}

namespace mould {
  template<typename Format, typename ... Arguments>
  void format_constexpr(
    Format& format_string,
    std::string& output,
    Arguments&&... arguments)
  {
    using namespace internal::constexpr_driver;

    internal::StringEngine engine{output};

    eval<Format>(engine, arguments...);
  }

  template<typename Format, typename OStream, typename ... Arguments>
  void write_constexpr(
    Format& format_string,
    OStream& output,
    Arguments&&... arguments)
  {
    using namespace internal::constexpr_driver;
    using RdBuf = typename std::remove_pointer<decltype(std::declval<OStream>().rdbuf())>::type;

    std::array<char, 512> buf;
    internal::BufferStreamEngine<RdBuf> engine{buf.data(), buf.size(), output.rdbuf()};
    eval<Format>(engine, arguments...);
  }
}

#endif
