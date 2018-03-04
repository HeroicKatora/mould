#include <iostream>
#include <iterator>

#include <cpp_mould/compile.hpp>
#include <cpp_mould/debug.hpp>

using namespace mould;

static constexpr char format[] = "Hello {s}\n";

template<typename Formatter>
auto dump_bytecode(const Formatter& formatter) {
  std::cout << "Bytecode\n";
  for(const auto i : formatter.code) {
    std::cout << (unsigned int) i << "\n";
  }
  std::cout << "Immediates\n";
  for(const auto i : formatter.immediates) {
    std::cout << i << "\n";
  }
}

int main() {
  constexpr auto formatter = compile<format>();

  dump_bytecode(formatter);

  auto description = descriptor(formatter);
  while(description) {
    std::cout << *description << "\n";
  }
}
