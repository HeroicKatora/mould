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

    unsigned fixed_count = format.has_precision ? format.precision : 6;

    for(auto rest = result_buffer; rest < buffer + 100; rest++)
      *rest = '\0';
    d2s_buffered(value, result_buffer);

    if(buffer[0] == 'N') {
      const auto nan_buffer = std::string_view{buffer, 3};
      formatter.append(nan_buffer);
      return FormattingResult::Success;
    } else if(buffer[1] == 'N') {
      const auto nan_buffer = std::string_view{buffer, 4};
      formatter.append(nan_buffer);
      return FormattingResult::Success;
    } else if(buffer[0] == 'i') {
      const auto nan_buffer = std::string_view{buffer, 8};
      formatter.append(nan_buffer);
      return FormattingResult::Success;
    } else if(buffer[1] == 'i') {
      const auto nan_buffer = std::string_view{buffer, 9};
      formatter.append(nan_buffer);
      return FormattingResult::Success;
    }

    unsigned length = 0;
    unsigned dot = 0;
    unsigned exp_begin = 0;
    int exponent = 0;

    for(auto search = buffer;;) {
      if(*search == '\0') {
        break;
      } else if(*search == '.') {
        dot = length;
      } else if(*search == 'E') {
        exp_begin = length;
      }
      search++;
      length++;
    }

    // Reparse the exponent.
    if(exp_begin != 0) {
      bool negative = buffer[exp_begin + 1] == '-';
      for(int i = 0; i < 3; i++) {
        unsigned char c = buffer[exp_begin + i + 1];
        c = c - '0';
        exponent = c < 10 ? exponent*10+c : exponent;
      }
      if(negative)
        exponent = -exponent;

      for(unsigned i = exp_begin; i < length; i++)
        buffer[i] = '\0';
      length = exp_begin;
    }

    if(dot == 0 && exponent >= 0) {
      std::fill_n(buffer + length, exponent, '0');
      length += exponent;
      exponent = 0;

      dot = length;
      buffer[length++] = '.';
    } else if(exponent > 0) {
      assert(dot != 0 && dot < length);
      const int max_shift = length - dot - 1;

      if(exponent <= max_shift) {
        std::rotate(buffer + dot, buffer + dot + 1, buffer + dot + max_shift + 1);
        exponent = 0;
      } else {
        const int fill_length = exponent - max_shift;
        std::memmove(buffer + dot, buffer + dot + 1, max_shift);
        std::fill_n(buffer + dot + max_shift, fill_length, '0');
        length += fill_length;

        dot = dot + max_shift + fill_length + 1;
        buffer[dot] = '.';

        exponent = 0;
      }
    } else if(exponent < 0) {
      // WTF?
      std::abort();
    } else if(exponent < 0) {
      assert(length > dot);
      const int shift_width = -exponent;

      const int suffix_len = length - dot - 1;
      const auto suffix_start = buffer + dot + 1;
      const auto suffix_dest = suffix_start + shift_width;
      std::memmove(suffix_dest, suffix_start, suffix_len);

      suffix_dest[-1] = buffer[dot - 1];
      buffer[dot - 1] = '0';
      std::fill(buffer + dot + 1, buffer + dot + shift_width, '0');

      dot = 1;
      exponent = 0;
      length += shift_width;
    }

    unsigned used_precision = length - dot - 1;
    for(auto fill = used_precision; fill < fixed_count; fill++) {
      buffer[length++] = '0';
    }

    const auto important_buffer = std::string_view{buffer, length};
    formatter.append(important_buffer);
    return FormattingResult::Success;
  }
}

#endif
