#include <iostream>

#include <cpp_mould/compile.hpp>
#include <cpp_mould/engine.hpp>

static constexpr char format[] = "Hello {}\n";

int main() {
  	constexpr auto formatter = mould::compile<format>();
    std::string result = mould::format(formatter, 42);

    std::cout << result;
}