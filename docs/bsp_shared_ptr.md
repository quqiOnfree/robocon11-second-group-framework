# BSP 共享指针模块（bsp_shared_ptr.hpp）

## 原理
该模块提供面向嵌入式环境的轻量级 shared_ptr、weak_ptr 和 enable_shared_from_this 实现，使用原子计数与自定义内存资源实现引用计数管理，避免依赖完整的标准库实现。

## 实现思想
- **双计数控制块**：shared_count 管理共享引用（对象存活），weak_count 管理弱引用（控制块存活）。
- **两种控制块**：
  - control_block_separate<T, Deleter>：对象在堆外（如 
ew），控制块在内存池。
  - control_block_combined<T, Deleter>：对象嵌入控制块，一次分配（合并模式）。
- **删除器支持**：每个控制块包含删除器，支持任意删除策略（函数指针、仿函数、lambda）。
- **原子操作**：使用 memory_order_release/acquire 确保对象销毁的可见性。
- **内存池集成**：通过 pmr::polymorphic_allocator 与 BSP 内存资源无缝集成。

## 核心概念

### shared_ptr
- 拥有式智能指针，引用计数管理对象生命周期。
- 支持多线程安全的拷贝与移动。
- 通过 use_count() 查看当前共享引用数。

### weak_ptr
- 非拥有式观察指针，不增加共享计数。
- 通过 lock() 原子地尝试获取 shared_ptr（需要控制块未销毁且对象未过期）。
- 通过 expired() 检查对象是否已销毁。

### enable_shared_from_this
- CRTP 基类，允许对象内部通过 shared_from_this() 获取指向自身的 shared_ptr。
- 内部维护弱指针，确保避免循环引用。

## 如何使用

### 1. 包装裸指针（分离分配模式）

```cpp
#include "bsp_shared_ptr.hpp"

struct my_obj {
    int value;
    explicit my_obj(int v) : value(v) {}
};

void example_raw_ptr() {
    // 使用默认删除器（pmr 分配器）
    gdut::shared_ptr<my_obj> p1(new my_obj{42});
    if (p1) {
        p1->value += 1;  // 访问对象
    }
    // p1 超出作用域时自动销毁对象

    // 使用自定义删除器
    auto custom_deleter = [](my_obj* ptr) {
        // 可以在这里加日志
        delete ptr;
    };
    gdut::shared_ptr<my_obj> p2(new my_obj{100}, custom_deleter);
    // p2 超出作用域时用 custom_deleter 释放对象
}
```

### 2. 合并分配模式（推荐：一次分配）

```cpp
#include "bsp_shared_ptr.hpp"

struct node {
    int data;
    std::string name;
    node(int d, const std::string& n) : data(d), name(n) {}
};

void example_make_shared() {
    // make_shared 对象与控制块一起分配，效率最高
    auto p = gdut::make_shared<node>(10, "test");
    if (p) {
        p->data += 5;
        p->name = "updated";
    }
    // 超出作用域时自动销毁，一次释放
}
```

### 3. 自定义分配器的合并分配模式

```cpp
#include "bsp_shared_ptr.hpp"
#include "bsp_memorypool.hpp"

struct widget {
    int id;
    widget(int i) : id(i) {}
};

void example_allocate_shared() {
    // 使用特定的内存资源进行分配
    gdut::pmr::new_delete_resource resource;
    gdut::pmr::polymorphic_allocator<widget> alloc(&resource);
    
    // allocate_shared 使用自定义分配器，对象和控制块一起分配
    auto p = gdut::allocate_shared<widget>(alloc, 123);
    if (p) {
        p->id = 456;
    }
    // 超出作用域时用 alloc 对应的内存资源释放
}
```

### 4. 拷贝与移动

```cpp
#include "bsp_shared_ptr.hpp"

struct data {
    int value;
    explicit data(int v) : value(v) {}
};

void example_copy_move() {
    auto p1 = gdut::make_shared<data>(100);
    
    // 拷贝：引用计数递增
    gdut::shared_ptr<data> p2 = p1;
    assert(p1.use_count() == 2);
    assert(p1.get() == p2.get());
    
    // 移动：转移所有权，引用计数不变
    gdut::shared_ptr<data> p3 = std::move(p1);
    assert(p1.get() == nullptr);
    assert(p2.use_count() == 2);  // 仍为 2
    assert(p3->value == 100);
    
    // 作用域结束：p2 和 p3 释放，引用计数归零，对象销毁
}
```

### 5. weak_ptr 与对象观察

