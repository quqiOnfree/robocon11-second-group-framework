# BSP 内存池模块（bsp_memorypool.hpp）

## 原理
该模块实现了 PMR（Polymorphic Memory Resource）标准接口，提供多种内存资源后端（FreeRTOS 堆、TLSF 池、OS 内存池等），支持可预测的嵌入式内存管理。

## 实现思想
- **虚基类接口**：memory_resource 提供统一的 allocate/deallocate 虚接口，支持对齐与大小参数。
- **多种资源实现**：
  - 
ew_delete_resource：使用 pvPortMalloc/vPortFree（FreeRTOS 堆）。
  - unsynchronized_pool_resource：基于 TLSF 的非线程安全内存池。
  - synchronized_pool_resource：TLSF 池加互斥锁保护。
  - os_memory_pool_resource：CMSIS-RTOS2 内存池接口。
- **类型化分配器**：polymorphic_allocator<T> 在虚接口上实现 C++ 分配器，支持 allocate/deallocate 与便捷的 
ew_object/delete_object。
- **与 shared_ptr 集成**：shared_ptr 通过 polymorphic_allocator 分配控制块，实现与内存来源的解耦。

## 核心类

### memory_resource（抽象基类）
- allocate(bytes, alignment) / deallocate(p, bytes, alignment)：虚接口。
- 所有派生类实现 do_allocate / do_deallocate / do_is_equal。

### new_delete_resource
- 使用 FreeRTOS 堆（pvPortMalloc/vPortFree）。
- 单例模式：get_instance()。
- 对齐限制：不超过 portBYTE_ALIGNMENT。

### unsynchronized_pool_resource
- 非线程安全，基于 TLSF 分配器。
- 支持扩展池：分配失败时自动从上游资源追加新池。
- 自动销毁：析构时清理所有块。

### synchronized_pool_resource
- 在 unsynchronized_pool_resource 基础上加互斥锁保护。
- 多线程安全。

### os_memory_pool_resource
- 封装 CMSIS-RTOS2 的 osMemoryPoolNew/Delete/Alloc/Free。
- 固定块大小，块数固定。

### polymorphic_allocator\<T\>
- 模板分配器，在 memory_resource 基础上实现类型化接口。
- allocate(n) / deallocate(p, n)：返回 T*。
- 
ew_object(...args) / delete_object(ptr)：自动调用构造/析构。
- 支持转换构造（rebind 语义）。

## 如何使用

### 1. 使用默认 FreeRTOS 堆资源

```cpp
#include "bsp_memorypool.hpp"

struct data_t {
    int value;
    std::string name;
    data_t(int v, const std::string& n) : value(v), name(n) {}
};

void example_default_resource() {
    // 使用全局 FreeRTOS 堆
    gdut::pmr::polymorphic_allocator<data_t> alloc;
    
    // 分配并构造
    auto* ptr = alloc.new_object(42, "test");
    if (ptr) {
        ptr->value = 100;
        // 使用对象
        alloc.delete_object(ptr);  // 销毁并释放
    }
}
```

### 2. 使用 TLSF 池资源（非线程安全）

```cpp
#include "bsp_memorypool.hpp"

struct config {
    int id;
    explicit config(int i) : id(i) {}
};

void example_unsync_pool() {
    // 创建 TLSF 池，初始块大小 4096 字节
    gdut::pmr::unsynchronized_pool_resource pool_res(
        gdut::pmr::new_delete_resource::get_instance(),
        4096);
    
    if (!pool_res) {
        return;  // 池创建失败
    }
    
    gdut::pmr::polymorphic_allocator<config> alloc(&pool_res);
    
    // 单线程分配
    auto* cfg = alloc.new_object(123);
    if (cfg) {
        cfg->id = 456;
        alloc.delete_object(cfg);
    }
}
```

### 3. 使用线程安全的 TLSF 池

```cpp
#include "bsp_memorypool.hpp"
#include "bsp_thread.hpp"

struct message {
    int type;
    std::string content;
    message(int t, const std::string& c) : type(t), content(c) {}
};

void example_sync_pool() {
    // 创建线程安全的 TLSF 池
    gdut::pmr::synchronized_pool_resource safe_pool(
        gdut::pmr::new_delete_resource::get_instance(),
        2048);  // 初始块大小 2048 字节
    
    if (!safe_pool) {
        return;
    }
    
    gdut::pmr::polymorphic_allocator<message> alloc(&safe_pool);
    
    // 多线程可安全使用
    auto* msg = alloc.new_object(1, "hello");
    if (msg) {
        msg->type = 2;
        alloc.delete_object(msg);
    }
}
```

### 4. 使用 CMSIS-RTOS2 内存池

```cpp
#include "bsp_memorypool.hpp"

struct packet {
    uint16_t length;
    uint8_t data[256];
    packet() : length(0) {}
};

void example_os_pool() {
    // 创建 RTOS 内存池：10 个块，每块 sizeof(packet) 字节
    gdut::pmr::os_memory_pool_resource os_pool(10, sizeof(packet));
    
    if (!os_pool) {
        return;
    }
    
    gdut::pmr::polymorphic_allocator<packet> alloc(&os_pool);
    
    // 从 RTOS 池分配
    auto* pkt = alloc.new_object();
    if (pkt) {
        pkt->length = 64;
        alloc.delete_object(pkt);
    }
}
```

### 5. 与 shared_ptr 集成

