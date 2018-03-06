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
    auto buffer = formatter.reserve(sizeof(void*)*2 + 2);

    buffer[0] = '0';
    buffer[1] = 'x';
    size_t asint = (size_t) ptr;
    size_t shft = sizeof(void*)*8 -4;
    
    for(int i = 0; i < 2*sizeof(void*); i++) {
        auto chr = ((asint >> shft) & 0xF);
        buffer[2+i] = chr < 10 ? '0' + chr : 'a' + chr - 10;
        shft -= 4;
    }
    
    return FormattingResult::Success;
  }
}

#endif
