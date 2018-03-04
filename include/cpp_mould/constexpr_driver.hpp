#ifndef CPP_MOULD_CONSTEXPR_DRIVER_HPP
#define CPP_MOULD_CONSTEXPR_DRIVER_HPP
#include "argument.hpp"

namespace mould::internal::constexpr_driver {
  template<size_t N>
  struct ArgumentInformation {
    size_t indices[N];
  };

  template<typename T>
  struct TypedArgument {
    const T& value;
    FormattingResult (*function)(const T&, Formatter);
  };

  template<typename T>
  constexpr auto argument_indices()
  -> ArgumentInformation<insert_count(T::data.code)> {
    ArgumentInformation<insert_count(T::data.code)> result = {{}};

    size_t* output_ptr = &result.indices[0];
    FullOperationIterator iterator{T::data.code, T::data.immediates};

    while(!iterator.code_buffer.empty()) {
      auto op = *iterator;
      if(op.operation.operation.type == OpCode::Insert) {
        *output_ptr++ = op.formatting.index;
      }
    }

    return result;
  }
}

namespace mould {
  template<typename Format, typename ... Arguments>
  std::string format_constexpr(
    Format& format_string,
    Arguments&&... arguments)
  {
    using namespace internal::constexpr_driver;
    std::stringstream output;

    auto arg_indices = argument_indices<Format>();
    for(auto index: arg_indices.indices) output << index;

    return output.str();
  }
}

#endif
