#ifndef CPP_MOULD_ARGUMENTS_FLOAT_HPP
#define CPP_MOULD_ARGUMENTS_FLOAT_HPP
#include <algorithm>
#include <cstdio>

#include <double-conversion/double-conversion.h>

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
    using namespace double_conversion;
    char buffer[100];

    auto format = formatter.format();
    auto& converter = DoubleToStringConverter::EcmaScriptConverter();
    StringBuilder builder{buffer, 100};
    if(format.sign == internal::Sign::Always && value >= 0)
      builder.AddCharacter('+'); // Add the sign

    if(format.sign == internal::Sign::Pad && value >= 0)
      builder.AddCharacter(format.padding); // Add the sign

    if(!converter.ToShortest(value, &builder))
      return FormattingResult::Error;

    int length = builder.position();

    const auto important_buffer = std::string_view{buffer, length};
    /*  = (formatter.format().sign == internal::Sign::Default && value >= 0)
      ? std::string_view{buffer + 1, (unsigned) length - 1}
      : std::string_view{buffer, (unsigned) length};*/
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
