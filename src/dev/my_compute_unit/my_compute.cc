#include <cstring>
#include <iostream>

#include "base/logging.hh"
#include "base/trace.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"
#include "my_compute_unit/debug/MyCompute.hh"
#include "my_compute_unit/my_compute.hh"

namespace gem5 {

MyCompute::MyCompute(const Params &p)
    : BasicPioDevice(p, p.pio_size), computeEvent(*this)
{
}

Tick
MyCompute::read(PacketPtr pkt)
{
    DPRINTF(MyCompute, "read addr\n");
    //pkt->makeAtomicResponse();//lhb
    // Support atomic swap semantics and timing vs atomic responses.
    bool is_atomic = pkt->isAtomicOp() && pkt->cmd == MemCmd::SwapReq;

    Addr offset = pkt->getAddr() - pioAddr;

        DPRINTF(MyCompute, "read addr=%#x offset=%#x size=%d atomic=%d\n",
            pkt->getAddr(), offset, pkt->getSize(), is_atomic);

    // Bounds check: if offset is outside device PIO region, return default
    if (offset >= pioSize) {
        // For atomic we still respond as bad/zero; follow callers' expectation
        if (is_atomic)
            pkt->makeAtomicResponse();
        else
            pkt->makeResponse();
        std::memset(pkt->getPtr<uint8_t>(), 0, pkt->getSize());
        return pioDelay;
    }

    // If this is an atomic swap, populate packet buffer with current data,
    // invoke the atomic op to modify the buffer, then forward to write()
    if (is_atomic) {
        uint8_t *buf = pkt->getPtr<uint8_t>();
        for (unsigned i = 0; i < pkt->getSize(); ++i) {
            Addr off = offset + i;
            uint8_t val = 0;
            switch (off) {
            case 0: val = op_a; break;
            case 1: val = op_b; break;
            case 2: val = config; break;
            case 3: val = result; break;
            case 4: val = status & 0x1; break;
            default: val = 0; break;
            }
            buf[i] = val;
        }

        // Apply the atomic operation provided by the packet
        (*(pkt->getAtomicOp()))(buf);

        // Now let write() handle storing the modified buffer back into regs
        return write(pkt);
    }

    // Non-atomic/timing read: fill packet buffer or set LE value for size==1
    if (pkt->getSize() == 1) {
        uint8_t val = 0;
        switch (offset) {
        case 0: val = op_a; break;
        case 1: val = op_b; break;
        case 2: val = config; break;
        case 3: val = result; break;
        case 4: val = status & 0x1; break;
        default: val = 0; break;
        }
        pkt->setLE<uint8_t>(val);
    } else {
        uint8_t *buf = pkt->getPtr<uint8_t>();
        for (unsigned i = 0; i < pkt->getSize(); ++i) {
            Addr off = offset + i;
            uint8_t val = 0;
            switch (off) {
            case 0: val = op_a; break;
            case 1: val = op_b; break;
            case 2: val = config; break;
            case 3: val = result; break;
            case 4: val = status & 0x1; break;
            default: val = 0; break;
            }
            buf[i] = val;
        }
    }

    // Proper response type depending on packet mode
    if (pkt->isAtomicOp())
        pkt->makeAtomicResponse();
    else
        pkt->makeResponse();

    DPRINTF(MyCompute, "read complete addr=%#x size=%d\n",
            pkt->getAddr(), pkt->getSize());

    return pioDelay;
}

Tick
MyCompute::write(PacketPtr pkt)
{
    //pkt->makeAtomicResponse();//lhb
    bool is_atomic = pkt->isAtomicOp() && pkt->cmd == MemCmd::SwapReq;

    Addr offset = pkt->getAddr() - pioAddr;

    DPRINTF(MyCompute, "write addr=%#x offset=%#x size=%d atomic=%d\n",
            pkt->getAddr(), offset, pkt->getSize(), is_atomic);

    if (offset >= pioSize) {
        if (is_atomic)
            pkt->makeAtomicResponse();
        else
            pkt->makeResponse();
        return pioDelay;
    }

    (void) pkt; (void) offset; // silence unused warnings if any

    // For writes, read data from pkt buffer (supports multi-byte writes)
    uint8_t *buf = pkt->getPtr<uint8_t>();
    for (unsigned i = 0; i < pkt->getSize(); ++i) {
        Addr off = offset + i;
        uint8_t v = buf[i];
        switch (off) {
        case 0: op_a = v; break;
        case 1: op_b = v; break;
        case 2:
            config = v;
            // trigger computation on config write:
            //  clear done, schedule compute
            status &= ~0x1; // clear done bit
            schedule(&computeEvent, curTick() + pioDelay);
            break;
        case 3:
            // result is read-only; ignore writes
            break;
        case 4:
            // allow clearing the done bit by writing 0
            if ((v & 0x1) == 0)
                status &= ~0x1;
            break;
        default:
            // ignore
            break;
        }
    }

    if (is_atomic)
        pkt->makeAtomicResponse();
    else
        pkt->makeResponse();

    DPRINTF(MyCompute, "write complete addr=%#x size=%d\n",
            pkt->getAddr(), pkt->getSize());

    return pioDelay;
}

void
MyCompute::completeOperation()
{
    // perform operation based on config bit0
    if ((config & 0x1) == 0) {
        // add
        result = static_cast<uint8_t>(op_a + op_b);
    } else {
        // sub
        result = static_cast<uint8_t>(op_a - op_b);
    }

    // set done flag
    status |= 0x1;

    // notify (if bus wants to detect changes, this could be extended)

    std::cout << "MyCompute: Compute complete: " << unsigned(op_a)
              << ( (config & 0x1) ? " - " : " + " ) << unsigned(op_b)
              << " = " << unsigned(result) << "\n";
    std::cout << "Address: 0x" << std::hex << pioAddr << std::dec
              << ", Range: " << pioSize
              << ", Delay: " << pioDelay << std::endl;
    //CPU 时钟周期是 500 ticks，除以 500 得到周期数
    std::cout << "Delay cycles: " << (pioDelay / 500) << std::endl;
}


} // namespace gem5
