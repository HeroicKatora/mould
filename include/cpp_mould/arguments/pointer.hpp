#ifndef CPP_MOULD_ARGUMENTS_POINTER_HPP
#define CPP_MOULD_ARGUMENTS_POINTER_HPP
#include "../argument.hpp"

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
