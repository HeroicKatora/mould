#ifndef CPP_MOULD_ARGUMENTS_STRINGS_HPP
#define CPP_MOULD_ARGUMENTS_STRINGS_HPP
#include "../format.hpp"

namespace mould {
  /* Standard implementation for const char* */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::String> format_auto(const char*, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::String> { };
  }

  template<typename Formatter>
  FormattingResult format_string(const char* value, Formatter formatter) {
    formatter.append(value);
    return FormattingResult::Success;
  }
}

#endif
