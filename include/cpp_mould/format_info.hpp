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
  inline auto uniq_binary_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_binary(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_binary(val, formatter);
  }

  template<typename T>
  constexpr auto binary_formatter(int)
  -> decltype(uniq_binary_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_binary_formatter<T>;
  }

  template<typename T>
  constexpr auto binary_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }


  template<typename T>
  inline auto uniq_octal_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_octal(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_octal(val, formatter);
  }

  template<typename T>
  constexpr auto octal_formatter(int)
  -> decltype(uniq_octal_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_octal_formatter<T>;
  }

  template<typename T>
  constexpr auto octal_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }


  template<typename T>
  inline auto uniq_hex_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_hex(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_hex(val, formatter);
  }

  template<typename T>
  constexpr auto hex_formatter(int)
  -> decltype(uniq_hex_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_hex_formatter<T>;
  }

  template<typename T>
  constexpr auto hex_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }


  template<typename T>
  inline auto uniq_HEX_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_HEX(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_HEX(val, formatter);
  }

  template<typename T>
  constexpr auto HEX_formatter(int)
  -> decltype(uniq_HEX_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_HEX_formatter<T>;
  }

  template<typename T>
  constexpr auto HEX_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }


  template<typename T>
  inline auto uniq_exponent_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_exponent(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_exponent(val, formatter);
  }

  template<typename T>
  constexpr auto exponent_formatter(int)
  -> decltype(uniq_exponent_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_exponent_formatter<T>;
  }

  template<typename T>
  constexpr auto exponent_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }


  template<typename T>
  inline auto uniq_EXPONENT_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_EXPONENT(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_EXPONENT(val, formatter);
  }

  template<typename T>
  constexpr auto EXPONENT_formatter(int)
  -> decltype(uniq_EXPONENT_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_EXPONENT_formatter<T>;
  }

  template<typename T>
  constexpr auto EXPONENT_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }


  template<typename T>
  inline auto uniq_fpoint_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_fpoint(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_fpoint(val, formatter);
  }

  template<typename T>
  constexpr auto fpoint_formatter(int)
  -> decltype(uniq_fpoint_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_fpoint_formatter<T>;
  }

  template<typename T>
  constexpr auto fpoint_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }


  template<typename T>
  inline auto uniq_FPOINT_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_FPOINT(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_FPOINT(val, formatter);
  }

  template<typename T>
  constexpr auto FPOINT_formatter(int)
  -> decltype(uniq_FPOINT_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_FPOINT_formatter<T>;
  }

  template<typename T>
  constexpr auto FPOINT_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }


  template<typename T>
  inline auto uniq_pointer_formatter(const T& val, Formatter formatter)
  -> typename Validate<decltype(format_pointer(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type {
    return format_pointer(val, formatter);
  }

  template<typename T>
  constexpr auto pointer_formatter(int)
  -> decltype(uniq_pointer_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) {
    return uniq_pointer_formatter<T>;
  }

  template<typename T>
  constexpr auto pointer_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
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

  struct Choice { };

  template<typename T>
  inline auto uniq_auto_formatter(const T& val, Choice choice)
  -> decltype(format_auto(std::declval<const T&>(), std::declval<Formatter>())) {
    return decltype(format_auto(val, choice)){ };
  }

  template<typename T>
  constexpr auto auto_formatter(int)
  -> typename Validate<decltype(uniq_auto_formatter(std::declval<const T&>(), std::declval<Choice>())), FormattingResult>::type (*)(const T&, Formatter) {
    using ChoiceT = decltype(uniq_auto_formatter(std::declval<const T&>(), std::declval<Choice>()));
    if constexpr(ChoiceT::value == AutoFormattingChoice::Decimal) {
      return decimal_formatter<T>(0);
    } else if constexpr(ChoiceT::value == AutoFormattingChoice::String) {
      return string_formatter<T>(0);
    } else {
      return nullptr;
    }
  }

  template<typename T>
  constexpr auto auto_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }

  template<typename T>
  struct TypedFormatter {
    using formatting_function = FormattingResult (*)(const T&, Formatter);
    constexpr static formatting_function automatic = auto_formatter<T>(0);
    constexpr static formatting_function decimal = decimal_formatter<T>(0);
    constexpr static formatting_function binary = binary_formatter<T>(0);
    constexpr static formatting_function octal = octal_formatter<T>(0);
    constexpr static formatting_function hex = hex_formatter<T>(0);
    constexpr static formatting_function HEX = HEX_formatter<T>(0);
    constexpr static formatting_function exponent = exponent_formatter<T>(0);
    constexpr static formatting_function EXPONENT = EXPONENT_formatter<T>(0);
    constexpr static formatting_function fpoint = fpoint_formatter<T>(0);
    constexpr static formatting_function FPOINT = FPOINT_formatter<T>(0);
    constexpr static formatting_function pointer = pointer_formatter<T>(0);
    constexpr static formatting_function string = string_formatter<T>(0);
  };

  using type_erase_formatting_function = FormattingResult (*)(const void*, Formatter);

  template<typename T>
  struct type_erase_function {
    static FormattingResult format_auto(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::automatic == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::automatic(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_decimal(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::decimal == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::decimal(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_binary(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::binary == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::binary(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_octal(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::octal == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::octal(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_hex(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::hex == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::hex(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_HEX(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::HEX == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::HEX(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_exponent(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::exponent == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::exponent(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_EXPONENT(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::EXPONENT == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::EXPONENT(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_fpoint(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::fpoint == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::fpoint(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_FPOINT(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::FPOINT == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::FPOINT(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_pointer(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::pointer == nullptr) {
        return FormattingResult::Error;
      } else {
        return TypedFormatter<T>::pointer(*reinterpret_cast<const T*>(self), formatter);
      }
    }

    static FormattingResult format_string(const void* self, Formatter formatter) {
      if constexpr(TypedFormatter<T>::string == nullptr) {
        return FormattingResult::Error;
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
