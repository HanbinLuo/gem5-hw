###### 编译命令

```bash
riscv64-unknown-elf-gcc -march=rv32imac -mabi=ilp32 -nostdlib -nostartfiles -ffreestanding -Wl,-T,link.ld -O2 start.S main.c -o dma_baremetal.elf
```
