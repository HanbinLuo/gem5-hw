// simple_dma.cc
#include "dev/simple_dma/simple_dma.hh"

#include "base/trace.hh"
#include "debug/SimpleDMA.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"

namespace gem5 {

SimpleDMA::SimpleDMA(const Params &p)
  : DmaDevice(p),
    pioAddr(p.pio_addr),
    pioSize(p.pio_size),
    pioDelay(p.pio_latency),
    readDoneEvent(this),
    writeDoneEvent(this)
{
    DPRINTF(SimpleDMA,
            "SimpleDMA created at pio_addr=%#lx size=%#lx latency=%llu\n",
            pioAddr, pioSize, (unsigned long long)pioDelay);
}

//  PIO 设备接口[pioAddr, pioAddr+pioSize)
AddrRangeList
SimpleDMA::getAddrRanges() const
{
    return AddrRangeList{ RangeSize(pioAddr, pioSize) };
}

// PIO 读寄存器
Tick
SimpleDMA::read(PacketPtr pkt)
{
    Addr offset = pkt->getAddr() - pioAddr;

    uint32_t val = 0;
    switch (offset) {
      case 0x00: val = static_cast<uint32_t>(srcAddr); break;
      case 0x04: val = static_cast<uint32_t>(dstAddr); break;
      case 0x08: val = length; break;
      case 0x0C:
        val = busy ? 1 : 0;
        break;
      case 0x10:
        val = done ? 1 : 0;
        break;
      default:
        panic("SimpleDMA: invalid read offset %#lx\n", offset);
    }

    pkt->setLE<uint32_t>(val);
    pkt->makeResponse();
    return pioDelay;
}

// PIO 写寄存器
Tick
SimpleDMA::write(PacketPtr pkt)
{
    Addr offset = pkt->getAddr() - pioAddr;
    uint32_t val = pkt->getLE<uint32_t>();

    switch (offset) {
      case 0x00:
        srcAddr = static_cast<Addr>(val);
        break;
      case 0x04:
        dstAddr = static_cast<Addr>(val);
        break;
      case 0x08:
        length = val;
        break;
      case 0x0C:
        if (val & 0x1) {
            startCopy();
        }
        break;
      case 0x10:
        if (val & 0x1) {
            done = false;
        }
        break;
      default:
        panic("SimpleDMA: invalid write offset %#lx\n", offset);
    }

    pkt->makeResponse();
    return pioDelay;
}

// 启动 DMA 传输
void
SimpleDMA::startCopy()
{
    if (busy || length == 0) {
        return;
    }

    busy = true;
    done = false;

    buffer.resize(length);

    DPRINTF(SimpleDMA, "startCopy: src=%#lx dst=%#lx len=%u\n",
            srcAddr, dstAddr, length);

    dmaRead(srcAddr, length, &readDoneEvent, buffer.data());
}

// 读完成回调
void
SimpleDMA::onReadDone()
{
    DPRINTF(SimpleDMA, "onReadDone: issuing dmaWrite\n");
    dmaWrite(dstAddr, length, &writeDoneEvent, buffer.data());
}

// 写完成回调
void
SimpleDMA::onWriteDone()
{
    DPRINTF(SimpleDMA, "onWriteDone: copy finished\n");
    busy = false;
    done = true;
    // TODO: 预留拉高中断位置
}

} // namespace gem5
