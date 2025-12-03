#include "riscv.h"
#include "riscv-virt.h"
#include "printf/printf.h"
#include <stdint.h>

#define CU_BASE    0x10009000UL
#define CU_OP_A    (CU_BASE + 0)
#define CU_OP_B    (CU_BASE + 1)
#define CU_CONFIG  (CU_BASE + 2)
#define CU_RESULT  (CU_BASE + 3)
#define CU_STATUS  (CU_BASE + 4)

// 简单的 MMIO 读写（8-bit）
static inline void mmio_write8(uintptr_t addr, uint8_t v) {
    volatile uint8_t *p = (volatile uint8_t *)addr;
    *p = v;
}

static inline uint8_t mmio_read8(uintptr_t addr) {
    volatile uint8_t *p = (volatile uint8_t *)addr;
    return *p;
}

// 读 RISC-V mcycle（裸机运行在 machine/privileged 模式），用于测周期数
static inline unsigned long long rdcycle64(void) {
    unsigned long long v;
    // 如果目标支持 rdcycle:
    __asm__ volatile("rdcycle %0" : "=r"(v));
    return v;
}

int main( void )
{
	plt_virt_init();

    printf("自定义计算单元测试\n");
        uint8_t a = 10, b = 3;
    uint8_t cfg = 1; // 0 = add, 1 = sub

    // 清除 done（可选）
    mmio_write8(CU_STATUS, 0);

    // 写入操作数
    mmio_write8(CU_OP_A, a);
    mmio_write8(CU_OP_B, b);

    // 触发计算（写 config）
    unsigned long long t0 = rdcycle64();
    mmio_write8(CU_CONFIG, cfg);

    printf("waiting for computation to complete\n");
    // 轮询 status bit0 == 1
    while ((mmio_read8(CU_STATUS) & 0x1) == 0) {
         //printf(".");
    }
    //printf("/n");
    unsigned long long t1 = rdcycle64();

    uint8_t res = mmio_read8(CU_RESULT);
    unsigned long long cycles = t1 - t0;

        printf("%d - %d= %u\n", a,b,res);
        printf("Compute cycles: %d \n", (int)cycles);
//为什么atomic cpu是1倍pioDelay
//不考虑寄存器读写延迟
//为什么minor cpu是2倍pioDelay
//写操作耗时：1 倍 pioDelay（计算任务同时开始延时）。
//读操作耗时：1 倍 pioDelay
//总耗时：理论上约 2 倍 pioDelay

    printf("测量单次写操作耗时\n");
    t0 = rdcycle64();
    mmio_write8(CU_OP_A, a);
    t1 = rdcycle64();
    printf("Write cycles: %d \n", (int)(t1 - t0));

    printf("测量单次读操作耗时\n");
    t0 = rdcycle64();
    mmio_read8(CU_RESULT);
    t1 = rdcycle64();
    printf("read cycles: %d \n", (int)(t1 - t0));

        return 0;
}
