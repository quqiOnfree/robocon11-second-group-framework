#ifndef BSP_TIMER_HPP
#define BSP_TIMER_HPP

#include <functional>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_tim.h"

#include "bsp_gpio_pin.hpp"
#include "bsp_type_traits.hpp"

#include "bsp_uncopyable.hpp"

namespace gdut {

class time_manager {
public:

   // 回调函数类型定义
    using CallbackType = std::function<void()>;

    // 注册更新中断回调
    void register_period_elapsed_callback(CallbackType cb) {
        period_elapsed_cb_ = cb;
    }

    // 注册捕获中断回调
    void register_capture_callback(uint32_t channel, CallbackType cb) {
        if (channel >= 1 && channel <= 4) {
            capture_cb_[channel - 1] = cb;
        }
    }

    // 注册错误回调
    void register_error_callback(CallbackType cb) {
        error_cb_ = cb;
    }

    // 调用回调函数（供中断服务例程使用）
    void call_period_elapsed_callback() {
        if (period_elapsed_cb_) period_elapsed_cb_();
    }

    // 调用捕获中断回调
    void call_capture_callback(uint32_t channel) {
        if (channel >= 1 && channel <= 4 && capture_cb_[channel - 1]) {
            capture_cb_[channel - 1]();
        }
    }

    // 调用错误回调
    void call_error_callback() {
        if (error_cb_) error_cb_();
    }

    // DMA 回调注册与调用
    void register_dma_xfer_cplt_callback(CallbackType cb) { dma_xfer_cplt_cb_ = cb; }
    void register_dma_xfer_half_callback(CallbackType cb) { dma_xfer_half_cb_ = cb; }
    void register_dma_error_callback(CallbackType cb) { dma_error_cb_ = cb; }

    void call_dma_xfer_cplt_callback() { if (dma_xfer_cplt_cb_) dma_xfer_cplt_cb_(); }
    void call_dma_xfer_half_callback() { if (dma_xfer_half_cb_) dma_xfer_half_cb_(); }
    void call_dma_error_callback() { if (dma_error_cb_) dma_error_cb_(); }

  private:
  CallbackType period_elapsed_cb_;      // 更新中断回调
  CallbackType capture_cb_[4];             // 捕获中断回调（最多4个通道）
  CallbackType error_cb_;
  CallbackType dma_xfer_cplt_cb_;
  CallbackType dma_xfer_half_cb_;
  CallbackType dma_error_cb_;
};



class time:private uncopyable {

public:
  // 构造函数
  time(TIM_HandleTypeDef* htim, DMA_HandleTypeDef* hdma = nullptr)
    : htim(htim), hdma(nullptr), callback_mgr_() {
    if (hdma) attach_dma(hdma);
  }
  // 初始化和去初始化定时器
  HAL_StatusTypeDef init() { return HAL_TIM_Base_Init(htim); }
  HAL_StatusTypeDef deinit() { return HAL_TIM_Base_DeInit(htim); }
  
  // 启动和停止定时器
  HAL_StatusTypeDef start() { return HAL_TIM_Base_Start(htim); }
  HAL_StatusTypeDef stop() { return HAL_TIM_Base_Stop(htim); }

  // 中断和 DMA 控制
  HAL_StatusTypeDef enableIT(uint32_t interrupt) {
        __HAL_TIM_ENABLE_IT(htim, interrupt);
        return HAL_OK;
      }
  HAL_StatusTypeDef disableIT(uint32_t interrupt) {
      __HAL_TIM_DISABLE_IT(htim, interrupt);
      return HAL_OK;
  }
// 回调注册接口（委托给 callback_manager）
    void register_period_elapsed_callback(time_manager::CallbackType cb) {
      callback_mgr_.register_period_elapsed_callback(cb);
    }

    void register_capture_callback(uint32_t channel, time_manager::CallbackType cb) {
      callback_mgr_.register_capture_callback(channel, cb);
    }

    void register_error_callback(time_manager::CallbackType cb) {
      callback_mgr_.register_error_callback(cb);
    }

    // 回调调用接口（供中断服务例程使用）
    void call_period_elapsed_callback() {
      callback_mgr_.call_period_elapsed_callback();
    }

    void call_capture_callback(uint32_t channel) {
      callback_mgr_.call_capture_callback(channel);
    }

    void call_error_callback() {
      callback_mgr_.call_error_callback();
    }
    
