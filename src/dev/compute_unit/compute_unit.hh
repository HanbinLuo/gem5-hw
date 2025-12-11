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
    // Compute unit registers
    uint8_t op_a[16];     // offset 0x00 - 0x0F
    uint8_t op_b[16];     // offset 0x10 - 0x1F
    uint8_t result[16];   // offset 0x20 - 0x2F
    uint8_t length{1};    // offset 0x30
    uint8_t config{0};    // offset 0x31 (bit0: 0=add, 1=sub)
    uint8_t status{0};    // offset 0x32 (bit0 = done)

    // Latency for the computation operation
    const Tick computeDelay;

    // Event to model compute delay
    MemberEventWrapper<&ComputeUnit::completeOperation> computeEvent;
};

} // namespace gem5

#endif
