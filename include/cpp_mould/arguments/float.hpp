#ifndef CPP_MOULD_ARGUMENTS_FLOAT_HPP
#define CPP_MOULD_ARGUMENTS_FLOAT_HPP
#include <algorithm>
#include <cstdio>

#include <double-conversion/double-conversion.h>
#include <ryu/ryu.h>
#include <dragonbox.h>

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
    char* result_buffer = formatter.show_buf(100);
    result_buffer = result_buffer ? result_buffer : buffer;
    const auto start = result_buffer;

    if(format.sign == internal::Sign::Always && value >= 0)
      *result_buffer++ = '+'; // Add the sign
    else if(format.sign == internal::Sign::Pad && value >= 0)
      *result_buffer++ = ' '; // Add the sign

    result_buffer = dragonbox::Dtoa(result_buffer, value);
    const size_t length = result_buffer - start;

    if (start == buffer) {
      const auto important_buffer = std::string_view{start, length};
      formatter.append(important_buffer);
    } else {
      formatter.put_buf(length);
    }

    return FormattingResult::Success;
  }

  template<typename Formatter>
  ResultWithInformation<DoubleResultInformation> format_fpoint(double value, Formatter formatter) {
    using namespace double_conversion;
    char buffer[100];

    auto format = formatter.format();
    char* result_buffer = formatter.show_buf(100);
    result_buffer = result_buffer ? result_buffer : buffer;
    const auto start = result_buffer;

    if(format.sign == internal::Sign::Always && value >= 0)
      *result_buffer++ = '+'; // Add the sign
    else if(format.sign == internal::Sign::Pad && value >= 0)
      *result_buffer++ = ' '; // Add the sign

    unsigned fixed_count = format.has_precision ? format.precision : 6;

    int written = d2fixed_buffered_n(value, fixed_count, result_buffer);
    const size_t length = static_cast<size_t>((result_buffer + written) - start);

    if (start == buffer) {
      const auto important_buffer = std::string_view{start, length};
      formatter.append(important_buffer);
    } else {
      formatter.put_buf(length);
    }

    return FormattingResult::Success;
  }
}

#endif
