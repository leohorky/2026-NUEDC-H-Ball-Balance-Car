#ifndef PID_PID_H_
#define PID_PID_H_

#include <stdint.h>


typedef struct {
    int32_t A;           // 合并后的系数 A (已放大 256 倍)
    int32_t B;           // 合并后的系数 B (已放大 256 倍)
    int32_t C;           // 合并后的系数 C (已放大 256 倍)

    int16_t target;      // 目标速度
    int16_t err_last;    // e(k-1)
    int16_t err_prev;    // e(k-2)

    int16_t out;         // 当前输出 PWM
    int16_t out_max;     // PWM 输出限幅
} FastPID_t;

extern FastPID_t Position;

void FastPID_Init(void);
void FastPID_Reset(void);
void FastPID_SetParam(FastPID_t *pid, float kp, float ki, float kd);
void FastPID_Calc(FastPID_t *pid, int16_t actual);
void FastPID_SetTarget(FastPID_t *pid,int16_t target);


#endif /* PID_PID_H_ */
