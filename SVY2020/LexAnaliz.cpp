#include "pch.h"

int DecimicalNotation(std::string input, int scaleofnot) {
	std::string num = input.substr(1, input.size());
	return std::stoi(num, nullptr, scaleofnot);
}
namespace Lexer
{
	Graph graphs[N_GRAPHS] =
	{
		{ LEX_SEPARATORS, FST::FST(GRAPH_SEPARATORS) },
		{ LEX_LITERAL, FST::FST(GRAPH_INT_LITERAL) },
		{ LEX_LITERAL, FST::FST(GRAPH_STRING_LITERAL) },
		{ LEX_NOW, FST::FST(GRAPH_NOW) },	
		{ LEX_POEXALI, FST::FST(GRAPH_POEXALI) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_NUMBER) },
		{ LEX_ID_TYPE, FST::FST(GRAPH_STRING) },
		{ LEX_OPERATION, FST::FST(GRAPH_OPERATION) },
		{ LEX_PROCEDURE, FST::FST(GRAPH_PROCEDURE) },
		{ LEX_CONCLUSION, FST::FST(GRAPH_CONCLUSION) },
		{ LEX_ZAPISHI, FST::FST(GRAPH_WRITE) },
		{ LEX_NEWLINE, FST::FST(GRAPH_NEWLINE) },
		{ LEX_CONDITION, FST::FST(GRAPH_CONDITION) },
		{ LEX_CYCLE, FST::FST(GRAPH_CYCLE) },
		{ LEX_ISFALSE, FST::FST(GRAPH_ISFALSE) },
		{ LEX_ISTRUE, FST::FST(GRAPH_ISTRUE) },
		{ LEX_ID, FST::FST(GRAPH_ID) }
	};
	
	IT::IDDATATYPE TypeOfValue(char* curword, char* idtype)
	{
		if (!strcmp(TYPE_PROCEDURE, idtype))
			return IT::IDDATATYPE::PROC; 
		if (!strcmp(TYPE_NUMBER, idtype))
			return IT::IDDATATYPE::NUM;	 
		if (!strcmp(TYPE_STRING, idtype))
			return IT::IDDATATYPE::STR;  
		if (isdigit(*curword) || *curword == LEX_MINUS)
			return IT::IDDATATYPE::NUM;				
		else if (*curword == IN_CODE_QUOTE)
			return IT::IDDATATYPE::STR;	

		return IT::IDDATATYPE::UNDEF;		
	}

	int IndexOfInLT(LT::LexTable& lextable, int itTableIndex)					
	{
		if (itTableIndex == NULLIDX_TI)		
			return lextable.size;
		for (int i = 0; i < lextable.size; i++)
			if (itTableIndex == lextable.table[i].idxTI)
				return i;
		return NULLIDX_TI;
	}

	bool isLiteral(char* id)
	{
		if (isdigit(*id) || *id == IN_CODE_QUOTE || *id == LEX_MINUS)
			return true;
		return false;
	}

	IT::STDFNC StandOfFunc(char* id)
	{
		if (!strcmp(CONCAT, id))
			return IT::STDFNC::F_CONCAT;
		if (!strcmp(LENGHT, id))
			return IT::STDFNC::F_LENGHT;
		if (!strcmp(ATOII, id))
			return IT::STDFNC::F_ATOII;
		return IT::STDFNC::F_NOT_STD;
	}

	char* NextLiteralName()						
	{
		static int literalNumber = 1;
		char* buf = new char[SCOPED_ID_MAXSIZE], num[3];
		strcpy_s(buf, MAXSIZE_ID, "VALS");
		_itoa_s(literalNumber++, num, 10);
		strcat(buf, num);
		return buf;
	}

	int LiteralOfIndex(IT::IdTable ittable, char* value, IT::IDDATATYPE type) 
	{
		for (int i = 0; i < ittable.size; i++)
		{
			if (ittable.table[i].idtype == IT::IDTYPE::L && ittable.table[i].iddatatype == type)
			{
				switch (type)
				{
				case IT::IDDATATYPE::NUM:
					if (ittable.table[i].value.vint == atoi(value))
						return i;
					break;
				case IT::IDDATATYPE::STR:
					char buf[STR_MAXSIZE];
					for (unsigned j = 1; j < strlen(value) - 1; j++) 
						buf[j - 1] = value[j];
					buf[strlen(value) - 2] = IN_CODE_NULL;
					if (strcmp(ittable.table[i].value.vstr.str, buf) == 0)
						return i;
					break;
				}
			}
		}
		return NULLIDX_TI;
	}

	char* AreaName(IT::IdTable idtable, char* prevword)
	{
		char* area = new char[8];
		area[0] = 'p';
		area[2] = 'e';
		area[1] = 'o';
		area[3] = 'x';
		area[4] = 'a';
		area[5] = 'l';
		area[6] = 'i';
		area[7] = '\0';
		if (strcmp(prevword, POEXALI) == 0)
			return area;
		for (int i = idtable.size - 1; i >= 0; i--)
			if (idtable.table[i].idtype == IT::IDTYPE::F)
				return idtable.table[i].id;
		return nullptr; 
	}

	IT::Entry* getEntry(		
		Lexer::LEX& tables,						
		char lex,								
		char* id,								
		char* idtype,							
		bool isParam,							
		bool isFunc,							
		Log::LOG log,							
		int line,								
		bool& lex_ok)							
	{

		IT::IDDATATYPE type = TypeOfValue(id, idtype);
		int index = IT::isId(tables.idtable, id);	
		if (lex == LEX_LITERAL)
			index = LiteralOfIndex(tables.idtable, id, type);
		if (index != NULLIDX_TI)
			return nullptr;	

		IT::Entry* itentry = new IT::Entry;
		itentry->iddatatype = type; 

		itentry->idxfirstLE = IndexOfInLT(tables.lextable, index);

		if (lex == LEX_LITERAL)
		{
			bool int_ok = IT::SetValue(itentry, id);
			if (!int_ok)
			{
				Log::writeError(log.stream, Error::GetError(313, line, 0));
				lex_ok = false;
			}
			if (itentry->iddatatype == IT::IDDATATYPE::STR && itentry->value.vstr.len == 0)
			{
				Log::writeError(log.stream, Error::GetError(310, line, 0));
				lex_ok = false;
			}
			strcpy_s(itentry->id, NextLiteralName());
			itentry->idtype = IT::IDTYPE::L;
		}
		else 
		{
			switch (type)
			{
			case IT::IDDATATYPE::NUM:
				itentry->value.vint = NUM_DEFAULT;
				break;
			case IT::IDDATATYPE::STR:
				strcpy_s(itentry->value.vstr.str, "");
				itentry->value.vstr.len = STR_DEFAULT;
				break;
			}

			if (isFunc)
			{
				switch (StandOfFunc(id))
				{
				case IT::STDFNC::F_ATOII:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = ATOII_TYPE;
					itentry->value.params.count = ATOII_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[ATOII_PARAMS_CNT];
					for (int k = 0; k < ATOII_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::ATOII_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_CONCAT:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = CONCAT_TYPE;
					itentry->value.params.count = CONCAT_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[CONCAT_PARAMS_CNT];
					for (int k = 0; k < CONCAT_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::CONCAT_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_LENGHT:
				{
					itentry->idtype = IT::IDTYPE::S;
					itentry->iddatatype = LENGHT_TYPE;
					itentry->value.params.count = LENGHT_PARAMS_CNT;
					itentry->value.params.types = new IT::IDDATATYPE[LENGHT_PARAMS_CNT];
					for (int k = 0; k < LENGHT_PARAMS_CNT; k++)
						itentry->value.params.types[k] = IT::LENGHT_PARAMS[k];
					break;
				}
				case IT::STDFNC::F_NOT_STD:
					itentry->idtype = IT::IDTYPE::F;
					break;
				}
			}
			else if (isParam)
				itentry->idtype = IT::IDTYPE::P;
			else
				itentry->idtype = IT::IDTYPE::V;

			strncpy_s(itentry->id, id, SCOPED_ID_MAXSIZE);
		}
		int i = tables.lextable.size; 

		if (i > 1 && itentry->idtype == IT::IDTYPE::F && tables.lextable.table[i - 1].lexema != LEX_OPERATION)
		{
			Log::writeError(log.stream, Error::GetError(303, line, 0)); // в объ€влении не указан тип функции
			lex_ok = false;
		}
		if (itentry->iddatatype == IT::IDDATATYPE::UNDEF)
		{
			Log::writeError(log.stream, Error::GetError(300, line, 0));// невозможно определелить тип
			lex_ok = false;
		}
		if (i > 1 && itentry->idtype == IT::IDTYPE::V && tables.lextable.table[i - 2].lexema != LEX_NOW)
		{
			Log::writeError(log.stream, Error::GetError(304, line, 0));// в объ€влении отсутствует ключевое слово now
			lex_ok = false;
		}
		return itentry;
	}

	bool analyze(LEX& tables, In::IN& in, Log::LOG& log, Parm::PARM& parm)
	{
		tables.lextable = LT::Create(MAXSIZE_LT);
		tables.idtable = IT::Create(MAXSIZE_TI);
		std::stack <char*> areanames;			
		bool isParam = false, isFunc = false;
		static bool lex_ok = true;
		int enterPoint = NULL;
		int curline;
		char curword[STR_MAXSIZE], nextword[STR_MAXSIZE];

		for (int i = 0; i < In::InWord::size; i++)
		{
			strcpy_s(curword, in.words[i].word);
			if (i < In::InWord::size - 1)
				strcpy_s(nextword, in.words[i + 1].word);
			curline = in.words[i].line;
			isFunc = false;
			int idxTI = NULLIDX_TI;

			for (int j = 0; j < N_GRAPHS; j++)
			{
				FST::FST fst(curword, graphs[j].graph);
				if (FST::execute(fst))
				{
					char lexema = graphs[j].lexema;
					switch (lexema)
					{
					case LEX_POEXALI:
						enterPoint++;
						break;
					case LEX_SEPARATORS:
					{
						switch (*curword)
						{
						case LEX_LEFTBRACE:		// start area view
						{
							if (i > 0 && *in.words[i - 1].word == LEX_ISFALSE || *in.words[i - 1].word == LEX_ISTRUE || *in.words[i - 1].word == LEX_CYCLE)
								break;
							char* functionname = new char[MAXSIZE_ID];
							char* scopename = AreaName(tables.idtable, in.words[i - 1].word);
							if (scopename == nullptr)  break;
							strcpy_s(functionname, MAXSIZE_ID, scopename);
							areanames.push(functionname);
							break;
						}
						case LEX_BRACELET:		// exit area view
						{

							if (*in.words[i + 1].word == LEX_ID_TYPE || *in.words[i + 1].word == LEX_PROCEDURE || *in.words[i + 1].word == LEX_POEXALI)
							{
								if (!areanames.empty())
									areanames.pop();
							}
							break;
						}
						case LEX_LEFTSK:		
						{
							isParam = true;
							if (*nextword == LEX_RIGHTSK || ISTYPE(nextword))
							{
								char* functionname = new char[MAXSIZE_ID];
								char* scopename = AreaName(tables.idtable, in.words[i - 1].word);
								if (scopename == nullptr)
									break;
								strcpy_s(functionname, MAXSIZE_ID, scopename);
								areanames.push(functionname);
							}
							break;
						}
						case LEX_RIGHTSK:	
						{
							isParam = false;
							
							if (*in.words[i - 1].word == LEX_LEFTSK || (i > 2 && (tables.lextable.table[tables.lextable.size - 2].lexema == LEX_ID_TYPE)))
								areanames.pop();
							break;
						}
						}
						lexema = *curword;
						break;
					}
					case LEX_LITERAL_NUMO:
					{
						int value = DecimicalNotation(curword, 8);

						if (value < NUM_MINSIZE || value > NUM_MAXSIZE)
							throw ERROR_THROW(204);
						tables.idtable.table[tables.idtable.size - 1].value.vint = value;

						LT::Entry entrylt(graphs[j].lexema, in.words[i].line);
					
						break;
					}
					case LEX_ID:
					case LEX_LITERAL:
					{
						char id[STR_MAXSIZE] = "";
						idxTI = NULLDX_TI;  
						if (*nextword == LEX_LEFTSK)
							isFunc = true;						
						char* idtype = (isFunc && i > 1) ?	
							in.words[i - 2].word : in.words[i - 1].word;		
						if (!isFunc && !areanames.empty())
							strncpy_s(id, areanames.top(), MAXSIZE_ID);
						strncat(id, curword, MAXSIZE_ID);
						if (isLiteral(curword))
							strcpy_s(id, curword); 

						IT::Entry* itentry = getEntry(tables, lexema, id, idtype, isParam, isFunc, log, curline, lex_ok);
						if (itentry != nullptr) 
						{
							if (isFunc) 
							{
								if (StandOfFunc(id) == IT::STDFNC::F_NOT_STD) 
								{
									itentry->value.params.count = NULL;
									itentry->value.params.types = new IT::IDDATATYPE[MAX_PARAMS_COUNT];
									for (int k = i; in.words[k].word[0] != LEX_RIGHTSK; k++)
									{
										if (k == In::InWord::size || in.words[k].word[0] == LEX_SEPARATOR)
											break;
										if (ISTYPE(in.words[k].word))
										{
											if (itentry->value.params.count >= MAX_PARAMS_COUNT)
											{
												Log::writeError(log.stream, Error::GetError(306, curline, 0));
												lex_ok = false;
												break;
											}
											itentry->value.params.types[itentry->value.params.count++] = TypeOfValue(NULL, in.words[k].word);
										}
									}
								}
							}
							IT::Add(tables.idtable, *itentry);
							idxTI = tables.idtable.size - 1;
						}
						else 
						{
							int i = tables.lextable.size - 1; 
							if (i > 0 && tables.lextable.table[i - 1].lexema == LEX_NOW || tables.lextable.table[i].lexema == LEX_NOW
								|| tables.lextable.table[i - 1].lexema == LEX_OPERATION || tables.lextable.table[i].lexema == LEX_OPERATION
								|| tables.lextable.table[i - 1].lexema == LEX_ID_TYPE || tables.lextable.table[i].lexema == LEX_ID_TYPE
								|| tables.lextable.table[i - 1].lexema == LEX_PROCEDURE || tables.lextable.table[i].lexema == LEX_PROCEDURE)
							{
								Log::writeError(log.stream, Error::GetError(305, curline, 0));
								lex_ok = false;
							}
							idxTI = IT::isId(tables.idtable, id);	
							if (lexema == LEX_LITERAL)
								idxTI = LiteralOfIndex(tables.idtable, curword, TypeOfValue(id, in.words[i - 1].word)); // или литерала
						}
					}
					break;
					}

					LT::Entry* ltentry = new LT::Entry(lexema, curline, idxTI);
					LT::Add(tables.lextable, *ltentry);
					break;
				}
				else if (j == N_GRAPHS - 1) 
				{
					Log::writeError(log.stream, Error::GetError(201, curline, 0));
					lex_ok = false;
				}
			}
		}

		if (enterPoint == NULL) 
		{
			Log::writeError(log.stream, Error::GetError(301));
			lex_ok = false;
		}
		if (enterPoint > 1) 
		{
			Log::writeError(log.stream, Error::GetError(302));
			lex_ok = false;
		}
		return lex_ok;
	}
};