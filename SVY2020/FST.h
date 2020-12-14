#pragma once
#pragma once

#include "IT.h"

namespace FST
{
	struct RELATION		
	{
		char  symbol;	
		short nnode;	
		RELATION(
			char c,		
			short ns	
		);
	};

	struct NODE					
	{
		short n_relation;		
		RELATION* relations;	
		NODE();					
		NODE(short n, RELATION rel, ...);  
	};

	struct FST   
	{
		char* string;				
		short position;				
		short nstates;				
		NODE* node;					
		short* rstates;				
		FST(short ns, NODE n, ...); 
		FST(char* s, FST& fst);		
	};

	bool execute(FST& fst); 
};

