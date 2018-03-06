#ifndef CPP_MOULD_ARGUMENTS_FLOAT_HPP
#define CPP_MOULD_ARGUMENTS_FLOAT_HPP
#include "../format.hpp"

namespace mould {
  /* Standard implementation for const char* */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::fpoint> format_auto(const float&, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::fpoint> { };
  }

  template<typename Formatter>
  FormattingResult format_fpoint(const float& value, Formatter formatter) {
    return FormattingResult::Success;
  }
}

#endif
