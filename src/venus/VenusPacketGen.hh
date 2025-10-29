#ifndef VENUSPACKET_H
#define VENUSPACKET_H

#include <iostream>
#include <random>
#include <vector>
#include <cassert>

#include "params/VenusPacketGen.hh" // 包含自动生成的Params类

#include "sim/sim_object.hh"
#include "base/trace.hh"
#include "mem/port.hh"
#include "mem/packet.hh"

#include "venus_instr_pkt.hh"

namespace gem5
{
// C++实体类声明
class VenusPacketGen : public SimObject{
  
    public:
        class VenusPacketGenSequencerSidePort : public RequestPort
        {
        private:
            VenusPacketGen *owner;
            PacketPtr blockedPacket = nullptr;
        protected:
            bool recvTimingResp(PacketPtr pkt) override { panic("recvTimingResp unimpl."); }
            void recvRangeChange() override { panic("recvRangeChange unimpl."); }
        public:
            bool isBlocked = false;
            void sendPacket(PacketPtr pkt);
            void recvReqRetry() override;
            VenusPacketGenSequencerSidePort(const std::string& name, VenusPacketGen *owner) :
                RequestPort(name, owner), owner(owner)
            { }
        };
        Port &getPort(const std::string &if_name, PortID idx=InvalidPortID) override;

    private:
        // 内部私有变量
        VenusPacketGenSequencerSidePort port_venuspacketgen_sendto_venussequencer;
        unsigned int schedule_interval = 100;
        EventFunctionWrapper nextTickEvent;

    public:
        // 成员变量
        VenusInstrPkt venus_instr_pkt;

        // 构造函数
        VenusPacketGen(const VenusPacketGenParams &params) : SimObject(params),
            port_venuspacketgen_sendto_venussequencer(params.name + ".port_venuspacketgen_sendto_venussequencer", this),
            nextTickEvent([this]{sendOneVenusPkt();},name()) 
        { }

        // 发送函数
        void sendOneVenusPkt();
        void startup() override;

    private:
        // 私有成员函数，计算vlength
        // int vlength_cal(int vew, int vl);
        // int Nrlines = 1024;
};
}
#endif // VENUS_PACKET_H
