nasm -f bin boot.asm -o boot.bin
nasm -f bin main.asm -o pm.bin
cat boot.bin pm.bin > termOS.img