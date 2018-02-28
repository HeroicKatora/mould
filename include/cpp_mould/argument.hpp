#ifndef CPP_MOULD_ARGUMENT_HPP
#define CPP_MOULD_ARGUMENT_HPP
/* Definitions required for something to become an argument to the execution engine
 * Also, definitions for all the standard arguments, ints, floats, strings, etc.
 */

#include "format.hpp"

namespace mould {
  namespace {
    // A signalling type. This enables us the declare the functions and
    // confidently not rely on SFINAE for everything.
    struct NotImplemented;
    using Implemented = bool;

    struct AutoDecimal;
    struct AutoBinary;
    struct AutoOctal;
  }

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
  bool format_decimal(const int& value, Formatter formatter) {
    formatter.append("Some int");
  }
}

#endif
