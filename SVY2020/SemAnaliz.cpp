#include "pch.h"


namespace Semantic
{
	bool Semantic::semanticsCheck(Lexer::LEX& tables, Log::LOG& log)
	{
		bool sem_ok = true;

		for (int i = 0; i < tables.lextable.size; i++)
		{
			switch (tables.lextable.table[i].lexema)
			{
			case LEX_NOW:
			{
				if (tables.lextable.table[i + 1].lexema != LEX_ID_TYPE)
				{
					sem_ok = false;
					Log::writeError(log.stream, Error::GetError(303, tables.lextable.table[i].sn, 0));
				}
			}
			case LEX_DIRSLASH:
			{
				int k = i;
				if (tables.lextable.table[i + 1].lexema == LEX_ID)
				{
					for (k; k > 0; k--)
					{
						if (tables.lextable.table[k].lexema == LEX_ID)
						{
							if (tables.idtable.table[tables.lextable.table[k].idxTI].id == tables.idtable.table[tables.lextable.table[i + 1].idxTI].id)
							{
								if (tables.lextable.table[k + 2].lexema == LEX_LITERAL && tables.idtable.table[tables.lextable.table[k + 2].idxTI].value.vint == 0)
								{
									sem_ok = false;
									Log::writeError(log.stream, Error::GetError(318, tables.lextable.table[k].sn, 0));
								}
							}
						}
					}
				}
				if (tables.lextable.table[i + 1].lexema == LEX_LITERAL)
				{
					if (tables.idtable.table[tables.lextable.table[i + 1].idxTI].value.vint == 0)
					{
						sem_ok = false;
						Log::writeError(log.stream, Error::GetError(318, tables.lextable.table[k].sn, 0));
					}
				}
				break;
			}
			case LEX_EQUAL: // выражение
			{
				if (i > 0 && tables.lextable.table[i - 1].idxTI != NULLIDX_TI) // левый операнд
				{
					IT::IDDATATYPE lefttype = tables.idtable.table[tables.lextable.table[i - 1].idxTI].iddatatype;
					bool ignore = false;

					for (int k = i + 1; tables.lextable.table[k].lexema != LEX_SEPARATOR; k++)
					{
						if (k == tables.lextable.size)
							break; // синтакс ошибка - нет ;
						if (tables.lextable.table[k].idxTI != NULLIDX_TI) // если ид - проверить совпадение типов
						{
							if (!ignore)
							{
								IT::IDDATATYPE righttype = tables.idtable.table[tables.lextable.table[k].idxTI].iddatatype;
								if (lefttype != righttype) // типы данных в выражении не совпадают
								{
									Log::writeError(log.stream, Error::GetError(314, tables.lextable.table[k].sn, 0));
									sem_ok = false;
									break;
								}
							}
							// если лексема сразу после идентиф скобка - это вызов функции
							if (tables.lextable.table[k + 1].lexema == LEX_LEFTSK)
							{
								ignore = true;
								continue;
							}
							// закрывающая скобка после списка параметров
							if (ignore && tables.lextable.table[k + 1].lexema == LEX_RIGHTSK)
							{
								ignore = false;
								continue;
							}
						}
						if (lefttype == IT::IDDATATYPE::STR) // справа только литерал, ид или вызов строковой ф-ции
						{
							char l = tables.lextable.table[k].lexema;
							if (l == LEX_PL || l == LEX_MINUS || l == LEX_STAR) // выражения недопустимы
							{
								Log::writeError(log.stream, Error::GetError(316, tables.lextable.table[k].sn, 0));
								sem_ok = false;
								break;
							}
						}
					}
				}
				break;
			}
			case LEX_ID: // проверка типа возвращаемого значения  
			{
				IT::Entry e = tables.idtable.table[tables.lextable.table[i].idxTI];

				if (i > 0 && tables.lextable.table[i - 1].lexema == LEX_OPERATION)
				{
					if (e.idtype == IT::IDTYPE::F) //функция, не процедура
					{
						for (int k = i + 1; ; k++)
						{
							char l = tables.lextable.table[k].lexema;
							if (l == LEX_CONCLUSION)
							{
								int next = tables.lextable.table[k + 1].idxTI; // след. за return
								if (next != NULLIDX_TI)
								{
									// тип функции и возвращаемого значения не совпадают
									if (tables.idtable.table[next].iddatatype != e.iddatatype)
									{
										Log::writeError(log.stream, Error::GetError(315, tables.lextable.table[k].sn, 0));
										sem_ok = false;
										break;
									}
								}
								break; // нашли exit
							}

							if (k == tables.lextable.size) break;
						}
					}
				}
				if (tables.lextable.table[i + 1].lexema == LEX_LEFTSK && tables.lextable.table[i - 1].lexema != LEX_OPERATION) // именно вызов
				{
					if (e.idtype == IT::IDTYPE::F || e.idtype == IT::IDTYPE::S) // точно функция
					{
						int paramscount = NULL;
						// проверка передаваемых параметров
						for (int j = i + 1; tables.lextable.table[j].lexema != LEX_RIGHTSK; j++)
						{
							// проверка соответствия передаваемых параметров прототипам
							if (tables.lextable.table[j].lexema == LEX_ID || tables.lextable.table[j].lexema == LEX_LITERAL)
							{
								paramscount++;
								if (e.value.params.count == NULL)
									break;
								IT::IDDATATYPE ctype = tables.idtable.table[tables.lextable.table[j].idxTI].iddatatype;
								if (ctype != e.value.params.types[paramscount - 1])
								{
									// Несовпадение типов передаваемых параметров
									Log::writeError(log.stream, Error::GetError(309, tables.lextable.table[i].sn, 0));
									sem_ok = false;
									break;
								}
							}
							if (j == tables.lextable.size)
								break;
						}
						if (paramscount != e.value.params.count)
						{
							// Количество передаваемых и принимаемых параметров не совпадает
							Log::writeError(log.stream, Error::GetError(308, tables.lextable.table[i].sn, 0));
							sem_ok = false;
						}
						if (paramscount > 3)
						{
							// Слишком много параметров в вызове
							Log::writeError(log.stream, Error::GetError(307, tables.lextable.table[i].sn, 0));
							sem_ok = false;
						}
					}
				}
				break;
			}
			case LEX_MORE:	
			case LEX_LESS: 
			case LEX_EQUALS:   
			case LEX_NOTEQUALS:
			{
				// левый и правый операнд - числовой тип
				bool flag = true;
				if (i > 1 && tables.lextable.table[i - 1].idxTI != NULLIDX_TI)
				{
					if (tables.idtable.table[tables.lextable.table[i - 1].idxTI].iddatatype != IT::IDDATATYPE::NUM)
						flag = false;
				}
				if (tables.lextable.table[i + 1].idxTI != NULLIDX_TI)
				{
					if (tables.idtable.table[tables.lextable.table[i + 1].idxTI].iddatatype != IT::IDDATATYPE::NUM)
						flag = false;
				}
				if (!flag)
				{
					// строка или неизвестный ид в условии
					Log::writeError(log.stream, Error::GetError(317, tables.lextable.table[i].sn, 0));
					sem_ok = false;
				}
				break;
			}
			}
		}

		return sem_ok;
	}
};