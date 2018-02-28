#ifndef CPP_MOULD_ARGUMENT_HPP
#define CPP_MOULD_ARGUMENT_HPP
/* Definitions required for something to become an argument to the execution engine
 * Also, definitions for all the standard arguments, ints, floats, strings, etc.
 */

#include "format.hpp"

namespace mould {

  // Default return type which signals that the formatting is not possible.
  // Through this, a specialization might also be explicitely disallowed.
  struct NotImplemented;

  // Return value of a valid formatting function.
  enum struct FormattingResult {
    Success,
    Error,
  };

  enum struct AutoFormattingChoice {
    NoAuto,
    Decimal,
    Binary,
    Octal,
    Hex,
    HEX,
    Exponent,
    EXPONENT,
    Fpoint,
    FPOINT,
    Point,
    String,
  };

  template<AutoFormattingChoice choice>
  struct AutoFormat { static constexpr AutoFormattingChoice value = choice; };

  // The argument here is always nullptr and only used for deciding the callee.
  template<typename T>
  AutoFormat<AutoFormattingChoice::NoAuto> format_auto(const T&);

  template<typename T>
  NotImplemented format_decimal(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_binary(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_octal(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_hex(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_HEX(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_exponent(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_EXPONENT(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_fpoint(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_FPOINT(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_pointer(const T& value, Formatter formatter);

  template<typename T>
  NotImplemented format_string(const T& value, Formatter formatter);

  /* Standard definition for int */
  AutoFormat<AutoFormattingChoice::Decimal> format_auto(const int&);

  FormattingResult format_decimal(const int& value, Formatter formatter) {
    formatter.append("Some int");
    return FormattingResult::Success;
  }

  /* Standard implementation for const char* */
  AutoFormat<AutoFormattingChoice::String> format_auto(const char*);

  FormattingResult format_string(const char* value, Formatter formatter) {
    formatter.append(value);
    return FormattingResult::Success;
  }
}

#endif
