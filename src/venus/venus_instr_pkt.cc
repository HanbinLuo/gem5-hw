#include "venus_instr_pkt.hh"

#include <iostream>
#include <random>

namespace gem5
{
// 显示函数，打印成员变量
void VenusInstrPkt::display() const {
    std::cout << "use_vs1: " << use_vs1 << ", use_vs2: " << use_vs2
              << ", use_vd2: " << use_vd2 << ", use_vd1: " << use_vd1
              << ", use_vd1_op: " << use_vd1_op << ", use_vd2_op: " << use_vd2_op
              << ", op: " << static_cast<int>(op) << ", vl: " << vl
              << ", vm_r: " << vm_r << ", vm_w: " << vm_w
              << ", vl_shuffle: " << vl_shuffle << ", vew: " << vew
              << ", scalar_op: " << scalar_op << ", use_scalar_op: " << use_scalar_op
              << "\n";
    std::cout << "vs1_head: " << vs1_head << ", vs1_tail: " << vs1_tail
              << ", vs2_head: " << vs2_head << ", vs2_tail: " << vs2_tail
              << ", vd1_head: " << vd1_head << ", vd1_tail: " << vd1_tail
              << ", vd2_head: " << vd2_head << ", vd2_tail: " << vd2_tail
              << "\n";
}

// 随机化设置值的函数
void VenusInstrPkt::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 100);  // 适用于范围值的随机生成
    use_vs1 = dist(gen) % 2;
    use_vs2 = dist(gen) % 2;
    use_vd2 = dist(gen) % 2;
    use_vd1 = dist(gen) % 2;
    use_vd1_op = dist(gen) % 2;
    use_vd2_op = dist(gen) % 2;
    op = static_cast<VenusOp>(dist(gen) % static_cast<int>(VenusOp::VPRINTF));
    vl = dist(gen);
    vm_r = dist(gen) % 2;
    vm_w = dist(gen) % 2;
    vl_shuffle = dist(gen);
    vew = dist(gen) % 16;  // 假设 vew 取值范围是 0-15
    scalar_op = dist(gen);
    use_scalar_op = dist(gen) % 2;
    vs1_head = dist(gen) % Nrlines;
    vs1_tail = dist(gen) % Nrlines;
    vs2_head = dist(gen) % Nrlines;
    vs2_tail = dist(gen) % Nrlines;
    vd1_head = dist(gen) % Nrlines;
    vd1_tail = dist(gen) % Nrlines;
    vd2_head = dist(gen) % Nrlines;
    vd2_tail = dist(gen) % Nrlines;
}
}
