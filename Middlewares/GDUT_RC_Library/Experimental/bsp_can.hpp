#ifndef BSP_CAN_HPP
#define BSP_CAN_HPP

#include "stm32f407xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace gdut {

enum class can_type : uint8_t { standard_type = 1, extended_type = 2 };

enum class can_mailbox : uint32_t { mailbox0 = 1, mailbox1 = 2, mailbox2 = 4 };

enum class can_fifo : uint8_t { fifo0 = 0, fifo1 = 1 };

inline constexpr can_mailbox operator|(can_mailbox lhs, can_mailbox rhs) {
  return static_cast<can_mailbox>(std::to_underlying(lhs) |
                                  std::to_underlying(rhs));
}

inline constexpr can_mailbox operator&(can_mailbox lhs, can_mailbox rhs) {
  return static_cast<can_mailbox>(std::to_underlying(lhs) &
                                  std::to_underlying(rhs));
}

inline constexpr can_mailbox all_mailboxes =
    can_mailbox::mailbox0 | can_mailbox::mailbox1 | can_mailbox::mailbox2;

inline constexpr bool mailbox_allowed(can_mailbox mask, can_mailbox value) {
  return std::to_underlying(mask & value) != 0U;
}

class base_can_proxy {
public:
  static constexpr size_t can_max_count =
      10;                                // 每个总线最多支持 10 个 CAN 代理实例
  static constexpr size_t bus_count = 2; // STM32F407 支持 CAN1/CAN2 两条总线

  virtual ~base_can_proxy() noexcept = default;

  // 注销当前实例（从全局实例表中移除，避免悬空指针）
  bool unregister_self(size_t bus_index) {
    uint32_t can_id = get_can_id();
    if (bus_index >= bus_count) {
      return false; // 总线索引越界
    }

    bool success = false;

    // 保护实例表的修改，避免与中断中的 dispatch 并发访问
    __disable_irq();

    // 查找并移除当前实例
    for (size_t i = 0; i < instances_count[bus_index]; ++i) {
      if (instances[bus_index][i] == this) {
        // 将后续元素前移覆盖当前位置
        for (size_t j = i; j < instances_count[bus_index] - 1; ++j) {
          instances[bus_index][j] = instances[bus_index][j + 1];
        }
        instances[bus_index][instances_count[bus_index] - 1] = nullptr;
        instances_count[bus_index]--;
        success = true;
        break;
      }
    }

    __enable_irq();

    return success;
  }

  // 将当前实例注册到全局实例表，并按 CAN ID 排序以便二分查找
  bool register_self(size_t bus_index) {
    uint32_t can_id = get_can_id();
    // 快速失败检查：不进入临界区
    if (bus_index >= bus_count || instances_count[bus_index] >= can_max_count) {
      return false; // 总线索引越界或实例表已满
    }

    bool success = false;

    // 保护实例表的修改，避免与中断中的 dispatch 并发访问
    __disable_irq();

    // 在临界区内再次检查容量和重复 ID，以防并发注册导致状态改变
    if (instances_count[bus_index] < can_max_count) {
      // 检查是否已注册相同 ID（遍历已有实例）
      bool id_conflict = false;
      for (size_t i = 0; i < instances_count[bus_index]; ++i) {
        if (instances[bus_index][i] != nullptr &&
            instances[bus_index][i]->get_can_id() == can_id) {
          id_conflict = true;
          break;
        }
      }

      if (!id_conflict) {
        // 追加到末尾
        instances[bus_index][instances_count[bus_index]] = this;
        instances_count[bus_index]++;
        // 按 CAN ID 升序排序（便于二分查找）
        std::sort(instances[bus_index],
                  instances[bus_index] + instances_count[bus_index],
                  [](const base_can_proxy *a, const base_can_proxy *b) {
                    return a->get_can_id() < b->get_can_id();
                  });
        success = true;
      }
    }

    __enable_irq();

    return success;
  }

  // 全局分发函数：从接收中断回调中调用，二分查找对应的实例并调用其 receive
  static void dispatch(size_t bus_index, CAN_RxHeaderTypeDef *rxh,
                       uint8_t data[8]) {
    if (bus_index >= bus_count) {
      return; // 总线索引非法
    }
    // 从帧头提取 CAN ID（区分标准帧/扩展帧）
    uint32_t can_id = (rxh->IDE == CAN_ID_STD) ? rxh->StdId : rxh->ExtId;
    using iter_t = base_can_proxy **;
    iter_t begin = instances[bus_index];
    iter_t end = instances[bus_index] + instances_count[bus_index];
    // 二分查找 CAN ID 对应的实例（实例数组已按 ID 升序排列）
    iter_t it = std::lower_bound(begin, end, can_id,
                                 [](const base_can_proxy *proxy, uint32_t id) {
                                   return proxy->get_can_id() < id;
                                 });
    if (it != end && (*it)->get_can_id() == can_id) {
      (*it)->receive(rxh, data); // 交给实例处理接收数据
    }
  }

protected:
  // 获取当前实例的 CAN ID（子类必须实现）
  virtual uint32_t get_can_id() const = 0;
  // 接收回调（子类可选实现，用于处理接收到的 CAN 数据）
  virtual bool receive(CAN_RxHeaderTypeDef *rxh, uint8_t data[8]) {
    static_cast<void>(rxh);
    static_cast<void>(data);
    return true;
  }

private:
  inline static base_can_proxy *instances[bus_count][can_max_count] =
      {};                                               // 全局实例表
  inline static size_t instances_count[bus_count] = {}; // 各总线已注册实例数量
};

