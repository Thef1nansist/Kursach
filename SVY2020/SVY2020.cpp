#include "pch.h"



using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "Russian");
	Log::LOG log;

	try
	{
		Parm::PARM parm = Parm::getparm(argc, argv);
		log = Log::getstream(parm.log);
		Log::writeLog(log);
		Log::writeParm(log, parm);
		In::IN in = In::getin(parm.in, log.stream);
		Log::writeIn(log.stream, in);

		in.words = In::getWordsTable(log.stream, in.text, in.code, in.size);
		Lexer::LEX tables;
		bool lex_ok = Lexer::analyze(tables, in, log, parm);
		LT::writeLexTable(log.stream, tables.lextable);
		IT::writeIdTable(log.stream, tables.idtable);
		LT::writeLexemsOnLines(log.stream, tables.lextable);
		LT::writeLexTable(log.stream, tables.lextable);
		IT::writeIdTable(log.stream, tables.idtable);
		LT::writeLexemsOnLines(log.stream, tables.lextable);
		if (!lex_ok)
		{
			Log::writeLine(log.stream, (char*)"Лексический анализ завершен с ошибками", "");
			Log::writeLine(&std::cout, (char*)"Лексический анализ завершен с ошибками", (char*)"Выполнение программы остановлено", "");
			return 0;
		}
		else
		{
			Log::writeLine(&std::cout, (char*)"Лексический анализ завершен без ошибок", "");
		}
		MFST_TRACE_START(log.stream);
		MFST::Mfst mfst(tables, GRB::getGreibach());
		bool synt_ok = mfst.start(log);
		mfst.savededucation();
		mfst.printrules(log);
		if (!synt_ok)
		{
			Log::writeLine(log.stream, (char*)"Синтаксический анализ завершен с ошибками", "");
			Log::writeLine(&std::cout, (char*)"Синтаксический анализ завершен с ошибками", (char*)"Выполнение программы остановлено", "");
			return 0;
		}
		else Log::writeLine(&std::cout, (char*)"Синтаксический анализ завершен без ошибок", "");
		bool sem_ok = Semantic::semanticsCheck(tables, log);
		if (!sem_ok)
		{
			Log::writeLine(log.stream, (char*)"Обнаружены семантические ошибки", "");
			Log::writeLine(&std::cout, (char*)"Обнаружены семантические ошибки", (char*)"Выполнение программы остановлено", "");
			return 0;
		}
		else Log::writeLine(&std::cout, (char*)"Семантический анализ завершен без ошибок", "");
		bool polish_ok = Polish::PolishNotation(tables, log);
		if (!polish_ok)
		{
			Log::writeLine(log.stream, (char*)"Ошибка при попытке преобразования выражения", "");
			Log::writeLine(&std::cout, (char*)"Ошибка при попытке преобразования выражения", (char*)"Выполнение программы остановлено", "");
			return 0;
		}
		else Log::writeLine(&std::cout, (char*)"Преобразование выражений завершено без ошибок", "");
		Log::writeLine(log.stream, (char*)"--------------------КОНЕЧНЫЕ ТАБЛИЦЫ ЛЕКСЕМ И ИДЕНТИФИКАТОРОВ-------------------", "");
		LT::writeLexTable(log.stream, tables.lextable);
		IT::writeIdTable(log.stream, tables.idtable);
		LT::writeLexemsOnLines(log.stream, tables.lextable);
		Log::writeLine(&std::cout, (char*)"--------------------КОНЕЧНЫЕ ТАБЛИЦЫ ЛЕКСЕМ И ИДЕНТИФИКАТОРОВ-------------------", "");
		IT::writeIdTable(&std::cout, tables.idtable);
		LT::writeLexTable(&std::cout, tables.lextable);
		LT::writeLexemsOnLines(&std::cout, tables.lextable);
		/*idrename(tables.idtable);*/
		Gener::idrename(tables.idtable);
		Gener::EntryPoint(tables, parm, log);
		Log::writeLine(log.stream, (char*)"Программа успешно завершена!", "");
		Log::writeLine(&std::cout, (char*)"Программа успешно завершена!", "");
		Log::Close(log);
	}
	catch (Error::ERROR e)
	{
		Log::writeError(log.stream, e);
	}
}




