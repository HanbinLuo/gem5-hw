#include "riscv.h"
#include "riscv-virt.h"
#include "printf/printf.h"
#include <stdint.h>

// CU0 Base Address (0x10100000)
#define CU0_BASE    0x10100000UL
// CU1 Base Address (0x10100000 + 0x100000)
#define CU1_BASE    0x10200000UL

/* Register Map (from compute_unit.hh/cc)
 * 0x00: cu_id_reg
 * ...
 * 0x14: dma_src_addr (32-bit)
 * 0x18: dma_dst_addr (32-bit)
 * 0x1C: dma_size (32-bit)
 * 0x20: dma_trigger (8-bit, bit0=start)
 * ...
 * 0x1000: input_region start
 * 0x3000: output_region start
 */

#define CU_DMA_SRC_ADDR_OFFSET 0x14
#define CU_DMA_DST_ADDR_OFFSET 0x18
#define CU_DMA_SIZE_OFFSET     0x1C
#define CU_DMA_TRIGGER_OFFSET  0x20

#define CU_INPUT_OFFSET        0x1000

// 32-bit MMIO access
static inline void mmio_write32(uintptr_t addr, uint32_t v) {
    volatile uint32_t *p = (volatile uint32_t *)addr;
    *p = v;
}

static inline uint32_t mmio_read32(uintptr_t addr) {
    volatile uint32_t *p = (volatile uint32_t *)addr;
    return *p;
}

// 8-bit MMIO access
static inline void mmio_write8(uintptr_t addr, uint8_t v) {
    volatile uint8_t *p = (volatile uint8_t *)addr;
    *p = v;
}

static inline uint8_t mmio_read8(uintptr_t addr) {
    volatile uint8_t *p = (volatile uint8_t *)addr;
    return *p;
}

static inline unsigned long long rdcycle64(void) {
    unsigned long long v;
    __asm__ volatile("rdcycle %0" : "=r"(v));
    return v;
}

int main( void )
{
    plt_virt_init();

    printf("CU DMA Interconnect Test\n");

    // 1. Initialize data in CU0 Input Region
    uintptr_t cu0_input = CU0_BASE + CU_INPUT_OFFSET;
    uintptr_t cu1_input = CU1_BASE + CU_INPUT_OFFSET;
    
    printf("Write pattern to CU0 Input at 0x%08x\n", cu0_input);
    mmio_write8(cu0_input + 0, 0xAA);
    mmio_write8(cu0_input + 1, 0xBB);
    mmio_write8(cu0_input + 2, 0xCC);
    mmio_write8(cu0_input + 3, 0xDD);

    // Verify write
    printf("CU0 Input[0] = 0x%x (Expected 0xAA)\n", mmio_read8(cu0_input));

    // 2. Configure CU0 DMA to copy from CU0 Input -> CU1 Input
    // Note: DMA addresses are physical system addresses
    uint32_t src_phys = (uint32_t)cu0_input;
    uint32_t dst_phys = (uint32_t)cu1_input;
    uint32_t size = 4;

    printf("Configuring DMA on CU0:\n");
    printf("  SRC: 0x%08lx\n", src_phys);
    printf("  DST: 0x%08lx\n", dst_phys);
    printf("  SIZE: %ld\n", size);

    mmio_write32(CU0_BASE + CU_DMA_SRC_ADDR_OFFSET, src_phys);
    mmio_write32(CU0_BASE + CU_DMA_DST_ADDR_OFFSET, dst_phys);
    mmio_write32(CU0_BASE + CU_DMA_SIZE_OFFSET, size);

    // 3. Trigger DMA
    printf("Triggering DMA...\n");
    unsigned long long t0 = rdcycle64();
    mmio_write8(CU0_BASE + CU_DMA_TRIGGER_OFFSET, 1);
    
    // Simple polling/delay - wait for completion
    // Since we don't have a dedicated DMA status bit yet, we assume it completes quickly in simulation
    // or we poll the destination for the expected value
    volatile int timeout = 10000;
    while((mmio_read8(cu1_input) != 0xAA) && timeout-- > 0);
    unsigned long long t1 = rdcycle64();

    // 4. Verify Data in CU1
    printf("Verifying CU1 Input at 0x%08x\n", cu1_input);
    uint8_t d0 = mmio_read8(cu1_input + 0);
    uint8_t d1 = mmio_read8(cu1_input + 1);
    uint8_t d2 = mmio_read8(cu1_input + 2);
    uint8_t d3 = mmio_read8(cu1_input + 3);

    printf("CU1 Data: 0x%x 0x%x 0x%x 0x%x\n", d0, d1, d2, d3);

    if (d0 == 0xAA && d1 == 0xBB && d2 == 0xCC && d3 == 0xDD) {
        printf("PASS: DMA Copy Successful! Cycles: %d\n", (int)(t1 - t0));
    } else {
        printf("FAIL: DMA Copy Data Mismatch\n");
    }

    return 0;
}
