#pragma once

#include"LT.h"
#include "IT.h"
#include "LexAnaliz.h"


#define SEPSTREMP  "\n;------------------------------\n"
#define SEPSTR(x)  "\n;----------- " + string(x) + " ------------\n"


#define BEGIN ".586\n"\
".model flat, stdcall\n"\
"includelib libucrt.lib\n"\
"includelib kernel32.lib\n"\
"includelib \"../Debug/GenLib.lib\n"\
"ExitProcess PROTO:DWORD \n"\
".stack 4096\n"

#define END "push 0\ncall ExitProcess\nmain ENDP\nend main"

#define EXTERN "\n outnum PROTO : DWORD\n"\
"\n outstr PROTO : DWORD\n"\
"\n concat PROTO : DWORD, : DWORD, : DWORD\n"\
"\n lenght PROTO : DWORD, : DWORD\n"\
"\n atoii  PROTO : DWORD\n"


#define ITENTRY(x)  tables.idtable.table[tables.lextable.table[x].idxTI]
#define LEXEMA(x)   tables.lextable.table[x].lexema


#define CONST ".const\n\t\tnewline byte 13, 10, 0"
#define DATA ".data\n\t\ttemp sdword ?\n\t\tbuffer byte 256 dup(0)"
#define CODE ".code"

namespace Gener
{
	void CodeGeneration(Lexer::LEX& tables, Parm::PARM& parm, Log::LOG& log);
};
