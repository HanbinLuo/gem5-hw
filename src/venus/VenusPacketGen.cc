#include "VenusPacketGen.hh"
#include "mem/packet.hh"

namespace gem5
{


Port& VenusPacketGen::getPort(const std::string &if_name, PortID idx)
{
    // 检查请求的端口名称是否与 Python 文件中定义的 "port_venuspacketgen_sendto_venussequencer" 匹配
    if (if_name == "port_venuspacketgen_sendto_venussequencer") {
        return port_venuspacketgen_sendto_venussequencer; // 返回对应的端口对象
    }
    // 如果名称不匹配，则调用基类方法（可能会报错，但符合框架规范）
    return SimObject::getPort(if_name, idx);
}

// 构造函数，初始化成员变量
void VenusPacketGen::VenusPacketGenSequencerSidePort::sendPacket(PacketPtr pkt) {
    panic_if(blockedPacket != nullptr, "Should never try to send if blocked!");
    if (!sendTimingReq(pkt)) {
        std::cout << "at tick = " << curTick() << ", VenusPacketGenSequencerSidePort's sendTimingReq function get an nack." << std::endl;
        blockedPacket = pkt;
    }
}
void VenusPacketGen::VenusPacketGenSequencerSidePort::recvReqRetry()
{
    assert(blockedPacket != nullptr);
    PacketPtr pkt = blockedPacket;
    blockedPacket = nullptr;
    sendPacket(pkt);
}

// 启动函数
void VenusPacketGen::startup() {
    schedule(nextTickEvent, curTick() + schedule_interval); // 首次调度
}

void VenusPacketGen::sendOneVenusPkt() {
    if(port_venuspacketgen_sendto_venussequencer.isBlocked == true) {
        this->venus_instr_pkt.display();
        port_venuspacketgen_sendto_venussequencer.recvReqRetry();
    }
    else {
        std::cout << "at tick = " << curTick() << ",execute sendOneVenusPkt, where port_venuspacketgen_sendto_venussequencer.isBlocked is false." << std::endl;
        this->venus_instr_pkt.randomize();
        std::cout << "at tick = " << curTick() << ", randomize complete." << std::endl;
        this->venus_instr_pkt.display();
        std::cout << "at tick = " << curTick() << ", start sending." << std::endl;
        port_venuspacketgen_sendto_venussequencer.sendPacket((PacketPtr)(&(this->venus_instr_pkt)));
        std::cout << "at tick = " << curTick() << ", send complete." << std::endl;
    }

    if(port_venuspacketgen_sendto_venussequencer.isBlocked == true) {
        schedule(nextTickEvent, curTick() + schedule_interval);
    }
    else {
        schedule(nextTickEvent, curTick() + schedule_interval);
    }
}
// // 用于计算vlength
// int venuspacket::vlength_cal(int vew, int vl) {
//     int vlength = 0;
//     switch (vew) {
//         case 8:
//             vlength = vl / (NrLanes * NrBankPerLane * 2);
//             if ((vl % (NrLanes * NrBankPerLane * 2)) != 0)
//                 vlength += 1;
//             break;
//         case 16:
//             vlength = vl / (NrLanes * NrBankPerLane);
//             if ((vl % (NrLanes * NrBankPerLane)) != 0)
//                 vlength += 1;
//             break;
//         default:
//             break;
//     }
//     return vlength;
// }
}
