#ifndef CPP_MOULD_FORMAT_INFO_HPP
#define CPP_MOULD_FORMAT_INFO_HPP

#include "format.hpp"

namespace mould::internal {

  template<typename Ignore, typename Then>
  struct Validate { using type = Then; };
  template<typename Then>
  struct Validate<NotImplemented, Then>;

#ifdef CPP_MOULD_DELAYED_FORMATTER
#error Trying #undef CPP_MOULD_DELAYED_FORMATTER before including this file
#endif
#define CPP_MOULD_DELAYED_FORMATTER(kind) \
  template<typename T> \
  inline auto uniq_##kind##_formatter(const T& val, Formatter formatter) \
  -> typename Validate<decltype(format_##kind(std::declval<const T&>(), std::declval<Formatter>())), FormattingResult>::type { \
    return format_##kind(val, formatter); \
  } \
 \
  template<typename T> \
  constexpr auto kind##_formatter(int) \
  -> decltype(uniq_##kind##_formatter(std::declval<const T&>(), std::declval<Formatter>()))(*)(const T&, Formatter) { \
    return uniq_##kind##_formatter<T>; \
  } \
 \
  template<typename T> \
  constexpr auto kind##_formatter(...) -> FormattingResult (*)(const T&, Formatter) { \
    return nullptr; \
  } \

CPP_MOULD_DELAYED_FORMATTER(decimal)
CPP_MOULD_DELAYED_FORMATTER(binary)
CPP_MOULD_DELAYED_FORMATTER(octal)
CPP_MOULD_DELAYED_FORMATTER(hex)
CPP_MOULD_DELAYED_FORMATTER(HEX)
CPP_MOULD_DELAYED_FORMATTER(exponent)
CPP_MOULD_DELAYED_FORMATTER(EXPONENT)
CPP_MOULD_DELAYED_FORMATTER(fpoint)
CPP_MOULD_DELAYED_FORMATTER(FPOINT)
CPP_MOULD_DELAYED_FORMATTER(pointer)
CPP_MOULD_DELAYED_FORMATTER(string)

#undef CPP_MOULD_DELAYED_FORMATTER

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
#define CPP_MOULD_AUTO_CHOICE(Kind, kind) if constexpr(ChoiceT::value == AutoFormattingChoice:: Kind) { return kind##_formatter<T>(0); } else
    CPP_MOULD_AUTO_CHOICE(Decimal, decimal)
    CPP_MOULD_AUTO_CHOICE(Binary, binary)
    CPP_MOULD_AUTO_CHOICE(Octal, octal)
    CPP_MOULD_AUTO_CHOICE(Hex, hex)
    CPP_MOULD_AUTO_CHOICE(HEX, HEX)
    CPP_MOULD_AUTO_CHOICE(Exponent, exponent)
    CPP_MOULD_AUTO_CHOICE(EXPONENT, EXPONENT)
    CPP_MOULD_AUTO_CHOICE(Fpoint, fpoint)
    CPP_MOULD_AUTO_CHOICE(FPOINT, FPOINT)
    CPP_MOULD_AUTO_CHOICE(Pointer, pointer)
    CPP_MOULD_AUTO_CHOICE(String, string)
#undef CPP_MOULD_AUTO_CHOICE
    /* else */ {
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
#define CPP_MOULD_TYPE_ERASED_FORMAT(kind) \
    static FormattingResult format_##kind(const void* self, Formatter formatter) { \
      if constexpr(TypedFormatter<T>:: kind == nullptr) { \
        return FormattingResult::Error; \
      } else { \
        return TypedFormatter<T>:: kind(*reinterpret_cast<const T*>(self), formatter); \
      } \
    }

    CPP_MOULD_TYPE_ERASED_FORMAT(automatic)
    CPP_MOULD_TYPE_ERASED_FORMAT(decimal)
    CPP_MOULD_TYPE_ERASED_FORMAT(binary)
    CPP_MOULD_TYPE_ERASED_FORMAT(octal)
    CPP_MOULD_TYPE_ERASED_FORMAT(hex)
    CPP_MOULD_TYPE_ERASED_FORMAT(HEX)
    CPP_MOULD_TYPE_ERASED_FORMAT(exponent)
    CPP_MOULD_TYPE_ERASED_FORMAT(EXPONENT)
    CPP_MOULD_TYPE_ERASED_FORMAT(fpoint)
    CPP_MOULD_TYPE_ERASED_FORMAT(FPOINT)
    CPP_MOULD_TYPE_ERASED_FORMAT(pointer)
    CPP_MOULD_TYPE_ERASED_FORMAT(string)
#undef CPP_MOULD_TYPE_ERASED_FORMAT
  };

  struct TypeErasedFormatter {
    template<typename T>
    constexpr static TypeErasedFormatter Construct() {
      return TypeErasedFormatter {
        type_erase_function<T>::format_automatic,
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
