#include <iostream>

#include <cpp_mould.hpp>

static constexpr char format[] = "Hello {}{}\n";

int main() {
  	constexpr auto formatter = mould::compile<format>();
    std::string result = mould::format(formatter, -42, ", your mouse is interesting");

    std::cout << result;
}
