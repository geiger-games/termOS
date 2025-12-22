; 32-bit protected mode minimal bootloader

[org 0x7c00]
bits 16

jmp start_real

; ------------------------
; real mode start
start_real:
	xor ax, ax
	mov ds, ax
	mov es, ax
	
	; load stage2: 4 sectors -> 0x7e00
	mov ah, 0x02        ; int 13h read
	mov al, 4           ; sectors
	mov ch, 0           ; cylinder
	mov cl, 2           ; sector (starts at 1, boot = 1)
	mov dh, 0           ; head
	mov dl, 0x80        ; flop
	mov bx, 0x7e00      ; load address
	int 0x13
	jc disk_error

	cli                 ; disable interrupts
	lgdt [gdt_descriptor] ; load GDT
	mov eax, cr0
	or eax, 1           ; set PE bit
	mov cr0, eax

	jmp 0x08:0x7e00

disk_error:
	mov al, "E"
	int 0x10
	mov al, "R"
	int 0x10
	mov al, "R"
	int 0x10
	jmp $

; ------------------------
; GDT (Global Descriptor Table)
gdt_start:
	; null descriptor
	dd 0
	dd 0
	; code segment descriptor: base=0, limit=4GB, type=code/readable
	dd 0x0000ffff
	dd 0x00cf9a00
	; data segment descriptor: base=0, limit=4GB, type=data/writable
	dd 0x0000ffff
	dd 0x00cf9200
gdt_end:

gdt_descriptor:
dw gdt_end - gdt_start - 1
dd gdt_start

times 510-($-$$) db 0
dw 0xaa55