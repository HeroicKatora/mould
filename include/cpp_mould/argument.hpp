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

  // The result of a formatter which carries compile time information about
  // the usage of the formatter engine etc.
  template<typename Information>
  struct ResultWithInformation {
    FormattingResult result;
    constexpr ResultWithInformation(FormattingResult result)
      : result(result) { }
    constexpr operator FormattingResult() { return result; }
    
    /* Forwards the user defined max_width, gives documentation */
    static constexpr int max_width() { return Information::max_width; }
  };

  enum struct AutoFormattingChoice {
    NO_AUTO,

    decimal,
    binary,
    octal,
    hex,
    HEX,
    exponent,
    EXPONENT,
    fpoint,
    FPOINT,
    pointer,
    string,
    character,
  };

  template<AutoFormattingChoice choice>
  struct AutoFormatting {
    constexpr static AutoFormattingChoice value = choice;
  };

  // The argument here is always nullptr and only used for deciding the callee.
  template<typename Choice>
  NotImplemented format_auto(const NotImplemented&, Choice choice);

  template<typename Formatter>
  NotImplemented format_decimal(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_binary(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_octal(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_hex(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_HEX(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_exponent(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_EXPONENT(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_fpoint(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_FPOINT(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_pointer(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_string(const NotImplemented& value, Formatter formatter);

  template<typename Formatter>
  NotImplemented format_character(const NotImplemented& value, Formatter formatter);
}

#endif
