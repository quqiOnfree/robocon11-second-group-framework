#ifndef BSP_UART_HPP   
#define BSP_UART_HPP


#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_dma.h"
#include "bsp_gpio_pin.hpp"
#include "bsp_type_traits.hpp"

#include "bsp_uncopyable.hpp"
#include "functional"
#include "cstdint"
#include "vector"
#include "cstring"
#include <atomic>
#include <cstddef>
#include <cstdint>


namespace gdut 
{
class urat;

class uart_manager {
public:
    // 回调函数类型定义
    using RxCallbackType = std::function<void(const uint8_t* data, uint16_t size)>;
    using TxCallbackType = std::function<void()>;
    using ErrorCallbackType = std::function<void(uint32_t error)>;
    using IdleCallbackType = std::function<void()>;
    using DmaRxCpltCallbackType = std::function<void()>;
    using DmaTxCpltCallbackType = std::function<void()>;
    using DmaErrorCallbackType = std::function<void()>;

    // 注册接收完成回调
    void register_rx_callback(RxCallbackType cb) {
        rx_cb_ = cb;
    }
    
    // 注册发送完成回调
    void register_tx_callback(TxCallbackType cb) {
        tx_cb_ = cb;
    }
    
    // 注册错误回调
    void register_error_callback(ErrorCallbackType cb) {
        error_cb_ = cb;
    }
    
    // 注册空闲中断回调
    void register_idle_callback(IdleCallbackType cb) {
        idle_cb_ = cb;
    }
    
    // 注册DMA接收完成回调
    void register_dma_rx_cplt_callback(DmaRxCpltCallbackType cb) {
        dma_rx_cplt_cb_ = cb;
    }
    
    // 注册DMA发送完成回调
    void register_dma_tx_cplt_callback(DmaTxCpltCallbackType cb) {
        dma_tx_cplt_cb_ = cb;
    }
    
    // 注册DMA错误回调
    void register_dma_error_callback(DmaErrorCallbackType cb) {
        dma_error_cb_ = cb;
    }
    
    // 调用回调函数
    void call_rx_callback(const uint8_t* data, uint16_t size) {
        if (rx_cb_) rx_cb_(data, size);
    }
    void call_tx_callback() {
        if (tx_cb_) tx_cb_();
    }
    void call_error_callback(uint32_t error) {
        if (error_cb_) error_cb_(error);
    }
    void call_idle_callback() {
        if (idle_cb_) idle_cb_();
    }
    void call_dma_rx_cplt_callback() {
        if (dma_rx_cplt_cb_) dma_rx_cplt_cb_();
    }
    void call_dma_tx_cplt_callback() {
        if (dma_tx_cplt_cb_) dma_tx_cplt_cb_();
    }
    void call_dma_error_callback() {
        if (dma_error_cb_) dma_error_cb_();
    }

private:
    RxCallbackType rx_cb_ = nullptr;             // 接收完成回调
    TxCallbackType tx_cb_ = nullptr;             // 发送完成回调
    ErrorCallbackType error_cb_ = nullptr;       // 错误回调
    IdleCallbackType idle_cb_ = nullptr;         // 空闲中断回调
    DmaRxCpltCallbackType dma_rx_cplt_cb_ = nullptr;  // DMA接收完成回调
    DmaTxCpltCallbackType dma_tx_cplt_cb_ = nullptr;  // DMA发送完成回调
    DmaErrorCallbackType dma_error_cb_ = nullptr;     // DMA错误回调
};


class uart:private uncopyable {
public:
    uart(UART_HandleTypeDef* huart, 
         DMA_HandleTypeDef* hdma_rx = nullptr, 
         DMA_HandleTypeDef* hdma_tx = nullptr)
        : huart_(huart), hdma_rx_(nullptr), hdma_tx_(nullptr), callback_mgr_() {
        if (hdma_rx) attach_dma_rx(hdma_rx);
        if (hdma_tx) attach_dma_tx(hdma_tx);
    }

