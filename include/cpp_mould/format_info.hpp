#ifndef CPP_MOULD_FORMAT_INFO_HPP
#define CPP_MOULD_FORMAT_INFO_HPP

#include "format.hpp"

namespace mould::internal {

  template<typename T>
  constexpr auto build_formatter(FormattingResult (*fn)(const T&, Formatter)) {
    return fn;
  }

  /* Signal that the validated type itself should be the Then type */
  struct _validated_type;

  /* Validate the result first type is not `NotImplemented`, then return the second type or the first */
  template<typename Result, typename Then>
  struct Validate { using type = Then; };
  template<typename Result>
  struct Validate<Result, _validated_type> { using type = Result; };
  template<typename Then>
  struct Validate<NotImplemented, Then>;
  template<>
  struct Validate<NotImplemented, _validated_type>;

#ifdef CPP_MOULD_DELAYED_FORMATTER
#error Trying #undef CPP_MOULD_DELAYED_FORMATTER before including this file
#endif
#define CPP_MOULD_DELAYED_FORMATTER(kind) \
  template<typename T> \
  inline auto uniq_##kind##_formatter(const T& val, Formatter formatter) \
  -> typename Validate<decltype(format_##kind(std::declval<const T&>(), std::declval<Formatter>())), _validated_type>::type { \
    return format_##kind(val, formatter); \
  } \
 \
  template<typename T> \
  constexpr auto kind##_formatter(int) \
  -> decltype(build_formatter(uniq_##kind##_formatter<T>)) { \
    return build_formatter(uniq_##kind##_formatter<T>); \
  } \
 \
  template<typename T> \
  constexpr auto kind##_formatter(...) -> FormattingResult (*)(const T&, Formatter) { \
    return nullptr; \
  } \

CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_DELAYED_FORMATTER)
#undef CPP_MOULD_DELAYED_FORMATTER

  struct Choice { };

  template<typename T>
  inline auto uniq_automatic_formatter(const T& val, Choice choice)
  -> decltype(format_auto(std::declval<const T&>(), std::declval<Formatter>())) {
    return decltype(format_auto(val, choice)){ };
  }

  template<typename T>
  constexpr auto automatic_formatter(typename Validate<decltype(uniq_automatic_formatter(std::declval<const T&>(), std::declval<Choice>())), int>::type) {
    using ChoiceT = decltype(uniq_automatic_formatter(std::declval<const T&>(), std::declval<Choice>()));

#define CPP_MOULD_AUTO_CHOICE(kind)\
    if constexpr(ChoiceT::value == AutoFormattingChoice:: kind) { return kind##_formatter<T>(0); } else

    CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_AUTO_CHOICE)
#undef CPP_MOULD_AUTO_CHOICE
    /* else */ {
    	return nullptr;
    }
  }

  template<typename T>
  constexpr auto automatic_formatter(...) -> FormattingResult (*)(const T&, Formatter) {
    return nullptr;
  }

  template<typename T>
  struct TypedFormatter {
    using formatting_function = FormattingResult (*)(const T&, Formatter);
#define CPP_MOULD_TYPED_CONSTEXPR(kind)\
    constexpr static formatting_function kind = kind##_formatter<T>(0);

    CPP_MOULD_TYPED_CONSTEXPR(automatic)
    CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_TYPED_CONSTEXPR)
#undef CPP_MOULD_TYPED_CONSTEXPR
  };

  /* Specialization point for addition information. Every type that should be formatted 
   * MUST provide an implemenation of this.
   */
  template<typename T>
  struct TypedFormatterInformation {
    typename TypedFormatter<T>::formatting_function function;
    constexpr static TypedFormatterInformation get(FullOperation operation) {
      TypedFormatterInformation<T> info = {};
      switch(operation.formatting.kind) {
      case FormatKind::Auto: info.function = TypedFormatter<T>::automatic; break;
#define CPP_MOULD_TYPED_FORMATTER_TYPE_SWITCH(kind) \
      case FormatKind:: kind : info.function = TypedFormatter<T>:: kind; break;

      CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_TYPED_FORMATTER_TYPE_SWITCH)
#undef CPP_MOULD_TYPED_FORMATTER_TYPE_SWITCH
      }
      return info;
    }
  };

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
    CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_TYPE_ERASED_FORMAT)
#undef CPP_MOULD_TYPE_ERASED_FORMAT
  };

  using type_erased_formatting_function = FormattingResult (*)(const void*, Formatter);

  struct TypeErasedFormatter {

#   define CPP_MOULD_TYPE_ERASED_FORMATTER_MEMBER(kind)\
    type_erased_formatting_function kind;

      CPP_MOULD_TYPE_ERASED_FORMATTER_MEMBER(automatic)
      CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_TYPE_ERASED_FORMATTER_MEMBER)
#   undef CPP_MOULD_TYPE_ERASED_FORMATTER_MEMBER

    template<typename T>
    constexpr static TypeErasedFormatter Construct() {

      return TypeErasedFormatter {
        #define CPP_MOULD_TYPE_ERASED_FORMATTER_INIT(kind)\
        type_erase_function<T>::format_##kind,

        CPP_MOULD_TYPE_ERASED_FORMATTER_INIT(automatic)
        CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_TYPE_ERASED_FORMATTER_INIT)
        #undef CPP_MOULD_TYPE_ERASED_FORMATTER_INIT
      };
    }

    constexpr type_erased_formatting_function formatter_for(FormatKind kind) const {
      switch(kind) {
      case FormatKind::Auto: return automatic;
#     define CPP_MOULD_TYPE_ERASED_FORMATTER_SWITCH_CASE(kind)\
      case FormatKind:: kind : return kind;

      CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_TYPE_ERASED_FORMATTER_SWITCH_CASE)
#     undef CPP_MOULD_TYPE_ERASED_FORMATTER_SWITCH_CASE
      }
    }
  };

}

#endif
