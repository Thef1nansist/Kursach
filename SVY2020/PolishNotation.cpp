#include "pch.h"
#include "Header.h"
#include <cstring>

using namespace std;
namespace Polish
{
	int  getPriority(LT::Entry& e)
	{
		switch (e.lexema)
		{
		case LEX_LEFTHESIS: case LEX_RIGHTTHESIS: return 0;
		case LEX_PLUS: case LEX_MINUS: return 1;
		case LEX_STAR: case LEX_DIRSLASH: return 2;
		case LEX_LEFT: case LEX_RIGHT: return 3;
		default: return -1;
		}
	}

	bool PolishNotation(Lexer::LEX& tbls, Log::LOG& log)
	{
		unsigned curExprBegin = 0;
		ltvec v; // вектор элементов таблицы лексем
		LT::LexTable new_table = LT::Create(tbls.lextable.maxsize);
		intvec vpositions = getExprPositions(tbls); // позиции начала выражений

		for (int i = 0; i < tbls.lextable.size; i++)
		{
			if (curExprBegin < vpositions.size() && i == vpositions[curExprBegin]) // дошли до следующего начала выражения
			{
				int lexcount = fillVector(vpositions[curExprBegin], tbls.lextable, v); // заполняем вектор строками ТЛ в прямом порядке
				if (lexcount > 1)
				{
					bool rc = setPolishNotation(tbls.idtable, log, vpositions[curExprBegin], v);   // получаем вектор co строками ТЛ в польской нотации
					if (!rc)
						return false;
				}

				addToTable(new_table, tbls.idtable, v); // добавляем постpоенный вектор к ТЛ + корректировка ТИ
				i += lexcount - 1;
				curExprBegin++;
				continue;
			}
			if (tbls.lextable.table[i].lexema == LEX_ID || tbls.lextable.table[i].lexema == LEX_LITERAL)
			{
				int firstind = Lexer::getIndexInLT(new_table, tbls.lextable.table[i].idxTI);
				if (firstind == -1)
					firstind = new_table.size;
				tbls.idtable.table[tbls.lextable.table[i].idxTI].idxfirstLE = firstind;
			}
			LT::Add(new_table, tbls.lextable.table[i]);
		}

		tbls.lextable = new_table;
		return true;
	}

	int __cdecl fillVector(int posExprBegin, LT::LexTable& lextable, ltvec& v)
	{
		v.clear();
		for (int i = posExprBegin; i < lextable.size; i++)
		{
			if (lextable.table[i].lexema == LEX_SEPARATOR)
				break;
			else v.push_back(LT::Entry(lextable.table[i]));
		}
		return v.size();
	}

	void addToTable(LT::LexTable& new_table, IT::IdTable& idtable, ltvec& v)
	{
		for (unsigned i = 0; i < v.size(); i++)
		{
			LT::Add(new_table, v[i]);
			// правильная обратная связь между ТЛ и ТИ
			if (v[i].lexema == LEX_ID || v[i].lexema == LEX_LITERAL)
			{
				int firstind = Lexer::getIndexInLT(new_table, v[i].idxTI);
				idtable.table[v[i].idxTI].idxfirstLE = firstind;
			}
		}
	}

	intvec getExprPositions(Lexer::LEX& tbls)
	{
		intvec v;
		bool f_begin = false; // признак найденного конца выражения
		bool f_end = false;  // признак найденного начала выражения
		int begin = 0;  int end = 0;

		for (int i = 0; i < tbls.lextable.size; i++)
		{
			if (tbls.lextable.table[i].lexema == LEX_EQUAL) // начало выражения
			{
				begin = i + 1;
				f_begin = true;
				continue;
			}
			if (f_begin && tbls.lextable.table[i].lexema == LEX_SEPARATOR) // конец выражения
			{
				end = i;
				f_end = true;
				continue;
			}
			if (f_begin && f_end)	// добавить начало и конец выражения в вектор
			{
				v.push_back(begin);
				f_begin = f_end = false;
			}
		}
		return v;
	}

	bool __cdecl setPolishNotation(IT::IdTable& idtable, Log::LOG& log, int lextable_pos, ltvec& v)
	{
		//результирующий вектор
		vector < LT::Entry > result;
		// стек для сохранения операторов
		stack < LT::Entry > s;
		// флаг вызова функции
		bool ignore = false;

		for (unsigned i = 0; i < v.size(); i++)
		{
			if (ignore)	// вызов функции считаем подставляемым значением и заносим в результат
			{
				result.push_back(v[i]);
				if (v[i].lexema == LEX_RIGHTTHESIS)
					ignore = false;
				continue;
			}
			int priority = getPriority(v[i]); // его приоритет

			if (v[i].lexema == LEX_LEFTHESIS || v[i].lexema == LEX_RIGHTTHESIS || v[i].lexema == LEX_PLUS || v[i].lexema == LEX_MINUS || v[i].lexema == LEX_STAR || v[i].lexema == LEX_DIRSLASH || v[i].lexema == LEX_LEFT || v[i].lexema == LEX_RIGHT)
			{
				if (s.empty() || v[i].lexema == LEX_LEFTHESIS)
				{
					s.push(v[i]);
					continue;
				}

				if (v[i].lexema == LEX_RIGHTTHESIS)
				{
					//выталкивание элементов до  скобки
					while (!s.empty() && s.top().lexema != LEX_LEFTHESIS)
					{
						result.push_back(s.top());
						s.pop();
					}
					if (!s.empty() && s.top().lexema == LEX_LEFTHESIS)
						s.pop();
					continue;
				}
				//выталкивание элем с большим/равным приоритетом в результат
				while (!s.empty() && getPriority(s.top()) >= priority)
				{
					result.push_back(s.top());
					s.pop();
				}
				s.push(v[i]);
			}

			if (v[i].lexema == LEX_LITERAL || v[i].lexema == LEX_ID) // идентификатор, идентификатор функции или литерал
			{
				if (idtable.table[v[i].idxTI].idtype == IT::IDTYPE::F || idtable.table[v[i].idxTI].idtype == IT::IDTYPE::S)
					ignore = true;
				result.push_back(v[i]);	// операнд заносим в результирующий вектор
			}
			if (v[i].lexema != LEX_LEFTHESIS & v[i].lexema != LEX_RIGHTTHESIS & v[i].lexema != LEX_PLUS & v[i].lexema != LEX_MINUS & v[i].lexema != LEX_STAR & v[i].lexema != LEX_DIRSLASH & v[i].lexema != LEX_ID & v[i].lexema != LEX_LITERAL & v[i].lexema != LEX_LEFT & v[i].lexema != LEX_RIGHT)
			{
				Log::writeError(log.stream, Error::GetError(1));
				return false;
			}
		}

		while (!s.empty()) { result.push_back(s.top()); s.pop(); }
		v = result;
		return true;
	}

}

