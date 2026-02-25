#ifndef BSP_TIMER_HPP
#define BSP_TIMER_HPP

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_tim.h"

#include "bsp_uncopyable.hpp"

#include <functional>
#include <utility>

namespace gdut {

class timer : private uncopyable {
public:
  using callback_t = std::function<void()>;

  // 构造函数
  timer(TIM_HandleTypeDef *htim, DMA_HandleTypeDef *hdma = nullptr)
      : htim(htim), hdma(nullptr) {
    if (hdma)
      attach_dma(hdma);
  }
  // 初始化和去初始化定时器
  HAL_StatusTypeDef init() { return HAL_TIM_Base_Init(htim); }
  HAL_StatusTypeDef deinit() { return HAL_TIM_Base_DeInit(htim); }

  // 启动和停止定时器
  HAL_StatusTypeDef start() { return HAL_TIM_Base_Start(htim); }
  HAL_StatusTypeDef stop() { return HAL_TIM_Base_Stop(htim); }

  // 中断和 DMA 控制
  HAL_StatusTypeDef enable_it(uint32_t interrupt) {
    __HAL_TIM_ENABLE_IT(htim, interrupt);
    return HAL_OK;
  }
  HAL_StatusTypeDef disable_it(uint32_t interrupt) {
    __HAL_TIM_DISABLE_IT(htim, interrupt);
    return HAL_OK;
  }
  // 回调注册接口（委托给 callback_manager）
  void register_period_elapsed_callback(callback_t cb) {
    m_callbacks.period_elapsed_cb = std::move(cb);
  }

  [[nodiscard]] bool register_capture_callback(uint32_t channel,
                                               callback_t cb) {
    if (channel >= 1 && channel <= 4) {
      m_callbacks.capture_cbs[channel - 1] = std::move(cb);
      return true;
    }
    return false;
  }

  void register_error_callback(callback_t cb) {
    m_callbacks.error_cb = std::move(cb);
  }

  // 回调调用接口（供中断服务例程使用）
  void call_period_elapsed_callback() {
    if (m_callbacks.period_elapsed_cb) {
      std::invoke(m_callbacks.period_elapsed_cb);
    }
  }

  void call_capture_callback(uint32_t channel) {
    if (channel >= 1 && channel <= 4 && m_callbacks.capture_cbs[channel - 1]) {
      std::invoke(m_callbacks.capture_cbs[channel - 1]);
    }
  }

  void call_error_callback() {
    if (m_callbacks.error_cb) {
      std::invoke(m_callbacks.error_cb);
    }
  }

  // DMA 注册接口（委托给 callback_mgr_）
  void register_dma_xfer_cplt_callback(callback_t cb) {
    m_callbacks.dma_xfer_cplt_cb = std::move(cb);
  }
  void register_dma_xfer_half_callback(callback_t cb) {
    m_callbacks.dma_xfer_half_cb = std::move(cb);
  }
  void register_dma_error_callback(callback_t cb) {
    m_callbacks.dma_error_cb = std::move(cb);
  }

  // time提供时钟基功能
  class timer_proxy {
  public:
    timer_proxy(timer *timer) : timer(timer) {}
    ~timer_proxy() noexcept = default;
    timer_proxy(const timer_proxy &) = default;
    timer_proxy &operator=(const timer_proxy &other) = default;
    timer_proxy(timer_proxy &&other) noexcept
        : timer(std::exchange(other.timer, nullptr)) {}
    timer_proxy &operator=(timer_proxy &&other) noexcept {
      if (this != std::addressof(other)) {
        timer = std::exchange(other.timer, nullptr);
      }
      return *this;
    }

    HAL_StatusTypeDef base_init() { return HAL_TIM_Base_Init(timer->htim); }
    HAL_StatusTypeDef base_deinit() { return HAL_TIM_Base_DeInit(timer->htim); }

    void set_arr(uint32_t arr) { __HAL_TIM_SET_AUTORELOAD(timer->htim, arr); }
    uint32_t get_arr() const { return __HAL_TIM_GET_AUTORELOAD(timer->htim); }

    void set_psc(uint32_t psc) { __HAL_TIM_SET_PRESCALER(timer->htim, psc); }
    uint32_t get_psc() const { return timer->htim->Instance->PSC; }

    void set_counter(uint32_t counter) {
      __HAL_TIM_SET_COUNTER(timer->htim, counter);
    }
    uint32_t get_counter() const { return __HAL_TIM_GET_COUNTER(timer->htim); }

    timer *operator->() { return timer; }
    const timer *operator->() const { return timer; }

  private:
    timer *timer;
  };

  // time提供pwm
  class timer_pwm {
  public:
    timer_pwm(timer *timer) : m_proxy(timer) {}
    ~timer_pwm() = default;
    timer_pwm(const timer_pwm &) = default;
    timer_pwm &operator=(const timer_pwm &) = default;
    timer_pwm(timer_pwm &&other) noexcept : m_proxy(std::move(other.m_proxy)) {}
    timer_pwm &operator=(timer_pwm &&other) noexcept {
      if (this != std::addressof(other)) {
        m_proxy = std::move(other.m_proxy);
      }
      return *this;
    }

