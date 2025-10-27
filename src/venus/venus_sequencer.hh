#ifndef __SIM_VENUS_SEQUENCER_HH__
#define __SIM_VENUS_SEQUENCER_HH__

#include "sim/sim_object.hh"
#include "base/trace.hh"

class VenusSequencer : public SimObject
{
  public:
    VenusSequencer(const std::string &name) : SimObject(name),
        venus_req_valid_i("venus_req_valid_i", false),
        venus_req_ready_o("venus_req_ready_o", false),
        venus_yield_o("venus_yield_o", false),
        venus_idle_o("venus_idle_o", false),
        pe_req_valid_o("pe_req_valid_o", false),
        pe_req_ready_i("pe_req_ready_i", false),
        Bitalu_vinsn_done_i("Bitalu_vinsn_done_i", false),
        Cau_vinsn_done_i("Cau_vinsn_done_i", false),
        Serdiv_vinsn_done_i("Serdiv_vinsn_done_i", false),
        ShuffleUnit_vinsn_done_i("ShuffleUnit_vinsn_done_i", false),
        Bitalu_mask_valid_i("Bitalu_mask_valid_i", false),
        Cau_mask_valid_i("Cau_mask_valid_i", false),
        Serdiv_mask_valid_i("Serdiv_mask_valid_i", false)
    {
    }

    // 输入/输出接口信号
    bool venus_req_valid_i;
    bool venus_req_ready_o;
    bool venus_yield_o;
    bool venus_idle_o;
    bool pe_req_valid_o;
    bool pe_req_ready_i;
    bool Bitalu_vinsn_done_i;
    bool Cau_vinsn_done_i;
    bool Serdiv_vinsn_done_i;
    bool ShuffleUnit_vinsn_done_i;
    bool Bitalu_mask_valid_i;
    bool Cau_mask_valid_i;
    bool Serdiv_mask_valid_i;

    // 全局障碍表输出
    std::vector<std::vector<bool>> global_hazard_table_o;

    // 类的方法
    void init() {
        // 初始化或配置逻辑
    }

    // 启动时调用的方法
    void tick() {
        // 更新状态逻辑
    }

  private:
    // 内部私有变量
};

#endif // __SIM_VENUS_SEQUENCER_HH__
