#include <iostream>

#include <cpp_mould.hpp>

static constexpr char format[] = "Hello {d}{s}\n";

int main() {
  std::ios_base::sync_with_stdio(false);

  constexpr auto formatter = mould::compile<format>();
  std::string buffer;
  for(int i = 0; i<2000000;i++) {
    mould::format_constexpr(formatter, buffer, -42, ", your mouse is interesting");
    std::cout << buffer;
    buffer.clear();
  }
}
