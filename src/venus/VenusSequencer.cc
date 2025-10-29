#include "VenusSequencer.hh"
#include "venus_instr_pkt.hh"

namespace gem5
{


Port & VenusSequencer::getPort(const std::string &if_name, PortID idx)
{
    // 检查请求的端口名称是否与 Python 文件中定义的 "port_venussequencer_receivefrom_venuspacketgen" 匹配
    if (if_name == "port_venussequencer_receivefrom_venuspacketgen") {
        return port_venussequencer_receivefrom_venuspacketgen; // 返回对应的端口对象
    }
    // 如果名称不匹配，则调用基类方法（可能会报错，但符合框架规范）
    return SimObject::getPort(if_name, idx);
}

bool
VenusSequencer::VenusSequencerRVSidePort::recvTimingReq(PacketPtr pkt)
{
    
    std::cout << "at tick = " << curTick() << ", VenusSequencerRVSidePort received some info." << std::endl;
    if (!owner->handleRequest((VenusInstrPkt*)pkt)) {
        // needRetry = true;
        std::cout << "at tick = " << curTick() << ", VenusSequencerRVSidePort reports an nack." << std::endl;
        return false;
    } else {
        std::cout << "at tick = " << curTick() << ", VenusSequencerRVSidePort reports an ack." << std::endl;
        return true;
    }
}


bool VenusSequencer::handleRequest(VenusInstrPkt* pkt)
{
    if(isBusy) {
        return false;
    }
    isBusy = true;
    //pkt->display();
    this->venus_instr_pkt = new VenusInstrPkt(pkt);
    schedule(nextTickEvent,curTick()+20);
    return true;
}

void VenusSequencer::executeSequence()
{
    if(isBusy == true)
    {
        std::cout << "at tick = " << curTick() << ", VenusSequencer has received an vns instr successfully. The content is:" << std::endl;
        this->venus_instr_pkt->display();
        delete this->venus_instr_pkt;
        isBusy = false;
    }
}
}