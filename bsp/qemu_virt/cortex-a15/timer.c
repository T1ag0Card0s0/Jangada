#include <jangada/platform.h>
#include <stdint.h>

static inline uint32_t read_cntfrq(void)
{
    uint32_t v;
    asm volatile("mrc p15, 0, %0, c14, c0, 0" : "=r"(v));
    return v;
}

static inline void write_cntp_tval(uint32_t ticks)
{
    asm volatile("mcr p15, 0, %0, c14, c2, 0" ::"r"(ticks));
}

static inline void write_cntp_ctl(uint32_t val)
{
    asm volatile("mcr p15, 0, %0, c14, c2, 1" ::"r"(val));
    asm volatile("isb");
}

uint32_t platform_timer_get_freq(void)
{
    return read_cntfrq();
}

void platform_timer_init(uint32_t ticks)
{
    write_cntp_tval(ticks);
    write_cntp_ctl(1);
}

void platform_timer_reload(uint32_t ticks)
{
    write_cntp_tval(ticks);
    write_cntp_ctl(1);
}
