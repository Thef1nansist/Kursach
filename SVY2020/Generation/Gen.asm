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
		VALS3 sdword 9
		VALS4 sdword -9
		VALS5 byte 'nebeite ', 0
		VALS6 byte 'pogaluista', 0
		VALS7 byte '15December', 0
		VALS8 byte 'iz srtoki v chislo:', 0
		VALS9 sdword 40
		VALS10 sdword 1
		VALS11 byte 'sdvig vlevo:', 0
		VALS12 sdword 127
		VALS13 sdword 5
		VALS14 sdword 82
		VALS15 byte ' ', 0
		VALS16 sdword 3
		VALS17 sdword 2
.data
		temp sdword ?
		buffer byte 256 dup(0)
		mulforlen sdword 0
		mulstr dword ?
		minresult sdword 0
		poexalix sdword 0
		poexaliy sdword 0
		poexalistrokx dword ?
		poexalistroky dword ?
		poexalistrokz dword ?
		poexalichil sdword 0
		poexalitemp sdword 0
		poexalipre sdword 0
		poexaliresult sdword 0
		poexalixru sdword 0
.code

;~~~~~~~~~~ mul0 ~~~~~~~~~~~~
mul0 PROC,
	mulada : dword, mulbred : dword  
push ebx
push edx


push mulada
call lenght
push eax

pop ebx
mov mulforlen, ebx


push offset VALS1
call outstr


push mulforlen
call outnum

push offset newline
call outstr


push mulbred
push mulada
call concat
mov mulstr, eax

push offset VALS2
call outstr


push mulstr
call outstr

push offset newline
call outstr


pop edx
pop ebx
ret
mul0 ENDP
;~~~~~~~~~~~~~~~~~~~~~~~~~~~


;~~~~~~~~~~ min1 ~~~~~~~~~~~~
min1 PROC,
	minx : sdword, miny : sdword  
push ebx
push edx

mov edx, minx
cmp edx, miny

jl right1
jge wrong1
right1:
push minx

pop ebx
mov minresult, ebx

jmp next1
wrong1:
push miny

pop ebx
mov minresult, ebx

next1:

pop edx
pop ebx
mov eax, minresult
ret
min1 ENDP
;~~~~~~~~~~~~~~~~~~~~~~~~~~~


;~~~~~~~~~~ POEXALI ~~~~~~~~~~~~
main PROC
push VALS3

pop ebx
mov poexalix, ebx

push VALS4

pop ebx
mov poexaliy, ebx

mov poexalistrokx, offset VALS5
mov poexalistroky, offset VALS6
mov poexalistrokz, offset VALS7

push offset VALS8
call outstr


push poexalistrokz
call atoii
push eax

pop ebx
mov poexalichil, ebx


push poexalichil
call outnum

push offset newline
call outstr

push poexalix
push VALS9
pop ebx
pop eax
sub eax, ebx
push eax

pop ebx
mov poexalipre, ebx


push poexalipre
call outnum

push offset newline
call outstr

push poexalix
push VALS10
pop ebx 
pop eax 
mov cl, bl 
shl eax, cl
push eax

pop ebx
mov poexaliresult, ebx


push offset VALS11
call outstr


push poexaliresult
call outnum

push offset newline
call outstr

push VALS12
push VALS12
pop ebx
pop eax
add eax, ebx
push eax

push poexaliy
push poexalix
call min1
push eax
pop ebx
pop eax
add eax, ebx
push eax

pop ebx
mov poexalitemp, ebx


push poexalitemp
call outnum

push offset newline
call outstr

push VALS13

pop ebx
mov poexalixru, ebx

mov edx, poexalixru
cmp edx, VALS14

jl cycle2
jmp cyclenext2
cycle2:

push poexalixru
call outnum


push offset VALS15
call outstr

push poexalixru
push VALS16
pop ebx
pop eax
add eax, ebx
push eax
push VALS17
pop ebx
pop eax
imul eax, ebx
push eax

pop ebx
mov poexalixru, ebx

mov edx, poexalixru
cmp edx, VALS14

jl cycle2
cyclenext2:

push offset VALS15
call outstr


push poexalixru
call outnum

push offset newline
call outstr


push poexalistroky
push poexalistrokx
call mul0

push 0
call ExitProcess
main ENDP
end main
