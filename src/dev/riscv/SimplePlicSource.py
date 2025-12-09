from m5.objects.PlicDevice import PlicIntDevice
from m5.params import *


class SimplePlicSource(PlicIntDevice):
    type = "SimplePlicSource"
    cxx_header = "dev/riscv/simple_plic_source.hh"
    cxx_class = "gem5::SimplePlicSource"
    abstract = False
