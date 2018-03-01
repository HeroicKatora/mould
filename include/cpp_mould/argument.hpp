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
  AutoFormat<AutoFormattingChoice::NoAuto> format_auto(const NotImplemented&);

  NotImplemented format_decimal(const NotImplemented& value, Formatter formatter);

  NotImplemented format_binary(const NotImplemented& value, Formatter formatter);

  NotImplemented format_octal(const NotImplemented& value, Formatter formatter);

  NotImplemented format_hex(const NotImplemented& value, Formatter formatter);

  NotImplemented format_HEX(const NotImplemented& value, Formatter formatter);

  NotImplemented format_exponent(const NotImplemented& value, Formatter formatter);

  NotImplemented format_EXPONENT(const NotImplemented& value, Formatter formatter);

  NotImplemented format_fpoint(const NotImplemented& value, Formatter formatter);

  NotImplemented format_FPOINT(const NotImplemented& value, Formatter formatter);

  NotImplemented format_pointer(const NotImplemented& value, Formatter formatter);

  NotImplemented format_string(const NotImplemented& value, Formatter formatter);
}

#endif
