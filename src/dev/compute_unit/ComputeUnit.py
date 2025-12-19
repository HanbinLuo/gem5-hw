from m5.objects.PlicDevice import PlicIntDevice
from m5.params import *


class ComputeUnit(PlicIntDevice):
    type = "ComputeUnit"
    cxx_header = "dev/compute_unit/compute_unit.hh"
    cxx_class = "gem5::ComputeUnit"

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

    # compute_latency removed: compute delay comes from device register at runtime
    # Compute unit identifier (configurable from top-level Python)
    cu_id = Param.UInt32(0, "Compute Unit ID")
    # interrupt_id is provided via PlicIntDevice/Params (ensure top-level config sets this)
