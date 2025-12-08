from m5.objects.PlicDevice import PlicIntDevice
from m5.params import *


class MyCompute(PlicIntDevice):
    type = "MyCompute"
    cxx_header = "dev/my_compute_unit/my_compute.hh"
    cxx_class = "gem5::MyCompute"

    # Parameters: base address and size. pio_latency provided by BasicPioDevice
    # pio_addr is inherited from BasicPioDevice,
    # do not redefine it to avoid shadowing
    # UART-like device uses small register space: data(0) + status(4)
    # pio_size = Param.Addr(0x40, "Size of address range")

    # Whether writes should be printed to host stdout
    # (useful to disable in tests)
    print_to_console = Param.Bool(
        True, "Print transmitted bytes to host stdout"
    )

    compute_latency = Param.Latency("100ns", "Time taken for computation")
    # PLIC 中断号：设备完成计算后会调用 platform->postPciInt(interrupt_id)
    # 在 top-level 配置脚本中应确保 PLIC 的 n_src 至少为 interrupt_id+1
    # interrupt_id = Param.Int(0xB, "PLIC interrupt ID for MyCompute")
