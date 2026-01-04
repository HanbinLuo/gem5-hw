from m5.objects.Device import BasicPioDevice
from m5.params import *


class DagDebug(BasicPioDevice):
    type = "DagDebug"
    cxx_header = "dev/dag_debug/dag_debug.hh"
    cxx_class = "gem5::DagDebug"

    # PIO size covers registers: cu_id(4) + job_id(4) + config(1) + padding
    pio_size = Param.Addr(0x10, "Size of address range")

    # Clock period in ticks for tick-to-cycle conversion
    # Default 1000 ticks = 1 cycle (1 GHz at 1ps tick)
    clock_period = Param.Tick(
        1000, "Clock period in ticks for cycle calculation"
    )
