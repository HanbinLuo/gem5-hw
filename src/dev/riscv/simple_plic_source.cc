#include "dev/riscv/simple_plic_source.hh"

#include "base/trace.hh"
#include "debug/Plic.hh"

namespace gem5
{

SimplePlicSource::SimplePlicSource(const Params &params)
    : PlicIntDevice(params),
      platform_ptr(params.platform),
      trigger_event([this]{ doTrigger(); }, name() + ".trigger")
{
}

void
SimplePlicSource::init()
{
    PlicIntDevice::init();

    // Schedule a one-shot trigger a short time after simulation start
    Tick when = curTick() + cyclesToTicks(Cycles(1e6));
    DPRINTF(Plic, "SimplePlicSource scheduling trigger at %llu\n", when);
    schedule(trigger_event, when);
}

void
SimplePlicSource::doTrigger()
{
    if (platform_ptr) {
        DPRINTF(Plic, "SimplePlicSource posting interrupt id %d\n", id());
        platform_ptr->postPciInt(id());
    } else {
        DPRINTF(Plic, "SimplePlicSource: no platform_ptr, cannot post\n");
    }
}

Tick
SimplePlicSource::read(PacketPtr pkt)
{
    // Respond to PIO reads with an empty response
    pkt->makeResponse();
    return pioDelay;
}

Tick
SimplePlicSource::write(PacketPtr pkt)
{
    // Respond to PIO writes; no side-effects needed for this test device
    pkt->makeResponse();
    return pioDelay;
}

} // namespace gem5