     // 初始化
    HAL_StatusTypeDef init() {return HAL_UART_Init(huart_);}
     HAL_StatusTypeDef deinit() {return HAL_UART_DeInit(huart_);}

      // 发送函数（阻塞模式）
    HAL_StatusTypeDef send(const uint8_t* data, uint16_t size, uint32_t timeout ) {
        return HAL_UART_Transmit(huart_,const_cast<uint8_t*>(data), size, timeout);
    }
    HAL_StatusTypeDef send(const char* str, uint32_t timeout ) {
        return send(reinterpret_cast<const uint8_t*>(str), strlen(str), timeout);
    }

    // 接收函数（阻塞模式）
    HAL_StatusTypeDef receive(uint8_t* data, uint16_t size, uint32_t timeout) {
        return HAL_UART_Receive(huart_, data, size, timeout);
    }

    // 发送函数（中断模式）
    HAL_StatusTypeDef send_it(const uint8_t* data, uint16_t size) {
        return HAL_UART_Transmit_IT(huart_, const_cast<uint8_t*>(data), size);
    }
    HAL_StatusTypeDef send_it(const char* str) {
        return send_it(reinterpret_cast<const uint8_t*>(str), strlen(str));
    }
    
    // 接收函数（中断模式）
    HAL_StatusTypeDef receive_it(uint8_t* data, uint16_t size) {
        return HAL_UART_Receive_IT(huart_, data, size);
    }
    
    // 发送函数（DMA模式）
    HAL_StatusTypeDef send_dma(const uint8_t* data, uint16_t size) {
        if (!hdma_tx_) return HAL_ERROR;
        return HAL_UART_Transmit_DMA(huart_, const_cast<uint8_t*>(data), size);
    }
    HAL_StatusTypeDef send_dma(const char* str) {
        return send_dma(reinterpret_cast<const uint8_t*>(str), strlen(str));
    }

    // 接收函数（DMA模式）
    HAL_StatusTypeDef receive_dma(uint8_t* data, uint16_t size) {
        if (!hdma_rx_) return HAL_ERROR;
        return HAL_UART_Receive_DMA(huart_, data, size);
    }

    // 中断控制
    HAL_StatusTypeDef enable_it(uint32_t interrupt) {
        __HAL_UART_ENABLE_IT(huart_, interrupt);
        return HAL_OK;
    }
    HAL_StatusTypeDef disable_it(uint32_t interrupt) {
        __HAL_UART_DISABLE_IT(huart_, interrupt);
        return HAL_OK;
    }
    

    // 获取HAL句柄
    UART_HandleTypeDef* get_huart() const {return huart_;}
    
    // 获取DMA句柄
    DMA_HandleTypeDef* get_hdma_rx() const {return hdma_rx_;}
    DMA_HandleTypeDef* get_hdma_tx() const {return hdma_tx_;}

    // 检查发送是否完成
    bool is_tx_complete() const {
        return __HAL_UART_GET_FLAG(huart_, UART_FLAG_TC) != RESET;
    }

    // 检查是否有数据可读
    bool is_rx_ready() const {
        return __HAL_UART_GET_FLAG(huart_, UART_FLAG_RXNE) != RESET;
    }

    // 获取UART实例索引
    uint8_t get_uart_index() const {
        if (huart_->Instance == USART1) return 1;
        if (huart_->Instance == USART2) return 2;
        if (huart_->Instance == USART3) return 3;
        if (huart_->Instance == UART4) return 4;
        if (huart_->Instance == UART5) return 5;
        if (huart_->Instance == USART6) return 6;
        return 0xFF;
    }

    // 配置接口
    void set_baudrate(uint32_t baudrate) {
        huart_->Init.BaudRate = baudrate;
    }
    void set_word_length(uint32_t word_length) {
        huart_->Init.WordLength = word_length;
    }
    void set_stop_bits(uint32_t stop_bits) {
        huart_->Init.StopBits = stop_bits;
    }
    void set_parity(uint32_t parity) {
        huart_->Init.Parity = parity;
    }
    void set_mode(uint32_t mode) {
        huart_->Init.Mode = mode;
    }
    void set_hw_flow_ctl(uint32_t hw_flow_ctl) {
        huart_->Init.HwFlowCtl = hw_flow_ctl;
    }
    void set_over_sampling(uint32_t over_sampling) {
        huart_->Init.OverSampling = over_sampling;
    }
    HAL_StatusTypeDef apply_config() {
        return HAL_UART_Init(huart_);
    }

