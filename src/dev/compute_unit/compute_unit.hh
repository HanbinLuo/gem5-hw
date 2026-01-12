#ifndef DEV_MY_COMPUTE_HH
#define DEV_MY_COMPUTE_HH

#include <vector>

#include "dev/io_device.hh"
#include "dev/riscv/plic_device.hh"
#include "mem/port.hh"
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

    Port &getPort(const std::string &if_name,
                  PortID idx=InvalidPortID) override;

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

    // Memory-mapped input/output regions (each 0x2000 bytes)
    // Input region mapped at PIO offsets 0x1000 .. 0x2FFF
    // Output region mapped at PIO offsets 0x3000 .. 0x4FFF
    static constexpr Addr input_base = 0x1000;
    static constexpr Addr output_base = 0x3000;
    static constexpr size_t region_size = 0x2000;

    std::vector<uint8_t> input_region;
    std::vector<uint8_t> output_region;

    // computeDelay param removed: computation delay is read from `compute_delay_reg` at runtime
    // (keep member for compatibility with existing constructors but set to 0)
    const Tick computeDelay{0};

    // Event to model compute delay
    MemberEventWrapper<&ComputeUnit::completeOperation> computeEvent;

    class DmaPort : public RequestPort
    {
      private:
        ComputeUnit *owner;
      public:
        DmaPort(const std::string& name, ComputeUnit *owner) :
            RequestPort(name), owner(owner)
        {}
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
    };

    DmaPort dmaPort;

    // DMA registers
    // 0x14: src
    // 0x18: dst
    // 0x1C: size
    // 0x20: trigger (bit 0 = start)
    uint32_t dma_src_addr{0};
    uint32_t dma_dst_addr{0};
    uint32_t dma_size{0};

    RequestorID requestorId;

    enum DmaState { DMA_IDLE, DMA_READING, DMA_WRITING };
    DmaState dmaState{DMA_IDLE};
    std::vector<uint8_t> dmaBuffer;

    void startDma();
};

} // namespace gem5

#endif
