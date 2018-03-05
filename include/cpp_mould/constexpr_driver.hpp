#ifndef CPP_MOULD_CONSTEXPR_DRIVER_HPP
#define CPP_MOULD_CONSTEXPR_DRIVER_HPP
#include <tuple>
#include "argument.hpp"

namespace mould::internal::constexpr_driver {
  template<size_t N>
  struct ExpressionInformation {
    int indices[N];
  };

  template<typename T>
  struct TypedArgumentExpression {
    int argument_index;
    FormattingResult (*function)(const T&, Formatter);

    constexpr TypedArgumentExpression initialize(FullOperation operation) {
      return { argument_index, nullptr };
    }
  };

  struct LiteralExpression {
    size_t offset, length;

    constexpr LiteralExpression initialize(FullOperation operation) {
      return { offset, length };
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
}

namespace mould {
  template<typename Format, typename ... Arguments>
  std::string format_constexpr(
    Format& format_string,
    Arguments&&... arguments)
  {
    using namespace internal::constexpr_driver;
    constexpr auto expressions = CompiledExpressions<Format, Arguments...>;
    std::stringstream output;

    // for(auto index: arg_indices.indices) output << index;

    return output.str();
  }
}

#endif
