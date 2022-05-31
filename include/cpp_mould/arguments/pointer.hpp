#ifndef CPP_MOULD_ARGUMENTS_POINTER_HPP
#define CPP_MOULD_ARGUMENTS_POINTER_HPP
#include "../argument.hpp"

#ifdef __has_include
  #if __has_include(<bit>) && __cpp_lib_bitops
  #define MOULD_POINTER_WITH_BIT_WIDTH_FUNCTION
  #endif
#endif

#ifdef MOULD_POINTER_WITH_BIT_WIDTH_FUNCTION
#include <bit>

namespace mould {
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::pointer> format_auto(void*, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::pointer> { };
  }

  template<typename Formatter>
  FormattingResult format_pointer(void* ptr, Formatter formatter) {
    char buffer[sizeof(void*)*2 + 3] = {};

    size_t asint = (size_t) ptr;
    const size_t lzeroch = std::countl_zero(asint) / 4;
    const size_t width = 2*sizeof(void*) - lzeroch;

    char* result_buffer = formatter.show_buf();
    result_buffer = result_buffer ? result_buffer : buffer;
    const auto start = result_buffer;

    for(int i = width; i >= 0; i--) {
        auto chr = asint & 0xF;
        result_buffer[2+i] = chr < 10 ? '0' + chr : 'a' + chr - 10;
        asint >>= 4;
    }

    result_buffer[0] = '0';
    result_buffer[1] = 'x';

    if (start == buffer) {
      formatter.append(std::string_view{buffer, 2 + width});
    } else {
      formatter.put_buf(2 + width);
    }

    return FormattingResult::Success;
  }
}

#else

namespace mould {
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::pointer> format_auto(void*, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::pointer> { };
  }

  template<typename Formatter>
  FormattingResult format_pointer(void* ptr, Formatter formatter) {
    char buffer[sizeof(void*)*2 + 3] = {};

    size_t asint = (size_t) ptr;
    
    int begin_index = 2*sizeof(void*) + 2;
    for(int i = 2*sizeof(void*) - 1; i >= 0; i--) {
        auto chr = asint & 0xF;
        if(chr != 0) begin_index = i;
        buffer[2+i] = chr < 10 ? '0' + chr : 'a' + chr - 10;
        asint >>= 4;
    }

    buffer[begin_index] = '0';
    buffer[begin_index + 1] = 'x';

    formatter.append(std::string_view{buffer + begin_index, 2*sizeof(void*) + 3 - begin_index});
    
    return FormattingResult::Success;
  }
}

#endif

#endif
