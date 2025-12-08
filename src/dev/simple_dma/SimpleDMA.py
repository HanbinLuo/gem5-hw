from m5.objects import DmaDevice
from m5.params import *
from m5.proxy import *


class SimpleDMA(DmaDevice):
    type = "SimpleDMA"
    cxx_class = "gem5::SimpleDMA"
    cxx_header = "dev/simple_dma/simple_dma.hh"

    pio_addr = Param.Addr("SimpleDMA PIO base address")
    pio_size = Param.Addr("SimpleDMA PIO size")
    pio_latency = Param.Latency("PIO access latency")

    platform = Param.Platform(Parent.any, "Platform")
    interrupt_id = Param.Int("PLIC Interrupt ID")
