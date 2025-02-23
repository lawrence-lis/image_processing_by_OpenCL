#pragma once
/* Этот модуль содержит набор функций, которые управляют объектами OpenCL, такими как:
	- очереди команд;
	- объекты памяти;
	- программные объекты;
	- объекты ядра для функций ядра в программе;
  А также функции, которые позволяют помещать команды в очередь команд, например, выполнение ядра, чтение или запись объекта памяти и т.д.*/

#define PROCESSINGLIBRARY_EXPORTS

#ifdef PROCESSINGLIBRARY_EXPORTS
#define PROCESSINGLIBRARY_API __declspec(dllexport)
#else
#define PROCESSINGLIBRARY_API __declspec(dllimport)
#endif

#include "CL/cl.h"

