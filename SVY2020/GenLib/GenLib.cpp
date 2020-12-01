// GenLib.cpp : Определяет функции для статической библиотеки.
//

#include "pch.h"
#include "framework.h"


#include <iostream>

extern "C"
{
	int __stdcall lenght(char* buffer, char* str)
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



	int __stdcall atoii(char* ptr)
	{
		if (ptr == nullptr)
		{
			std::cout << std::endl;
		}

		int arr[3], n;
		for (int i = 0; i < 4; i++) {
			if (ptr[i] != '1' && ptr[i] != '2' && ptr[i] != '3' && ptr[i] != '4' && ptr[i] != '5' && ptr[i] != '6' && ptr[i] != '7' && ptr[i] != '8' && ptr[i] != '9' && ptr[i] != '0') {
				if (ptr[0] == NULL)
					return 0;
				break;
			}
			else
				arr[i] = (int)ptr[i] - 48;
		}
		if (arr[2] == 1 || arr[2] == 2 || arr[2] == 3 || arr[2] == 4 || arr[2] == 5 || arr[2] == 6 || arr[2] == 7 || arr[2] == 8 || arr[2] == 9 || arr[2] == 0) {
			n = arr[0] * 100 + arr[1] * 10 + arr[2] * 1;
			if (n > 128)
				return 128;
			return n;
		}
		if (arr[1] != NULL) {
			n = arr[0] * 10 + arr[1] * 1;
			return n;
		}
		if (arr[0] != NULL) {
			n = arr[0] * 1;
			return n;
		}

	}


	char* __stdcall concat(char* buffer, char* str1, char* str2)
	{
		int i = NULL, len1 = NULL, len2 = NULL;
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
