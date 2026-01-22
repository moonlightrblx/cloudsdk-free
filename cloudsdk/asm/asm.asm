.code

; void* __getpeb();
__getpeb PROC
    mov rax, gs:[60h]
    ret
__getpeb ENDP

; void* __getbase();
__getbase PROC
    mov rax, gs:[60h]        ; PEB
    mov rax, [rax + 10h]     ; ImageBaseAddress
    ret
__getbase ENDP

END
