#pragma once
#include "pch.h"
#include "Header.h"


#include <vector>

typedef std::vector <LT::Entry> ltvec;
typedef std::vector <int> intvec;

namespace Polish
{
	bool PolishNotation(Lexer::LEX& lex, Log::LOG& log);
	intvec getExprPositions(Lexer::LEX& lex);
	int  fillVector(int lextable_pos, LT::LexTable& lextable, ltvec& v);
	bool StartPolishNat(IT::IdTable& idtable, Log::LOG& log, int lextable_pos, ltvec& v);
	void addToTable(LT::LexTable& new_table, IT::IdTable& idtable, ltvec& v);
};
