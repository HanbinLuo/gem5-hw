# 简单的uint8加减法计算单元

## 清理

```
scons -c build/RISCV/gem5.opt
```

## 编译

```
scons -j$(nproc) build/RISCV/gem5.opt
```

## 编译计算单元测试elf

```
cd tests/test-progs/riscv32/riscv32_compute_test
```

```
make all
```

## gem5直接运行

```
build/RISCV/gem5.opt configs/tutorial/part1/fs_linux_cu_test.py --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test/build/CU_test_demo.elf
```

## 调试--debug-flags=All

```
build/RISCV/gem5.opt --debug-flags=All configs/tutorial/part1/fs_linux_cu_test.py --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test/build/CU_test_demo.elf > gem5_cu_test_log_all.txt
```

## 使用minor cpu运行

```
build/RISCV/gem5.opt configs/tutorial/part1/fs_linux_cu_test.py --cpu-type=MinorCPU --caches --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test/build/CU_test_demo.elf
```

## 使用minor cpu运行--debug-flags=All

```
build/RISCV/gem5.opt --debug-flags=All configs/tutorial/part1/fs_linux_cu_test.py --cpu-type=MinorCPU --caches --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test/build/CU_test_demo.elf > gem5_cu_test_log_min_All.txt
```

## 另开一个终端

```
m5term localhost 3456
```

# DMA测试

## 清理

```
scons -c build/RISCV/gem5.opt
```

## 编译

```
scons -j$(nproc) build/RISCV/gem5.opt
```

## 编译DMA测试elf

```
cd tests/test-progs/riscv32/riscv32_dma_test
```

```
make all
```

## atomic cpu运行

### 直接运行

```
build/RISCV/gem5.opt configs/tutorial/part1/fs_linux_dma_test.py --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_dma_test/build/dma_test_demo.elf
```

### 调试ALL

```
build/RISCV/gem5.opt --debug-flags=All configs/tutorial/part1/fs_linux_dma_test.py --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_dma_test/build/dma_test_demo.elf > gem5_dma_test_log_all.txt
```

## minor cpu运行

### 直接运行

```
build/RISCV/gem5.opt configs/tutorial/part1/fs_linux_dma_test.py --cpu-type=MinorCPU --caches --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_dma_test/build/dma_test_demo.elf
```

### 调试ALL

```
build/RISCV/gem5.opt --debug-flags=All configs/tutorial/part1/fs_linux_dma_test.py --cpu-type=MinorCPU --caches --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_dma_test/build/dma_test_demo.elf > gem5_dma_test_log_all.txt
```

### 另开终端

```
m5term 3456
```

# DMA与CU联调

实现dma分别搬运16个数到cu的寄存器，计算完成再dma搬回

## 清理

```
scons -c build/RISCV/gem5.opt
```

## 编译

```
scons -j$(nproc) build/RISCV/gem5.opt
```

## 编译联调测试elf

```
cd tests/test-progs/riscv32/riscv32_compute_test_with_dma
```

```
make all
```

## atomic cpu运行

### 直接运行

```
build/RISCV/gem5.opt configs/tutorial/part1/fs_linux_cu_test_with_dma.py --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test_with_dma/build/CU_test_demo_with_dma.elf
```

## minor cpu运行

### 直接运行

```
build/RISCV/gem5.opt configs/tutorial/part1/fs_linux_cu_test_with_dma.py --cpu-type=MinorCPU --caches --bare-metal   --riscv-32bits --num-cpus 2 --kernel tests/test-progs/riscv32/riscv32_compute_test_with_dma/build/CU_test_demo_with_dma.elf
```


### 另开一个终端

```
m5term localhost 3456
```
