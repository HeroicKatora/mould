#ifndef CPP_MOULD_CONSTEXPR_DRIVER_HPP
#define CPP_MOULD_CONSTEXPR_DRIVER_HPP

namespace mould::internal {
  template<size_t N>
  struct ArgumentIndices {
    size_t indices[N];
  };

  template<typename T>
  struct TypedArgument {
    const T& value;
    FormattingResult (*function)(const T&, Formatter);
  };
}

#endif
