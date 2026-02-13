# BSP 线程模块（bsp_thread.hpp）

## 原理
该模块对 CMSIS-RTOS2 线程进行 C++ RAII 封装，提供类似 `std::thread` 的接口，并支持线程完成后的 join 语义。

## 实现思想
- 通过信号量实现 join 等待。
- 构造时创建线程与同步对象；析构时自动终止与释放资源。
- 通过模板参数配置线程栈大小与优先级。

## 如何使用

```cpp
#include "bsp_thread.hpp"

void do_work() {
  // 任务逻辑
}

void thread_demo() {
  gdut::thread<512> worker([] { do_work(); });

  if (worker.joinable()) {
    worker.join();
  }
}
```

## 与代码规范的对应
- 禁止拷贝，避免系统资源被重复管理。
- 通过 RAII 自动释放 OS 资源。

## 注意事项/坑点
- `join()` 只能调用一次，重复调用无效。
- `terminate()` 为强制终止，可能导致资源未释放。
- 线程栈大小要足够，否则可能溢出。

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_thread.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_thread.hpp)
