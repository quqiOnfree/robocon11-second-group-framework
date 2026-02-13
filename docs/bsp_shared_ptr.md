# BSP 共享指针模块（bsp_shared_ptr.hpp）

## 原理
该模块提供面向嵌入式环境的轻量级 `shared_ptr` 实现，使用 BSP 原子计数与自定义内存资源实现引用计数管理，避免依赖完整的标准库实现。

## 实现思想
- 通过 `atomic<std::size_t>` 实现线程安全引用计数。
- 通过自定义 `pmr::polymorphic_allocator` 分配计数与删除器包装对象。
- 删除器使用虚基类包装，支持任意删除策略。
- 在引用计数归零时，使用 release/acquire 内存序确保可见性。

## 如何使用
- 包装裸指针（带自定义删除器）：

```cpp
#include "bsp_shared_ptr.hpp"

struct my_obj {
	int value;
};

gdut::shared_ptr<my_obj> make_obj() {
	my_obj* raw = new my_obj{123};
	return gdut::shared_ptr<my_obj>(raw, [](my_obj* p) { delete p; });
}

void use_obj() {
	auto p = make_obj();
	if (p) {
		p->value += 1;
	}
}
```

- 接管 `unique_ptr`（完整流程）：

```cpp
#include "bsp_shared_ptr.hpp"
#include <memory>

struct my_obj {
	int value;
};

gdut::shared_ptr<my_obj> wrap_unique_ptr() {
	std::unique_ptr<my_obj> up(new my_obj{42});
	gdut::shared_ptr<my_obj> sp(std::move(up));
	return sp;
}
```

## 与代码规范的对应
- 避免直接 `new/delete` 在上层散落，通过封装统一释放逻辑。
- 使用 `snake_case` 命名与 `m_` 前缀私有成员。
- 不使用 C 风格强转，资源释放严格在 RAII 生命周期内进行。

## 注意事项/坑点
- 删除器必须与对象的分配方式匹配（如 `new` 对应 `delete`）。
- 仅引用计数是线程安全的，被管理对象本身仍需上层同步。
- 使用 `pmr::polymorphic_allocator` 分配控制块，内存不足时构造会失败。
- 避免循环引用导致内存无法释放。

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_shared_ptr.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_shared_ptr.hpp)
