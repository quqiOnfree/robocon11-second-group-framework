# BSP 类型与硬件映射模块（bsp_type_traits.hpp）

## 原理
该模块提供 GPIO 端口、定时器 ID 的强类型枚举，并提供从枚举到 HAL 结构体指针的安全映射。

## 实现思想
- 使用 `enum class` 防止隐式转换与误用。
- 通过 `constexpr` 函数在编译期进行合法映射。

## 如何使用

```cpp
#include "bsp_type_traits.hpp"

GPIO_TypeDef* port = gdut::get_gpio_port_ptr(gdut::gpio_port::A);
TIM_TypeDef* tim = gdut::get_timer_ptr(gdut::timer_id::tim2);

if (port == nullptr || tim == nullptr) {
	// 处理非法映射
}
```

## 与代码规范的对应
- 强类型枚举减少魔法数。
- 命名统一为蛇形风格。

## 注意事项/坑点
- 传入非法枚举会返回 `nullptr`，调用 HAL 前需检查。
- 枚举值与芯片型号绑定，移植时需确认是否一致。

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_type_traits.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_type_traits.hpp)
