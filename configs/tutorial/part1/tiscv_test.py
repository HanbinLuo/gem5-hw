# from m5.objects import *

# system = System()

# system.clk_domain = SrcClockDomain()
# system.clk_domain.clock = '1GHz'
# system.clk_domain.voltage_domain = VoltageDomain()

# # 32-bit RISC-V CPU 配置
# system.mem_mode = 'timing'
# system.mem_ranges = [AddrRange('1GB')]
# system.membus = SystemXBar()
# system.cpu = MinorCPU(cpu_id=0, numThreads=1)
# system.cpu.createInterruptController()
# system.cpu.icache_port = system.membus.cpu_side_ports
# system.cpu.dcache_port = system.membus.cpu_side_ports
# # system.cpu.interrupts[0].pio = system.membus.mem_side_ports
# # system.cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
# # system.cpu.interrupts[0].int_responder = system.membus.mem_side_ports
# # system.cpu.interrupts = [InterruptController()]
# # system.cpu = MinorCPU()
# # system.cpu.isa = "riscv32"  # 设置为 32 位

# system.system_port = system.membus.cpu_side_ports
# system.mem_ctrl = MemCtrl()
# system.mem_ctrl.dram = DDR3_1600_8x8()
# system.mem_ctrl.dram.range = system.mem_ranges[0]
# system.mem_ctrl.port = system.membus.mem_side_ports
# # system.mem_ctrl = MemCtrl()
# # system.mem_ctrl.dram = DDR3_1600_8x8()
# # system.mem_ctrl.dram.range = system.mem_ranges[0]
# # system.mem_ctrl.port = system.membus.mem_side_ports


# # 配置内存总线

# # system.cpu.icache_port = system.membus.cpu_side_ports
# # system.cpu.dcache_port = system.membus.cpu_side_ports

# # 创建线程和进程
# binary = 'tests/test-progs/riscv32/hello.elf'
# system.workload = SEWorkload.init_compatible(binary)
# process = Process()
# process.cmd = [binary]
# system.cpu.workload = process
# system.cpu.createThreads()

# # 设置根系统并开始仿真
# root = Root(full_system=False, system=system)
# import m5
# m5.instantiate()

# exit_event = m5.simulate()
# print(f"Exiting @ tick {m5.curTick()} because {exit_event.getCause()}")


import m5
from m5.objects import *
system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]
system.cpu = MinorCPU()
system.membus = SystemXBar()
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports

system.cpu.createInterruptController()
# system.cpu.interrupts[0].pio = system.membus.mem_side_ports
# system.cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
# system.cpu.interrupts[0].int_responder = system.membus.mem_side_ports

system.system_port = system.membus.cpu_side_ports

# system.mem_ctrl = MemCtrl()
# system.mem_ctrl.dram = DDR3_1600_8x8()
# system.mem_ctrl.dram.range = system.mem_ranges[0]
# system.mem_ctrl.port = system.membus.mem_side_ports
system.physmem = SimpleMemory()
system.physmem.port = system.membus.mem_side_ports

binary = 'tests/test-progs/riscv32/RTOSDemo.elf'

# for gem5 V21 and beyond
system.workload = SEWorkload.init_compatible(binary)

process = Process()
process.cmd = [binary]
system.cpu.workload = process
system.cpu.createThreads()
root = Root(full_system = False, system = system)
m5.instantiate()
print("Beginning simulation!")
exit_event = m5.simulate()
print('Exiting @ tick {} because {}'
      .format(m5.curTick(), exit_event.getCause()))