#ifndef DEV_DAG_DEBUG_HH
#define DEV_DAG_DEBUG_HH

#include "dev/io_device.hh"
#include "params/DagDebug.hh"

namespace gem5
{

class DagDebug : public BasicPioDevice
{
  public:
    using Params = DagDebugParams;
    DagDebug(const Params &p);

    // PIO-style access: devices implement read/write
    Tick read(PacketPtr pkt) override;
    Tick write(PacketPtr pkt) override;

  private:
    // Register layout (all offsets are device PIO offsets, little-endian):
    // 0x00-0x03 : cu_id_reg (32-bit) - CU identifier
    // 0x04-0x07 : job_id_reg (32-bit) - Job identifier
    // 0x08      : config (8-bit) - write triggers debug print

    uint32_t cu_id_reg{0};   // offset 0x00
    uint32_t job_id_reg{0};  // offset 0x04
    uint8_t config{0};       // offset 0x08

    // Clock frequency for tick to cycle conversion
    const Tick clockPeriod;

    // Helper function to print debug info
    void printDebugInfo();
};

} // namespace gem5

#endif
