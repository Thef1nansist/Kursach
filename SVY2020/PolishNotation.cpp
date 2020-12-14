#include "pch.h"
#include "Header.h"


using namespace std;
namespace Polish
{
	int  PriorityOperation(LT::Entry& ent)
	{
		switch (ent.lexema)
		{
		case LEX_LEFTSK:
		case LEX_RIGHTSK:
			return 0;
		case LEX_PL:
		case LEX_MINUS:
			return 1;
		case LEX_STAR:
		case LEX_DIRSLASH:
			return 2;
		case LEX_LEFT:
		case LEX_RIGHT:
			return 3;
		default:
			return -1;
		}
	}

	bool PolishNotation(Lexer::LEX& tbls, Log::LOG& log)
	{
		unsigned curExprBegin = 0;
		ltvec v; 
		LT::LexTable new_table = LT::Create(tbls.lextable.maxsize);
		intvec vpositions = getExprPositions(tbls); 

		for (int i = 0; i < tbls.lextable.size; i++)
		{
			if (curExprBegin < vpositions.size() && i == vpositions[curExprBegin]) 
			{
				int lexcount = fillVector(vpositions[curExprBegin], tbls.lextable, v); 
				if (lexcount > 1)
				{
					bool rc = StartPolishNat(tbls.idtable, log, vpositions[curExprBegin], v);  
					if (!rc)
						return false;
				}

				addToTable(new_table, tbls.idtable, v); 
				i += lexcount - 1;
				curExprBegin++;
				continue;
			}
			if (tbls.lextable.table[i].lexema == LEX_ID || tbls.lextable.table[i].lexema == LEX_LITERAL)
			{
				int firstind = Lexer::IndexOfInLT(new_table, tbls.lextable.table[i].idxTI);
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
		
			if (v[i].lexema == LEX_ID || v[i].lexema == LEX_LITERAL)
			{
				int firstind = Lexer::IndexOfInLT(new_table, v[i].idxTI);
				idtable.table[v[i].idxTI].idxfirstLE = firstind;
			}
		}
	}

	intvec getExprPositions(Lexer::LEX& tbls)
	{
		intvec v;
		bool f_begin = false; 
		bool f_end = false; 
		int begin = 0;  int end = 0;

		for (int i = 0; i < tbls.lextable.size; i++)
		{
			if (tbls.lextable.table[i].lexema == LEX_EQUAL) 
			{
				begin = i + 1;
				f_begin = true;
				continue;
			}
			if (f_begin && tbls.lextable.table[i].lexema == LEX_SEPARATOR) 
			{
				end = i;
				f_end = true;
				continue;
			}
			if (f_begin && f_end)	
			{
				v.push_back(begin);
				f_begin = f_end = false;
			}
		}
		return v;
	}

	bool __cdecl StartPolishNat(IT::IdTable& idtable, Log::LOG& log, int lextable_pos, ltvec& v)
	{
		vector < LT::Entry > result;
		stack < LT::Entry > s;
		bool ignore = false;

		for (unsigned i = 0; i < v.size(); i++)
		{
			if (ignore)	
			{
				result.push_back(v[i]);
				if (v[i].lexema == LEX_RIGHTSK)
					ignore = false;
				continue;
			}
			int priority = PriorityOperation(v[i]);

			if (v[i].lexema == LEX_LEFTSK || v[i].lexema == LEX_RIGHTSK || v[i].lexema == LEX_PL || v[i].lexema == LEX_MINUS || v[i].lexema == LEX_STAR || v[i].lexema == LEX_DIRSLASH || v[i].lexema == LEX_LEFT || v[i].lexema == LEX_RIGHT)
			{
				if (s.empty() || v[i].lexema == LEX_LEFTSK)
				{
					s.push(v[i]);
					continue;
				}

				if (v[i].lexema == LEX_RIGHTSK)
				{
					while (!s.empty() && s.top().lexema != LEX_LEFTSK)
					{
						result.push_back(s.top());
						s.pop();
					}
					if (!s.empty() && s.top().lexema == LEX_LEFTSK)
						s.pop();
					continue;
				}
				while (!s.empty() && PriorityOperation(s.top()) >= priority)
				{
					result.push_back(s.top());
					s.pop();
				}
				s.push(v[i]);
			}

			if (v[i].lexema == LEX_LITERAL || v[i].lexema == LEX_ID) 
			{
				if (idtable.table[v[i].idxTI].idtype == IT::IDTYPE::F || idtable.table[v[i].idxTI].idtype == IT::IDTYPE::S)
					ignore = true;
				result.push_back(v[i]);	
			}
			if (v[i].lexema != LEX_LEFTSK & v[i].lexema != LEX_RIGHTSK & v[i].lexema != LEX_PL & v[i].lexema != LEX_MINUS & v[i].lexema != LEX_STAR & v[i].lexema != LEX_DIRSLASH & v[i].lexema != LEX_ID & v[i].lexema != LEX_LITERAL & v[i].lexema != LEX_LEFT & v[i].lexema != LEX_RIGHT)
			{
				Log::writeError(log.stream, Error::GetError(1));
				return false;
			}
		}

		while (!s.empty())
		{
			result.push_back(s.top());
			s.pop();
		}
		v = result;
		return true;
	}

}