    // DMA关联函数
    void attach_dma_rx(DMA_HandleTypeDef* hdma_rx) {
        hdma_rx_ = hdma_rx;
        if (!hdma_rx_) return;
        
        // 关联DMA回调
        hdma_rx->Parent = this;
        hdma_rx->XferCpltCallback = &uart::dma_rx_xfer_cplt_cb;
        hdma_rx->XferHalfCpltCallback = &uart::dma_rx_xfer_half_cplt_cb;
        hdma_rx->XferErrorCallback = &uart::dma_error_cb;
        hdma_rx->XferAbortCallback = &uart::dma_abort_cb;
        
        // 设置UART的DMA接收句柄
        __HAL_LINKDMA(huart_, hdmarx, *hdma_rx_);
    }
    
    void attach_dma_tx(DMA_HandleTypeDef* hdma_tx) {
        hdma_tx_ = hdma_tx;
        if (!hdma_tx_) return;
        
        // 关联DMA回调
        hdma_tx->Parent = this;
        hdma_tx->XferCpltCallback = &uart::dma_tx_xfer_cplt_cb;
        hdma_tx->XferHalfCpltCallback = nullptr;  // 发送通常不需要半传输完成回调
        hdma_tx->XferErrorCallback = &uart::dma_error_cb;
        hdma_tx->XferAbortCallback = &uart::dma_abort_cb;
        // 设置UART的DMA发送句柄
        __HAL_LINKDMA(huart_, hdmatx, *hdma_tx_);
    }
    
    // DMA静态回调函数
    static void dma_rx_xfer_cplt_cb(DMA_HandleTypeDef* hdma) {
        if (!hdma) return;
        uart* u = static_cast<uart*>(hdma->Parent);
        if (u) u->callback_mgr_.call_dma_rx_cplt_callback();
    }
    static void dma_rx_xfer_half_cplt_cb(DMA_HandleTypeDef* hdma) {
        if (!hdma) return;
        uart* u = static_cast<uart*>(hdma->Parent);
        if (u) u->callback_mgr_.call_dma_rx_cplt_callback();  // 半传输完成也调用同一回调
    }
    static void dma_tx_xfer_cplt_cb(DMA_HandleTypeDef* hdma) {
        if (!hdma) return;
        uart* u = static_cast<uart*>(hdma->Parent);
        if (u) u->callback_mgr_.call_dma_tx_cplt_callback();
    }
    static void dma_error_cb(DMA_HandleTypeDef* hdma) {
        if (!hdma) return;
        uart* u = static_cast<uart*>(hdma->Parent);
        if (u) u->callback_mgr_.call_dma_error_callback();
    }
    static void dma_abort_cb(DMA_HandleTypeDef* hdma) {
        if (!hdma) return;
        uart* u = static_cast<uart*>(hdma->Parent);
        if (u) u->callback_mgr_.call_dma_error_callback();
    }

    // 回调注册接口
    void register_rx_callback(uart_manager::RxCallbackType cb) {
        callback_mgr_.register_rx_callback(cb);
    }
    void register_tx_callback(uart_manager::TxCallbackType cb) {
        callback_mgr_.register_tx_callback(cb);
    }
    void register_error_callback(uart_manager::ErrorCallbackType cb) {
        callback_mgr_.register_error_callback(cb);
    }
    void register_idle_callback(uart_manager::IdleCallbackType cb) {
        callback_mgr_.register_idle_callback(cb);
    }
    void register_dma_rx_cplt_callback(uart_manager::DmaRxCpltCallbackType cb) {
        callback_mgr_.register_dma_rx_cplt_callback(cb);
    }
    void register_dma_tx_cplt_callback(uart_manager::DmaTxCpltCallbackType cb) {
        callback_mgr_.register_dma_tx_cplt_callback(cb);
    }
    void register_dma_error_callback(uart_manager::DmaErrorCallbackType cb) {
        callback_mgr_.register_dma_error_callback(cb);
    }
    
