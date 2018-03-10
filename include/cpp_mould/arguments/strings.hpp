#ifndef CPP_MOULD_ARGUMENTS_STRINGS_HPP
#define CPP_MOULD_ARGUMENTS_STRINGS_HPP
#include "../format.hpp"

namespace mould {
  /* Standard implementation for const char* */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::string> format_auto(const char*, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::string> { };
  }

  template<typename Formatter>
  FormattingResult format_string(const char* value, Formatter formatter) {
    formatter.append(value);
    return FormattingResult::Success;
  }

  /* Standard implementation for const char&[N] */
  template<size_t N, typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::string> format_auto(const char(&val)[N], Choice choice) {
    return AutoFormatting<AutoFormattingChoice::string> { };
  }

  template<size_t N, typename Formatter>
  FormattingResult format_string(const char(&value)[N], Formatter formatter) {
    formatter.append(std::string_view{value, N});
    return FormattingResult::Success;
  }

  /* Standard implementation for char */
  template<typename Choice>
  constexpr AutoFormatting<AutoFormattingChoice::character> format_auto(char, Choice choice) {
    return AutoFormatting<AutoFormattingChoice::character> { };
  }

  template<typename Formatter>
  FormattingResult format_string(const char value, Formatter formatter) {
    formatter.append(value);
    return FormattingResult::Success;
  }

  template<typename Formatter>
  FormattingResult format_character(const char value, Formatter formatter) {
    formatter.append(value);
    return FormattingResult::Success;
  }
}

#endif
