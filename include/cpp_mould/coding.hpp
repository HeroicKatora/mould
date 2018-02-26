#ifndef CPP_MOULD_CODING_HPP
#define CPP_MOULD_CODING_HPP
/* Interpretable values and their encoding */

namespace mould {
  static_assert(sizeof(size_t) == sizeof(void*));
  static_assert(sizeof(size_t) == sizeof(const char*));

  struct EncodedStringLiteral {
    size_t begin;
    size_t length;

    template<typename CharT = const char>
    constexpr EncodedStringLiteral(CharT* begin, CharT* end)
      : begin((size_t) begin), length(end - begin)
      { }

    template<typename CharT = const char>
    constexpr CharT* begin_ptr() const { return (CharT) begin; }

    template<typename CharT = const char>
    constexpr CharT* end_ptr() const { return (CharT) length; }
  };
}

#endif
