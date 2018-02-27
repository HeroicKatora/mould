#include <iostream>
#include <iterator>

#include <cpp_mould/all.hpp>
#include <cpp_mould/debug.hpp>

using namespace mould;

static constexpr char format[] = "Hello {}";

int main() {
  	constexpr auto formatter = compile<format>();
    auto description = descriptor(formatter);

    while(description) {
      std::cout << description.describe_next_byte_code() << "\n";
    }
}
