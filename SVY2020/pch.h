#ifndef PCH_H
#define PCH_H
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <locale>
#include <cwchar>
#include <string>
#include <ctime>
#include <vector>
#include <stack>

//Для Generator.cpp
#include <iosfwd>
#include "Generator.h"
#include "Parm.h"
#include "LexAnaliz.h"
#include "IT.h"
#include "LT.h"
#include <sstream>
#include <cstring>

//Для лексического анализа
#include "Graphs.h"

//
#include "Error.h"
#include "SemAnaliz.h"

// для мэина
#include "Log.h"
#include "In.h"
#include "PolishNotation.h"
#include "MFST.h"


#endif //PCH_H
