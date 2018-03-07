#ifndef CPP_MOULD_ARGUMENTS_INT_HPP
#define CPP_MOULD_ARGUMENTS_INT_HPP
#include <climits>

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
      formatter.append("0");
      return FormattingResult::Success;
    }

    int pre_length = (pvalue < 0 || formatter.format().sign != internal::Sign::Default) ? 1 : 0;
    // This convoluted mess avoids the failure on -MAX_INT
    const unsigned value = (pvalue < 0) ? (~static_cast<unsigned>(pvalue)) + static_cast<unsigned>(1) : pvalue;

    static_assert(~static_cast<unsigned>(INT_MIN) + 1 == 2147483648u);
    constexpr static unsigned comparisons[] = // -INT_MIN = 2147483648
      {10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

    int val_length = 1;
    for(auto cmp : comparisons) {
      val_length += (value >= cmp) ? 1 : 0;
    }
    val_length = std::max(val_length, (int) formatter.format().width);

    const auto view = formatter.reserve(pre_length + val_length);

    unsigned iterval = value;
    for(;;) {
      if(iterval > 0) view[pre_length + val_length - 1] = '0' + (iterval % 10);
      if(iterval > 10) view[pre_length + val_length - 2] = '0' + ((iterval/10) % 10);
      if(iterval > 100) view[pre_length + val_length - 3] = '0' + ((iterval/100) % 10);
      if(iterval > 1000) view[pre_length + val_length - 4] = '0' + ((iterval/1000) % 10);
      iterval /= 10000;
      val_length -= 4;
      if(val_length <= 0) break;
    }

    if(pvalue < 0) view[0] = '-';
    else if(formatter.format().sign == internal::Sign::Always) view[0] = '+';
    else if(formatter.format().sign == internal::Sign::Pad) view[0] = ' ';

    return FormattingResult::Success;
  }
}

#endif
