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
    size_t shft = sizeof(void*)*8 -4;
    
    int begin_index = 2*sizeof(void*) + 3;
    for(int i = 0; i < 2*sizeof(void*); i++) {
        auto chr = ((asint >> shft) & 0xF);
        if(chr != 0) begin_index = std::min(begin_index, i);
        buffer[2+i] = chr < 10 ? '0' + chr : 'a' + chr - 10;
        shft -= 4;
    }

    buffer[begin_index] = '0';
    buffer[begin_index + 1] = 'x';

    formatter.append(buffer + begin_index);
    
    return FormattingResult::Success;
  }
}

#endif
