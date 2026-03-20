#include "cmsis_os2.h"
#include <cstdio>   

namespace BspTest {
    
    static osSemaphoreId_t sem_id = nullptr;
    
    void task_wait(void *arg) {
        while (1) {
            osSemaphoreAcquire(sem_id, osWaitForever);
            std::printf("Task Wait: Got semaphore!\n");
            osDelay(1000);
        }
    }
    
    void task_signal(void *arg) {
        uint32_t count = 0;
        while (1) {
            osDelay(2000);
            osSemaphoreRelease(sem_id);
            std::printf("Task Signal: Released semaphore (%lu)\n", ++count);
        }
    }
    
    void test_semaphore_init() {
        sem_id = osSemaphoreNew(1, 0, nullptr);
        if (sem_id == nullptr) {
            std::printf("Failed to create semaphore\n");
            return;
        }
        osThreadNew(task_wait, nullptr, nullptr);
        osThreadNew(task_signal, nullptr, nullptr);
    }
    
    // 定时器
    static osTimerId_t timer_id = nullptr;
    static volatile uint32_t timer_count = 0;
    
    void timer_callback(void *arg) {
        timer_count++;
        std::printf("Timer callback: %lu\n", timer_count);
    }
    
    void test_timer_init() {
        timer_id = osTimerNew(timer_callback, osTimerPeriodic, nullptr, nullptr);
        if (timer_id == nullptr) {
            std::printf("Failed to create timer\n");
            return;
        }
        osTimerStart(timer_id, 1000U);
    }
} 