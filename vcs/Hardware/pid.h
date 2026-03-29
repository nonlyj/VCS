#ifndef __PID_H
#define __PID_H

/* PID 控制器定义 */
typedef struct {
    float Kp;           // 比例系数：决定门打开的速度响应
    float Kd;           // 微分系数：提供阻尼，防止门到位时来回甩动（过冲）
    float target;       // 目标角度
    float current;      // 当前正在输出的角度
    float last_error;   // 上次误差
    float max_step;     // 最大步进限制（限速）
} PID_Controller;


float PID_Compute_Step(PID_Controller *pid);



#endif
