import m5
from m5.objects import *
root = Root(full_system=False)
root.packet_gen = random_number_gen(max_count=5)
m5.instantiate()
print("Beginning simulation!")
exit_event = m5.simulate()
print(f'Exiting @ tick {m5.curTick()} because {exit_event.getCause()}')
 