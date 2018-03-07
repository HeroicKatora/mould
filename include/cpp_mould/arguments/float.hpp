#ifndef CPP_MOULD_ARGUMENTS_FLOAT_HPP
#define CPP_MOULD_ARGUMENTS_FLOAT_HPP
#include <cstdio>

#include "../format.hpp"

namespace mould {
  /* Standard implementation for float */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::fpoint> format_auto(const float&, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::fpoint> { };
  }

  template<typename Formatter>
  FormattingResult format_fpoint(const float& value, Formatter formatter) {
    char buffer[100];
    if(0 > std::snprintf(buffer, 100, "%*.*f", (int) formatter.format().width, (int) formatter.format().precision, value))
      return FormattingResult::Error;
    formatter.append(buffer);
    return FormattingResult::Success;
  }

  /* Standard implementation for double */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::fpoint> format_auto(const double&, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::fpoint> { };
  }

  template<typename Formatter>
  FormattingResult format_fpoint(const double& value, Formatter formatter) {
    char buffer[100];
    if(0 > std::snprintf(buffer, 100, "%*.*f", (int) formatter.format().width, (int) formatter.format().precision, value))
      return FormattingResult::Error;
    formatter.append(buffer);
    return FormattingResult::Success;
  }
}

#endif
