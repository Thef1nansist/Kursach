#include "pch.h"
#include <iosfwd>
#include "Generator.h"
#include "Parm.h"
#include "LexAnaliz.h"
#include "IT.h"
#include "LT.h"
#include <sstream>
#include <cstring>


using namespace std;

namespace Gener
{
	static int conditionnum = 0;

	string itoS(int x) { stringstream r;  r << x;  return r.str(); }
	string genCallFuncCode(Lexer::LEX& tables, Log::LOG& log, int i)
	{
		string str;

		IT::Entry e = ITENTRY(i); // идентификатор вызываемой функции
		stack <IT::Entry> temp;
		bool stnd = (e.idtype == IT::IDTYPE::S);

		for (int j = i + 1; LEXEMA(j) != LEX_RIGHTTHESIS; j++)
		{
			if (LEXEMA(j) == LEX_ID || LEXEMA(j) == LEX_LITERAL)
				temp.push(ITENTRY(j)); // // заполн€ем стек в пр€мом пор€дке	
		}
		str += "\n";

		// раскручиваем стек
		while (!temp.empty())
		{
			if (temp.top().idtype == IT::IDTYPE::L && temp.top().iddatatype == IT::IDDATATYPE::STR)
				str = str + "push offset " + temp.top().id + "\n";
			else   str = str + "push " + temp.top().id + "\n";
			temp.pop();
		}

		if (stnd)
			str += "push offset buffer\n";
		str = str + "call " + string(e.id) + IN_CODE_ENDL;
		// выравниваниe стека
		/*if (e.value.params.count != 0) str = str + "add esp, " + itoS(4 * e.value.params.count + 4) + "\n";*/

		return str;
	}
	string genEqualCode(Lexer::LEX& tables, Log::LOG& log, int i)
	{
		string str;
		IT::Entry e1 = ITENTRY(i - 1); // левый операнд

		switch (e1.iddatatype)
		{
		case IT::IDDATATYPE::NUM:
		{
			bool first = true;
			for (int j = i + 1; LEXEMA(j) != LEX_SEPARATOR; j++)
			{
				switch (LEXEMA(j))
				{
				case LEX_LITERAL:
				case LEX_ID:
				{
					if (ITENTRY(j).idtype == IT::IDTYPE::F || ITENTRY(j).idtype == IT::IDTYPE::S) // если в выражении вызов функции
					{
						str = str + genCallFuncCode(tables, log, j); // функци€ возвращает результат в eax
						str = str + "push eax\n";				// результат выражени€ в стек дл€ дальнейшего вычислени€ выражени€
						while (LEXEMA(j) != LEX_RIGHTTHESIS) j++;
						break;
					}
					else  str = str + "push " + ITENTRY(j).id + "\n";
					break;
				}
				case LEX_PLUS:
					str = str + "pop ebx\npop eax\nadd eax, ebx\npush eax\n"; break;
				case LEX_MINUS:
					str = str + "pop ebx\npop eax\nsub eax, ebx\npush eax\n"; break;
				case LEX_STAR:
					str = str + "pop ebx\npop eax\nimul eax, ebx\npush eax\n"; break;
				case LEX_DIRSLASH:
					str = str + "pop ebx\npop eax\ncdq\nidiv ebx\npush eax\n"; break;
				case LEX_RIGHT:
					str = str + "pop ebx \npop eax \nmov cl, bl \nshr eax, cl\npush eax\n"; break;
				case LEX_LEFT:
					str = str + "pop ebx \npop eax \nmov cl, bl \nshl eax, cl\npush eax\n"; break;
				}
			} // цикл вычислени€

			str = str + "\npop ebx\nmov " + e1.id + ", ebx\n";			// вычисленное выражение в ebx 
			break;
		}
		case IT::IDDATATYPE::STR:// разрешить присваивать строкам только строки, литералы и вызовы функций
		{
			char lex = LEXEMA(i + 1);
			IT::Entry e2 = ITENTRY(i + 1);
			if (lex == LEX_ID && (e2.idtype == IT::IDTYPE::F || e2.idtype == IT::IDTYPE::S)) // вызов функции
			{
				str += genCallFuncCode(tables, log, i + 1);
				str = str + "mov " + e1.id + ", eax";
			}
			else if (lex == LEX_LITERAL) // литерал
			{
				str = str + "mov " + e1.id + ", offset " + e2.id;
			}
			else // ид(переменна€) - через регистр
			{
				str = str + "mov ecx, " + e2.id + "\nmov " + e1.id + ", ecx";
			}
		}
		}

		return str;
	}
	string genFunctionCode(Lexer::LEX& tables, int i, string funcname, int pcount)
	{
		string str;
		IT::Entry e = ITENTRY(i + 1);
		IT::IDDATATYPE type = e.iddatatype;

		str = SEPSTR(funcname) + string(e.id) + string(" PROC,\n\t");
		//дальше параметры
		int j = i + 3; // начало - то что сразу после открывающей скобки
		while (LEXEMA(j) != LEX_RIGHTTHESIS) // пока параметры не кончатс€
		{
			if (LEXEMA(j) == LEX_ID) // параметр
				str = str + string(ITENTRY(j).id) + (ITENTRY(j).iddatatype == IT::IDDATATYPE::NUM ? " : sdword, " : " : dword, ");
			j++;
		}
		int f = str.rfind(',');
		if (f > 0)
			str[f] = IN_CODE_SPACE;

		str += "\n; --- save registers ---\npush ebx\npush edx\n; ----------------------";

		return str;
	}
	string genExitCode(Lexer::LEX& tables, int i, string funcname, int pcount)
	{
		string str = "; --- restore registers ---\npop edx\npop ebx\n; -------------------------\n";
		if (LEXEMA(i + 1) != LEX_SEPARATOR)	// выход из функции (вернуть значение)
		{
			str = str + "mov eax, " + string(ITENTRY(i + 1).id) + "\n";
		}
		str += "ret\n";
		str += funcname + " ENDP" + SEPSTREMP;
		return str;
	}
	string genConditionCode(Lexer::LEX& tables, int i, string& cyclecode)
	{
		string str;
		conditionnum++;
		cyclecode.clear();
		IT::Entry lft = ITENTRY(i + 1); // левый операнд
		IT::Entry rgt = ITENTRY(i + 3); // правый операнд
		bool w = false, r = false, c = false;
		string wstr, rstr;

		for (int j = i + 5; LEXEMA(j) != LEX_DIEZ; j++) // пропустили открывающую решетку
		{
			if (LEXEMA(j) == LEX_ISTRUE) r = true;
			if (LEXEMA(j) == LEX_ISFALSE) w = true;
			if (LEXEMA(j) == LEX_CYCLE) c = true;
		}
		str = str + "mov edx, " + lft.id + "\ncmp edx, " + rgt.id + "\n";
		switch (LEXEMA(i + 2))
		{
		case LEX_MORE:  rstr = "jg";  wstr = "jl";  break;
		case LEX_LESS:   rstr = "jl";  wstr = "jg";  break;
		case LEX_EQUALS:    rstr = "jz";  wstr = "jnz";  break;
		case LEX_NOTEQUALS:   rstr = "jnz";  wstr = "jz";  break;
		}

		if (!c && r) str = str + "\n" + rstr + " right" + itoS(conditionnum);
		if (!c && w) str = str + "\n" + wstr + " wrong" + itoS(conditionnum);
		if (c)
		{
			str = str + "\n" + rstr + " cycle" + itoS(conditionnum);
			cyclecode = str;
			str = str + "\njmp cyclenext" + itoS(conditionnum);
		}
		else if (!r || !w)  str = str + "\njmp next" + itoS(conditionnum);
		return str;
	}
	vector <string> startFillVector(Lexer::LEX& tables)
	{
		vector <string> v;
		v.push_back(BEGIN);
		v.push_back(EXTERN);

		vector <string> vlt;  vlt.push_back(CONST);
		vector <string> vid;  vid.push_back(DATA);

		for (int i = 0; i < tables.idtable.size; i++)// const, data
		{
			IT::Entry e = tables.idtable.table[i];
			string str = "\t\t" + string(e.id);

			if (tables.idtable.table[i].idtype == IT::IDTYPE::L)	// литерал - в .const
			{
				switch (e.iddatatype)
				{
				case IT::IDDATATYPE::NUM:  str = str + " sdword " + itoS(e.value.vint);  break;
				case IT::IDDATATYPE::STR:  str = str + " byte '" + string(e.value.vstr.str) + "', 0";  break;
				}
				vlt.push_back(str);
			}
			else if (tables.idtable.table[i].idtype == IT::IDTYPE::V)// переменна€ - в .data
			{
				switch (e.iddatatype)
				{
				case IT::IDDATATYPE::NUM: str = str + " sdword 0";  break;
				case IT::IDDATATYPE::STR: str = str + " dword ?";  break;
				}
				vid.push_back(str);
			}
		}
		v.insert(v.end(), vlt.begin(), vlt.end());
		v.insert(v.end(), vid.begin(), vid.end());
		v.push_back(CODE);
		return v;
	}
	void CodeGeneration(Lexer::LEX& tables, Parm::PARM& parm, Log::LOG& log)
	{
		vector <string> v = startFillVector(tables);
		ofstream ofile("D:\\KDV_2019\\Generation\\Generation\\Gen.asm");
		string funcname;	// им€ текущей функции
		string cyclecode;	// эпилог цикла: cmp + j
		int pcount;			// количество параметров текущей функции
		string str;

		for (int i = 0; i < tables.lextable.size; i++)
		{
			switch (LEXEMA(i))
			{
			case LEX_MAIN:
			{
				str = str + SEPSTR("MAIN") + "main PROC";
				break;
			}
			case LEX_FUNCTION:
			{
				funcname = ITENTRY(i + 1).id;
				pcount = ITENTRY(i + 1).value.params.count;
				str = genFunctionCode(tables, i, funcname, pcount);
				break;
			}
			case LEX_RETURN:
			{
				str = genExitCode(tables, i, funcname, pcount);
				break;
			}
			case LEX_ID: // вызов функции
			{
				if (LEXEMA(i + 1) == LEX_LEFTHESIS && LEXEMA(i - 1) != LEX_FUNCTION) // не объ€вление, а вызов
					str = genCallFuncCode(tables, log, i);
				break;
			}
			case LEX_CONDITION: // условие
			{
				str = genConditionCode(tables, i, cyclecode);
				break;
			}
			case LEX_BRACELET:	// переход на метку в конце кондишна
			{
				if (LEXEMA(i + 1) == LEX_ISFALSE || LEXEMA(i + 1) == LEX_ISTRUE)
					str = str + "jmp next" + itoS(conditionnum);
			}
			case LEX_DIEZ:		// поставить метки в конце кондишна
			{
				if (LEXEMA(i - 1) == LEX_BRACELET) //   ]#
				{
					bool c = false;
					for (int j = i; j > 0 && LEXEMA(j) != LEX_CONDITION; j--)
						if (LEXEMA(j) == LEX_CYCLE)
							c = true;
					if (c)
						str = cyclecode + "\ncyclenext" + itoS(conditionnum) + ":";
					else  str += "next" + itoS(conditionnum) + ":";
				}
				break;
			}
			case LEX_ISTRUE: // условие верно(метка)
			{
				str = str + "right" + itoS(conditionnum) + ":";
				break;
			}
			case LEX_ISFALSE: // условие неверно(метка)
			{
				str = str + "wrong" + itoS(conditionnum) + ":";
				break;
			}
			case LEX_CYCLE: // цикл с условием (метка)
			{
				str = str + "cycle" + itoS(conditionnum) + ":";
				break;
			}
			case LEX_EQUAL: // присваивание (вычисление выражений)
			{
				str = genEqualCode(tables, log, i);
				while (LEXEMA(++i) != LEX_SEPARATOR);	// пропускаем выражение
				break;
			}
			case LEX_NEWLINE: // перевод строки 
			{
				str = str + "push offset newline\ncall outstr\n";
				break;
			}
			case LEX_WRITE: // вывод
			{
				IT::Entry e = ITENTRY(i + 1);
				switch (e.iddatatype)
				{
				case IT::IDDATATYPE::NUM:
					str = str + "\npush " + e.id + "\ncall outnum\n";
					break;
				case IT::IDDATATYPE::STR:
					if (e.idtype == IT::IDTYPE::L)  str = str + "\npush offset " + e.id + "\ncall outstr\n";
					else  str = str + "\npush " + e.id + "\ncall outstr\n";
					break;
				}
				break;
			}

			}

			if (!str.empty())
				v.push_back(str);
			str.clear();
		}
		v.push_back(END);
		// вывод в файл
		for (auto x : v)
			ofile << x << endl;
		ofile.close();
	}
};