#ifndef __SIM_VENUS_SEQUENCER_HH__
#define __SIM_VENUS_SEQUENCER_HH__

#include "params/VenusSequencer.hh" // 包含自动生成的Params类
#include "sim/sim_object.hh"
#include "base/trace.hh"
#include "mem/port.hh"

#include "venus_instr_pkt.hh"


namespace gem5
{
class VenusSequencer : public SimObject
{
  public:
    class VenusSequencerRVSidePort : public ResponsePort
    {
      private:
          VenusSequencer *owner;
      protected:
          Tick recvAtomic(PacketPtr pkt) override { panic("recvAtomic unimpl."); }
          void recvFunctional(PacketPtr pkt) override { panic("recvFunctional unimpl."); }
          bool recvTimingReq(PacketPtr pkt) override;
          void recvRespRetry() override { panic("recvRespRetry unimpl."); }
      public:
          AddrRangeList getAddrRanges() const override { panic("getAddrRanges unimpl."); }

          VenusSequencerRVSidePort(const std::string& name, VenusSequencer *owner) :
          ResponsePort(name, owner), owner(owner)
          { }
    };
    Port &getPort(const std::string &if_name, PortID idx=InvalidPortID) override;

  private:
    // 关键仿真组件
    EventFunctionWrapper nextTickEvent;
    VenusSequencerRVSidePort port_venussequencer_receivefrom_venuspacketgen;
    // 内部私有变量
    VenusInstrPkt* venus_instr_pkt;

    // 内部私有状态
    bool isBusy = false; //sequencer忙状态信息
    
  public:
    // 构造函数，初始化成员变量
    VenusSequencer(const VenusSequencerParams &params) : SimObject(params),
        port_venussequencer_receivefrom_venuspacketgen(params.name + ".port_venussequencer_receivefrom_venuspacketgen", this),
        nextTickEvent([this]{executeSequence();},name())
    { }


    // 全局障碍表输出
    std::vector<std::vector<bool>> global_hazard_table_o;

    bool handleRequest(VenusInstrPkt* pkt);
    void executeSequence();

};
}
#endif // __SIM_VENUS_SEQUENCER_HH__
