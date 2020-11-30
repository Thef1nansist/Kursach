
#include "pch.h"
#include "Header.h"
#include <iostream>
#include <ctime>
#include <fstream>

#define W(x, y)  << std::setw(x) << (y) <<
#define STR(n, line, type, id)\
	"|" W(4,n) " |  " W(5,line) "    |" W(17,type) " |  " W(SCOPED_ID_MAXSIZE, id) " |"


namespace Log
{
	LOG getstream(wchar_t  logfile[])
	{
		LOG stream;
		stream.stream = new std::ofstream;
		stream.stream->open(logfile);
		if (!stream.stream->is_open())
			throw ERROR_THROW(103); // ошибка при создании файла протокола
		wcscpy_s(stream.logfile, logfile);
		return stream;
	}

	void writeLog(LOG& log)
	{
		char buffer[80];
		time_t seconds = time(NULL);
		tm* timeinfo = localtime(&seconds);
		const char* format = "%d.%m.%Y %H:%M:%S";
		strftime(buffer, 80, format, timeinfo);
		*log.stream << "\n----------- Протокол ------------ Дата: " << buffer << " ------------ \n\n";
	}

	void writeLine(std::ostream* stream, char* c, ...)		// вывести в протокол конкатенацию строк
	{
		char** ptr = &c;			// указатель для доступа к параметрам
		char* result;				// строка результата
		result = new char[15];
		int size = 0;

		while (strcmp(*ptr, "") != 0)
		{
			size_t slen = strlen(*ptr);
			result = (char*)realloc(result, size + slen);
			result[size] = '\0';
			size += slen; // size - ПОЛНЫЙ размер буфера
			strcat_s(result, size + 1, *ptr);
			ptr++;
		}
		*stream << result << std::endl;
	}

	void writeParm(LOG& log, Parm::PARM& parm)
	{
		char inTxt[PARM_MAX_SIZE],
			outTxt[PARM_MAX_SIZE],
			logTxt[PARM_MAX_SIZE];
		wcstombs(inTxt, parm.in, wcslen(parm.in) + 1);
		wcstombs(outTxt, parm.out, wcslen(parm.out) + 1);
		wcstombs(logTxt, parm.log, wcslen(parm.log) + 1);
		*log.stream << "\n----- Параметры --------";
		*log.stream << "\n-in: " << inTxt
			<< "\n-out: " << outTxt
			<< "\n-log: " << logTxt;
		//<< "\nДополнительные ключи:\n" <<
		//(parm.f_LEX ? "-l  " : "") << (parm.f_ID ? "-i  " : "")
		//<< (parm.f_RULES ? "-r  " : "") << (parm.f_COUT ? "-c  " : "") << "\n\n";
	}

	void writeIn(std::ostream* stream, In::IN& in)
	{
		*stream << "\n---- Исходные данные ------";
		*stream << "\nКоличество символов: " << std::setw(3) << in.size
			<< "\nПроигнорировано: " << std::setw(3) << in.ignor
			<< "\nКоличество строк: " << std::setw(3) << in.lines << "\n\n";
	}

	void writeError(std::ostream* stream, Error::ERROR e)
	{
		if (stream == NULL)
		{
			if (e.position.col == -1 || e.position.line == -1)
				std::cout << std::endl << "Ошибка N" << e.id << ": " << e.message << std::endl;
			else if (e.position.col == NULL)
				std::cout << std::endl << "Ошибка N" << e.id << ": " << e.message
				<< " Строка: " << e.position.line << std::endl;
			else
				std::cout << std::endl << "Ошибка N" << e.id << ": " << e.message
				<< " Строка: " << e.position.line
				<< " Позиция в строке: " << e.position.col << std::endl;
			system("pause");
		}
		else
		{
			if (e.position.col == -1 || e.position.line == -1)
				*stream << std::endl << "Ошибка N" << e.id << ": " << e.message;
			else if (e.position.col == NULL)
				*stream << std::endl << "Ошибка N" << e.id << ": " << e.message
				<< " Строка: " << e.position.line;
			else
				*stream << std::endl << "Ошибка N" << e.id << ": " << e.message
				<< " Строка: " << e.position.line
				<< " Позиция в строке: " << e.position.col;
			*stream << std::endl;
		}
	}

	void writeWords(std::ostream* stream, In::InWord* words)
	{
		*stream << " ------------------ ТАБЛИЦА СЛОВ: ------------------" << std::endl;
		for (int i = 0; i < words->size; i++)
			*stream << std::setw(2) << i << std::setw(3) << words[i].line << " |  " << words[i].word << std::endl;
		*stream << "\n-------------------------------------------------------------------------\n\n";
	}

	void Close(LOG& log)
	{
		(*log.stream).close();
	}
};