#ifndef VENUS_INSTR_PKT_H
#define VENUS_INSTR_PKT_H
#include "mem/packet.hh"

#include <iostream>
#include <random>

namespace gem5
{
// 枚举类型 VenusOp
enum class VenusOp {
    VAND, VOR, VXOR,
    VBRDCST, VSHUFFLE_CLBMV,
    VSLL, VSRL, VSRA,
    VMNOT,
    VSEQ, VSNE, VSLTU, VSLT, VSLEU, VSLE, VSGTU, VSGT,
    VADD, VSADD, VSADDU, VRANGE, VRSUB, VSSUB, VSSUBU, VSUB,
    VMUL, VMULH, VMULHU, VMULHSU,
    VMULADD, VMULSUB, VADDMUL, VSUBMUL,
    VCMXMUL,
    VDIV, VREM, VDIVU, VREMU,
    VSHUFFLE,
    VREDAND, VREDOR, VREDXOR, VREDMAX, VREDMAXU, VREDMIN, VREDMINU, VREDSUM,
    YIELD,
    VPRINTF
};
class VenusInstrPkt : public Packet {
    private:
        static unsigned int vns_instr_gencounter;
    public:
        unsigned int vns_instr_id;
        bool use_vs1;
        bool use_vs2;
        bool use_vd2;
        bool use_vd1;
        bool use_vd1_op;
        bool use_vd2_op;
        VenusOp op;  // 枚举类型
        int vl;  // vlen_t 类型，假设是int
        bool vm_r;
        bool vm_w;
        int vl_shuffle;  // vlen_t 类型
        int vew;  // vew_e 类型，假设是int
        int scalar_op;  // elen_t 类型，假设是int
        bool use_scalar_op;

        // 地址范围
        int vs1_head;
        int vs1_tail;
        int vs2_head;
        int vs2_tail;
        int vd1_head;
        int vd1_tail;
        int vd2_head;
        int vd2_tail;

        // 显示函数
        void display() const;

        // 随机化设置值的函数
        void randomize();

        VenusInstrPkt();
        VenusInstrPkt(VenusInstrPkt* pkt);

    private:
        int Nrlines = 1024;
} ;
}
#endif // VENUS_INSTR_PKT_H
