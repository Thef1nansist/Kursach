#pragma once
#include <fstream>
#define LEX_SEPARATORS	 'S'	// разделители
#define	LEX_ID_TYPE    	 't'	// типы данных		
#define	LEX_ID			 'i'	// идентификаторы		
#define	LEX_LITERAL		 'l'	// литералы		
#define	LEX_OPERATION    'f'	// function		
#define	LEX_POEXALI		 'm'	// Для главной функции (main)
#define	LEX_SEPARATOR	 ';'	// разделитель инструкций
#define	LEX_COMMA		 ','	// разделитель параметров 		
#define	LEX_LEFTBRACE	 '['	// начало блока		
#define	LEX_BRACELET	 ']'	// конец блока		
#define LEX_LEFT         '{'    //сдвиг влево
#define LEX_RIGHT        '}'    //сдвиг вправо
#define	LEX_LEFTSK		 '('	// начало списка параметров		
#define	LEX_RIGHTSK 	 ')'	// конец списка параметров		
#define	LEX_PL			 '+'	// сложение	
#define	LEX_MINUS		 '-'	// вычитание
#define	LEX_STAR		 '*'	// умножение
#define LEX_DIRSLASH	 '/'	// деление
#define	LEX_EQUAL		 '='	// присваивание				
#define LEX_CONDITION	 '?'	// condition:
#define LEX_DIEZ		 '#'	// разделитель блоков цикла/ усл.выражения
#define	LEX_LITERAL_NUMO 'q'        //восьмиричный литерал
#define LEX_ISTRUE		 'w'	// ложный блок
#define LEX_ISFALSE		 'r'	// истинный блок
#define LEX_CYCLE		 'c'	// цикл
#define LEX_ZAPISHI		 'o'	// вывод
#define LEX_NEWLINE		 '^'	// перевод строки
#define LEX_CONCLUSION	 'e'	// Выход из функции, процедуры
#define LEX_PROCEDURE	 'p'	// ключевое слово процедуры
#define LEX_NOW			 'n'	// Для объявления новых переменных
#define LEX_MORE		 '>'	// проверка на больше
#define LEX_LESS		 '<'	// проверка на меньше
#define LEX_EQUALS		 '&'	// проверка на равенство
#define LEX_NOTEQUALS	 '!'	// проверка на неравенство
#define	LEXEMA_FIXSIZE   1	        //размер лексемы
#define	MAXSIZE_LT		 4096	    //максимальное количество строк в ТЛ
#define	NULLDX_TI	 0xffffffff	//нет элемента ТИ


namespace LT	//таблица лексем
{
	struct Entry
	{
		char lexema;				
		int sn;							
		int idxTI;						

		Entry();
		Entry(char lexema, int snn, int idxti = NULLDX_TI);
	};

	struct LexTable						
	{
		int maxsize;					
		int size;						
		Entry* table;					
	};

	LexTable Create(int size);		            //ёмкость < LT_MAXSIZE
	void Add(LexTable& lextable, Entry entry);	//экземпляр ТЛ, строка ТЛ
	void writeLexTable(std::ostream* stream, LT::LexTable& lextable);		
	void writeLexemsOnLines(std::ostream* stream, LT::LexTable& lextable);	
};
