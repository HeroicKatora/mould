#ifndef CPP_MOULD_ARGUMENTS_FLOAT_HPP
#define CPP_MOULD_ARGUMENTS_FLOAT_HPP
#include <algorithm>
#include <cstdio>

#include <double-conversion/double-conversion.h>

#include "../format.hpp"

namespace mould {
  /* Standard implementation for double */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::string> format_auto(double, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::string> { };
  }

  struct DoubleResultInformation { };

  template<typename Formatter>
  ResultWithInformation<DoubleResultInformation> format_string(double value, Formatter formatter) {
    using namespace double_conversion;
    char buffer[100];

    auto format = formatter.format();
    auto& converter = DoubleToStringConverter::EcmaScriptConverter();
    StringBuilder builder{buffer, 100};

    if(format.sign == internal::Sign::Always && value >= 0)
      builder.AddCharacter('+'); // Add the sign
    else if(format.sign == internal::Sign::Pad && value >= 0)
      builder.AddCharacter(' '); // Add the sign

    if(!converter.ToShortest(value, &builder))
      return FormattingResult::Error;

    unsigned length = builder.position();

    const auto important_buffer = std::string_view{buffer, length};
    formatter.append(important_buffer);
    return FormattingResult::Success;
  }

  template<typename Formatter>
  ResultWithInformation<DoubleResultInformation> format_fpoint(double value, Formatter formatter) {
    using namespace double_conversion;
    char buffer[100];

    auto format = formatter.format();
    auto& converter = DoubleToStringConverter::EcmaScriptConverter();
    StringBuilder builder{buffer, 100};

    if(format.sign == internal::Sign::Always && value >= 0)
      builder.AddCharacter('+'); // Add the sign
    else if(format.sign == internal::Sign::Pad && value >= 0)
      builder.AddCharacter(' '); // Add the sign

    int fixed_count = format.has_precision ? format.precision : 6;
    if(!converter.ToFixed(value, fixed_count, &builder))
      return FormattingResult::Error;

    unsigned length = builder.position();

    const auto important_buffer = std::string_view{buffer, length};
    formatter.append(important_buffer);
    return FormattingResult::Success;
  }
}

#endif
