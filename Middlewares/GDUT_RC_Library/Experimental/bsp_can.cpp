#include "bsp_can.hpp"

// CAN FIFO0 接收中断回调（HAL 库会在 FIFO0 有消息挂起时自动调用）
// 强符号定义以可靠覆盖 HAL 的 __weak 默认实现
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
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
// 强符号定义以可靠覆盖 HAL 的 __weak 默认实现
extern "C" void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
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
