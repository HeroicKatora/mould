#ifndef CPP_MOULD_ARGUMENTS_FLOAT_HPP
#define CPP_MOULD_ARGUMENTS_FLOAT_HPP
#include <cstdio>

#include "../format.hpp"

namespace mould {
  /* Standard implementation for float */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::fpoint> format_auto(float, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::fpoint> { };
  }

  template<typename Formatter>
  FormattingResult format_fpoint(float value, Formatter formatter) {
    char buffer[100];
    if(0 > std::snprintf(buffer, 100, "%*.*f", (int) formatter.format().width, (int) formatter.format().precision, value))
      return FormattingResult::Error;
    formatter.append(buffer);
    return FormattingResult::Success;
  }

  /* Standard implementation for double */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::string> format_auto(double, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::string> { };
  }

  template<typename Formatter>
  FormattingResult format_string(double value, Formatter formatter) {
    char buffer[100];

    auto format = formatter.format();
    int length;
    if(!format.has_width && !format.has_precision) {
      if(0 > (length = std::snprintf(buffer, 100, "% g", value)))
        return FormattingResult::Error;
    } else if(format.has_width && !format.has_precision) {
      if(0 > (length = std::snprintf(buffer, 100, "% *g", (int) format.width + 3, value)))
        return FormattingResult::Error;
    } else if(!format.has_width && format.has_precision) {
      if(0 > (length = std::snprintf(buffer, 100, "% .*g", (int) format.precision + 4, value)))
        return FormattingResult::Error;
    } else {
      if(0 > (length = std::snprintf(buffer, 100, "% *.*g", (int) format.width + 8, (int) format.precision + 6, value)))
        return FormattingResult::Error;
    }

    if(formatter.format().sign == internal::Sign::Always && value >= 0)
      buffer[0] = '+';

    const auto important_buffer
      = (formatter.format().sign == internal::Sign::Default && value >= 0)
      ? std::string_view{buffer + 1, (unsigned) length - 1}
      : std::string_view{buffer, (unsigned) length};
    formatter.append(important_buffer);
    return FormattingResult::Success;
  }

  template<typename Formatter>
  FormattingResult format_fpoint(double value, Formatter formatter) {
    char buffer[100];
    if(0 > std::snprintf(buffer, 100, "%*.*f", (int) formatter.format().width, (int) formatter.format().precision, value))
      return FormattingResult::Error;
    formatter.append(buffer);
    return FormattingResult::Success;
  }
}

#endif
