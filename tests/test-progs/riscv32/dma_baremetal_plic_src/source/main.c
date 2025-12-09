#include "printf/printf.h"
#include "riscv-virt.h"
#include "riscv.h"
#include <stdint.h>

#define DMA_BASE 0x50000000u
#define PLIC_BASE 0x0C000000u
#define PLIC_IRQ 11u
#define PLIC_CONTEXT 0u

typedef struct {
  volatile uint32_t SRC;    // 0x00
  volatile uint32_t DST;    // 0x04
  volatile uint32_t LEN;    // 0x08
  volatile uint32_t CTRL;   // 0x0C
  volatile uint32_t STATUS; // 0x10
} simple_dma_regs_t;

#define DMA_CTRL_START (1u << 0)
#define DMA_STATUS_DONE (1u << 0)

static simple_dma_regs_t *const dma = (simple_dma_regs_t *)DMA_BASE;

/* Buffers in RAM */
#define SRC_BUF 0x80001000u
#define DST_BUF 0x80002000u

/* Scratch location where IRQ handler writes claimed IRQ id */
#define IRQ_FLAG_ADDR ((volatile uint32_t *)0x80003000u)

/* Helper to write memory-mapped 32-bit */
static inline void mmio_write32(uint32_t addr, uint32_t val) {
  *(volatile uint32_t *)addr = val;
}

static inline uint32_t mmio_read32(uint32_t addr) {
  return *(volatile uint32_t *)addr;
}

/* Simple DMA wrapper (polling) */
static void dma_memcpy(void *dst, const void *src, uint32_t len) {
  /* Clear DONE flag (write 1 to clear in this simple device) */
  dma->STATUS = DMA_STATUS_DONE;

  dma->SRC = (uint32_t)src;
  dma->DST = (uint32_t)dst;
  dma->LEN = len;
  dma->CTRL = DMA_CTRL_START;

  /* Poll for completion */
  while ((dma->STATUS & DMA_STATUS_DONE) == 0) {
    /* busy wait */
  }
}

/* C-level IRQ handler called from assembly entry.
 * It must:
 *  - read claim register (returns IRQ id)
 *  - optionally handle it
 *  - write same id back to claim register to complete
 * We'll write the claim ID to RAM at IRQ_FLAG_ADDR so main can observe it.
 */
void irq_handler_c(void) {
  /* Read mcause to determine trap/interrupt source. Only access the PLIC
   * claim/complete registers when this is a machine external interrupt.
   * This avoids reading the PLIC claim register on timer or other traps,
   * which produces repeated "Claim already cleared" messages in the log.
   */
  uint32_t mcause;
  __asm__ volatile("csrr %0, mcause" : "=r"(mcause));

  const uint32_t INTERRUPT_BIT = (1u << 31);
  const uint32_t CAUSE_MASK = 0xffu;
  uint32_t is_irq = (mcause & INTERRUPT_BIT) != 0;
  uint32_t cause = (mcause & CAUSE_MASK);

  /* RISC-V: machine external interrupt code is 11, machine timer is 7. */
  if (is_irq && cause == 11u) {
    uint32_t claim_addr = PLIC_BASE + 0x0200004 + (PLIC_CONTEXT * 0x1000);
    uint32_t claim = mmio_read32(claim_addr); /* read claim (claim ID) */

    /* record the claimed ID so main can observe it */
    *IRQ_FLAG_ADDR = claim;

    /* write back to complete the interrupt (use the claim value returned) */
    mmio_write32(claim_addr, claim);
  } else {
    /* Non-external interrupt (e.g. machine timer). Don't touch PLIC.
     * If you want to observe other causes for debugging, you can write
     * a code to IRQ_FLAG_ADDR (e.g. 0x80000000 | cause).
     */
  }
}

/* assembly-level IRQ entry (saves registers, calls C handler, returns via mret)
 */
__attribute__((naked)) void _irq_entry(void) {
  __asm__ volatile(
      /* save caller-saved / some regs (keep it minimal) */
      "addi sp, sp, -16\n"
      "sw ra, 12(sp)\n"
      "sw t0, 8(sp)\n"
      "sw t1, 4(sp)\n"
      "sw t2, 0(sp)\n"
      /* call C handler */
      "call irq_handler_c\n"
      /* restore */
      "lw t2, 0(sp)\n"
      "lw t1, 4(sp)\n"
      "lw t0, 8(sp)\n"
      "lw ra, 12(sp)\n"
      "addi sp, sp, 16\n"
      "mret\n");
}

