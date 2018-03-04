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
  -> ArgumentInformation<insert_count(T::code)> {
    ArgumentInformation<insert_count(T::code)> result;

    size_t* output_ptr = &result.indices[0];

    while((code_buffer >> op_buf)) {
      switch(op_buf.opcode()) {
      case OpCode::Insert:
        (imm_buffer >> formatting);
        *ouput_ptr++ = formatting.index();
      }
    }

    return result;
  }
}

#endif
