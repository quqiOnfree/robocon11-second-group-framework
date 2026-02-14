# BSP 内存资源模块（bsp_memory_resource.hpp）

该模块提供一组基于 `std::pmr::memory_resource` 的内存资源实现，面向 FreeRTOS 与 CMSIS-RTOS2 场景，统一通过标准库 PMR 接口进行分配与释放。所有资源类型均继承 `std::pmr::memory_resource`，可直接与 `std::pmr::polymorphic_allocator` 以及 PMR 容器协作。

## 资源类型

### 1. portable_resource
- 使用 FreeRTOS `pvPortMalloc/vPortFree` 作为上游分配器。
- 通过 `get_instance()` 获取单例。
- 当请求对齐大于 `portBYTE_ALIGNMENT` 时返回空指针（避免未对齐内存）。

**接口要点**
- `static std::pmr::memory_resource *get_instance()`：返回单例实例。
- `allocate/deallocate`：由 `std::pmr::memory_resource` 驱动，内部转发至 `pvPortMalloc/vPortFree`。

### 2. unsynchronized_tlsf_resource
- 基于 TLSF 的非线程安全内存池资源。
- 通过上游资源申请池内存；超大分配自动回退到上游资源。
- 适用于单线程或外部已保证互斥的场景。

**接口要点**
- `unsynchronized_tlsf_resource(upstream, pool_block_size)`：使用上游资源创建池。
- `explicit operator bool()`：检查池是否创建成功。
- 超过 `pool_block_size` 的请求自动回退到上游资源。

### 3. synchronized_tlsf_resource
- 线程安全版本的 TLSF 内存池资源。
- 内部使用互斥锁保护 `unsynchronized_tlsf_resource`。

**接口要点**
- `synchronized_tlsf_resource(upstream, pool_block_size)`：线程安全池。
- `explicit operator bool()`：检查池是否创建成功。

### 4. os_memory_pool_resource
- 封装 CMSIS-RTOS2 的 `osMemoryPool`。
- 分配尺寸超过块大小时返回空指针。

**接口要点**
- `os_memory_pool_resource(block_count, block_size)`：创建 CMSIS-RTOS2 内存池。
- `explicit operator bool()`：检查池是否创建成功。
- 分配尺寸超过 `block_size` 时直接失败。

## 使用示例

### 完整示例（仅分配内存与 allocator）

```cpp
#include <cstdint>
#include <memory_resource>
#include "bsp_memory_resource.hpp"

struct packet {
    std::uint32_t id;
    std::uint8_t data[32];
};

void example_memory_resources() {
    // 1) 获取上游资源（FreeRTOS heap）
    std::pmr::memory_resource *upstream =
        gdut::pmr::portable_resource::get_instance();

    if (upstream == nullptr) {
        return; // 上游资源不可用
    }

    // 2) TLSF 非线程安全池（适合单线程或外部互斥）
    gdut::pmr::unsynchronized_tlsf_resource tlsf_pool(upstream, 1024);
    if (!tlsf_pool) {
        return; // TLSF 池创建失败
    }

    // 3) TLSF 线程安全池
    gdut::pmr::synchronized_tlsf_resource tlsf_pool_safe(upstream, 2048);
    if (!tlsf_pool_safe) {
        return; // TLSF 线程安全池创建失败
    }

    // 4) CMSIS-RTOS2 内存池资源
    gdut::pmr::os_memory_pool_resource os_pool(16, sizeof(packet));
    if (!os_pool) {
        return; // CMSIS 内存池创建失败
    }

    // 5) 使用 polymorphic_allocator 直接分配对象
    std::pmr::polymorphic_allocator<packet> pkt_alloc(&os_pool);
    packet *p = pkt_alloc.allocate(1);
    if (p != nullptr) {
        p->id = 1;
        p->data[0] = 0xAA;
        pkt_alloc.deallocate(p, 1);
    }

    // 6) 使用上游资源直接分配原始缓冲区
    void *raw = upstream->allocate(128, alignof(std::max_align_t));
    if (raw != nullptr) {
        upstream->deallocate(raw, 128, alignof(std::max_align_t));
    }

    // 7) 使用 TLSF 资源分配原始缓冲区
    void *pool_raw = tlsf_pool_safe.allocate(64, alignof(std::max_align_t));
    if (pool_raw != nullptr) {
        tlsf_pool_safe.deallocate(pool_raw, 64, alignof(std::max_align_t));
    }
}
```

## 注意事项

- `portable_resource` 仅保证 `portBYTE_ALIGNMENT` 对齐，超出对齐要求会返回空指针。
- `unsynchronized_tlsf_resource` / `synchronized_tlsf_resource` 对于超大请求会回退到上游资源。
- `os_memory_pool_resource` 仅支持不超过 `block_size` 的分配请求。
- PMR 容器在销毁时会调用其关联资源的 `deallocate`，确保资源生命周期覆盖容器生命周期。

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_memory_resource.hpp](../Middlewares/GDUT_RC_Library/BSP/bsp_memory_resource.hpp)