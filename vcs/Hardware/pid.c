#include "stm32f10x.h"                  // Device header
#include "pid.h"

/* PID 计算核心函数 */
float PID_Compute_Step(PID_Controller *pid)
{
    // 计算当前偏差
    float error = pid->target - pid->current;
    
    // 如果误差极小，直接消除静态抖动，防止舵机一直“滋滋”作响
    if(error > -0.5f && error < 0.5f) {
        pid->current = pid->target;
        return pid->current;
    }
    
    // PD 位置计算公式：输出增量 = Kp*误差 + Kd*误差变化率
    float step = (pid->Kp * error) + (pid->Kd * (error - pid->last_error));
    pid->last_error = error;
    
    // 速度限幅：防止单次转动过猛
    if(step > pid->max_step) step = pid->max_step;
    if(step < -pid->max_step) step = -pid->max_step;
    
    // 更新当前虚拟位置
    pid->current += step;
    
    return pid->current;
}
