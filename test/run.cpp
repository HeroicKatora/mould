#include <iostream>

#include <cpp_mould/argument.hpp>
#include <cpp_mould/compile.hpp>
#include <cpp_mould/engine.hpp>

static constexpr char format[] = "Hello {}\n";

int main() {
  	constexpr auto formatter = mould::compile<format>();
    std::string result = mould::format(formatter, "world");

    std::cout << result;
}
