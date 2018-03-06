#include <iostream>

#include <cpp_mould.hpp>

static constexpr char format[] = "Hello {d}{s}\n";

int main() {
  std::ios_base::sync_with_stdio(false);

  constexpr auto formatter = mould::compile<format>();
  for(int i = 0; i<2000000;i++) {
    std::cout << mould::format_constexpr(formatter, -42, ", your mouse is interesting");
  }
}
