#ifndef __DEV_RISCV_SIMPLE_PLIC_SOURCE_HH__
#define __DEV_RISCV_SIMPLE_PLIC_SOURCE_HH__

#include "dev/riscv/plic_device.hh"
#include "params/SimplePlicSource.hh"
#include "dev/platform.hh"
#include "sim/eventq.hh"

namespace gem5
{

class SimplePlicSource : public PlicIntDevice
{
  public:
    typedef SimplePlicSourceParams Params;

    SimplePlicSource(const Params &params);

    void init() override;

    // Minimal PIO handlers to satisfy PioDevice interface.
    Tick read(PacketPtr pkt) override;
    Tick write(PacketPtr pkt) override;

  private:
    void doTrigger();

    Platform *platform_ptr{nullptr};
    EventFunctionWrapper trigger_event;
};

} // namespace gem5

#endif // __DEV_RISCV_SIMPLE_PLIC_SOURCE_HH__
