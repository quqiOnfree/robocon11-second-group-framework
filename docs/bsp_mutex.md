# BSP 互斥锁模块（bsp_mutex.hpp）

## 原理
该模块基于 CMSIS-RTOS2 的 `osMutex` 提供 C++ RAII 互斥锁封装，确保临界区在异常或提前返回时仍能安全释放锁。

## 实现思想
- `gdut::mutex` 对 CMSIS-RTOS2 互斥锁进行封装，构造时创建，析构时删除。
- 提供 `lock_guard` 与 `unique_lock` 两种 RAII 锁管理方式。
- `unique_lock` 支持延迟加锁、尝试加锁和可移动语义，满足复杂场景。

## 如何使用
- 基本互斥锁：

```cpp
#include "bsp_mutex.hpp"

gdut::mutex g_mtx;
int g_shared_value = 0;

void critical_section() {
  gdut::lock_guard<gdut::mutex> lock(g_mtx);
  g_shared_value += 1;
}
```

- 延迟加锁 + try_lock：

```cpp
#include "bsp_mutex.hpp"

gdut::mutex g_mtx;

void work() {
  gdut::unique_lock<gdut::mutex> lock(g_mtx, gdut::defer_lock);

  // 做一些准备工作
  if (lock.try_lock()) {
    // 已加锁
  } else {
    // 未获取到锁，可走降级流程
  }

  if (!lock.owns_lock()) {
    lock.lock();
  }
  // 临界区
}
```

## 与代码规范的对应
- 类与成员命名采用蛇形风格，私有成员使用 `m_` 前缀。
- 通过 RAII 管理系统资源，避免显式 `new/delete`。
- 禁止拷贝，避免硬件/系统资源被误用。

## 注意事项/坑点
- 互斥锁创建可能失败，建议通过 `valid()` 或布尔转换检查。
- `unique_lock` 允许提前 `unlock()`，但需确保后续不再访问共享资源。
- 在 ISR 中禁止使用互斥锁。
- 长时间持锁会导致优先级反转与实时性下降。

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_mutex.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_mutex.hpp)