    // 回调调用接口（供中断服务例程使用）
    void call_rx_callback(const uint8_t* data, uint16_t size) {
        callback_mgr_.call_rx_callback(data, size);
    }
    void call_tx_callback() {
        callback_mgr_.call_tx_callback();
    }
    void call_error_callback(uint32_t error) {
        callback_mgr_.call_error_callback(error);
    }
    void call_idle_callback() {
        callback_mgr_.call_idle_callback();
    }
    void call_dma_rx_cplt_callback() {
        callback_mgr_.call_dma_rx_cplt_callback();
    }
    void call_dma_tx_cplt_callback() {
        callback_mgr_.call_dma_tx_cplt_callback();
    }
    void call_dma_error_callback() {
        callback_mgr_.call_dma_error_callback();
    }

protected:
    UART_HandleTypeDef* huart_;
    DMA_HandleTypeDef* hdma_rx_;    //接收DMA句柄
    DMA_HandleTypeDef* hdma_tx_;    //发送DMA句柄
    uart_manager callback_mgr_;


};

class uart_irq_handler {
public:
     static void register_uart(uart* uart_obj) {
        if (!uart_obj) return;
        uint8_t idx = uart_obj->get_uart_index();
        if (idx<6) {
            uarts_[idx] = uart_obj;
        }
    }
    static void unregister_uart(uart* uart_obj) {
        if (!uart_obj) return;
        
        uint8_t idx = uart_obj->get_uart_index();
        if (idx < 6) {
            uarts_[idx] = nullptr;
        }
    }
    // 接收完成中断
    static void handle_rx_cplt(USART_TypeDef* instance, const uint8_t* data, uint16_t size) {
        uint8_t idx = get_uart_index(instance);
        if (idx < 6 && uarts_[idx]) {
            uarts_[idx]->call_rx_callback(data, size);
        }
    }
    
    // 发送完成中断
    static void handle_tx_cplt(USART_TypeDef* instance) {
        uint8_t idx = get_uart_index(instance);
        if (idx < 6 && uarts_[idx]) {
            uarts_[idx]->call_tx_callback();
        }
    }
    
    // 错误中断
    static void handle_error(USART_TypeDef* instance, uint32_t error) {
        uint8_t idx = get_uart_index(instance);
        if (idx < 6 && uarts_[idx]) {
            uarts_[idx]->call_error_callback(error);
        }
    }
    
    // 空闲中断
    static void handle_idle(USART_TypeDef* instance) {
        uint8_t idx = get_uart_index(instance);
        if (idx < 6 && uarts_[idx]) {
            uarts_[idx]->call_idle_callback();
        }
    }
    
    // 接收中断（字节接收）
    static void handle_rx_byte(USART_TypeDef* instance, uint8_t data) {
        uint8_t idx = get_uart_index(instance);
        if (idx < 6 && uarts_[idx]) {
            // 这里可以添加环形缓冲区处理
            // uarts_[idx]->rx_buffer_.push_back(data);
            // 或者直接回调
            uarts_[idx]->call_rx_callback(&data, 1);
        }
    }
    
private:
    static uart* uarts_[6]; // 支持USART1, USART2, USART3, UART4, UART5, USART6
    
    static uint8_t get_uart_index(USART_TypeDef* instance) {
        if (instance == USART1) return 1;
        if (instance == USART2) return 2;
        if (instance == USART3) return 3;
        if (instance == UART4) return 4;
        if (instance == UART5) return 5;
        if (instance == USART6) return 6;
        return 0xFF;
    }
};
uart* uart_irq_handler::uarts_[6] = {nullptr};


}// namespace gdut
#endif // BSP_UART_HPP