int main(void) {
  uint8_t *src = (uint8_t *)SRC_BUF;
  uint8_t *dst = (uint8_t *)DST_BUF;
  const uint32_t len = 256;

  /* init buffers */
  for (uint32_t i = 0; i < len; ++i) {
    src[i] = (uint8_t)(i & 0xFF);
    dst[i] = 0;
  }

  /* ---------------------------
   * Configure PLIC for IRQ PLIC_IRQ
   * ---------------------------
   * Priority register: PLIC_BASE + 4 * id
   * Enable register (context 0): PLIC_BASE + 0x2000 + context*0x80 + (id>>5)*4
   * Threshold (context 0): PLIC_BASE + 0x0200000 + context*0x1000
   * Claim/complete (context 0): PLIC_BASE + 0x0200004 + context*0x1000
   */

  /* set priority > 0 */
  mmio_write32(PLIC_BASE + 4 * PLIC_IRQ, 1u);

  /* enable bit for this source in context 0 */
  uint32_t enable_addr =
      PLIC_BASE + 0x2000 + (PLIC_CONTEXT * 0x80) + ((PLIC_IRQ >> 5) * 4);
  uint32_t val = mmio_read32(enable_addr);
  val |= (1u << (PLIC_IRQ & 0x1F));
  mmio_write32(enable_addr, val);

  /* threshold -> 0 (accept any priority > 0) */
  mmio_write32(PLIC_BASE + 0x0200000 + (PLIC_CONTEXT * 0x1000), 0u);

  /* clear any previous claim flag in RAM */
  *IRQ_FLAG_ADDR = 0;

  /* set mtvec to our irq entry */
  uintptr_t vec = (uintptr_t)&_irq_entry;
  /* write mtvec */
  __asm__ volatile("mv t0, %0\n\tcsrw mtvec, t0" ::"r"(vec));

  /* enable machine external interrupts in mie, and global interrupts in mstatus
   */
  const uint32_t MEIE_CSR = (1u << 11);
  const uint32_t MSTATUS_MIE_CSR = (1u << 3);

  __asm__ volatile("mv t0, %0\n\tcsrw mie, t0" ::"r"(MEIE_CSR));
  __asm__ volatile("mv t0, %0\n\tcsrw mstatus, t0" ::"r"(MSTATUS_MIE_CSR));

  /* At this point, external interrupts can be delivered to M-mode */

  /* perform DMA copy */
  dma_memcpy((void *)DST_BUF, (const void *)SRC_BUF, len);

  // printf("waiting irq\n");
  /* Wait for IRQ flag to be written by handler (with a timeout) */
  // uint32_t timeout = 100000u;
  // while ((*IRQ_FLAG_ADDR == 0) && (timeout-- > 0))
  while ((*IRQ_FLAG_ADDR) == 0) {
    // mmio_write32(PLIC_BASE + 0x0200000 + (PLIC_CONTEXT * 0x1000), 1u);
    /* busy wait for interrupt to happen (SimplePlicSource posts at tick
     * ~100000) */
  }
  uint32_t timeout = 100000u;
  while (timeout-- > 0) {
  }

  const uint32_t MEIE_CSR_NEW = (0u << 11);
  const uint32_t MSTATUS_MIE_CSR_NEW = (0u << 3);

  __asm__ volatile("mv t0, %0\n\tcsrw mie, t0" ::"r"(MEIE_CSR_NEW));
  __asm__ volatile("mv t0, %0\n\tcsrw mstatus, t0" ::"r"(MSTATUS_MIE_CSR_NEW));

  /* check results: DMA correctness and whether IRQ observed */
  int ok = 1;
  for (uint32_t i = 0; i < len; ++i) {
    if (dst[i] != src[i]) {
      ok = 0;
      break;
    }
  }

  uint32_t irq_seen = *IRQ_FLAG_ADDR;

  /* Spin to indicate final state */
  if (ok && (irq_seen == PLIC_IRQ)) {
    /* success: DMA ok and IRQ handled */
    uint32_t timeout = 100000u;
    while (timeout-- > 0) {
    }
    return 0;
    while (1) {
      __asm__ volatile("nop");
    }
  } else {
    /* failure path */
    // printf("failure\n");
    return 1;
    while (1) {
      __asm__ volatile("nop");
    }
  }

  return 0;
}
