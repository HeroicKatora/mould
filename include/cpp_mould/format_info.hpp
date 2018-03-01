#ifndef CPP_MOULD_FORMAT_INFO_HPP
#define CPP_MOULD_FORMAT_INFO_HPP

#include "format.hpp"

namespace mould::internal {

  template<typename Ignore, typename Then>
  struct Validate { using type = Then; };
  template<typename Then>
  struct Validate<NotImplemented, Then>;

  template<typename T>
  inline auto uniq_decimal_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_decimal(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_decimal(val, formatter);
  }

  template<typename T>
  constexpr auto decimal_formatter(int)
  -> decltype(uniq_decimal_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_decimal_formatter<T>;
  }

  template<typename T>
  constexpr auto decimal_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }


  template<typename T>
  inline auto uniq_string_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_string(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_string(val, formatter);
  }

  template<typename T>
  constexpr auto string_formatter(int)
  -> decltype(uniq_string_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_string_formatter<T>;
  }

  template<typename T>
  constexpr auto string_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }

  template<typename T>
  constexpr auto auto_formatter() {
    constexpr auto selection = decltype(format_auto(std::declval<const T&>()))::value;
    if constexpr(selection == AutoFormattingChoice::Decimal) {
      return decimal_formatter<T>(0);
    } else if constexpr(selection == AutoFormattingChoice::String) {
      return string_formatter<T>(0);
    } else {
      return nullptr;
    }
  }

  template<typename T>
  struct TypedFormatter {
    using formatting_function = FormattingResult (*)(const T&, Formatter);
    constexpr static formatting_function automatic = auto_formatter<T>();
    constexpr static formatting_function decimal = decimal_formatter<T>(0);
    constexpr static formatting_function string = string_formatter<T>(0);
  };

  using type_erase_formatting_function = FormattingResult (*)(const void*, Formatter);

  template<typename T>
  struct type_erase_function {
    static FormattingResult format_auto(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::automatic == nullptr) {
        return FormattingResult::Success;
      } else {
        return TypedFormatter<T>::automatic(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_decimal(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::decimal == nullptr) {
        return FormattingResult::Success;
      } else {
        return TypedFormatter<T>::decimal(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_string(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::string == nullptr) {
        return FormattingResult::Success;
      } else {
        return TypedFormatter<T>::string(*reinterpret_cast<const T*>(self), formatter);
      }
    }
  };

  struct TypeErasedFormatter {
    template<typename T>
    constexpr static TypeErasedFormatter Construct() {
      return TypeErasedFormatter {
        type_erase_function<T>::format_auto,
        type_erase_function<T>::format_decimal,
        type_erase_function<T>::format_string,
      };
    }

    type_erase_formatting_function automatic;
    type_erase_formatting_function decimal;
    type_erase_formatting_function string;

    constexpr type_erase_formatting_function formatter_for(FormatKind kind) const {
      switch(kind) {
      case FormatKind::Auto: return automatic;
      case FormatKind::Decimal: return decimal;
      case FormatKind::String: return string;
      }
    }
  };

}

#endif
