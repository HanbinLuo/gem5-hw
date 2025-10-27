#include "venuspacket.hh"

// 构造函数，初始化成员变量
venuspacket::venuspacket() 
    : use_vs1(false), use_vs2(false), use_vd2(false), use_vd1(false), 
      use_vd1_op(false), use_vd2_op(false), op(VenusOp::VAND), vl(0),
      vm_r(false), vm_w(false), vl_shuffle(0), vew(8), scalar_op(0), use_scalar_op(false),
      vs1_head(0), vs1_tail(0), vs2_head(0), vs2_tail(0),
      vd1_head(0), vd1_tail(0), vd2_head(0), vd2_tail(0) {}

// 显示函数，打印成员变量
void venuspacket::display() const {
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
void venuspacket::randomize() {
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
