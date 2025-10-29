from m5.SimObject import SimObject
from m5.params import *

class VenusPacketGen(SimObject):
    type = 'VenusPacketGen'
    cxx_header = "venus/VenusPacketGen.hh"
    cxx_class = 'gem5::VenusPacketGen'

    port_venuspacketgen_sendto_venussequencer = RequestPort("send venus instruction to venus sequencer")

    max_count = Param.Int(10, "How many ticks to run before stopping")
