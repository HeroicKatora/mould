#include <iostream>
#include <cpp_mould/all.hpp>

using namespace mould;

static const char format[] = "{}";

int main() {
	std::ios::sync_with_stdio(false);

	const auto formatter = compile<format>();

	for(int i = 0; i < 2000000; i++) {
/* Format equivalent to "%0.10f:%04d:%+g:%s:%p:%c:%%\n" */
	}
}
