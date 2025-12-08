// simple_dma.hh
#ifndef __DEV_SIMPLE_DMA_HH__
#define __DEV_SIMPLE_DMA_HH__

#include "dev/dma_device.hh"
#include "dev/platform.hh"
#include "params/SimpleDMA.hh"

namespace gem5 {

class SimpleDMA : public DmaDevice
{
  public:
    using Params = SimpleDMAParams;
    SimpleDMA(const Params &p);

    Tick read(PacketPtr pkt) override;
    Tick write(PacketPtr pkt) override;

  protected:
    // PioDevice 要求纯虚函数
    AddrRangeList getAddrRanges() const override;

  private:
    // pio 信息
    Addr pioAddr;
    Addr pioSize;
    Tick pioDelay;
    Platform *platform;
    int interruptId;

    Addr    srcAddr = 0;
    Addr    dstAddr = 0;
    uint32_t length = 0;
    bool    busy    = false;
    bool    done    = false;

    std::vector<uint8_t> buffer;

    // 读完成回调事件
    class ReadDoneEvent : public Event
    {
      public:
        SimpleDMA *dev;
        ReadDoneEvent(SimpleDMA *d)
          : Event(Default_Pri), dev(d) {}
        void process() override { dev->onReadDone(); }
        const char *description() const override {
            return "SimpleDMA read done";
        }
    } readDoneEvent;

    // 写完成回调事件
    class WriteDoneEvent : public Event
    {
      public:
        SimpleDMA *dev;
        WriteDoneEvent(SimpleDMA *d)
          : Event(Default_Pri), dev(d) {}
        void process() override { dev->onWriteDone(); }
        const char *description() const override {
            return "SimpleDMA write done";
        }
    } writeDoneEvent;

    void startCopy();

    void onReadDone();
    void onWriteDone();
};

} // namespace gem5

#endif
