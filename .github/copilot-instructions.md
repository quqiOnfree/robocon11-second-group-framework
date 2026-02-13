# Copilot Instructions for Robocon11 Second Group STM32 Framework

## Project Overview
- **Target**: STM32F407VET6, embedded C++ (C++17+) with FreeRTOS and STM32 HAL.
- **Build System**: CMake (see `CMakeLists.txt`), Ninja recommended. Do NOT use `make` directly.
- **Key Libraries**: 
  - `Middlewares/GDUT_RC_Library`: Group-maintained, core data structures, drivers, and tools.
  - `Drivers/STM32F4xx_HAL_Driver`: STM32 HAL.
  - `Third_Party/FreeRTOS`: RTOS support.

## Essential Developer Workflows
- **Clone with submodules**: `git clone --recursive ...` or `git submodule update --init --recursive`.
- **Build**:
  - `cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake`
  - `cmake --build build`
- **Clean**: `cmake --build build --target clean` or delete `build/`.
- **Format**: Use `format_all.ps1` (Windows) or `format_all.sh` (Linux/macOS). All code must be clang-formatted before commit.
- **Code style**: Strict Modern C++ (C++17+), snake_case for all symbols, see `docs/代码规范.md` for details.

## Project Structure & Conventions
- **Headers**: `Core/Inc/`
- **Sources**: `Core/Src/`
- **Drivers**: `Drivers/`
- **Middlewares**: `Middlewares/` (esp. `GDUT_RC_Library`)
- **CMake/toolchain**: `cmake/`
- **Entry point**: `Core/Src/main.cpp`
- **All new code**: Use smart pointers (`std::unique_ptr`, `std::shared_ptr`), avoid raw pointers unless interfacing with hardware.
- **Naming**: All variables, functions, types, and enums use snake_case. Private members use `m_` prefix.
- **No C-style casts**: Use `static_cast`, `reinterpret_cast`, etc.
- **No direct `new`/`delete`**: Use `std::make_unique`/`std::make_shared`.
- **All FreeRTOS code**: Place in `freertos.c` or relevant RTOS files.

## Integration & Cross-Component Patterns
- **GDUT_RC_Library**: For reusable embedded abstractions, drivers, and utilities. Prefer using or extending this library for new hardware features.
- **HAL/FreeRTOS**: Use STM32 HAL for hardware, FreeRTOS for multitasking. See `Core/Src/` for integration examples.
- **CMake**: All new modules/components must be registered in the top-level `CMakeLists.txt`.

## References
- See `README.md` for build, format, and workflow details.
- See `docs/代码规范.md` for code style and patterns.
- See `Middlewares/GDUT_RC_Library/` for group library usage.

---

If any convention or workflow is unclear, consult the above files or ask for clarification.
