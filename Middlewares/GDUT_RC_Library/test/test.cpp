#include "bsp_clock.hpp"
#include "bsp_gpio_pin.hpp"
#include "bsp_memorypool.hpp"
#include "bsp_mutex.hpp"
#include "bsp_semaphore.hpp"
#include "bsp_shared_ptr.hpp"
#include "bsp_thread.hpp"
#include "bsp_timer.hpp"
#include "bsp_type_traits.hpp"
#include "bsp_uncopyable.hpp"

#include <cmsis_os2.h>
#include <coroutine>
#include <generator>

std::generator<int> count_up_to(int n) {
  for (int i = 0; i < n; ++i) {
    co_yield i;
  }
}

int main() {
  auto a = count_up_to(5);
  for (auto v : a) {
    // Should print 0, 1, 2, 3, 4
    (void)v; // Suppress unused variable warning
  }
  return 0;
}
