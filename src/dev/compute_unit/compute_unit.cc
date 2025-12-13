#include "dev/compute_unit/compute_unit.hh"

#include "dev/riscv/plic_device.hh"

#include <cstring>
#include <iostream>

#include "base/logging.hh"
#include "base/trace.hh"
#include "debug/ComputeUnit.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"
#include "sim/system.hh"

namespace gem5 {

ComputeUnit::ComputeUnit(const Params &p)
    : PlicIntDevice(p), computeDelay(p.compute_latency), computeEvent(*this)
{
    std::memset(op_a, 0, sizeof(op_a));
    std::memset(op_b, 0, sizeof(op_b));
    std::memset(result, 0, sizeof(result));
}

Tick
ComputeUnit::read(PacketPtr pkt)
{
    DPRINTF(ComputeUnit, "read addr\n");
    //pkt->makeAtomicResponse();//lhb
    // Support atomic swap semantics and timing vs atomic responses.
    bool is_atomic = pkt->isAtomicOp() && pkt->cmd == MemCmd::SwapReq;

    Addr offset = pkt->getAddr() - pioAddr;

        DPRINTF(ComputeUnit, "read addr=%#x offset=%#x size=%d atomic=%d\n",
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
            if (off < 16)
                val = op_a[off];
            else if (off < 32)
                val = op_b[off - 16];
            else if (off < 48)
                val = result[off - 32];
            else if (off == 48)
                val = length;
            else if (off == 49)
                val = config;
            else if (off == 50)
                val = status & 0x1;
<<<<<<< HEAD
=======
            else if (off == 51)
                val = busy & 0x1;
>>>>>>> 1fbbaed47c3f936226c0a0c97cbaeb8f8647b401
            else
                val = 0;
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
        if (offset < 16)
            val = op_a[offset];
        else if (offset < 32)
            val = op_b[offset - 16];
        else if (offset < 48)
            val = result[offset - 32];
        else if (offset == 48)
            val = length;
        else if (offset == 49)
            val = config;
        else if (offset == 50)
            val = status & 0x1;
<<<<<<< HEAD
=======
        else if (offset == 51)
            val = busy & 0x1;
>>>>>>> 1fbbaed47c3f936226c0a0c97cbaeb8f8647b401
        else
            val = 0;
        pkt->setLE<uint8_t>(val);
    } else {
        uint8_t *buf = pkt->getPtr<uint8_t>();
        for (unsigned i = 0; i < pkt->getSize(); ++i) {
            Addr off = offset + i;
            uint8_t val = 0;
            if (off < 16)
                val = op_a[off];
            else if (off < 32)
                val = op_b[off - 16];
            else if (off < 48)
                val = result[off - 32];
            else if (off == 48)
                val = length;
            else if (off == 49)
                val = config;
            else if (off == 50)
                val = status & 0x1;
<<<<<<< HEAD
=======
            else if (off == 51)
                val = busy & 0x1;
>>>>>>> 1fbbaed47c3f936226c0a0c97cbaeb8f8647b401
            else
                val = 0;
            buf[i] = val;
        }
    }

    // Proper response type depending on packet mode
    if (pkt->isAtomicOp())
        pkt->makeAtomicResponse();
    else
        pkt->makeResponse();

    DPRINTF(ComputeUnit, "read complete addr=%#x size=%d\n",
            pkt->getAddr(), pkt->getSize());

    return pioDelay;
}

Tick
ComputeUnit::write(PacketPtr pkt)
{
    //pkt->makeAtomicResponse();//lhb
    bool is_atomic = pkt->isAtomicOp() && pkt->cmd == MemCmd::SwapReq;

    Addr offset = pkt->getAddr() - pioAddr;

    DPRINTF(ComputeUnit, "write addr=%#x offset=%#x size=%d atomic=%d\n",
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
        if (off < 16) {
            op_a[off] = v;
        } else if (off < 32) {
            op_b[off - 16] = v;
        } else if (off < 48) {
            // result is read-only; ignore writes
        } else if (off == 48) {
            length = v;
        } else if (off == 49) {
            config = v;
            // trigger computation on config write:
<<<<<<< HEAD
            //  clear done, schedule compute
            status &= ~0x1; // clear done bit
=======
            //  clear done, set busy, schedule compute
            status &= ~0x1; // clear done bit
            busy = 1;       // set busy bit immediately
>>>>>>> 1fbbaed47c3f936226c0a0c97cbaeb8f8647b401
            {//计划实现根据配置长度启动计算延迟
                Tick when = curTick() + computeDelay;
                if (!sys->isAtomicMode()) {
                    when += pioDelay;
                }
                schedule(&computeEvent, when);
            }
        } else if (off == 50) {
            // allow clearing the done bit by writing 0
            if ((v & 0x1) == 0) {
                status &= ~0x1;
                // 如果正在向 PLIC 清除中断，也同时通知平台
                if (platform) {
                    DPRINTF(ComputeUnit, "Clearing PLIC interrupt id %d\n", _interruptID);
                    platform->clearPciInt(_interruptID);
                }
            }
        }
    }

    if (is_atomic)
        pkt->makeAtomicResponse();
    else
        pkt->makeResponse();

    DPRINTF(ComputeUnit, "write complete addr=%#x size=%d\n",
            pkt->getAddr(), pkt->getSize());

    return pioDelay;
}

void
ComputeUnit::completeOperation()
{
    // perform operation based on config bit0
    // Limit length to 16
    uint8_t len = (length > 16) ? 16 : length;
    if (len == 0) len = 1; // Default to 1 if 0? Or just do nothing? Let's assume at least 1.

    for (int i = 0; i < len; ++i) {
        if ((config & 0x1) == 0) {
            // add
            result[i] = static_cast<uint8_t>(op_a[i] + op_b[i]);
        } else {
            // sub
            result[i] = static_cast<uint8_t>(op_a[i] - op_b[i]);
        }
    }

<<<<<<< HEAD
    // set done flag
    status |= 0x1;
=======
    // set done flag, clear busy flag
    status |= 0x1;
    busy = 0;  // clear busy bit when computation completes
>>>>>>> 1fbbaed47c3f936226c0a0c97cbaeb8f8647b401

    // notify (if bus wants to detect changes, this could be extended)

    std::cout << "ComputeUnit: Compute complete (len=" << unsigned(len) << ")\n";
    for (int i = 0; i < len; ++i) {
        std::cout << "  [" << i << "]: " << unsigned(op_a[i])
                  << ( (config & 0x1) ? " - " : " + " ) << unsigned(op_b[i])
                  << " = " << unsigned(result[i]) << "\n";
    }
    std::cout << "Address: 0x" << std::hex << pioAddr << std::dec
              << ", Range: " << pioSize
              << ", Delay: " << pioDelay
              << ", Compute Delay: " << computeDelay << std::endl;
    //CPU 时钟周期是 500 ticks，除以 500 得到周期数
    std::cout << "Delay cycles: " << (pioDelay / 500) << std::endl;
    std::cout << "Compute Delay cycles: " << (computeDelay / 500) << std::endl;
    // 触发 PLIC 中断，通知处理器：计算单元完成
    if (platform) {
        DPRINTF(ComputeUnit, "Posting PLIC interrupt id %d\n", _interruptID);
        platform->postPciInt(_interruptID);
    }
}


} // namespace gem5
