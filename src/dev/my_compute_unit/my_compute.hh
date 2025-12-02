#ifndef DEV_MY_COMPUTE_HH
#define DEV_MY_COMPUTE_HH

#include "dev/io_device.hh"
#include "params/MyCompute.hh"
#include "sim/eventq.hh"

namespace gem5
{

class MyCompute : public BasicPioDevice
{
  public:
    using Params = MyComputeParams;
    MyCompute(const Params &p);

    // PIO-style access: devices implement read/write
    Tick read(PacketPtr pkt) override;
    Tick write(PacketPtr pkt) override;

    void completeOperation();

  private:
    // use pioAddr/pioSize/pioDelay from BasicPioDevice
    // Compute unit registers (all 8-bit)
    uint8_t op_a{0};      // offset 0
    uint8_t op_b{0};      // offset 1
    uint8_t config{0};    // offset 2 (bit0: 0=add, 1=sub)
    uint8_t result{0};    // offset 3
    uint8_t status{0};    // offset 4 (bit0 = done)

    // Event to model compute delay
    MemberEventWrapper<&MyCompute::completeOperation> computeEvent;
};

} // namespace gem5

#endif
