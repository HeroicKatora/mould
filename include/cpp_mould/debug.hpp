#ifndef CPP_MOULD_DEBUG_HPP
#define CPP_MOULD_DEBUG_HPP
#include <iterator>
#include <sstream>

#include "bytecode.hpp"

namespace mould::internal {
  template<size_t N>
  constexpr ByteCodeBuffer byte_code_buffer(const Codepoint(&buffer)[N]) {
    return { std::begin(buffer), std::end(buffer) };
  }

  template<size_t N>
  constexpr ImmediateBuffer immediate_buffer(const Immediate(&buffer)[N]) {
    return { std::begin(buffer), std::end(buffer) };
  }

  constexpr const char* describe(ReadStatus status) {
    switch(status) {
    case ReadStatus::NoError: return "NoError";
    case ReadStatus::MissingOpcode: return "MissingOpcode";
    case ReadStatus::MissingFormatImmediate: return "MissingFormatImmediate";
    case ReadStatus::MissingLiteralImmediate: return "MissingLiteralImmediate";
    case ReadStatus::MissingWidth: return "MissingWidth";
    case ReadStatus::MissingPrecision: return "MissingPrecision";
    case ReadStatus::MissingPadding: return "MissingPadding";
    case ReadStatus::InvalidIndex: return "InvalidIndex";
    case ReadStatus::InvalidOpcode: return "InvalidOpcode";
    case ReadStatus::InvalidFormatImmediate: return "InvalidFormatImmediate";
    }
  }

  constexpr const char* describe(ImmediateValue val) {
    if(val == ImmediateValue::Auto) return "Auto";
    else return "Immediate";
  }

  constexpr const char* describe(FormatKind kind) {
    switch(kind) {
    case FormatKind::Auto: return "Auto";
#define CPP_MOULD_DESCRIBE_FORMAT_KIND(kind)\
    case FormatKind:: kind: return  #kind;

    CPP_MOULD_REPEAT_FOR_FORMAT_KINDS_MACRO(CPP_MOULD_DESCRIBE_FORMAT_KIND)
#undef CPP_MOULD_DESCRIBE_FORMAT_KIND
    }
  }

  constexpr const char* describe(InlineValue kind) {
    switch(kind) {
    case InlineValue::Auto: return "Auto";
    case InlineValue::Immediate: return "Immediate";
    case InlineValue::Inline: return "Inline";
    case InlineValue::Parameter: return "Parameter";
    }
  }

  constexpr const char* describe(FormatArgument kind) {
    switch(kind) {
    case FormatArgument::Auto: return "Auto";
    case FormatArgument::Value: return "Value";
    case FormatArgument::Parameter: return "Parameter";
    }
  }

  constexpr const char* describe(Alignment align) {
    switch(align) {
    case Alignment::Default: return "Default";
    case Alignment::Left: return "Left";
    case Alignment::Right: return "Right";
    case Alignment::Center: return "Center";
    }
  }

  constexpr const char* describe(Sign sign) {
    switch(sign) {
    case Sign::Default: return "Default";
    case Sign::Always: return "Always";
    case Sign::Pad: return "Pad";
    }
  }

  template<typename CharT = const char>
  std::string describe_operation(
    const Buffer<CharT>& format_string,
    FullOperationIterator& operation
  ) {
    if(operation.status != ReadStatus::NoError)
      return describe(operation.status);

    auto latest = *operation;
    switch(latest.operation.operation.type) {
    case OpCode::Literal:
      return std::string("Literal: \"")
             + std::string(
                format_string.begin() + latest.literal.offset,
                latest.literal.length)
             + "\"";
    case OpCode::Insert: {
      std::stringstream description(std::string{});
      description << "Insert: format (" << describe(latest.operation.operation.insert_format);
      description << ") index (" << describe(latest.formatting.index)
                  << ", " << (int) latest.formatting.index_value;
      description << ") kind (" << describe(latest.formatting.kind);
      description << ") width (" << describe(latest.formatting.width)
                  << ", " << latest.formatting.width_value;
      description << ") precision (" << describe(latest.formatting.precision)
                  << ", " << latest.formatting.precision_value;
      description << ") padding (" << describe(latest.formatting.padding)
                  << ", " << latest.formatting.padding_value;
      description << ") alignment (" << describe(latest.formatting.alignment);
      description << ") sign (" << describe(latest.formatting.sign);
      description << ")";
      return description.str();
    }
    default:
      return "!!!Unknown op code";
    }
  }

  template<typename CharT>
  struct Descriptor {
    Buffer<const CharT> format_buffer;
    FullOperationIterator iterator;
    bool error_read;

    constexpr Descriptor(const TypeErasedByteCode<CharT>& formatter) :
        format_buffer { formatter.format_buffer() },
        iterator { formatter.code_buffer(), formatter.immediate_buffer() },
        error_read(true)
        { }

    constexpr bool empty() const {
      return iterator.code_buffer.empty() && error_read;
    };

    constexpr operator bool() const {
      return !empty();
    }

    std::string operator*() {
      if(iterator.status != ReadStatus::NoError) {
        error_read = true;
      }
      return describe_operation<const CharT>(format_buffer, iterator);
    }

    ReadStatus operator++() {
      if(iterator.status != ReadStatus::NoError) {
        return iterator.status;
      }
      ++iterator;
      error_read = iterator.status == ReadStatus::NoError;
      return iterator.status;
    }
  };
}

namespace mould {
  template<typename CharT>
  constexpr auto descriptor(const internal::TypeErasedByteCode<CharT>& formatter)
  -> internal::Descriptor<CharT> {
    return internal::Descriptor<CharT> { formatter };
  }
}

#endif
