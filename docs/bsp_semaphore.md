# BSP 信号量模块（bsp_semaphore.hpp）

## 原理
该模块对 CMSIS-RTOS2 的计数信号量进行封装，提供 C++ 风格接口，并支持 `std::chrono` 超时参数。

## 实现思想
- `counting_semaphore<LeastMaxValue>` 在构造时创建 OS 信号量。
- `acquire` 支持超时与无限等待，内部将时长转换为 OS tick。
- 通过移动语义管理 OS 资源，禁止拷贝。

## 如何使用

```cpp
#include "bsp_semaphore.hpp"

gdut::counting_semaphore<4> sem(0);

void producer() {
  // 生产完成，释放一个信号
  sem.release();
}

void consumer() {
  if (sem.acquire(std::chrono::milliseconds(10)) == osOK) {
    // 获取成功，执行消费逻辑
  } else {
    // 超时或错误
  }
}
```

## 与代码规范的对应
- 明确的资源生命周期管理与 RAII 习惯。
- 命名使用蛇形风格。

## 注意事项/坑点
- 信号量创建可能失败，建议通过 `valid()` 或布尔转换检查。
- 计数上限由模板参数限定，超出可能导致逻辑错误。
- 超时单位为毫秒，子毫秒会被截断。

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_semaphore.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_semaphore.hpp)