```cpp
#include "bsp_shared_ptr.hpp"

struct resource {
    int id;
    explicit resource(int i) : id(i) {}
};

void example_weak_ptr() {
    gdut::weak_ptr<resource> weak;
    
    {
        auto strong = gdut::make_shared<resource>(42);
        weak = strong;  // weak 从 strong 构造
        assert(!weak.expired());
        assert(weak.use_count() == 1);
        
        // 从 weak_ptr 尝试获取 shared_ptr
        auto locked = weak.lock();
        if (locked) {
            locked->id = 100;  // 对象仍活跃，可以访问
        }
    }  // strong 销毁，对象释放
    
    // 对象已销毁
    assert(weak.expired());
    assert(weak.use_count() == 0);
    
    auto locked = weak.lock();
    assert(!locked);  // lock() 返回空指针
}
```

### 6. enable_shared_from_this（成员函数中获取自身指针）

```cpp
#include "bsp_shared_ptr.hpp"

struct handler : public gdut::enable_shared_from_this<handler> {
    int id;
    
    explicit handler(int i) : id(i) {}
    
    // 成员函数需要获取指向自身的 shared_ptr
    void register_callback(std::vector<gdut::shared_ptr<handler>>& registry) {
        // 通过 shared_from_this() 获取自身的 shared_ptr
        registry.push_back(this->shared_from_this());
    }
};

void example_enable_shared_from_this() {
    std::vector<gdut::shared_ptr<handler>> registry;
    
    {
        auto h = gdut::make_shared<handler>(1);
        h->register_callback(registry);
        // h 在此超出作用域，但 registry 中仍持有引用
    }
    
    // h 原对象未销毁，因为 registry 还持有引用
    assert(registry[0]->id == 1);
    assert(registry[0].use_count() == 1);
    
    registry.clear();  // 现在对象才真正销毁
}
```

### 7. 类型转换与兼容性

```cpp
#include "bsp_shared_ptr.hpp"

struct base {
    virtual ~base() = default;
};

struct derived : public base {
    int value;
    explicit derived(int v) : value(v) {}
};

void example_polymorphism() {
    auto d = gdut::make_shared<derived>(100);
    
    // 隐式转换为基类指针（requires 可转换性检查）
    gdut::shared_ptr<base> b = d;
    assert(b.get() == d.get());
    assert(b.use_count() == 2);
    
    // 同时接管 unique_ptr
    std::unique_ptr<derived> up(new derived{200});
    auto sp = gdut::shared_ptr<derived>(std::move(up));
    assert(sp->value == 200);
}
```

## 与代码规范的对应
- **避免裸指针**：使用 shared_ptr / make_shared 替代 
ew/delete。
- **蛇形命名**：所有类、函数、成员使用 snake_case，私有成员使用 m_ 前缀。
- **RAII 原则**：资源在构造时获取，析构时释放，无需手动 delete。
- **多线程安全**：引用计数操作是原子的，但被管理对象本身需上层同步。

## 注意事项/坑点

### 内存管理
- **默认删除器**：default_deleter<T> 使用 pmr::polymorphic_allocator 删除对象。
- **分离模式 vs 合并模式**：
  - 分离模式适合已分配的对象（如从其他库接收）。
  - 合并模式（make_shared、allocate_shared）效率最高，一次分配。

### 引用计数
- use_count() 返回的是快照，不是原子操作结果。
- 循环引用（A 持有 B，B 通过 shared_from_this 持有 A）会导致泄漏，使用 weak_ptr 打破循环。

### weak_ptr 特性
- expired() 和 lock() 都使用原子 CAS 确保对象未在中途销毁。
- 即使 expired() == false，lock() 仍可能返回空（竞态条件下对象同时释放）。

### enable_shared_from_this
- 对象必须已被 shared_ptr 管理，才能在构造函数内调用 shared_from_this()（否则 internal_accept_owner_ 未被调用）。
- 禁止拷贝 enable_shared_from_this 的子类来创建新对象（拷贝后 m_weak_this 仍指向原对象）。

### 异常安全
- 嵌入式环境禁用异常，所以分配失败时返回空指针而非抛异常。
- 调用 allocate 后需检查返回值，避免解引用空指针。

### 原子操作内存序
- **递增引用计数**：memory_order_relaxed（不需同步）。
- **递减引用计数**：memory_order_release（保证销毁可见）。
- **检查过期**：memory_order_acquire（同步销毁操作）。

## 与 std::shared_ptr 的差异
| 特性 | BSP shared_ptr | std::shared_ptr |
|------|---|---|
| 内存资源 | pmr 集成，可自定义 | 使用全局 operator new/delete |
| 异常处理 | 无异常，失败返回空指针 | 分配失败抛异常 |
| 合并分配 | make_shared / allocate_shared | std::make_shared |
| weak_ptr | 完整支持 CAS lock() | 完整支持 |
| enable_shared_from_this | 支持 | 支持 |
| 嵌入式友好性 | 优化的原子序，小内存占用 | 标准库完整实现 |

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_shared_ptr.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_shared_ptr.hpp)
