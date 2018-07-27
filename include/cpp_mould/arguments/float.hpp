#ifndef CPP_MOULD_ARGUMENTS_FLOAT_HPP
#define CPP_MOULD_ARGUMENTS_FLOAT_HPP
#include <algorithm>
#include <cstdio>

#include <double-conversion/double-conversion.h>
#include <ryu/ryu.h>

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
    char* result_buffer = buffer;

    if(format.sign == internal::Sign::Always && value >= 0)
      *result_buffer++ = '+'; // Add the sign
    else if(format.sign == internal::Sign::Pad && value >= 0)
      *result_buffer++ = ' '; // Add the sign

    for(auto rest = result_buffer; rest < buffer + 100; rest++)
      *rest = '\0';
    d2s_buffered(value, result_buffer);
    unsigned length = std::strlen(buffer);

    const auto important_buffer = std::string_view{buffer, length};
    formatter.append(important_buffer);
    return FormattingResult::Success;
  }

  template<typename Formatter>
  ResultWithInformation<DoubleResultInformation> format_fpoint(double value, Formatter formatter) {
    using namespace double_conversion;
    char buffer[100];

    auto format = formatter.format();
    char* result_buffer = buffer;

    if(format.sign == internal::Sign::Always && value >= 0)
      *result_buffer++ = '+'; // Add the sign
    else if(format.sign == internal::Sign::Pad && value >= 0)
      *result_buffer++ = ' '; // Add the sign

    for(auto rest = result_buffer; rest < buffer + 100; rest++)
      *rest = '\0';
    d2s_buffered(value, result_buffer);

    unsigned length = std::strlen(buffer);

    auto dot = std::find(buffer, buffer + length, '.');
    if(dot == buffer + length) {
      *dot = '.';
      length++;
    }

    unsigned used_precision = length - (dot - buffer) - 1;
    unsigned fixed_count = format.has_precision ? format.precision : 6;
    for(auto fill = used_precision; fill < fixed_count; fill++) {
      buffer[length++] = '0';
    }

    const auto important_buffer = std::string_view{buffer, length};
    formatter.append(important_buffer);
    return FormattingResult::Success;
  }
}

#endif
