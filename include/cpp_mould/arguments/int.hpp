#ifndef CPP_MOULD_ARGUMENTS_INT_HPP
#define CPP_MOULD_ARGUMENTS_INT_HPP
#include <climits>
#include <iostream>

#include "../argument.hpp"

namespace mould {
  /* Standard definition for int */
  AutoFormat<AutoFormattingChoice::Decimal> format_auto(const int&);

  FormattingResult format_decimal(const int& pvalue, Formatter formatter) {
    if(pvalue == 0) {
      formatter.append("0");
      return FormattingResult::Success;
    }

    int length = pvalue < 0 ? 2 : 1;
    // This convoluted mess avoids the failure on -MAX_INT
    const unsigned value = (pvalue < 0) ? (~static_cast<unsigned>(pvalue)) + static_cast<unsigned>(1) : pvalue;

    static_assert(~static_cast<unsigned>(INT_MIN) + 1 == 2147483648u);
    constexpr static unsigned comparisons[] = // -INT_MIN = 2147483648
      {10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

    for(auto cmp : comparisons) {
      length += (value >= cmp) ? 1 : 0;
    }

    const auto view = formatter.reserve(length);

    unsigned iterval = value;
    for(;;) {
      if(iterval > 0) view[length - 1] = '0' + (iterval % 10);
      if(iterval > 10) view[length - 2] = '0' + ((iterval/10) % 10);
      if(iterval > 100) view[length - 2] = '0' + ((iterval/100) % 10);
      if(iterval > 1000) view[length - 2] = '0' + ((iterval/1000) % 10);
      iterval /= 10000;
      length -= 4;
      if(iterval == 0) break;
    }

    if(pvalue < 0) view[0] = '-';

    return FormattingResult::Success;
  }
}

#endif
