#include "pid.h"
#include "pid.h"

FastPID_t Position;
//================= PID 参数设置 =================

// 设置 PID 参数 (此函数只在修改参数时调用，不占用实时循环资源)
void FastPID_SetParam(FastPID_t *pid, float kp, float ki, float kd)
{
    // 提前计算好 A, B, C 并放大10倍，转为纯整数
    pid->A = (int32_t)((kp + ki + kd) * 1000);
    pid->B = (int32_t)(-(kp + 2 * kd) * 1000);
    pid->C = (int32_t)(kd * 1000);

}

void FastPID_Init(void)
{
    // 初始化左右轮参数 (假设你原本希望的 Kp=12.0, Ki=1.5, Kd=0.2)
    FastPID_SetParam(&Position, 12.0f, 1.5f, 0.2f);
    Position.out_max = 110;
    Position.err_last = 0; Position.err_prev = 0; Position.out = 0;

}

void FastPID_Reset(void){
	Position.err_last = 0; Position.err_prev = 0; Position.out = 0;
}

// ================= 核心闭环运算 =================
// 每次 10ms 中断只需调用此函数，速度极快！
void FastPID_Calc(FastPID_t *pid, int16_t actual)
{
    int32_t err = actual - pid->target ;

    // 3次乘法，2次加法，1次位移。这就是整个 PID 所有的计算量。
    int32_t delta_out = (pid->A * err + pid->B * pid->err_last + pid->C * pid->err_prev) /1000;

    // 滚动更新误差历史
    pid->err_prev = pid->err_last;
    pid->err_last = err;

    // 累加计算当前输出
    int32_t current_out = pid->out + delta_out;

    // 输出限幅 (采用无分支思维，直接钳位)
    if (current_out > pid->out_max) current_out = pid->out_max;
    else if (current_out < -pid->out_max) current_out = -pid->out_max;

    pid->out = (int16_t)current_out;
}

void FastPID_SetTarget(FastPID_t *pid,int16_t target){
	pid->target = target;

}
