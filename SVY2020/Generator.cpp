#include "pch.h"

using namespace std;

namespace Gener
{
	static int conditionnum = 0;

	string int_to_string(int valueofint)
	{
		stringstream valueofstring;
		valueofstring << valueofint;
		return valueofstring.str();
	}

	string CallOfFunction(Lexer::LEX& tables, Log::LOG& log, int i)
	{
		stack <IT::Entry> IDs;
		IT::Entry name_of_function = ENTRYIT(i); // идентификатор вызываемой функции
		string stroka;

		for (int j = i + 1; LEX_RIGHTSK != MYLEX(j); j++)
			if (MYLEX(j) == LEX_ID || MYLEX(j) == LEX_LITERAL)
				IDs.push(ENTRYIT(j)); // записываем в стек
		stroka += "\n";

		for (; !IDs.empty();) // разбираем стек
		{
			if (IDs.top().idtype == IT::IDTYPE::L && IDs.top().iddatatype == IT::IDDATATYPE::STR)
				stroka = stroka + "push offset " + IDs.top().id + "\n";
			else
				stroka = stroka + "push " + IDs.top().id + "\n";

			IDs.pop();
		}

		stroka = stroka + "call " + string(name_of_function.id) + IN_CODE_ENDL;

		return stroka;
	}
	string EqualizationOfValue(Lexer::LEX& tables, Log::LOG& log, int i)
	{

		IT::Entry operandleft = ENTRYIT(i - 1);
		string stroka;
		switch (operandleft.iddatatype)
		{
		case IT::IDDATATYPE::NUM:
		{
			bool first = true;
			for (int j = i + 1; MYLEX(j) != LEX_SEPARATOR; j++)
			{
				switch (MYLEX(j))
				{
				case LEX_LITERAL:
				case LEX_ID:
				{
					if (ENTRYIT(j).idtype == IT::IDTYPE::F || ENTRYIT(j).idtype == IT::IDTYPE::S) // если в выражении вызов функции
					{
						stroka += CallOfFunction(tables, log, j); //возврат в eax будет
						stroka += "push eax\n";				// ну и тут мы снова добавл€ем результат в стек
						while (MYLEX(j) != LEX_RIGHTSK)
							j++;
						break;
					}
					else
						stroka = stroka + "push " + ENTRYIT(j).id + "\n";
					break;
				}
				case LEX_PL:
					stroka += "pop ebx\npop eax\nadd eax, ebx\npush eax\n";
					break;
				case LEX_MINUS:
					stroka += "pop ebx\npop eax\nsub eax, ebx\npush eax\n";
					break;
				case LEX_STAR:
					stroka += "pop ebx\npop eax\nimul eax, ebx\npush eax\n";
					break;
				case LEX_DIRSLASH:
					stroka += "pop ebx\npop eax\ncdq\nidiv ebx\npush eax\n";
					break;
				case LEX_RIGHT:
					stroka += "pop ebx \npop eax \nmov cl, bl \nshr eax, cl\npush eax\n";
					break;
				case LEX_LEFT:
					stroka += "pop ebx \npop eax \nmov cl, bl \nshl eax, cl\npush eax\n";
					break;
				}
			} // цикл вычислени€

			stroka = stroka + "\npop ebx\nmov " + operandleft.id + ", ebx\n"; //теперь значение, которое получилось, запихиваем в регистр ebx
			break;
		}
		case IT::IDDATATYPE::STR:// разрешить присваивать строкам только строки, литералы и вызовы функций
		{
			char lexema = MYLEX(i + 1);
			IT::Entry operandright = ENTRYIT(i + 1);
			if ((operandright.idtype == IT::IDTYPE::F || operandright.idtype == IT::IDTYPE::S) && lexema == LEX_ID) // вызов функции
			{
				stroka += CallOfFunction(tables, log, i + 1);
				stroka = stroka + "mov " + operandleft.id + ", eax";
			}
			else if (lexema == LEX_LITERAL) // литерал
			{
				stroka = stroka + "mov " + operandleft.id + ", offset " + operandright.id;
			}
			else // ид(переменна€) - через регистр
			{
				stroka = stroka + "mov ecx, " + operandright.id + "\nmov " + operandleft.id + ", ecx";
			}
		}
		}

		return stroka;
	}
	string genFunctionCode(Lexer::LEX& tables, int i, string funcname, int pcount)
	{
		IT::Entry valueinlextable = ENTRYIT(i + 1);
		string stroka;

		stroka = SEPSTR(funcname) + string(valueinlextable.id) + string(" PROC,\n\t");
		//дальше параметры
		int j = i + 3; // начало - то что сразу после открывающей скобки
		while (MYLEX(j) != LEX_RIGHTSK) // пока параметры не кончатс€
		{
			if (MYLEX(j) == LEX_ID) // параметр
				stroka = stroka + string(ENTRYIT(j).id) + (ENTRYIT(j).iddatatype == IT::IDDATATYPE::NUM ? " : sdword, " : " : dword, ");
			j++;
		}
		int f = stroka.rfind(',');
		if (f > 0)
			stroka[f] = IN_CODE_SPACE;

		stroka += "\n; ~~~~~~~~~ soxranit registers ~~~~~~~~ \npush ebx\npush edx\n; ~~~~~~~~~~~~~";

		return stroka;
	}
	string ExitCodeGener(Lexer::LEX& tables, int i, string funcname, int pcount)
	{
		string stroka = "; ~~~~~ vostanovit registers ~~~~~~ \npop edx\npop ebx\n; ~~~~~~~~~~~~~~~~\n";
		
		if (MYLEX(i + 1) != LEX_SEPARATOR)	// выход из функции (вернуть значение)
		{
			stroka = stroka + "mov eax, " + string(ENTRYIT(i + 1).id) + "\n";
		}
		stroka += "ret\n";
		stroka = stroka + funcname + " ENDP" + SEPSTREMP;
		return stroka;
	}
	string StatusCodeFunc(Lexer::LEX& tables, int i, string& cyclecode)
	{
		bool ist = false, isf = false, cycle = false;
		IT::Entry operanfleft = ENTRYIT(i + 1);
		IT::Entry operandright = ENTRYIT(i + 3);
		string stroka;
		conditionnum++;
		cyclecode.clear();
		string wstr, rstr;

		for (int j = i + 5; MYLEX(j) != LEX_DIEZ; j++) // пропустили открывающую решетку
		{
			if (MYLEX(j) == LEX_ISTRUE)
				isf = true;
			if (MYLEX(j) == LEX_ISFALSE)
				ist = true;
			if (MYLEX(j) == LEX_CYCLE)
				cycle = true;
		}
		stroka = stroka + "mov edx, " + operanfleft.id + "\ncmp edx, " + operandright.id + "\n";
		switch (MYLEX(i + 2))
		{
		case LEX_MORE:  
			rstr = "jg";  
			wstr = "jl";  
			break;
		case LEX_LESS:   
			rstr = "jl"; 
			wstr = "jg";  
			break;
		case LEX_EQUALS:    
			rstr = "jz";  
			wstr = "jnz";  
			break;
		case LEX_NOTEQUALS:   
			rstr = "jnz";  
			wstr = "jz";  
			break;
		}

		if (!cycle && isf)
			stroka = stroka + "\n" + rstr + " right" + int_to_string(conditionnum);
		if (!cycle && ist)
			stroka = stroka + "\n" + wstr + " wrong" + int_to_string(conditionnum);
		if (cycle)
		{
			stroka = stroka + "\n" + rstr + " cycle" + int_to_string(conditionnum);
			cyclecode = stroka;
			stroka = stroka + "\njmp cyclenext" + int_to_string(conditionnum);
		}
		else if (!isf || !ist)
			stroka = stroka + "\njmp next" + int_to_string(conditionnum);
		return stroka;
	}
	vector <string> PackVectorWithValues(Lexer::LEX& tables)
	{
		vector <string> mainvec;
		vector <string> vectorforliteral;
		vector <string> vectorforid;

		mainvec.push_back(BEGIN);
		mainvec.push_back(EXTERN); 
		vectorforliteral.push_back(CONST);
		vectorforid.push_back(DATA);

		for (int i = 0; i < tables.idtable.size; i++)// const, data
		{
			IT::Entry exv = tables.idtable.table[i];
			string str = "\t\t" + string(exv.id);

			if (tables.idtable.table[i].idtype == IT::IDTYPE::L)	// литерал - в .const
			{
				switch (exv.iddatatype)
				{
				case IT::IDDATATYPE::NUM:  
					str = str + " sdword " + int_to_string(exv.value.vint);  
					break;
				case IT::IDDATATYPE::STR:  
					str = str + " byte '" + string(exv.value.vstr.str) + "', 0";  
					break;
				}
				vectorforliteral.push_back(str);
			}
			else if (tables.idtable.table[i].idtype == IT::IDTYPE::V)// переменна€ - в .data
			{
				switch (exv.iddatatype)
				{
				case IT::IDDATATYPE::NUM: 
					str = str + " sdword 0"; 
					break;
				case IT::IDDATATYPE::STR:
					str = str + " dword ?";  
					break;
				}
				vectorforid.push_back(str);
			}
		}
		mainvec.insert(mainvec.end(), vectorforliteral.begin(), vectorforliteral.end());
		mainvec.insert(mainvec.end(), vectorforid.begin(), vectorforid.end());
		mainvec.push_back(CODE);
		return mainvec;
	}
	void EntryPoint(Lexer::LEX& tables, Parm::PARM& parm, Log::LOG& log)
	{
		string functionname;
		string cycle;
		string str;//  cmp + j
		int countofparm;
		
		vector <string> mainvector = PackVectorWithValues(tables);
		ofstream ofile("D:\\Kursach\\SVY2020\\Generation\\Gen.asm");

		for (int i = 0; i < tables.lextable.size; i++)
		{
			switch (MYLEX(i))
			{
			case LEX_POEXALI:
			{
				str = str + SEPSTR("POEXALI") + "main PROC";
				break;
			}
			case LEX_OPERATION:
			{
				functionname = ENTRYIT(i + 1).id;
				countofparm = ENTRYIT(i + 1).value.params.count;
				str = genFunctionCode(tables, i, functionname, countofparm);
				break;
			}
			case LEX_CONCLUSION:
			{
				str = ExitCodeGener(tables, i, functionname, countofparm);
				break;
			}
			case LEX_ID: // вызов функции
			{
				if (MYLEX(i + 1) == LEX_LEFTSK && MYLEX(i - 1) != LEX_OPERATION) // не объ€вление, а вызов
					str = CallOfFunction(tables, log, i);
				break;
			}
			case LEX_CONDITION: // условие
			{
				str = StatusCodeFunc(tables, i, cycle);
				break;
			}
			case LEX_BRACELET:	// переход на метку в конце кондишна
			{
				if (MYLEX(i + 1) == LEX_ISFALSE || MYLEX(i + 1) == LEX_ISTRUE)
					str = str + "jmp next" + int_to_string(conditionnum);
			}
			case LEX_DIEZ:		// поставить метки в конце кондишна
			{
				if (MYLEX(i - 1) == LEX_BRACELET) //   ]#
				{
					bool c = false;
					for (int j = i; j > 0 && MYLEX(j) != LEX_CONDITION; j--)
						if (MYLEX(j) == LEX_CYCLE)
							c = true;
					if (c)
						str = cycle + "\ncyclenext" + int_to_string(conditionnum) + ":";
					else  str += "next" + int_to_string(conditionnum) + ":";
				}
				break;
			}
			case LEX_ISTRUE: // условие верно(метка)
			{
				str = str + "right" + int_to_string(conditionnum) + ":";
				break;
			}
			case LEX_ISFALSE: // условие неверно(метка)
			{
				str = str + "wrong" + int_to_string(conditionnum) + ":";
				break;
			}
			case LEX_CYCLE: // цикл с условием (метка)
			{
				str = str + "cycle" + int_to_string(conditionnum) + ":";
				break;
			}
			case LEX_EQUAL: // присваивание (вычисление выражений)
			{
				str = EqualizationOfValue(tables, log, i);
				while (MYLEX(++i) != LEX_SEPARATOR);	// пропускаем выражение
				break;
			}
			case LEX_NEWLINE: // перевод строки 
			{
				str += "push offset newline\ncall outstr\n";
				break;
			}
			case LEX_WRITE: // вывод
			{
				IT::Entry e = ENTRYIT(i + 1);
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
				mainvector.push_back(str);
			str.clear();
		}
		mainvector.push_back(END);
		// вывод в файл
		for (auto x : mainvector)
			ofile << x << endl;
		ofile.close();
	}
};