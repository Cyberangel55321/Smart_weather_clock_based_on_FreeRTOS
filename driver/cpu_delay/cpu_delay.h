#ifndef __CPU_DELAY_H__
#define __CPU_DELAY_H__

#include <stdint.h>

#define cpu_delay_us(x) cpu_delay(x)
#define cpu_delay_ms(x) cpu_delay(x * 1000)

void cpu_delay(uint32_t us);

#endif /* __CPU_DELAY_H__ */
