#ifndef DEV_MY_COMPUTE_HH
#define DEV_MY_COMPUTE_HH

#include "dev/io_device.hh"
#include "dev/riscv/plic_device.hh"
#include "params/ComputeUnit.hh"
#include "sim/eventq.hh"

namespace gem5
{

class ComputeUnit : public PlicIntDevice
{
  protected:
    int _computeUnit;

  public:
    using Params = ComputeUnitParams;
    ComputeUnit(const Params &p);

    // PIO-style access: devices implement read/write
    Tick read(PacketPtr pkt) override;
    Tick write(PacketPtr pkt) override;

    void completeOperation();

  private:
    // use pioAddr/pioSize/pioDelay from BasicPioDevice
    // New register layout (all offsets are device PIO offsets, little-endian):
    // 0x00-0x03 : cu_id_reg (32-bit) - configurable from Python
    // 0x04-0x07 : job_id_reg (32-bit)
    // 0x08-0x0B : compute_size_reg (32-bit) -- number of items to compute
    // 0x0C-0x0F : compute_delay_reg (32-bit ticks) -- overrides device param when non-zero
    // 0x10      : config (8-bit, bit0: 0=add, 1=sub)
    // 0x11      : status (8-bit, bit0 = done)
    // 0x12      : busy   (8-bit, bit0: 0=idle, 1=computing)

    uint32_t cu_id_reg{0};
    uint32_t job_id_reg{0};
    uint32_t compute_size_reg{0};
    uint32_t compute_delay_reg{0};
    uint8_t config{0};    // offset 0x10
    uint8_t status{0};    // offset 0x11
    uint8_t busy{0};      // offset 0x12

    // computeDelay param removed: computation delay is read from `compute_delay_reg` at runtime
    // (keep member for compatibility with existing constructors but set to 0)
    const Tick computeDelay{0};

    // Event to model compute delay
    MemberEventWrapper<&ComputeUnit::completeOperation> computeEvent;
};

} // namespace gem5

#endif