```cpp
#include "bsp_shared_ptr.hpp"
#include "bsp_memorypool.hpp"

struct resource {
    int id;
    explicit resource(int i) : id(i) {}
};

void example_shared_ptr_with_pool() {
    // 创建 TLSF 池用于 shared_ptr 控制块
    gdut::pmr::synchronized_pool_resource pool_res(
        gdut::pmr::new_delete_resource::get_instance(),
        1024);
    
    gdut::pmr::polymorphic_allocator<resource> alloc(&pool_res);
    
    // allocate_shared 使用自定义内存资源
    auto sp = gdut::allocate_shared<resource>(alloc, 999);
    if (sp) {
        sp->id = 1000;
    }
    // sp 超出作用域时，控制块和对象都从 pool_res 释放
}
```

### 6. 创建多个类型的分配器链

```cpp
#include "bsp_memorypool.hpp"

struct small_obj {
    int x;
    explicit small_obj(int v) : x(v) {}
};

struct large_obj {
    char data[1024];
    explicit large_obj(int i) { data[0] = i; }
};

void example_multiple_allocators() {
    // 为小对象创建 256 字节块的池
    gdut::pmr::unsynchronized_pool_resource small_pool(
        gdut::pmr::new_delete_resource::get_instance(),
        256);
    
    // 为大对象创建 4096 字节块的池
    gdut::pmr::unsynchronized_pool_resource large_pool(
        gdut::pmr::new_delete_resource::get_instance(),
        4096);
    
    gdut::pmr::polymorphic_allocator<small_obj> small_alloc(&small_pool);
    gdut::pmr::polymorphic_allocator<large_obj> large_alloc(&large_pool);
    
    // 按对象大小使用对应的池
    auto* s = small_alloc.new_object(42);
    auto* l = large_alloc.new_object(10);
    
    if (s && l) {
        // 使用对象
        small_alloc.delete_object(s);
        large_alloc.delete_object(l);
    }
}
```

### 7. 分配失败处理

```cpp
#include "bsp_memorypool.hpp"

struct item {
    int id;
    explicit item(int i) : id(i) {}
};

void example_allocation_failure() {
    // 创建一个小池（故意限制大小以演示失败）
    gdut::pmr::os_memory_pool_resource tiny_pool(2, sizeof(item));
    
    gdut::pmr::polymorphic_allocator<item> alloc(&tiny_pool);
    
    std::vector<item*> items;
    
    // 尝试分配，超过池容量时会失败
    for (int i = 0; i < 5; ++i) {
        auto* it = alloc.new_object(i);
        if (it) {
            items.push_back(it);
        } else {
            // 分配失败，停止或重试
            break;
        }
    }
    
    // 清理
    for (auto* it : items) {
        alloc.delete_object(it);
    }
}
```

## 与代码规范的对应
- **避免裸 new/delete**：使用 polymorphic_allocator::new_object/delete_object。
- **蛇形命名**：所有类、函数、成员使用 snake_case，私有成员使用 m_ 前缀。
- **虚接口规范**：do_allocate / do_deallocate 为虚方法，隐藏实现。
- **RAII 原则**：资源在构造时获取，析构时自动清理。
- **多线程安全**：synchronized_pool_resource 和 os_memory_pool_resource 线程安全，单线程场景用 unsynchronized_pool_resource 避免锁开销。

## 注意事项/坑点

### 内存管理
- **分配失败检查**：allocate 和 
ew_object 失败返回 
ullptr，必须检查。
- **块大小限制**：unsynchronized_pool_resource 和 synchronized_pool_resource 超过初始块大小的请求会自动扩展池，但仍受总内存限制。
- **TLSF 开销**：TLSF 每个分配都有元数据开销，不适合超小块分配（< 16 字节）。

### 线程安全
- **单线程优化**：unsynchronized_pool_resource 无锁开销，适合单线程环节。
- **synchronized_pool_resource 锁**：使用 gdut::mutex，全局锁，高竞争时可能瓶颈。
- **os_memory_pool_resource**：CMSIS-RTOS2 原生支持多线程，无额外锁。

### 对齐
- **new_delete_resource 对齐限制**：最多 portBYTE_ALIGNMENT 字节对齐，超过会失败。
- **TLSF 对齐**：自动处理对齐，大多数情况下默认对齐足够。
- **显式指定对齐**：allocate(bytes, alignment) 可传入自定义对齐需求。

### 资源生命周期
- **上游资源**：创建池时指定上游资源（如 
ew_delete_resource），池销毁时自动释放所有块回上游。
- **全局 get_instance()**：
ew_delete_resource::get_instance() 返回全局单例，不需手动释放。
- **栈分配**：资源对象可栈分配，超出作用域自动清理。

### 与 shared_ptr 配合
- **一致分配器**：allocate_shared<T>(alloc, args...) 会用 alloc 分配控制块和对象。
- **控制块释放**：shared_ptr 析构时调用控制块的 deallocate()，必须与分配器一致。
- **嵌入式优化**：合并分配模式一次分配，单从内存池释放，比分离模式高效。

## 与 std::pmr 的差异
| 特性 | BSP pmr | std::pmr |
|------|---|---|
| 基础设施 | 自实现虚接口 | C++17 标准库 |
| 池实现 | TLSF / CMSIS-RTOS2 | 不指定 |
| 对齐支持 | 手动传入 alignment | 自动推导 |
| 单例资源 | 用户管理 | std::pmr::get_default_resource() |
| 线程安全 | synchronized_pool_resource | 取决于实现 |
| 嵌入式友好 | 轻量级，可控 | 标准库完整实现 |
| 异常处理 | 分配失败返回 nullptr | 抛异常 |

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_memorypool.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_memorypool.hpp)