    // DMA 注册接口（委托给 callback_mgr_）
    void register_dma_xfer_cplt_callback(time_manager::CallbackType cb) {
      callback_mgr_.register_dma_xfer_cplt_callback(cb);
    }
    void register_dma_xfer_half_callback(time_manager::CallbackType cb) {
      callback_mgr_.register_dma_xfer_half_callback(cb);
    }
    void register_dma_error_callback(time_manager::CallbackType cb) {
      callback_mgr_.register_dma_error_callback(cb);
    }
    //time提供时钟基功能
class time_base{
public:  
time_base(time* timer) : timer(timer) {}
  HAL_StatusTypeDef base_init() {
      return HAL_TIM_Base_Init(timer->htim);
  }
  HAL_StatusTypeDef base_deinit() {
      return HAL_TIM_Base_DeInit(timer->htim);
  }

  void set_arr(uint32_t arr) {
      __HAL_TIM_SET_AUTORELOAD(timer->htim, arr);
  }
  void get_arr(uint32_t* arr) {
      *arr = __HAL_TIM_GET_AUTORELOAD(timer->htim);
  }

  void set_psc(uint32_t psc) {
      __HAL_TIM_SET_PRESCALER(timer->htim, psc);
  }
  void get_psc(uint32_t* psc) {
      *psc = timer->htim->Instance->PSC;
  }

  void set_counter(uint32_t counter) {
      __HAL_TIM_SET_COUNTER(timer->htim, counter);
  }
  void get_counter(uint32_t* counter) {
      *counter = __HAL_TIM_GET_COUNTER(timer->htim);
  }

  protected:
    time* timer;
};


//time提供pwm
class time_pwm:public time_base {
public:
time_pwm(time* timer) : time_base(timer) {}

  HAL_StatusTypeDef pwm_start(uint32_t channel) {
    return HAL_TIM_PWM_Start(timer->htim,channel);
  }
  HAL_StatusTypeDef pwm_stop(uint32_t channel) {
    return HAL_TIM_PWM_Stop(timer->htim,channel);
  }

  HAL_StatusTypeDef pwm_start_IT(uint32_t channel) {
    return HAL_TIM_PWM_Start_IT(timer->htim,channel);
  }
  HAL_StatusTypeDef pwm_stop_IT(uint32_t channel) {
    return HAL_TIM_PWM_Stop_IT(timer->htim,channel);
  }

  HAL_StatusTypeDef pwm_start_DMA(uint32_t channel, const uint32_t *pData, uint16_t Length) {
    return HAL_TIM_PWM_Start_DMA(timer->htim, channel, pData, Length);
  }
  HAL_StatusTypeDef pwm_stop_DMA(uint32_t channel) {
    return HAL_TIM_PWM_Stop_DMA(timer->htim, channel);
  } 

  HAL_StatusTypeDef pwm_start_complementary(uint32_t channel) {
    return HAL_TIMEx_PWMN_Start( timer->htim,channel);
  }
  HAL_StatusTypeDef pwm_stop_complementary(uint32_t channel) {
    return HAL_TIMEx_PWMN_Stop(timer->htim,channel);
  }

   HAL_StatusTypeDef set_duty(uint32_t channel, uint32_t duty) {
        switch (channel) {
            case TIM_CHANNEL_1:
                __HAL_TIM_SET_COMPARE(timer->htim, TIM_CHANNEL_1, duty);
                break;
            case TIM_CHANNEL_2:
                __HAL_TIM_SET_COMPARE(timer->htim, TIM_CHANNEL_2, duty);
                break;
            case TIM_CHANNEL_3:
                __HAL_TIM_SET_COMPARE(timer->htim, TIM_CHANNEL_3, duty);
                break;
            case TIM_CHANNEL_4:
                __HAL_TIM_SET_COMPARE(timer->htim, TIM_CHANNEL_4, duty);
                break;
            default:
                return HAL_ERROR;
        }
        return HAL_OK;
    }

    
};


//time提供编码器接口
class time_encoder:public time_base {
public:
  using time_base::time_base;

  HAL_StatusTypeDef encoder_start(uint32_t channel) {
    return HAL_TIM_Encoder_Start(timer->htim,channel);
  }
  HAL_StatusTypeDef encoder_stop(uint32_t channel) {
    return HAL_TIM_Encoder_Stop(timer->htim,channel);
  }

  HAL_StatusTypeDef encoder_start_IT(uint32_t channel) {
    return HAL_TIM_Encoder_Start_IT(timer->htim,channel);
  }
  HAL_StatusTypeDef encoder_stop_IT(uint32_t channel) {
    return HAL_TIM_Encoder_Stop_IT(timer->htim,channel);
  }

  HAL_StatusTypeDef encoder_start_DMA(uint32_t channel, uint32_t *pData1, uint32_t *pData2,uint16_t Length) {
    return HAL_TIM_Encoder_Start_DMA(timer->htim, channel, pData1, pData2,Length);
  }
  HAL_StatusTypeDef encoder_stop_DMA(uint32_t channel) {
    return HAL_TIM_Encoder_Stop_DMA(timer->htim, channel);
  } 

