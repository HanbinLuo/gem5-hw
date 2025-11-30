from m5.objects import BasicPioDevice
from m5.params import *


class MyCompute(BasicPioDevice):
    type = "MyCompute"
    cxx_header = "my_compute_unit/my_compute.hh"
    cxx_class = "gem5::MyCompute"

    # Parameters: base address and size. pio_latency provided by BasicPioDevice
    # pio_addr is inherited from BasicPioDevice,
    # do not redefine it to avoid shadowing
    # UART-like device uses small register space: data(0) + status(4)
    pio_size = Param.Addr(0x8, "IO region size (8 bytes: data + status)")

    # Whether writes should be printed to host stdout
    # (useful to disable in tests)
    print_to_console = Param.Bool(
        True, "Print transmitted bytes to host stdout"
    )
