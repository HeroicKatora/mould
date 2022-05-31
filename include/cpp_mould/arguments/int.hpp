#ifndef CPP_MOULD_ARGUMENTS_INT_HPP
#define CPP_MOULD_ARGUMENTS_INT_HPP
#include <limits>

#include "../argument.hpp"

namespace mould {
  /* Standard definition for int */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::decimal> format_auto(const int&, const Choice& choice) {
    return AutoFormatting<AutoFormattingChoice::decimal> { };
  }

  template<typename Formatter>
  FormattingResult format_decimal(const int& pvalue, Formatter formatter) {
    if(pvalue == 0) {
      formatter.append('0');
      return FormattingResult::Success;
    }

    int pre_length = (pvalue < 0 || formatter.format().sign != internal::Sign::Default) ? 1 : 0;
    // This convoluted mess avoids the failure on -MAX_INT
    const unsigned value = (pvalue < 0) ? (~static_cast<unsigned>(pvalue)) + static_cast<unsigned>(1) : pvalue;

    static_assert(~static_cast<unsigned>(std::numeric_limits<int>::min()) + 1 == 2147483648u);
    constexpr static unsigned comparisons[] = // -INT_MIN = 2147483648
      {10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

    int number_length = 1;
    for(auto cmp : comparisons) {
      number_length += (value >= cmp) ? 1 : 0;
    }
    const unsigned int formatted_length = number_length;

    int remaining_length = formatter.format().width - number_length;

    if(pvalue < 0) formatter.append('-');
    else if(formatter.format().sign == internal::Sign::Always) formatter.append('+');
    else if(formatter.format().sign == internal::Sign::Pad) formatter.append(' ');

    const char padding = formatter.format().has_padding ? (char) formatter.format().padding : ' ';
    for(int i = 0; i < remaining_length; i++) formatter.append(padding);

    char view[std::numeric_limits<int>::digits10] = {};
    char* result_buffer = formatter.show_buf(sizeof(view));
    result_buffer = result_buffer ? result_buffer : view;
    const auto start = result_buffer;

    for(unsigned iterval = value;;) {
      if(iterval > 0) result_buffer[number_length - 1] = '0' + (iterval % 10);
      if(iterval > 10) result_buffer[number_length - 2] = '0' + ((iterval/10) % 10);
      if(iterval > 100) result_buffer[number_length - 3] = '0' + ((iterval/100) % 10);
      if(iterval > 1000) result_buffer[number_length - 4] = '0' + ((iterval/1000) % 10);
      iterval /= 10000;
      number_length -= 4;
      if(number_length <= 0) break;
    }

    if (start == view) {
      formatter.append(std::string_view{view, formatted_length});
    } else {
      formatter.put_buf(formatted_length);
    }

    return FormattingResult::Success;
  }
}

#endif
