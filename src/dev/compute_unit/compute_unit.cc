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
#include "sim/cur_tick.hh"

namespace gem5 {

ComputeUnit::ComputeUnit(const Params &p)
        : PlicIntDevice(p), input_region(region_size, 0), output_region(region_size, 0),
            computeEvent(*this)
{
    // no per-element operand/result arrays anymore
    // initialize 32-bit registers from params where appropriate
    cu_id_reg = p.cu_id;
    job_id_reg = 0;
    compute_size_reg = 0;
    compute_delay_reg = 0;
    config = 0;
    status = 0;
    busy = 0;
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

    // Implement read behavior for new register layout
    if (is_atomic) {
        uint8_t *buf = pkt->getPtr<uint8_t>();
        for (unsigned i = 0; i < pkt->getSize(); ++i) {
            Addr off = offset + i;
            uint8_t val = 0;
            if (off <= 0x03) {
                unsigned byte = off - 0x00;
                val = (cu_id_reg >> (8 * byte)) & 0xFF;
            } else if (off >= 0x04 && off <= 0x07) {
                unsigned byte = off - 0x04;
                val = (job_id_reg >> (8 * byte)) & 0xFF;
            } else if (off >= 0x08 && off <= 0x0B) {
                unsigned byte = off - 0x08;
                val = (compute_size_reg >> (8 * byte)) & 0xFF;
            } else if (off >= 0x0C && off <= 0x0F) {
                unsigned byte = off - 0x0C;
                val = (compute_delay_reg >> (8 * byte)) & 0xFF;
            } else if (off >= 0x1000 && off <= 0x4FFF) {
                if (off <= 0x2FFF) {
                    Addr idx = off - 0x1000;
                    val = input_region[idx];
                } else {
                    Addr idx = off - 0x3000;
                    val = output_region[idx];
                }
            } else if (off == 0x10) {
                val = config;
            } else if (off == 0x11) {
                val = status & 0x1;
            } else if (off == 0x12) {
                val = busy & 0x1;
            }
            buf[i] = val;
        }

        // Apply the atomic operation provided by the packet then forward to write()
        (*(pkt->getAtomicOp()))(pkt->getPtr<uint8_t>());
        return write(pkt);
    }

    // Non-atomic/timing read: single-byte or multi-byte
    if (pkt->getSize() == 1) {
        uint8_t val = 0;
        if (offset <= 0x03) {
            unsigned byte = offset - 0x00;
            val = (cu_id_reg >> (8 * byte)) & 0xFF;
        } else if (offset >= 0x04 && offset <= 0x07) {
            unsigned byte = offset - 0x04;
            val = (job_id_reg >> (8 * byte)) & 0xFF;
        } else if (offset >= 0x08 && offset <= 0x0B) {
            unsigned byte = offset - 0x08;
            val = (compute_size_reg >> (8 * byte)) & 0xFF;
        } else if (offset >= 0x0C && offset <= 0x0F) {
            unsigned byte = offset - 0x0C;
            val = (compute_delay_reg >> (8 * byte)) & 0xFF;
        } else if (offset >= 0x1000 && offset <= 0x4FFF) {
            if (offset <= 0x2FFF) {
                Addr idx = offset - 0x1000;
                val = input_region[idx];
            } else {
                Addr idx = offset - 0x3000;
                val = output_region[idx];
            }
        } else if (offset == 0x10) {
            val = config;
        } else if (offset == 0x11) {
            val = status & 0x1;
        } else if (offset == 0x12) {
            val = busy & 0x1;
        }
        pkt->setLE<uint8_t>(val);
    } else {
        uint8_t *buf = pkt->getPtr<uint8_t>();
        for (unsigned i = 0; i < pkt->getSize(); ++i) {
            Addr off = offset + i;
            uint8_t val = 0;
            if (off <= 0x03) {
                unsigned byte = off - 0x00;
                val = (cu_id_reg >> (8 * byte)) & 0xFF;
            } else if (off >= 0x04 && off <= 0x07) {
                unsigned byte = off - 0x04;
                val = (job_id_reg >> (8 * byte)) & 0xFF;
            } else if (off >= 0x08 && off <= 0x0B) {
                unsigned byte = off - 0x08;
                val = (compute_size_reg >> (8 * byte)) & 0xFF;
            } else if (off >= 0x0C && off <= 0x0F) {
                unsigned byte = off - 0x0C;
                val = (compute_delay_reg >> (8 * byte)) & 0xFF;
            } else if (off >= 0x1000 && off <= 0x4FFF) {
                if (off <= 0x2FFF) {
                    Addr idx = off - 0x1000;
                    val = input_region[idx];
                } else {
                    Addr idx = off - 0x3000;
                    val = output_region[idx];
                }
            } else if (off == 0x10) {
                val = config;
            } else if (off == 0x11) {
                val = status & 0x1;
            } else if (off == 0x12) {
                val = busy & 0x1;
            }
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
        // write bytes into 32-bit registers, control bytes, or region buffers
        if (off <= 0x03) {
            unsigned byte = off - 0x00;
            uint32_t mask = uint32_t(0xFF) << (8 * byte);
            cu_id_reg = (cu_id_reg & ~mask) | (uint32_t(v) << (8 * byte));
        } else if (off >= 0x04 && off <= 0x07) {
            unsigned byte = off - 0x04;
            uint32_t mask = uint32_t(0xFF) << (8 * byte);
            job_id_reg = (job_id_reg & ~mask) | (uint32_t(v) << (8 * byte));
        } else if (off >= 0x08 && off <= 0x0B) {
            unsigned byte = off - 0x08;
            uint32_t mask = uint32_t(0xFF) << (8 * byte);
            compute_size_reg = (compute_size_reg & ~mask) | (uint32_t(v) << (8 * byte));
        } else if (off >= 0x0C && off <= 0x0F) {
            unsigned byte = off - 0x0C;
            uint32_t mask = uint32_t(0xFF) << (8 * byte);
            compute_delay_reg = (compute_delay_reg & ~mask) | (uint32_t(v) << (8 * byte));
        } else if (off >= 0x1000 && off <= 0x4FFF) {
            if (off <= 0x2FFF) {
                Addr idx = off - 0x1000;
                input_region[idx] = v;
            } else {
                Addr idx = off - 0x3000;
                output_region[idx] = v;
            }
        } else if (off == 0x10) {
            // config write: start computation when written
            config = v;
            status &= ~0x1; // clear done
            busy = 1;

            // compute delay: use compute_delay_reg (interpreted as ticks). pioDelay is NOT added.
            Tick delayTicks = Tick(compute_delay_reg);
            Tick when = curTick() + delayTicks;

            std::cout << "ComputeUnit: Starting compute\n";
            std::cout << "  CU_ID=" << cu_id_reg << " JOB_ID=" << job_id_reg
                      << " SIZE=" << compute_size_reg << " CONFIG=0x" << std::hex << int(config) << std::dec
                      << " ADDR=0x" << std::hex << pioAddr << std::dec
                      << " DELAY=" << compute_delay_reg << " ticks (" << (compute_delay_reg / 500) << " cycles)";

            // Include current simulation tick and equivalent clock cycles
            {
                Tick currentTick = curTick();
                uint64_t cycles = currentTick / 500;
                std::cout << " TICK=" << currentTick << " (" << cycles << " cycles)\n";
            }

            schedule(&computeEvent, when);
        } else if (off == 0x11) {
            // allow clearing the done bit by writing 0
            if ((v & 0x1) == 0) {
                status &= ~0x1;
                if (platform) {
                    DPRINTF(ComputeUnit, "Clearing PLIC interrupt id %d\n", _interruptID);
                    platform->clearPciInt(_interruptID);
                }
            }
        } else if (off == 0x12) {
            // Writes to busy can be used to clear or set; here accept write to clear
            busy = v & 0x1;
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

    // set done flag, clear busy flag
    status |= 0x1;
    busy = 0;

    // print register values (no per-element arithmetic anymore)
    std::cout << "ComputeUnit: Compute complete\n";
    std::cout << "  CU_ID=" << cu_id_reg << " JOB_ID=" << job_id_reg
              << " SIZE=" << compute_size_reg << " CONFIG=0x" << std::hex << int(config) << std::dec
              << " ADDR=0x" << std::hex << pioAddr << std::dec
              << " DELAY=" << compute_delay_reg << " ticks (" << (compute_delay_reg / 500) << " cycles)";

    // Include current simulation tick and equivalent clock cycles
    {
        Tick currentTick = curTick();
        uint64_t cycles = currentTick / 500;
        std::cout << " TICK=" << currentTick << " (" << cycles << " cycles)\n";
    }
    // std::cout << "Address: 0x" << std::hex << pioAddr << std::dec
    //           << ", Range: " << pioSize
    //           << ", pioDelay: " << pioDelay
    //           << ", compute_delay_reg: " << compute_delay_reg << std::endl;
    // CPU 时钟周期是 500 ticks，除以 500 得到周期数
    // std::cout << "pioDelay cycles: " << (pioDelay / 500) << std::endl;
    // std::cout << "compute_delay_reg cycles: " << (compute_delay_reg / 500) << std::endl;
    // 触发 PLIC 中断，通知处理器：计算单元完成
    if (platform) {
        DPRINTF(ComputeUnit, "Posting PLIC interrupt id %d\n", _interruptID);
        platform->postPciInt(_interruptID);
    }
}


} // namespace gem5
