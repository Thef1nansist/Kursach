// GenLib.cpp : Определяет функции для статической библиотеки.
//

#include "pch.h"
#include "framework.h"


#include <iostream>

extern "C"
{
	int __stdcall lenght(char* str)
	{
		if (str == nullptr)
			return 0;
		int len = 0;
		for (int i = 0; i < 256; i++)
			if (str[i] == '\0')
			{
				len = i; break;
			}
		return len;
	}
	
	int __stdcall atoii(const char* str)
	{
		int number = 0;
		for (int i = 0; str[i] && str[i] >= '0' && str[i] <= '9'; ++i)
			number = (number * 10) + str[i] - '0';

		return number;
	}
	

	char* __stdcall concat( char* str1, char* str2)
	{
		int i = NULL, len1 = NULL, len2 = NULL; char* buffer = NULL;
		
		buffer = (char*)malloc(sizeof(str1) + sizeof(str2));

		for (int j = 0; str1[j] != '\0'; j++)
		{
			if (i == 255)
				break;
			buffer[i++] = str1[j];
		}
		for (int j = 0; str2[j] != '\0'; j++)
		{
			if (i == 255)
				break;
			buffer[i++] = str2[j];
		}
		buffer[i] = '\0';
		return buffer;
	}

	int __stdcall outnum(int value)
	{
		std::cout << value;
		return 0;
	}

	int __stdcall outstr(char* ptr)
	{
		if (ptr == nullptr)
		{
			std::cout << std::endl;
		}
		for (int i = 0; ptr[i] != '\0'; i++)
			std::cout << ptr[i];
		return 0;
	}
}
