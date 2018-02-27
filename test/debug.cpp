#include <iostream>
#include <iterator>

#include <cpp_mould/all.hpp>

using namespace mould;

static constexpr char format[] = "Hello {}";

int main() {

  	constexpr auto formatter = compile<format>();

    std::copy(std::begin(formatter.code), std::end(formatter.code),
      std::ostream_iterator<Codepoint>(std::cout, " "));
    std::cout << "\n";
    std::copy(std::begin(formatter.immediates), std::end(formatter.immediates),
      std::ostream_iterator<Immediate>(std::cout, " "));
}
