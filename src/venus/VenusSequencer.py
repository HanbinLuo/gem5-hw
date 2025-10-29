from m5.SimObject import SimObject
from m5.params import *

class VenusSequencer(SimObject):
    type = 'VenusSequencer'
    cxx_header = "venus/VenusSequencer.hh"
    cxx_class = 'gem5::VenusSequencer'

    port_venussequencer_receivefrom_venuspacketgen = ResponsePort("receives venus instruction from SpiritRV32")

    max_count = Param.Int(10, "How many ticks to run before stopping")