    HAL_StatusTypeDef pwm_start(uint32_t channel) {
      return HAL_TIM_PWM_Start(m_proxy->htim, channel);
    }
    HAL_StatusTypeDef pwm_stop(uint32_t channel) {
      return HAL_TIM_PWM_Stop(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef pwm_start_it(uint32_t channel) {
      return HAL_TIM_PWM_Start_IT(m_proxy->htim, channel);
    }
    HAL_StatusTypeDef pwm_stop_it(uint32_t channel) {
      return HAL_TIM_PWM_Stop_IT(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef pwm_start_dma(uint32_t channel, const uint32_t *pData,
                                    uint16_t Length) {
      return HAL_TIM_PWM_Start_DMA(m_proxy->htim, channel, pData, Length);
    }
    HAL_StatusTypeDef pwm_stop_dma(uint32_t channel) {
      return HAL_TIM_PWM_Stop_DMA(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef pwm_start_complementary(uint32_t channel) {
      return HAL_TIMEx_PWMN_Start(m_proxy->htim, channel);
    }
    HAL_StatusTypeDef pwm_stop_complementary(uint32_t channel) {
      return HAL_TIMEx_PWMN_Stop(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef set_duty(uint32_t channel, uint32_t duty) {
      switch (channel) {
      case TIM_CHANNEL_1:
        __HAL_TIM_SET_COMPARE(m_proxy->htim, TIM_CHANNEL_1, duty);
        break;
      case TIM_CHANNEL_2:
        __HAL_TIM_SET_COMPARE(m_proxy->htim, TIM_CHANNEL_2, duty);
        break;
      case TIM_CHANNEL_3:
        __HAL_TIM_SET_COMPARE(m_proxy->htim, TIM_CHANNEL_3, duty);
        break;
      case TIM_CHANNEL_4:
        __HAL_TIM_SET_COMPARE(m_proxy->htim, TIM_CHANNEL_4, duty);
        break;
      default:
        return HAL_ERROR;
      }
      return HAL_OK;
    }

  private:
    timer_proxy m_proxy;
  };

  // time提供编码器接口
  class timer_encoder {
  public:
    timer_encoder(timer *timer) : m_proxy(timer) {}
    ~timer_encoder() = default;
    timer_encoder(const timer_encoder &) = default;
    timer_encoder &operator=(const timer_encoder &) = default;
    timer_encoder(timer_encoder &&other) noexcept
        : m_proxy(std::move(other.m_proxy)) {}
    timer_encoder &operator=(timer_encoder &&other) noexcept {
      if (this != std::addressof(other)) {
        m_proxy = std::move(other.m_proxy);
      }
      return *this;
    }

    HAL_StatusTypeDef encoder_start(uint32_t channel) {
      return HAL_TIM_Encoder_Start(m_proxy->htim, channel);
    }
    HAL_StatusTypeDef encoder_stop(uint32_t channel) {
      return HAL_TIM_Encoder_Stop(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef encoder_start_it(uint32_t channel) {
      return HAL_TIM_Encoder_Start_IT(m_proxy->htim, channel);
    }
    HAL_StatusTypeDef encoder_stop_it(uint32_t channel) {
      return HAL_TIM_Encoder_Stop_IT(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef encoder_start_dma(uint32_t channel, uint32_t *pData1,
                                        uint32_t *pData2, uint16_t Length) {
      return HAL_TIM_Encoder_Start_DMA(m_proxy->htim, channel, pData1, pData2,
                                       Length);
    }
    HAL_StatusTypeDef encoder_stop_dma(uint32_t channel) {
      return HAL_TIM_Encoder_Stop_DMA(m_proxy->htim, channel);
    }

    bool is_counting_down() const {
      return __HAL_TIM_IS_TIM_COUNTING_DOWN(m_proxy->htim);
    }

    HAL_StatusTypeDef enable_index_interrupt(uint32_t interrupt) {
      __HAL_TIM_ENABLE_IT(m_proxy->htim, interrupt);
      return HAL_OK;
    }
    HAL_StatusTypeDef disable_index_interrupt(uint32_t interrupt) {
      __HAL_TIM_DISABLE_IT(m_proxy->htim, interrupt);
      return HAL_OK;
    }

  private:
    timer_proxy m_proxy;
  };

  // time提供输出比较接口
  class time_oc {
  public:
    time_oc(timer *timer) : m_proxy(timer) {}
    ~time_oc() = default;
    time_oc(const time_oc &) = default;
    time_oc &operator=(const time_oc &) = default;
    time_oc(time_oc &&other) noexcept : m_proxy(std::move(other.m_proxy)) {}
    time_oc &operator=(time_oc &&other) noexcept {
      if (this != std::addressof(other)) {
        m_proxy = std::move(other.m_proxy);
      }
      return *this;
    }

    HAL_StatusTypeDef oc_start(uint32_t channel) {
      return HAL_TIM_OC_Start(m_proxy->htim, channel);
    }
    HAL_StatusTypeDef oc_stop(uint32_t channel) {
      return HAL_TIM_OC_Stop(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef oc_start_it(uint32_t channel) {
      return HAL_TIM_OC_Start_IT(m_proxy->htim, channel);
    }
    HAL_StatusTypeDef oc_stop_it(uint32_t channel) {
      return HAL_TIM_OC_Stop_IT(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef set_compare(uint32_t channel, uint32_t compare) {
      __HAL_TIM_SET_COMPARE(m_proxy->htim, channel, compare);
      return HAL_OK;
    }
    uint32_t get_compare(uint32_t channel) {
      return __HAL_TIM_GET_COMPARE(m_proxy->htim, channel);
    }

  private:
    timer_proxy m_proxy;
  };

  // time提供输入捕获接口
  class timer_ic {
  public:
    timer_ic(timer *timer) : m_proxy(timer) {}
    ~timer_ic() = default;
    timer_ic(const timer_ic &) = default;
    timer_ic &operator=(const timer_ic &) = default;
    timer_ic(timer_ic &&other) noexcept : m_proxy(std::move(other.m_proxy)) {}
    timer_ic &operator=(timer_ic &&other) noexcept {
      if (this != std::addressof(other)) {
        m_proxy = std::move(other.m_proxy);
      }
      return *this;
    }

    HAL_StatusTypeDef ic_start(uint32_t channel) {
      return HAL_TIM_IC_Start(m_proxy->htim, channel);
    }
    HAL_StatusTypeDef ic_stop(uint32_t channel) {
      return HAL_TIM_IC_Stop(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef ic_start_it(uint32_t channel) {
      return HAL_TIM_IC_Start_IT(m_proxy->htim, channel);
    }
    HAL_StatusTypeDef ic_stop_it(uint32_t channel) {
      return HAL_TIM_IC_Stop_IT(m_proxy->htim, channel);
    }

    HAL_StatusTypeDef ic_start_dma(uint32_t channel, uint32_t *pData,
                                   uint16_t Length) {
      return HAL_TIM_IC_Start_DMA(m_proxy->htim, channel, pData, Length);
    }
    HAL_StatusTypeDef ic_stop_dma(uint32_t channel) {
      return HAL_TIM_IC_Stop_DMA(m_proxy->htim, channel);
    }
    uint32_t get_capture(uint32_t channel) {
      return __HAL_TIM_GET_COMPARE(m_proxy->htim, channel);
    }

  private:
    timer_proxy m_proxy;
  };

  // 将 HAL DMA 回调桥接到 timer_manager
  void attach_dma(DMA_HandleTypeDef *hdma) {
    this->hdma = hdma;
    if (!hdma)
      return;
    hdma->Parent = this;
    hdma->XferCpltCallback = &timer::dma_xfer_cplt_cb;
    hdma->XferHalfCpltCallback = &timer::dma_xfer_half_cplt_cb;
    hdma->XferErrorCallback = &timer::dma_error_cb;
    hdma->XferAbortCallback = &timer::dma_abort_cb;
  }

  static void dma_xfer_cplt_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma)
      return;
    timer *t = static_cast<timer *>(hdma->Parent);
    if (t && t->m_callbacks.dma_xfer_cplt_cb) {
      std::invoke(t->m_callbacks.dma_xfer_cplt_cb);
    }
  }
  static void dma_xfer_half_cplt_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma)
      return;
    timer *t = static_cast<timer *>(hdma->Parent);
    if (t && t->m_callbacks.dma_xfer_half_cb) {
      std::invoke(t->m_callbacks.dma_xfer_half_cb);
    }
  }
  static void dma_error_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma)
      return;
    timer *t = static_cast<timer *>(hdma->Parent);
    if (t && t->m_callbacks.dma_error_cb) {
      std::invoke(t->m_callbacks.dma_error_cb);
    }
  }
  static void dma_abort_cb(DMA_HandleTypeDef *hdma) {
    if (!hdma)
      return;
    timer *t = static_cast<timer *>(hdma->Parent);
    if (t && t->m_callbacks.dma_error_cb) {
      std::invoke(t->m_callbacks.dma_error_cb);
    }
  }

protected:
  struct timer_callbacks {
    callback_t period_elapsed_cb; // 更新中断回调
    callback_t capture_cbs[4];    // 捕获中断回调（最多4个通道）
    callback_t error_cb;
    callback_t dma_xfer_cplt_cb;
    callback_t dma_xfer_half_cb;
    callback_t dma_error_cb;
  };

private:
  TIM_HandleTypeDef *htim{nullptr};
  DMA_HandleTypeDef *hdma{nullptr};
  timer_callbacks m_callbacks{};
};
} // namespace gdut
#endif