template <can_type Type, uint32_t CanId,
          can_mailbox MailboxMask = all_mailboxes>
class can_proxy : public base_can_proxy {
public:
  // 编译期检查 CAN ID 合法性（标准帧 11 位，扩展帧 29 位）
  static_assert((Type == can_type::standard_type && CanId <= 0x7FF) ||
                    (Type == can_type::extended_type && CanId <= 0x1FFFFFFF),
                "Invalid CAN ID for the specified CAN type.");
  // 发送帧头模板
  static constexpr CAN_TxHeaderTypeDef tx_header_v = {
      .StdId = (Type == can_type::standard_type) ? CanId : 0,
      .ExtId = (Type == can_type::extended_type) ? CanId : 0,
      .IDE = (Type == can_type::standard_type) ? CAN_ID_STD : CAN_ID_EXT,
      .RTR = CAN_RTR_DATA,
      .DLC = 8, // 固定发送 8 字节数据帧
      .TransmitGlobalTime = DISABLE};

  can_proxy(CAN_HandleTypeDef &hcan) : m_hcan(hcan) {}
  virtual ~can_proxy() noexcept = default;

  bool start() { return HAL_CAN_Start(&m_hcan) == HAL_OK; }

  bool stop() { return HAL_CAN_Stop(&m_hcan) == HAL_OK; }

  // 发送 CAN 数据帧（固定 8 字节）
  bool transmit(const uint8_t data[8]) {
    if (data == nullptr) {
      return false; // 参数非法
    }
    // 检查是否有空闲邮箱
    if (HAL_CAN_GetTxMailboxesFreeLevel(&m_hcan) == 0U) {
      return false; // 3 个邮箱都满，无法发送
    }
    // 复制模板帧头（DLC 固定为 8）
    CAN_TxHeaderTypeDef header = tx_header_v;
    uint32_t mailbox{0};
    // 将帧加入发送队列，硬件会自动仲裁并发送
    auto status = HAL_CAN_AddTxMessage(&m_hcan, &header, data, &mailbox);
    if (status != HAL_OK) {
      return false; // HAL 发送失败
    }
    // 检查是否使用了允许的邮箱（可选的邮箱掩码限制）
    if (!mailbox_allowed(MailboxMask, static_cast<can_mailbox>(mailbox))) {
      HAL_CAN_AbortTxRequest(&m_hcan, mailbox); // 终止不符合要求的邮箱
      return false;
    }
    return true;
  }

protected:
  // 返回模板参数指定的 CAN ID
  virtual uint32_t get_can_id() const override { return CanId; }

private:
  CAN_HandleTypeDef &m_hcan; // CAN 外设句柄引用
};

} // namespace gdut

// CAN FIFO0 接收中断回调（HAL 库会在 FIFO0 有消息挂起时自动调用）
extern "C" inline void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  CAN_RxHeaderTypeDef rxh;
  uint8_t data[8];
  // 从 FIFO0 读取一帧数据
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxh, data) != HAL_OK) {
    return; // 读取失败，跳过
  }

  // 根据 CAN 外设实例确定总线索引（CAN1=0, CAN2=1）
  size_t bus_index = (hcan->Instance == CAN1)   ? 0
                     : (hcan->Instance == CAN2) ? 1
                                                : 2;
  if (bus_index >= gdut::base_can_proxy::bus_count) {
    return; // 未知总线，跳过
  }

  // 交给全局分发函数，二分查找并调用对应实例的 receive
  gdut::base_can_proxy::dispatch(bus_index, &rxh, data);
}

// CAN FIFO1 接收中断回调（HAL 库会在 FIFO1 有消息挂起时自动调用）
extern "C" inline void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  CAN_RxHeaderTypeDef rxh;
  uint8_t data[8];
  // 从 FIFO1 读取一帧数据
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &rxh, data) != HAL_OK) {
    return; // 读取失败，跳过
  }

  // 根据 CAN 外设实例确定总线索引（CAN1=0, CAN2=1）
  size_t bus_index = (hcan->Instance == CAN1)   ? 0
                     : (hcan->Instance == CAN2) ? 1
                                                : 2;
  if (bus_index >= gdut::base_can_proxy::bus_count) {
    return; // 未知总线，跳过
  }

  // 交给全局分发函数，二分查找并调用对应实例的 receive
  gdut::base_can_proxy::dispatch(bus_index, &rxh, data);
}

#endif // BSP_CAN_HPP
