#ifndef CPP_MOULD_ARGUMENT_HPP
#define CPP_MOULD_ARGUMENT_HPP
/* Definitions required for something to become an argument to the execution engine
 * Also, definitions for all the standard arguments, ints, floats, strings, etc.
 */

#include "format.hpp"

namespace mould {
  struct NotImplemented;
  using Implemented = bool;

  struct NoAuto;
  struct AutoDecimal;
  struct AutoString;

  template<typename T>
  NoAuto format_auto(const T&);

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
  AutoDecimal format_auto(const int&);

  bool format_decimal(const int& value, Formatter formatter) {
    formatter.append("Some int");
  }
}

#endif
