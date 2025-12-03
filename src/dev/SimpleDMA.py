from m5.params import *
from m5.objects import DmaDevice

class SimpleDMA(DmaDevice):
    type = 'SimpleDMA'
    cxx_class = 'gem5::SimpleDMA'
    cxx_header = 'dev/simple_dma.hh'

    pio_addr   = Param.Addr(0x50000000, "SimpleDMA PIO base address")
    pio_size   = Param.Addr(0x1000,    "SimpleDMA PIO size")
    pio_latency = Param.Latency("10ns", "PIO access latency")