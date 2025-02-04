#include "injectx/stdext/static_assert.hpp"

#include <cstdlib>

int main() {
  constexpr auto value = 9;
  INJECTX_STDEXT_STATIC_ASSERT(value == 10, "value = {}, must be 10", value);

  return EXIT_SUCCESS;
}
