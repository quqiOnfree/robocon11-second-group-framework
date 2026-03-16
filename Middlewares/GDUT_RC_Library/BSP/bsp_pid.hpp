#ifndef BSP_PID_HPP
#define BSP_PID_HPP

#include "bsp_timer.hpp"
#include <cmath>
namespace gdut {

struct pid 
{
private:
    float ki, kp, kd;                                        //ki积分系数，kp比例系数，kd微分系数
    float last_error,current_error;                          //last_error上次误差，current_error当前误差
    float PID_MAX, PID_MIN;                                  //PID最大输出，PID最小输出
    float pid_output,last_pid_output;                        //PID输出,上次PID输出
    float error_integral,integral_limit,dead_zone;           //error_integral误差积分，integral_limit积分限幅，dead_zone死区
public:
    pid(float kp, float ki, float kd, float PID_MAX, float PID_MIN, float integral_limit, 
         float dead_zone)
    {
        this->ki = ki;
        this->kp = kp;
        this->kd = kd;
        this->PID_MAX = PID_MAX;
        this->PID_MIN = PID_MIN;
        this->integral_limit = integral_limit;
        this->last_error = 0;
        this->current_error = 0;
        this->pid_output = 0;
        this->error_integral = 0;
        this->last_pid_output = 0;
        this->dead_zone =dead_zone;
    }
    ~pid() = default;
    float pid_calculate(float error)
    {
        this->current_error = error;
        //死区处理
        if(dead_zone != 0 && fabs(current_error) < dead_zone)
        {
            pid_output = last_pid_output;
            return pid_output;
        }
        
        //计算PID输出
        float P_out = kp * current_error;

        error_integral += current_error;
        if(error_integral > integral_limit) error_integral = integral_limit;
        if(error_integral < -integral_limit) error_integral = -integral_limit;
        float I_out = ki * error_integral;
        
        float D_out = kd * (current_error - last_error);
        
        //pid输出总和
        pid_output = P_out + I_out + D_out;

        //输出限幅
        if(pid_output > PID_MAX) pid_output = PID_MAX;
        if(pid_output < PID_MIN) pid_output = PID_MIN;

        //更新历史状态
        last_error = current_error;
        last_pid_output = pid_output;

        return pid_output;
    }

    //公开current_error接口
    float get_current_error() 
    {
        return current_error;
    }

    //公开pid_output接口
    void set_pid(float new_kp, float new_ki, float new_kd)
    {
        this->kp = new_kp;
        this->ki = new_ki;
        this->kd = new_kd;
    }

    //设计current_error接口
    void  set_current_error(float error)
    {
        this->current_error = error;
    }

    //积分项上限
    void reset_integral()
    {
        this->error_integral = 0;
    }

    float get_pid_output()
    {
        return pid_output;
    }

    float get_integral()
    {
        return error_integral;
    }   

    void set_output_limits(float new_max, float new_min) {
        if (new_max > new_min) {
            PID_MAX = new_max;
            PID_MIN = new_min;
        }
    }

    // 动态设置积分限幅和死区
    void set_integral_limits(float new_integral_limit, float new_dead_zone) {
        if (new_integral_limit >= 0 && new_dead_zone >= 0) {
            integral_limit = new_integral_limit;
            dead_zone = new_dead_zone;
        }
    }
};

} // namespace gdut

#endif // BSP_PID_HPP