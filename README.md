# Robocon 第二组 - STM32F407 框架项目

欢迎来到大一 Robocon 比赛第二组的项目仓库！本项目基于 STM32F407 微控制器，使用 CMake 构建系统和 Modern C++ 开发。

## 📋 项目概述

- **目标设备**: STM32F407VET6
- **构建系统**: CMake + Ninja
- **编程语言**: C++
- **依赖库**: FreeRTOS、STM32 HAL Driver、GDUT 内部库定义

## 🚀 快速开始

### 1. Clone 仓库

使用 `--recursive` 标志递归克隆所有子模块：

```bash
git clone --recursive https://github.com/your-org/robocon11-second-group-framework.git
cd robocon11-second-group-framework
```

如果你已经 clone 过但忘记了 `--recursive` 标志，可以手动初始化子模块：

```bash
git submodule update --init --recursive
```

### 2. 构建项目

本项目使用 CMake 构建系统，请遵循以下步骤：

#### 方法 A: 使用 CMake 命令行（推荐）

```bash
# 创建构建目录（如果不存在）
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake

# 构建项目
cmake --build build
```

#### 方法 B: 使用 CMake GUI（可选）

```bash
cmake-gui .
```

**注意**: ❌ 不要直接使用 `make` 命令，我们使用 CMake 的构建抽象层，这样更加跨平台且易于维护。

### 3. 清理构建

```bash
cmake --build build --target clean
```

或者直接删除 `build` 目录：

```bash
rm -r build  # Linux/macOS
rmdir /s build  # Windows PowerShell
```

## 🎨 代码格式化

项目使用 **clang-format** 工具来确保代码风格统一。

### 安装 clang-format

**Windows**:
```powershell
choco install llvm  # 使用 Chocolatey
# 或从 https://releases.llvm.org/download.html 下载
```

**Linux (Ubuntu/Debian)**:
```bash
sudo apt-get install clang-format
```

**macOS**:
```bash
brew install clang-format
```

### 格式化代码

格式化整个项目：

**Windows PowerShell**:
```powershell
.\format_all.ps1
```

**Linux/macOS**:
```bash
./format_all.sh
```

或手动格式化特定文件：

```bash
clang-format -i <file_path>
```

## 📖 代码规范

本项目遵循严格的代码规范，详见 [代码规范.md](./docs/代码规范.md)

### 快速总结

- **命名约定**: 使用蛇形命名法 (snake_case)
  - 变量和函数: `my_variable`、`calculate_sum()`
  - 类和结构体: `network_manager`、`packet_header`
  - 私有成员变量: `m_socket_fd`

- **代码风格**: Modern C++ (C++17+)
  - 使用 STL 容器而非原生数组
  - 优先使用智能指针 (`std::unique_ptr`、`std::shared_ptr`)
  - 利用 GDUT 组内库（`GDUT_RC_Library`）进行嵌入式开发

- **文件组织**:
  - 头文件 (.h): `Core/Inc/`
  - 源文件 (.cpp/.c): `Core/Src/`
  - 驱动文件: `Drivers/`
  - 中间件: `Middlewares/`

## 📚 重要库说明

### GDUT 内部库（GDUT_RC_Library）

本项目使用组内维护的 `GDUT_RC_Library` 作为主要的轻量级库实现，提供常用的数据结构、驱动封装与工具接口，便于在竞赛期间快速迭代和定制。该库位于 `Middlewares/GDUT_RC_Library`，并与项目紧密集成。

### FreeRTOS

项目集成了 FreeRTOS 实时操作系统：

```cpp
#include "FreeRTOS.h"
#include "task.h"

void my_task(void* pvParameters) {
  while (1) {
    // 任务代码
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
```

## 🛠️ 开发工作流

1. **编写代码** - 遵循 [代码规范.md](代码规范.md)
2. **格式化代码** - 运行 `format_all.sh` 或 `format_all.ps1`
3. **构建项目** - 使用 `cmake --build build`
4. **测试和调试** - 烧写到硬件设备进行测试
5. **提交代码** - 确保已通过格式化检查

## 📂 项目结构

```
.
├── CMakeLists.txt              # CMake 配置文件
├── CMakePresets.json           # CMake 预设
├── Core/
│   ├── Inc/                    # 头文件
│   └── Src/                    # 源文件（包括 main.cpp）
├── Drivers/
│   ├── CMSIS/                  # ARM CMSIS 核心库
│   └── STM32F4xx_HAL_Driver/   # STM32 HAL 驱动
├── Middlewares/
│   ├── GDUT_RC_Library/        # GDUT 内部库定义
│   └── Third_Party/            # 第三方库（FreeRTOS）
├── cmake/                      # CMake 工具链文件
├── build/                      # 构建输出目录（自动生成）
├── 代码规范.md                  # 详细的代码规范文档
└── README.md                   # 本文件
```

## ⚙️ 环境要求

- **CMake**: >= 3.20
- **编译器**: arm-none-eabi-gcc (ARM Embedded GCC Toolchain)
- **构建工具**: Ninja (推荐) 或 Make
- **代码格式化**: clang-format >= 14.0

## 🔗 有用的链接

- [STM32F407 数据手册](https://www.st.com/resource/en/datasheet/stm32f407vg.pdf)
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) - 设备配置工具
- [FreeRTOS 官方网站](https://www.freertos.org/)


## 💬 团队协作

- 📌 遵守 [代码规范.md](代码规范.md) 中的所有规则
- 🔍 在提交前务必使用 clang-format 格式化代码
- 🚀 使用 `cmake --build` 而不是 `make` 进行构建
- 📝 在 Git 提交时写清楚 commit 信息

## 📞 支持与反馈

如有问题或建议，请联系组内成员。

---

**祝大家开发愉快！🎉**
