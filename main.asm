; ------------------------
; protected mode start
[org 0x7e00]
[bits 32]

pm_start:
	mov ax, 0x10        ; data segment selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0x90000    ; stack somewhere safe

	call clear

	mov edi, 0xb8000
	mov esi, msg
	call print

	jmp $

; ------------------------
; code

done:
	ret

print:
	lodsb
	or al, al
	jz done
	mov ah, 0x0f
	stosw
	jmp print

clear:
    mov edi, 0xb8000        ; VGA mem start
    mov ecx, 80*25          ; total characters
    mov ax, 0x0f20          ; space char, white on black
    rep stosw               ; write ECX words
    ret

; ------------------------
; data
msg db "Hello 32-bit protected mode!",0

times 2048-($-$$) db 0