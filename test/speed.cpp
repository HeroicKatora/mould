#include <iostream>
#include <cpp_mould.hpp>

using namespace std::literals::string_view_literals;
static constexpr char format[] = "{:0.10f}:{:04d}:{:+}:{:s}:{:p}:{:c}:%\n";

int main() {
	std::ios::sync_with_stdio(false);

	constexpr auto formatter = mould::compile<format>();

	for(int i = 0; i < 2000000; i++) {
		/* Format equivalent to "%0.10f:%04d:%+g:%s:%p:%c:%%\n" */
		mould::write_constexpr(formatter, std::cout,
			1.234, 42, 3.13, "str"sv, (void*)1000, 'X');
	}
}
