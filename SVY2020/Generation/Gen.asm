.586
.model flat, stdcall
includelib libucrt.lib
includelib kernel32.lib
includelib "D:\Kursach\SVY2020\Generation\Debug\GenLib.lib
ExitProcess PROTO:DWORD 
.stack 4096


 outnum PROTO : DWORD

 outstr PROTO : DWORD

 concat PROTO : DWORD, : DWORD

 lenght PROTO : DWORD

 atoii  PROTO : DWORD

.const
		newline byte 13, 10, 0
		VALS1 byte 'LenghtA:', 0
		VALS2 byte 'concat:', 0
		VALS3 sdword 127
		VALS4 sdword -9
		VALS5 byte 'Nebeute ', 0
		VALS6 byte 'pogaluista', 0
		VALS7 byte '10November', 0
		VALS8 byte 'iz srtoki v chislo:', 0
		VALS9 sdword 1
		VALS10 byte 'sdvig vlevo:', 0
		VALS11 sdword 5
		VALS12 sdword 82
		VALS13 byte ' ', 0
		VALS14 sdword 3
		VALS15 sdword 2
.data
		temp sdword ?
		buffer byte 256 dup(0)
		standforlen sdword 0
		standstr dword ?
		minres sdword 0
		poexalix sdword 0
		poexaliy sdword 0
		poexalistrx dword ?
		poexalistry dword ?
		poexalistrz dword ?
		poexalie sdword 0
		poexalit sdword 0
		poexaliresult sdword 0
		poexaliab sdword 0
		poexalid sdword 0
.code

;~~~~~~~~~~ stand ~~~~~~~~~~~~
stand PROC,
	standa : dword, standb : dword  
; ~~~~~~~~~ soxranit registers ~~~~~~~~ 
push ebx
push edx
; ~~~~~~~~~~~~~

push standa
call lenght
push eax

pop ebx
mov standforlen, ebx


push offset VALS1
call outstr


push standforlen
call outnum

push offset newline
call outstr


push standb
push standa
call concat
mov standstr, eax

push offset VALS2
call outstr


push standstr
call outstr

push offset newline
call outstr

; ~~~~~ vostanovit registers ~~~~~~ 
pop edx
pop ebx
; ~~~~~~~~~~~~~~~~
ret
stand ENDP
;~~~~~~~~~~~~~~~~~~~~~~~~~~~


;~~~~~~~~~~ min ~~~~~~~~~~~~
min PROC,
	minx : sdword, miny : sdword  
; ~~~~~~~~~ soxranit registers ~~~~~~~~ 
push ebx
push edx
; ~~~~~~~~~~~~~
mov edx, minx
cmp edx, miny

jl right1
jg wrong1
right1:
push minx

pop ebx
mov minres, ebx

jmp next1
wrong1:
push miny

pop ebx
mov minres, ebx

next1:
; ~~~~~ vostanovit registers ~~~~~~ 
pop edx
pop ebx
; ~~~~~~~~~~~~~~~~
mov eax, minres
ret
min ENDP
;~~~~~~~~~~~~~~~~~~~~~~~~~~~


;~~~~~~~~~~ POEXALI ~~~~~~~~~~~~
main PROC
push VALS3

pop ebx
mov poexalix, ebx

push VALS4

pop ebx
mov poexaliy, ebx

mov poexalistrx, offset VALS5
mov poexalistry, offset VALS6
mov poexalistrz, offset VALS7

push offset VALS8
call outstr


push poexalistrz
call atoii
push eax

pop ebx
mov poexalie, ebx


push poexalie
call outnum

push offset newline
call outstr

push poexalix
push VALS9
pop ebx 
pop eax 
mov cl, bl 
shl eax, cl
push eax

pop ebx
mov poexaliresult, ebx


push offset VALS10
call outstr


push poexaliresult
call outnum

push offset newline
call outstr


push poexaliy
push poexalix
call min
push eax

pop ebx
mov poexalit, ebx


push poexalit
call outnum

push offset newline
call outstr

push VALS11

pop ebx
mov poexaliab, ebx


pop ebx
mov poexalid, ebx

mov edx, poexaliab
cmp edx, VALS12

jnz cycle2
jmp cyclenext2
cycle2:

push poexaliab
call outnum


push offset VALS13
call outstr

push poexaliab
push VALS14
pop ebx
pop eax
add eax, ebx
push eax
push VALS15
pop ebx
pop eax
imul eax, ebx
push eax

pop ebx
mov poexaliab, ebx

mov edx, poexaliab
cmp edx, VALS12

jnz cycle2
cyclenext2:

push offset VALS13
call outstr


push poexaliab
call outnum

push offset newline
call outstr


push poexalistry
push poexalistrx
call stand

push 0
call ExitProcess
main ENDP
end main