  bool is_counting_down() const {
    return __HAL_TIM_IS_TIM_COUNTING_DOWN(timer->htim);
  }

  HAL_StatusTypeDef enable_index_interrupt(uint32_t interrupt) {
    __HAL_TIM_ENABLE_IT(timer->htim, interrupt);
    return HAL_OK;
  }
  HAL_StatusTypeDef disable_index_interrupt(uint32_t interrupt) {
    __HAL_TIM_DISABLE_IT(timer->htim, interrupt);
    return HAL_OK;
  }


};


//time提供输出比较接口
class time_oc:public time_base {
public:
  using time_base::time_base;

  HAL_StatusTypeDef oc_start(uint32_t channel) {
    return HAL_TIM_OC_Start(timer->htim,channel);
  }
  HAL_StatusTypeDef oc_stop(uint32_t channel) {
    return HAL_TIM_OC_Stop(timer->htim,channel);
  }

  HAL_StatusTypeDef oc_start_IT(uint32_t channel) {
    return HAL_TIM_OC_Start_IT(timer->htim,channel);
  } 
  HAL_StatusTypeDef oc_stop_IT(uint32_t channel) {
    return HAL_TIM_OC_Stop_IT(timer->htim,channel);
  }

  HAL_StatusTypeDef set_compare(uint32_t channel, uint32_t compare) {
    __HAL_TIM_SET_COMPARE(timer->htim, channel, compare);
    return HAL_OK;
  }
   uint32_t get_compare(uint32_t channel) {
    return __HAL_TIM_GET_COMPARE(timer->htim, channel);
  }
};

//time提供输入捕获接口
class time_ic:public time_base {
public:
  using time_base::time_base;

  HAL_StatusTypeDef ic_start(uint32_t channel) {
    return HAL_TIM_IC_Start(timer->htim,channel);
  }
  HAL_StatusTypeDef ic_stop(uint32_t channel) {
    return HAL_TIM_IC_Stop(timer->htim,channel);
  }

  HAL_StatusTypeDef ic_start_IT(uint32_t channel) {
    return HAL_TIM_IC_Start_IT(timer->htim,channel);
  }
  HAL_StatusTypeDef ic_stop_IT(uint32_t channel) {
    return HAL_TIM_IC_Stop_IT(timer->htim,channel);
  }
  
  HAL_StatusTypeDef ic_start_DMA(uint32_t channel, uint32_t *pData, uint16_t Length) {
    return HAL_TIM_IC_Start_DMA(timer->htim, channel, pData, Length);
  }
  HAL_StatusTypeDef ic_stop_DMA(uint32_t channel) {
    return HAL_TIM_IC_Stop_DMA(timer->htim, channel);
  }
  uint32_t get_capture(uint32_t channel) {
    return __HAL_TIM_GET_COMPARE(timer->htim, channel);
  }
};
  

// 将 HAL DMA 回调桥接到 time_manager
  void attach_dma(DMA_HandleTypeDef* hdma) {
    this->hdma = hdma;
    if (!hdma) return;
    hdma->Parent = this;
    hdma->XferCpltCallback = &time::dma_xfer_cplt_cb;
    hdma->XferHalfCpltCallback = &time::dma_xfer_half_cplt_cb;
    hdma->XferErrorCallback = &time::dma_error_cb;
    hdma->XferAbortCallback = &time::dma_abort_cb;
  }

  static void dma_xfer_cplt_cb(DMA_HandleTypeDef* hdma) {
    if (!hdma) return;
    time* t = static_cast<time*>(hdma->Parent);
    if (t) t->callback_mgr_.call_dma_xfer_cplt_callback();
  }
  static void dma_xfer_half_cplt_cb(DMA_HandleTypeDef* hdma) {
    if (!hdma) return;
    time* t = static_cast<time*>(hdma->Parent);
    if (t) t->callback_mgr_.call_dma_xfer_half_callback();
  }
  static void dma_error_cb(DMA_HandleTypeDef* hdma) {
    if (!hdma) return;
    time* t = static_cast<time*>(hdma->Parent);
    if (t) t->callback_mgr_.call_dma_error_callback();
  }
  static void dma_abort_cb(DMA_HandleTypeDef* hdma) {
    if (!hdma) return;
    time* t = static_cast<time*>(hdma->Parent);
    if (t) t->callback_mgr_.call_dma_error_callback();
  }

protected:
  TIM_HandleTypeDef* htim;
  DMA_HandleTypeDef* hdma;
  time_manager callback_mgr_;
  friend class time_pwm;
  friend class time_base;
  friend class time_encoder;
  friend class time_oc;
  friend class time_ic;
};
}// namespace gdut
#endif





