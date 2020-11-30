#pragma once
//bool PolishNotation(int i, Lexer::LEX& lex);
//bool startPolishNotation(Lexer::LEX& lex);
#include "pch.h"
#include "Header.h"


#include <vector>

typedef std::vector <LT::Entry> ltvec;
typedef std::vector <int> intvec;

namespace Polish
{
	bool PolishNotation(Lexer::LEX& lex, Log::LOG& log);
	// в вектор сохраняем позиции начала всех выражений в исходном тексте
	intvec getExprPositions(Lexer::LEX& lex);
	// заполняем вектор лексемами в инфиксной нотации
	int  fillVector(int lextable_pos, LT::LexTable& lextable, ltvec& v);
	// внутри вектора получаем порядок идентификаторов в польской нотации
	bool setPolishNotation(IT::IdTable& idtable, Log::LOG& log, int lextable_pos, ltvec& v);
	// добавляем вектор в новую таблицу лексем
	void addToTable(LT::LexTable& new_table, IT::IdTable& idtable, ltvec& v);
};
