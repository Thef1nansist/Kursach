#pragma once

#include"LT.h"
#include "IT.h"
#include "LexAnaliz.h"

#define BEGIN ".586\n"\
".model flat, stdcall\n"\
"includelib libucrt.lib\n"\
"includelib kernel32.lib\n"\
"includelib \"D:\\Kursach\\SVY2020\\Generation\\Debug\\GenLib.lib\n"\
"ExitProcess PROTO:DWORD \n"\
".stack 4096\n"

#define EXTERN "\n outnum PROTO : DWORD\n"\
"\n outstr PROTO : DWORD\n"\
"\n concat PROTO : DWORD, : DWORD\n"\
"\n lenght PROTO : DWORD\n"\
"\n atoii  PROTO : DWORD\n"

#define END "push 0\ncall ExitProcess\nmain ENDP\nend main"

#define CONST ".const\n\t\tnewline byte 13, 10, 0"
#define DATA ".data\n\t\ttemp sdword ?\n\t\tbuffer byte 256 dup(0)"
#define CODE ".code"

#define SEPSTREMP  "\n;~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
#define SEPSTR(x)  "\n;~~~~~~~~~~ " + string(x) + " ~~~~~~~~~~~~\n"

#define MYLEX(x)   tables.lextable.table[x].lexema
#define ENTRYIT(x)  tables.idtable.table[tables.lextable.table[x].idxTI]


namespace Gener
{
	void EntryPoint(Lexer::LEX& tables, Parm::PARM& parm, Log::LOG& log);
	void idrename(IT::IdTable& table);
};
