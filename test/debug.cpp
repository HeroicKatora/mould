#include <iostream>
#include <iterator>

#include <cpp_mould.hpp>

static constexpr char format[] = "{:0.10f}:{:04d}:{:+}:{:s}:{:p}:{:c}:%\n";

template<typename Formatter>
auto dump_bytecode(const Formatter& formatter) {
  std::cout << "Bytecode\n";
  for(const auto i : formatter.data.code) {
    std::cout << (unsigned int) i << "\n";
  }
  std::cout << "Immediates\n";
  for(const auto i : formatter.data.immediates) {
    std::cout << i << "\n";
  }
}

int main() {
  constexpr auto formatter = mould::compile<format>();

  dump_bytecode(formatter);

  auto description = mould::descriptor(formatter);
  while(description) {
    std::cout << *description << "\n";
  }
}
