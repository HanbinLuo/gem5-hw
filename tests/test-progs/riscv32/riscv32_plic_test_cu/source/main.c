#include "riscv.h"
#include "riscv32.h"
#include "riscv-virt.h"
#include "printf/printf.h"
#include "system_map.h"

// 读 RISC-V mcycle（裸机运行在 machine/privileged 模式），用于测周期数
static inline unsigned long long rdcycle64(void) {
unsigned long long v;
__asm__ volatile("rdcycle %0" : "=r"(v));
return v;
}

void vCuHwStartJob(uint32_t cu_id, uint32_t job_id,uint32_t size,uint32_t latency, uint8_t config)
{
    // Job id
    *((volatile uint32_t *)CU_JOB_ID) = job_id;
    // 计算大小
    *((volatile uint32_t *)CU_SIZE) = size;
    // 计算延迟（ticks）
    *((volatile uint32_t *)CU_DELAY) = latency;

    // 写入 config（触发计算），这个可以认为功能字
    mmio_write8(CU_CONFIG, config);

}

int main(void)
{
plt_virt_init();

    printf("Starting Interrupt-Driven Compute Test...\n");

    /*验证一下输入输出的0x4000范围部分地址*/
    printf("Testing CU input/output MMIO regions...\n");
    int failures = 0;
    for (int i = 0; i < 16; ++i) {
        uintptr_t addr = CU_INPUT_BASE + i;
        mmio_write8(addr, (uint8_t)(0xA0 + i));
    }
    for (int i = 0; i < 16; ++i) {
        uintptr_t addr = CU_INPUT_BASE + i;
        uint8_t v = mmio_read8(addr);
        if (v != (uint8_t)(0xA0 + i)) {
            printf("INPUT mismatch at %d: got 0x%02x expected 0x%02x\n", i, v, (uint8_t)(0xA0 + i));
            failures++;
        }
    }

    for (int i = 0; i < 16; ++i) {
        uintptr_t addr = CU_OUTPUT_BASE + i;
        mmio_write8(addr, (uint8_t)(0xB0 + i));
    }
    for (int i = 0; i < 16; ++i) {
        uintptr_t addr = CU_OUTPUT_BASE + i;
        uint8_t v = mmio_read8(addr);
        if (v != (uint8_t)(0xB0 + i)) {
            printf("OUTPUT mismatch at %d: got 0x%02x expected 0x%02x\n", i, v, (uint8_t)(0xB0 + i));
            failures++;
        }
    }
    if (failures == 0)
        printf("MMIO input/output self-test passed\n");
    else
        printf("MMIO input/output self-test failed: %d errors\n", failures);

    unsigned long long t0 = rdcycle64();
    // cu_id=0, job_id=42, size=2048, latency=12345*500 ticks, config=0
    vCuHwStartJob(0, 42, 2048, 12345*500, 0);

    // 等待计算完成（STATUS bit0 = done）
    while ((*(volatile uint8_t *)CU_STATUS & 0x1) == 0) {
        // __asm__ volatile("wfi");
    }
    unsigned long long t1 = rdcycle64();

    unsigned long long cycles = t1 - t0;

printf("Total cycles: %d \n", (int)cycles);

return 0;
}
