#include "riscv.h"
#include "riscv-virt.h"
#include "types.h"
#include "plic.h"

#define PLIC_PRIORITY           (PLIC0_ADDR + 0x0)
#define PLIC_PENDING            (PLIC0_ADDR + 0x1000)
#define PLIC_MENABLE(hart)      (PLIC0_ADDR + 0x2000 + (hart)*0x100)
#define PLIC_MTHRESHOLD(hart)   (PLIC0_ADDR + 0x200000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart)       (PLIC0_ADDR + 0x200004 + (hart)*0x2000)

#define PLIC_REG(reg) (*((volatile u32 *)(reg)))

void plic_init(void)
{   // set priority
  PLIC_REG(PLIC_PRIORITY + UART0_IRQ*4) = 1;
  /* 为 MyCompute 设备添加优先级（中断号 0xB / 11），设备在完成时会触发此中断 */
  PLIC_REG(PLIC_PRIORITY + 0xB * 4) = 1;
    // set threshold
    PLIC_REG(PLIC_MTHRESHOLD(0)) = 0;
    // enable
  PLIC_REG(PLIC_MENABLE(0)) = 1 << UART0_IRQ;
  /* 使能 MyCompute 的中断位 */
  PLIC_REG(PLIC_MENABLE(0)) |= (1 << 0xB);
}

// ask the PLIC what interrupt we should serve.
int plic_claim(void)
{
  int irq = PLIC_REG(PLIC_MCLAIM(0));
  return irq;
}

// tell the PLIC we've served this IRQ.
void plic_complete(int irq)
{
  PLIC_REG(PLIC_MCLAIM(0)) = irq;
}
