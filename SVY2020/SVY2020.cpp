// Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
#include "pch.h"
#include "Log.h"
#include "Error.h"
#include "Parm.h"
#include "LexAnaliz.h"
#include "In.h"
#include "IT.h"
#include "LT.h"
#include "PolishNotation.h"
#include "SemAnaliz.h"
#include "MFST.h"
#include "Generator.h"
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	char LEXERROR[] = "Лексический анализ завершен с ошибками";
	char SYNTERROR[] = "Синтаксический анализ завершен с ошибками";
	char SEMERROR[] = "Обнаружены семантические ошибки";
	char POLISHERROR[] = "Ошибка при попытке преобразования выражения";
	char LEXGOOD[] = "Лексический анализ завершен без ошибок";
	char SYNTGOOD[] = "Синтаксический анализ завершен без ошибок";
	char SEMGOOD[] = "Семантический анализ завершен без ошибок";
	char POLISHGOOD[] = "Преобразование выражений завершено без ошибок";
	char MESSAGE[] = "--------------------КОНЕЧНЫЕ ТАБЛИЦЫ ЛЕКСЕМ И ИДЕНТИФИКАТОРОВ-------------------";
	char STOP[] = "Выполнение программы остановлено";
	char ALLGOOD[] = "Программа успешно завершена!";
	setlocale(LC_ALL, "Russian");
	Log::LOG log;

	try
	{

		Parm::PARM parm = Parm::getparm(argc, argv);                            //получить параметры
		log = Log::getstream(parm.log);
		Log::writeLog(log);														//написать заголовок журнала
		Log::writeParm(log, parm);												//записать в журнал параметры
		In::IN in = In::getin(parm.in, log.stream);
		Log::writeIn(log.stream, in);

		in.words = In::getWordsTable(log.stream, in.text, in.code, in.size);	//разобрать на токены
		Lexer::LEX tables;
		bool lex_ok = Lexer::analyze(tables, in, log, parm);					//выполнить лексический анализ
		LT::writeLexTable(log.stream, tables.lextable);							//записать в журнал таблицы лексем и идентификаторов 
		IT::writeIdTable(log.stream, tables.idtable);							//а также соответствие токенов и лексем
		LT::writeLexemsOnLines(log.stream, tables.lextable);
		LT::writeLexTable(log.stream, tables.lextable);							//записать в журнал таблицы лексем и идентификаторов 
		IT::writeIdTable(log.stream, tables.idtable);							//а также соответствие токенов и лексем
		LT::writeLexemsOnLines(log.stream, tables.lextable);
		if (!lex_ok)
		{
			Log::writeLine(log.stream, LEXERROR, "");
			Log::writeLine(&std::cout, LEXERROR, STOP, "");
			return 0;
		}
		else
		{
			Log::writeLine(&std::cout, LEXGOOD, "");
		}
		MFST_TRACE_START(log.stream);
		MFST::Mfst mfst(tables, GRB::getGreibach());							//выполнить синтаксический анализ
		bool synt_ok = mfst.start(log);
		mfst.savededucation();
		mfst.printrules(log);													//вывести дерево разбора
		if (!synt_ok)
		{
			Log::writeLine(log.stream, SYNTERROR, "");
			Log::writeLine(&std::cout, SYNTERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, SYNTGOOD, "");
		bool sem_ok = Semantic::semanticsCheck(tables, log);					//выполнить семантический анализ
		if (!sem_ok)
		{
			Log::writeLine(log.stream, SEMERROR, "");
			Log::writeLine(&std::cout, SEMERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, SEMGOOD, "");
		bool polish_ok = Polish::PolishNotation(tables, log);					//выполнить преобразование выражений в ПОЛИЗ
		if (!polish_ok)
		{
			Log::writeLine(log.stream, POLISHERROR, "");
			Log::writeLine(&std::cout, POLISHERROR, STOP, "");
			return 0;
		}
		else Log::writeLine(&std::cout, POLISHGOOD, "");
		Log::writeLine(log.stream, MESSAGE, "");
		LT::writeLexTable(log.stream, tables.lextable);							//записать в журнал новые таблицы лексем и идентификаторов
		IT::writeIdTable(log.stream, tables.idtable);
		LT::writeLexemsOnLines(log.stream, tables.lextable);					//а также соответствие токенов и лексем
		Log::writeLine(&std::cout, MESSAGE, "");
		IT::writeIdTable(&std::cout, tables.idtable);							//записать в командную строку новые таблицы лексем и идентификаторов 
		LT::writeLexTable(&std::cout, tables.lextable);							//а также соответствие токенов и лексем
		LT::writeLexemsOnLines(&std::cout, tables.lextable);

		Gener::CodeGeneration(tables, parm, log);								//выполнить генерацию кода
		Log::writeLine(log.stream, ALLGOOD, "");									//итог работы программы
		Log::writeLine(&std::cout, ALLGOOD, "");
		Log::Close(log);													    //закрыть журнал
	}
	catch (Error::ERROR e)
	{
		Log::writeError(log.stream, e);
	}
}


