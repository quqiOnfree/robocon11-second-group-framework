# BSP SPI 通讯模块（bsp_spi.hpp）

## 原理
该模块对 STM32 HAL 的 SPI 通讯功能进行 C++ 包装，提供阻塞模式的同步 SPI 数据传输接口，支持 C++20 chrono 超时机制。

## 核心设计

### SPI 代理类 `spi_proxy`
- 基于 STM32 HAL 的 SPI 阻塞模式（Blocking Mode）
- 支持三种传输模式：发送、接收、全双工收发
- RAII 自动管理，禁止复制（`uncopyable`）
- 集成 C++20 `std::chrono` 超时机制
- 参数合法性检查（nullptr、零长度检查）

## 如何使用

### 基础初始化
```cpp
#include "bsp_spi.hpp"

// STM32 HAL SPI 句柄
SPI_HandleTypeDef hspi1;

// 创建 SPI 代理对象
gdut::spi_proxy spi(hspi1);
```

### 发送数据
```cpp
// 发送数据，使用默认超时（最大值 = 无限等待）
const uint8_t tx_data[] = {0x01, 0x02, 0x03, 0x04};
if (spi.transmit(tx_data, sizeof(tx_data))) {
    // 发送成功
}

// 发送数据，指定超时时间为 1000ms
if (spi.transmit(tx_data, sizeof(tx_data), std::chrono::milliseconds(1000))) {
    // 发送成功
}

// 发送数据，立即超时（检查是否就绪）
if (spi.transmit(tx_data, sizeof(tx_data), std::chrono::milliseconds(0))) {
    // SPI 已就绪
}
```

### 接收数据
```cpp
// 接收数据，使用默认超时
uint8_t rx_data[4];
if (spi.receive(rx_data, sizeof(rx_data))) {
    // 接收成功
    // rx_data[0..3] 包含接收到的数据
}

// 接收数据，指定超时时间为 500ms
if (spi.receive(rx_data, sizeof(rx_data), std::chrono::milliseconds(500))) {
    // 接收成功
}
```

### 全双工传输（同时发送和接收）
```cpp
// 全双工传输：同时发送和接收，常用于 SPI 传感器和 IC 通讯
const uint8_t tx_data[] = {0xA5, 0x5A};  // 发送数据
uint8_t rx_data[2];                       // 接收缓冲区

if (spi.transmit_receive(tx_data, rx_data, sizeof(tx_data))) {
    // 传输完成
    // rx_data 包含接收到的数据
}

// 指定超时
if (spi.transmit_receive(tx_data, rx_data, sizeof(tx_data), 
                         std::chrono::milliseconds(1000))) {
    // 传输成功
}
```

## 实际应用示例

### 与 SPI 传感器通讯
```cpp
// IMU 或加速度计等传感器通常采用全双工通讯

struct sensor_data {
    uint16_t accel_x;
    uint16_t accel_y;
    uint16_t accel_z;
};

gdut::spi_proxy imu_spi(hspi2);

// 读取传感器数据
uint8_t cmd[7] = {0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // 命令 + 占位符
uint8_t response[7];

if (imu_spi.transmit_receive(cmd, response, sizeof(cmd), 
                             std::chrono::milliseconds(100))) {
    sensor_data data;
    // 解析接收到的数据
    data.accel_x = (response[1] << 8) | response[2];
    data.accel_y = (response[3] << 8) | response[4];
    data.accel_z = (response[5] << 8) | response[6];
}
```

### 写入 SPI Flash
```cpp
gdut::spi_proxy flash_spi(hspi3);

// 发送写启用命令
const uint8_t write_enable[] = {0x06};
flash_spi.transmit(write_enable, sizeof(write_enable), 
                   std::chrono::milliseconds(10));

// 发送编程命令 + 地址 + 数据
uint8_t program_cmd[] = {0x02, 0x00, 0x00, 0x00, 
                         0xAA, 0xBB, 0xCC, 0xDD};
flash_spi.transmit(program_cmd, sizeof(program_cmd), 
                   std::chrono::milliseconds(50));
```

### 读取 SPI 存储器
```cpp
gdut::spi_proxy ram_spi(hspi4);

// 发送读命令 + 地址
uint8_t read_cmd[] = {0x03, 0x10, 0x20, 0x00};  // 读地址 0x102000
uint8_t read_data[16];

if (ram_spi.transmit_receive(read_cmd, read_data, sizeof(read_cmd), 
                             std::chrono::milliseconds(100))) {
    // read_data[1..16] 包含读取的内容
}
```

## 与代码规范的对应
- 对象禁止复制（`uncopyable`），避免句柄冲突
- 参数合法性检查，返回 `bool` 表示成功/失败
- C++20 `std::chrono` 超时机制，类型安全
- 蛇形命名约定
- 引用语义，直接使用 HAL 句柄

## 超时处理

### 超时转换规则
- `std::chrono::milliseconds::max()` → `osWaitForever`（无限等待）
- 其他值 → 直接传递给 HAL（以毫秒为单位）

### 常见超时场景
```cpp
// 快速操作（< 10ms）
spi.transmit(data, size, std::chrono::milliseconds(10));

// 中等操作（100ms 左右）
spi.receive(data, size, std::chrono::milliseconds(100));

// 长时间操作（>= 1s）
spi.transmit_receive(tx, rx, size, std::chrono::milliseconds(1000));

// 无限等待
spi.transmit(data, size, std::chrono::milliseconds::max());
```

## 注意事项/坑点
- 所有操作都是**阻塞模式**，会阻塞当前任务，不能在关键路径上使用
- 不支持 DMA 模式，仅支持 CPU 轮询模式
- 发送和接收数据必须 > 0 字节，否则返回 `false`
- 指针不能为 nullptr，否则返回 `false`
- HAL 期望 `uint8_t*` 但参数声明为 `const uint8_t*`，内部使用 `const_cast` 处理
- 不支持中断模式或 DMA 模式，若需要异步通讯应自行扩展或使用其他模块
- 超时过短（< 1ms）可能导致操作失败
- 对象生命周期必须长于所有 SPI 操作

相关源码：[Middlewares/GDUT_RC_Library/BSP/bsp_spi.hpp](../../Middlewares/GDUT_RC_Library/BSP/bsp_spi.hpp)
