#include <iostream>
#include <cpp_mould.hpp>

static constexpr char format[] = "Hello {}\n";

int main() {
	std::ios::sync_with_stdio(false);

	constexpr auto formatter = mould::compile<format>();

	for(int i = 0; i < 2000000; i++) {
		std::cout << mould::format(formatter, 42);
/* Format equivalent to "%0.10f:%04d:%+g:%s:%p:%c:%%\n" */
	}
}
