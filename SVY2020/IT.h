#pragma once
#include <iostream>

#define MAXSIZE_ID	8					//макс число символов идентификатора
#define SCOPED_ID_MAXSIZE   MAXSIZE_ID*2 //макс число символов идентификатор + область видимости
#define MAXSIZE_TI		4096			//макс число количество строк в таблице идентификаторов
#define NUM_DEFAULT	0x00000000		//значение по умолчанию дл€ int
#define STR_DEFAULT	0x00			//значение по умолчанию дл€ string
#define NULLIDX_TI		0xffffffff		//нет элемента таблицы идентификаторов
#define STR_MAXSIZE	255				//максимальна€ длина строкового литерала
#define NUM_MAXSIZE   127		//максимальное значение дл€ типа number
#define NUM_MINSIZE  -128		//минимальное значение дл€ типа number
#define MAX_PARAMS_COUNT 3				//максимальное количество параметров у функции
#define CONCAT_PARAMS_CNT 2			//кол-во параметров у функции concat
#define LENGHT_PARAMS_CNT 1	//кол-во параметров у функции lenght
#define ATOII_PARAMS_CNT 1 //кол-во параметров у функции atoii
#define CONCAT_TYPE IT::IDDATATYPE::STR
#define LENGHT_TYPE IT::IDDATATYPE::NUM
#define ATOII_TYPE IT::IDDATATYPE::NUM



namespace IT
{
	enum IDDATATYPE { NUM = 1, STR = 2, PROC = 3, UNDEF };//типы данных идентификаторов: числовой, строковый, без типа(дл€ процедур), неопределенный
	enum IDTYPE { V = 1, F = 2, P = 3, L = 4, S = 5 };	//типы идентификаторов: переменна€, функци€, параметр, литерал, стандартна€ функци€
	enum STDFNC { F_CONCAT, F_LENGHT, F_ATOII, F_NOT_STD };	//стандартные функции
	static const IDDATATYPE CONCAT_PARAMS[] = { IT::IDDATATYPE::STR, IT::IDDATATYPE::STR };//параметры функции  concatstr
	static const IDDATATYPE LENGHT_PARAMS[] = { IT::IDDATATYPE::STR };//параметры функции strlen
	static const IDDATATYPE ATOII_PARAMS[] = { IT::IDDATATYPE::STR };//параметры ф-ции atoi
	struct Entry
	{
		union
		{
			int	vint;            			
			struct
			{
				int len;					
				char str[STR_MAXSIZE - 1];//сами символы
			} vstr;							//строковое значение
			struct
			{
				int count;					// кол-во пр. функции
				IDDATATYPE* types;			//типы парм функции
			} params;
		} value;						//значение идентификатора
		int			idxfirstLE;				//индекс в таблице лексем		
		char		id[SCOPED_ID_MAXSIZE];	//идентификатор
		IDDATATYPE	iddatatype;				//тип данных
		IDTYPE		idtype;					//тип идентификатора
		// дальше 2 конструктора
		Entry() = default;
		Entry(char* id, int idxLT, IDDATATYPE datatype, IDTYPE idtype) 
		{
			strncpy_s(this->id, id, SCOPED_ID_MAXSIZE - 1);
			this->idxfirstLE = idxLT;
			this->iddatatype = datatype;
			this->idtype = idtype;
		};
	};
	struct IdTable		//экземпл€р таблицы идентификаторов
	{
		int maxsize;	
		int size;		
		Entry* table;	
	};
	IdTable Create(int size = NULL);	//Ємкость таблицы идентификаторов < TI_MAXSIZE
	void Add(					//добавить строку в таблицу идентификаторов
		IdTable& idtable,		//экземпл€р таблицы идентификаторов
		Entry entry);			//строка таблицы идентификаторов 
	int isId(					//возврат: номер строки(если есть), TI_NULLIDX(если есть)
		IdTable& idtable,		//экземпл€р таблицы идентификаторов
		char id[SCOPED_ID_MAXSIZE]);	//идентификатор
	bool SetValue(IT::Entry* entry, char* value);	//задать значение идентификатора
	bool SetValue(IT::IdTable& idtable, int index, char* value);
	void writeIdTable(std::ostream* stream, IT::IdTable& idtable); //вывести таблицу идентификаторов
};

