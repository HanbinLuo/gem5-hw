#include "dev/dag_debug/dag_debug.hh"

#include <cstring>
#include <iostream>

#include "base/logging.hh"
#include "base/trace.hh"
#include "debug/DagDebug.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"
#include "sim/cur_tick.hh"

namespace gem5 {

DagDebug::DagDebug(const Params &p)
    : BasicPioDevice(p, 0x10),  // PIO size covers registers 0x00-0x0F
      clockPeriod(p.clock_period)
{
    cu_id_reg = 0;
    job_id_reg = 0;
    config = 0;
}

void
DagDebug::printDebugInfo()
{
    Tick currentTick = curTick();
    uint64_t cycles = (clockPeriod > 0) ? (currentTick / clockPeriod) : 0;

    DPRINTF(DagDebug,
            "DAG_DEBUG: CU_ID=%u, JOB_ID=%u, config=%u, "
            "tick=%llu, cycles=%llu\n",
            cu_id_reg, job_id_reg, config, currentTick, cycles);

    // 使用 std::cout 直接打印，与 ComputeUnit 格式一致
    std::cout << "DagDebug: Debug Print\n";
    std::cout << "  CU_ID=" << cu_id_reg << " JOB_ID=" << job_id_reg
              << " CONFIG=" << int(config)
              << " TICK=" << currentTick << " (" << cycles << " cycles)\n";
}

Tick
DagDebug::read(PacketPtr pkt)
{
    Addr offset = pkt->getAddr() - pioAddr;

    DPRINTF(DagDebug, "read addr=%#x offset=%#x size=%d\n",
            pkt->getAddr(), offset, pkt->getSize());

    // Bounds check
    if (offset >= pioSize) {
        pkt->makeResponse();
        std::memset(pkt->getPtr<uint8_t>(), 0, pkt->getSize());
        return pioDelay;
    }

    // Single byte read
    if (pkt->getSize() == 1) {
        uint8_t val = 0;
        if (offset <= 0x03) {
            unsigned byte = offset - 0x00;
            val = (cu_id_reg >> (8 * byte)) & 0xFF;
        } else if (offset >= 0x04 && offset <= 0x07) {
            unsigned byte = offset - 0x04;
            val = (job_id_reg >> (8 * byte)) & 0xFF;
        } else if (offset == 0x08) {
            val = config;
        }
        pkt->setLE<uint8_t>(val);
    } else {
        // Multi-byte read
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
            } else if (off == 0x08) {
                val = config;
            }
            buf[i] = val;
        }
    }

    pkt->makeResponse();
    return pioDelay;
}

Tick
DagDebug::write(PacketPtr pkt)
{
    Addr offset = pkt->getAddr() - pioAddr;

    DPRINTF(DagDebug, "write addr=%#x offset=%#x size=%d\n",
            pkt->getAddr(), offset, pkt->getSize());

    // Bounds check
    if (offset >= pioSize) {
        pkt->makeResponse();
        return pioDelay;
    }

    bool config_written = false;

    // Single byte write
    if (pkt->getSize() == 1) {
        uint8_t val = pkt->getLE<uint8_t>();
        if (offset <= 0x03) {
            unsigned byte = offset - 0x00;
            uint32_t mask = ~(0xFFU << (8 * byte));
            cu_id_reg = (cu_id_reg & mask) | ((uint32_t)val << (8 * byte));
        } else if (offset >= 0x04 && offset <= 0x07) {
            unsigned byte = offset - 0x04;
            uint32_t mask = ~(0xFFU << (8 * byte));
            job_id_reg = (job_id_reg & mask) | ((uint32_t)val << (8 * byte));
        } else if (offset == 0x08) {
            config = val;
            config_written = true;
        }
    } else {
        // Multi-byte write
        const uint8_t *buf = pkt->getConstPtr<uint8_t>();
        for (unsigned i = 0; i < pkt->getSize(); ++i) {
            Addr off = offset + i;
            uint8_t val = buf[i];
            if (off <= 0x03) {
                unsigned byte = off - 0x00;
                uint32_t mask = ~(0xFFU << (8 * byte));
                cu_id_reg = (cu_id_reg & mask) | ((uint32_t)val << (8 * byte));
            } else if (off >= 0x04 && off <= 0x07) {
                unsigned byte = off - 0x04;
                uint32_t mask = ~(0xFFU << (8 * byte));
                job_id_reg = (job_id_reg & mask) | ((uint32_t)val << (8 * byte));
            } else if (off == 0x08) {
                config = val;
                config_written = true;
            }
        }
    }

    // Trigger debug print when config register is written
    if (config_written) {
        printDebugInfo();
    }

    pkt->makeResponse();
    return pioDelay;
}

} // namespace gem5
