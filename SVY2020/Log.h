#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Parm.h"
#include "Error.h"
#include "In.h"
#include "LT.h"
#include "IT.h"
#include <iomanip>
#include <iostream>
namespace Log
{
	struct LOG
	{
		wchar_t logfile[PARM_MAX_SIZE];
		std::ofstream* stream;
		LOG() : stream(nullptr)
		{
			wcscpy(logfile, L"");
		}
	};

	LOG getstream(wchar_t logfile[]);
	void writeLog(LOG& log);									
	void writeLine(std::ostream* stream, char* c, ...);			
	void writeParm(LOG& log, Parm::PARM& parm);					
	void writeIn(std::ostream* stream, In::IN& in);				
	void writeWords(std::ostream* stream, In::InWord* words);	
	void writeError(std::ostream* stream, Error::ERROR error);	
	void Close(LOG& log);										
}
