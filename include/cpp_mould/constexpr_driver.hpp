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
    FormatKind kind;

    constexpr TypedArgumentExpression initialize(FullOperation operation) {
      const auto info = TypedFormatterInformation<T>::get(operation);
      return { argument_index, info.function, operation.formatting.format.kind };
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
        *output_ptr++ = op.formatting.index;
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
  struct ExpressionContext {
    Engine& engine;
    Buffer<const char> format_buffer;
  };

  template<typename ExpT>
  struct Eval;

  template<>
  struct Eval<LiteralExpression> {
    template<typename Format, size_t index, typename ... Arguments>
    static inline auto evaluate(
      const ExpressionContext& context,
      const Arguments& ... args) 
    {
      constexpr auto& expression = std::get<index>(CompiledExpressions<Format, Arguments...>.expressions);
      context.engine.append(
        context.format_buffer.begin() + expression.offset, 
        context.format_buffer.begin() + expression.offset + expression.length);
    }
  };

  template<typename T>
  struct Eval<TypedArgumentExpression<T>> {
    template<typename Format, size_t index, typename ... Arguments>
    static inline auto evaluate(
      const ExpressionContext& context,
      const Arguments& ... args)
    {
      constexpr auto& expression = std::get<index>(CompiledExpressions<Format, Arguments...>.expressions);
      constexpr auto fn = expression.function;
#define CPP_MOULD_CONSTEXPR_EVAL_ASSERT(fkind) \
      if constexpr(expression.kind == FormatKind:: fkind) { \
        static_assert(fn != nullptr, "Requested formatting (" #fkind ") not implemented"); \
      } 

      CPP_MOULD_CONSTEXPR_EVAL_ASSERT(Auto)
      CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_CONSTEXPR_EVAL_ASSERT)
#undef CPP_MOULD_CONSTEXPR_EVAL_ASSERT
      const auto& argument = std::get<ExpressionData<Format>.indices[index]>(std::tie(args...));
      ::mould::Format format {
        0, 10, 0, Alignment::Default, Sign::Default
      };
      fn(argument, ::mould::Formatter{context.engine, format});
    }
  };

  struct Ignore {
    template<typename ... I>
    Ignore(I&& ...) {}
  };

  template<typename Format, typename ... Arguments, size_t ... Indices>
  inline auto _eval(ExpressionContext context, std::index_sequence<Indices...>, const Arguments& ... args) {
    using Compiled = decltype(CompiledExpressions<Format, Arguments...>);
    Ignore ignore{(Eval<typename Compiled::template ExpressionType<Indices>>::template evaluate<Format, Indices>(
      context, args...), 0) ...};
  }

  template<typename Format, typename ... Arguments>
  inline auto eval(Engine engine, const Arguments& ... args) {
    ExpressionContext context {
      engine,
      Format::data.format_buffer()
    };
    return _eval<Format>(
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

    internal::Engine engine{output};

    eval<Format>(engine, arguments...);
  }
}

#endif
